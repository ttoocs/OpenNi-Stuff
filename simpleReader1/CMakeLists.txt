cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(oniBS)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/" ${CMAKE_MODULE_PATH})

#find_package(PCL 1.7 REQUIRED)
find_package(OpenNI REQUIRED)
find_package(OpenNI2 REQUIRED)
find_package(OpenCV REQUIRED)

add_definitions(
  -g
  -O3
)

set(
#    CMAKE_RUNTIME_OUTPUT_DIRECTORY    ${CMAKE_HOME_DIRECTORY}/bin
    BUILD_SHARED_LIBS TRUE
)

include_directories(
  ${OPENNI_INCLUDE_DIR}
  ${OPENNI2_INCLUDE_DIRS}
)
#include_directories(${PCL_INCLUDE_DIRS})
#link_directories(  ${PCL_LIBRARY_DIRS})
#add_definitions(${PCL_DEFINITIONS})

add_executable(ni1Reader ni1Reader.cpp)
target_link_libraries(ni1Reader ${OPENNI_LIBRARY} ${OpenCV_LIBS})
