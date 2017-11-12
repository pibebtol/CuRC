cmake_minimum_required(VERSION 2.8)
project(RegionGrowing)
find_package(OpenCV REQUIRED)

add_executable(RegionGrowing RegionGrowing.cpp)

target_link_libraries(
RegionGrowing
${OpenCV_LIBS}
) 


