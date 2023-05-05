# .rst:
# FindOpenOCD
# -----------
#
# Find openocd executable. https://openocd.org
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ``OPENOCD_FOUND``
#
# ``OPENOCD_EXECUTABLE``
#
# ``OPENOCD_VERSION``
#
include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)
find_program(OPENOCD_EXECUTABLE openocd DOC "Open On-Chip Debugger")
mark_as_advanced(OPENOCD_EXECUTABLE)

if(EXISTS "${OPENOCD_EXECUTABLE}")
  execute_process(COMMAND ${OPENOCD_EXECUTABLE} --version ERROR_VARIABLE OPENOCD_VERSION)
  string(REGEX REPLACE "[^0-9]+([^\n ]+).*" "\\1" OPENOCD_VERSION "${OPENOCD_VERSION}")
endif()

find_package_handle_standard_args(OpenOCD
  FOUND_VAR
    OPENOCD_FOUND
  REQUIRED_VARS
    OPENOCD_EXECUTABLE
  VERSION_VAR
    OPENOCD_VERSION
)
