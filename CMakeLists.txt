list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/UseArmNoneEabi.cmake)
cmake_minimum_required(VERSION 2.8.12...4.0)
set(CMAKE_BUILD_TYPE Debug CACHE STRING "")
project(lgv VERSION 0.1.0 LANGUAGES C ASM)

find_package(OpenOCD)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_FLAGS_DEBUG "-Og -g -ggdb")
set(CMAKE_CXX_FLAGS_DEBUG "-Og -g -ggdb")
set(MCPU cortex-m3 CACHE STRING "Emit assembly for CPU")

file(GLOB_RECURSE SOURCES *.s Drivers/*.* Core/*.* bsp/*.* app/*.*)

file(GLOB STM32_LINKER *_FLASH.ld)
file(GLOB STM32_STARTUP startup_*.s)
file(GLOB STM32_OPENOCD openocd.cfg)
string(TOUPPER "${STM32_STARTUP}" STM32_FAMILY)
get_filename_component(STM32_FAMILY ${STM32_FAMILY} NAME_WE)
string(REGEX MATCH "STM32.+" STM32_FAMILY "${STM32_FAMILY}")
string(REPLACE "X" "x" STM32_FAMILY "${STM32_FAMILY}")

foreach(source ${SOURCES})
  get_filename_component(ext ${source} EXT)
  get_filename_component(dir ${source} DIRECTORY)

  if(NOT dir IN_LIST STM32_INCLUDE AND ext STREQUAL ".h")
    list(APPEND STM32_INCLUDE ${dir})
  endif()

  set(dir)
  set(ext)
endforeach()

set(CMAKE_EXECUTABLE_SUFFIX .elf)
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${STM32_INCLUDE})
target_compile_definitions(${PROJECT_NAME} PUBLIC USE_HAL_DRIVER ${STM32_FAMILY})
target_compile_options(${PROJECT_NAME} PUBLIC
  -mcpu=${MCPU} -mthumb -fdata-sections -ffunction-sections -Wall -Wextra -Wpedantic
)
target_link_libraries(${PROJECT_NAME} PRIVATE a c m nosys)
target_link_options(${PROJECT_NAME} PRIVATE
  -Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_NAME}.map,--cref
  -mcpu=${MCPU} -mthumb --specs=nano.specs -T${STM32_LINKER} -u_printf_float
)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.bin
  COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.hex
  COMMAND ${CMAKE_SIZE} --format=berkeley ${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX}
  COMMENT "Building ${PROJECT_NAME}.bin and ${PROJECT_NAME}.hex"
)
add_custom_target(flash
  COMMAND ${OPENOCD_EXECUTABLE} -f ${STM32_OPENOCD} -c init -c halt -c \"program $<TARGET_FILE:${PROJECT_NAME}> verify reset exit\"
)
add_custom_target(reset
  COMMAND ${OPENOCD_EXECUTABLE} -f ${STM32_OPENOCD} -c init -c halt -c reset -c shutdown
)

set(LIBA_CXX 0 CACHE BOOL "Disable C++")
set(LIBA_FLOAT 4 CACHE STRING "floating-point number bytes")
add_subdirectory(liba)
target_compile_options(a PRIVATE
  -mcpu=${MCPU} -mthumb -fdata-sections -ffunction-sections
)
target_link_libraries(${PROJECT_NAME} PRIVATE a)
