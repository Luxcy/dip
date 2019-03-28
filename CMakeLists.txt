cmake_minimum_required(VERSION 3.12)
project(DIP)

# OpenCV
find_package(OpenCV)
message(STATUS "OpenCV found (${OpenCV_CONFIG_PATH})")

set(IMAGE_FILES
    mat.h
    global.h
    mat.hpp
    operator.hpp
        base.hpp)

add_executable(DIP main.cpp ${IMAGE_FILES})
target_link_libraries(DIP ${OpenCV_LIBS})

#cmake_minimum_required(VERSION 3.12)
#project(DisplayImage)
#find_package(OpenCV REQUIRED)
#add_executable(DisplayImage DisplayImage.cpp)
#target_link_libraries( DisplayImage ${OpenCV_LIBS})