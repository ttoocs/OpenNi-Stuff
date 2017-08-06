cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(cameraProps)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/" ${CMAKE_MODULE_PATH})

#find_package(PCL 1.7 REQUIRED)
#find_package(OpenNI2 REQUIRED)
#find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem serialization)

add_definitions(
  -g
  -O4
)


include_directories(
#    ${OPENNI2_INCLUDE_DIRS}
)
#include_directories(${PCL_INCLUDE_DIRS})
#link_directories(  ${PCL_LIBRARY_DIRS})
#add_definitions(${PCL_DEFINITIONS})


#add_library(cam_props SHARED props.hpp)
#target_link_libraries(cam_props ${Boost_LIBRARIES})

add_executable(tester test.cpp)
target_link_libraries(tester ${Boost_LIBRARIES})
