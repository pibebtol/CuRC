cmake_minimum_required(VERSION 2.8)
project(Instrument_Rekcognition)
find_package(OpenCV REQUIRED)

add_executable(RecInstruments RecInstruments.cpp)

target_link_libraries(
RecInstruments
${OpenCV_LIBS}
) 

