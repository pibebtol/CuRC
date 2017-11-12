cmake_minimum_required(VERSION 2.8)
project(RayCast)
find_package(OpenCV REQUIRED)

add_executable(RayCast RayCast.cpp)

target_link_libraries(
RayCast
${OpenCV_LIBS}
) 

add_executable(IterRecon IterRecon.cpp)

target_link_libraries(
IterRecon
${OpenCV_LIBS}
) 