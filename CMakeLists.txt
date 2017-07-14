cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(oniBS)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/" ${CMAKE_MODULE_PATH})

#find_package(PCL 1.7 REQUIRED)
find_package(OpenNI REQUIRED)
find_package(OpenNI2 REQUIRED)

add_definitions(
  -g
)

set(
    CMAKE_RUNTIME_OUTPUT_DIRECTORY    ${CMAKE_HOME_DIRECTORY}/bin
    BUILD_SHARED_LIBS TRUE
)


include_directories(
  ${OPENNI_INCLUDE_DIR}
  ${OPENNI2_INCLUDE_DIRS}
)
#include_directories(${PCL_INCLUDE_DIRS})
#link_directories(  ${PCL_LIBRARY_DIRS})
#add_definitions(${PCL_DEFINITIONS})

add_library (depth SHARED Depth.cpp)
target_link_libraries (depth ${OPENNI2_LIBRARY})

add_library (oni2input SHARED ExportedDepth.cpp Registration.cpp)
target_link_libraries(oni2input depth)
