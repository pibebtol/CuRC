cmake_minimum_required(VERSION 2.8)
project(CalibratingCamera)
find_package(OpenCV REQUIRED)

add_executable(Callibrate Callibrate.cpp)

target_link_libraries(
Callibrate
${OpenCV_LIBS}
) 

