cmake_minimum_required(VERSION 2.8)
project(ICP)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++0x")
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
find_package(OpenCV REQUIRED)

add_executable(PerformICP PerformICP.cpp)

target_link_libraries(
PerformICP
${OpenCV_LIBS}
${VTK_LIBRARIES}
) 

add_executable(TransformMesh TransformMesh.cpp)

target_link_libraries(
TransformMesh
${OpenCV_LIBS}
${VTK_LIBRARIES}
) 
