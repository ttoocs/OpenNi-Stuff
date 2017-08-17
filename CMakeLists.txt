cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(ONI_STUFF)

#set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/" ${CMAKE_MODULE_PATH})

option(BuildNi2Only "Only build ni2Recorder" ON)

add_definitions(
#  -g
#  -O2
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/bin)


if(BuildNi2Only)
  add_subdirectory(ni2Recorder)
else()
  add_subdirectory(ni1Recorder)
  add_subdirectory(ni2Recorder)
  add_subdirectory(simpleReader1)
  add_subdirectory(simpleReader2)
  add_subdirectory(converter)
endif()
