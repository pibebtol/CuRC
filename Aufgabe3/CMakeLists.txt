cmake_minimum_required(VERSION 2.8)
project(ImageProcessing)
find_package(OpenCV REQUIRED)

add_executable(ImageProcessing ImageProcessing.cpp)

target_link_libraries(
ImageProcessing
${OpenCV_LIBS}
) 


