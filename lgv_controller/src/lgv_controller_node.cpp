#include <ros/ros.h>
#include <serial/serial.h>
#include <std_msgs/String.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
#include <sstream>

serial::Serial ser;

void serial_callback(const geometry_msgs::Twist& msg)
{
  ROS_INFO("[%g m/s, %g m/s, %g rad/s]", msg.linear.x, msg.linear.y, msg.angular.z);
  unsigned char s_buffer[32] = { 0xFF, 0xFF };
  *(int16_t*)(s_buffer + 2) = msg.linear.x * 1000;
  *(int16_t*)(s_buffer + 4) = msg.linear.y * 1000;
  *(int16_t*)(s_buffer + 6) = msg.angular.z * 1000;
  ser.write(s_buffer, 8);
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "lgv_controller");

  ros::NodeHandle node;

  std::string port = "/dev/ttyS3";
  node.param("port", port, port);

  int baudrate = 115200;
  node.param("baudrate", baudrate, baudrate);

  int timeout = 10;
  node.param("timeout", timeout, timeout);

  ros::Publisher odom_pub = node.advertise<nav_msgs::Odometry>("odom", 10);
  ros::Subscriber vel_sub = node.subscribe("cmd_vel", 10, serial_callback);
  tf::TransformBroadcaster odom_broadcaster;

  try
  {
    serial::Timeout to = serial::Timeout::simpleTimeout(timeout);
    ser.setPort(port);
    ser.setBaudrate(baudrate);
    ser.setTimeout(to);
    ser.open();
  }
  catch (serial::IOException& e)
  {
    ROS_ERROR_STREAM("Unable to open Serial Port !");
    return ~0;
  }
  if (ser.isOpen())
  {
    unsigned char buf[] = { 0xFF, 0xFE };
    ser.write(buf, sizeof(buf) / sizeof(*buf));
    ROS_INFO_STREAM("Serial Port initialized");
  }
  else
  {
    return ~0;
  }

  unsigned char r_buffer[32];
  ros::Rate loop_rate(100);

  while (ros::ok())
  {
    ros::spinOnce();
    ros::Time current_time = ros::Time::now();

    size_t length = ser.available();
    if (length)
    {
      length = ser.read(r_buffer, length);
      for (size_t i = 0; i < length; ++i)
      {
        if (r_buffer[i] == 0xFF && r_buffer[i + 1] == 0xFF)
        {
          float vx = *(int16_t*)(r_buffer + i + 2) * 0.001F;
          float vy = *(int16_t*)(r_buffer + i + 4) * 0.001F;
          float vw = *(int16_t*)(r_buffer + i + 6) * 0.001F;
          float x = *(float*)(r_buffer + i + 8);
          float y = *(float*)(r_buffer + i + 12);
          float w = *(float*)(r_buffer + i + 16);
          // printf("%g,%g,%g %g,%g,%g\n", vx, vy, vw, x, y, w);

          geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(w);

          geometry_msgs::TransformStamped odom_trans;
          odom_trans.header.stamp = current_time;
          odom_trans.header.frame_id = "odom";
          odom_trans.child_frame_id = "base_link";
          odom_trans.transform.translation.x = x;
          odom_trans.transform.translation.y = y;
          odom_trans.transform.translation.z = 0.0;
          odom_trans.transform.rotation = odom_quat;
          odom_broadcaster.sendTransform(odom_trans);

          nav_msgs::Odometry odom;
          odom.header.stamp = ros::Time::now();
          odom.header.frame_id = "odom";

          // set the velocity
          odom.child_frame_id = "base_link";
          odom.twist.twist.linear.x = vx;
          odom.twist.twist.linear.y = vy;
          odom.twist.twist.angular.z = vw;

          // set the position
          odom.pose.pose.position.x = x;
          odom.pose.pose.position.y = y;
          odom.pose.pose.position.z = 0.0;
          odom.pose.pose.orientation = odom_quat;

          odom_pub.publish(odom);
          break;
        }
      }
    }

    loop_rate.sleep();
  }

  ser.close();
  return 0;
}
