cmake_minimum_required(VERSION 2.8)
project(ReadDICOM)
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
find_package(OpenCV REQUIRED)

add_executable(ReadDICOM ReadDICOM.cpp)

target_link_libraries(
ReadDICOM
${OpenCV_LIBS}
${ITK_LIBRARIES}
) 
