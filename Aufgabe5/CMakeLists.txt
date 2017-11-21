cmake_minimum_required(VERSION 2.8)
project(ShapeDetection)
find_package(OpenCV REQUIRED)

add_executable(ShapeDetection ShapeDetection.cpp)

target_link_libraries(
ShapeDetection
${OpenCV_LIBS}
) 


