cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(oniBS)

find_package(PCL 1.7 REQUIRED)
#find_package(OpenNI REQUIRED)
#find_package(OpenNI2 REQUIRED)

set(
    CMAKE_RUNTIME_OUTPUT_DIRECTORY    ${CMAKE_HOME_DIRECTORY}/bin
    BUILD_SHARED_LIBS TRUE
)



include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

add_library (oni2input ExportedDepth.cpp Registration.cpp Depth.cpp)
#target_link_libraries (pcdTransfrom ${PCL_LIBRARIES})

