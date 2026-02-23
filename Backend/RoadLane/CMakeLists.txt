cmake_minimum_required(VERSION 3.10)

project(RoadLaneDetection)

set(CMAKE_CXX_STANDARD 17)

set(OpenCV_DIR "C:/opencv/opencv/build")


find_package(OpenCV REQUIRED)

add_executable(RoadLaneDetection
    src/main.cpp
    src/LaneDetector.cpp
)

target_link_libraries(RoadLaneDetection ${OpenCV_LIBS})

