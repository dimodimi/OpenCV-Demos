cmake_minimum_required(VERSION 2.8)
project( vision_proj )
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( camera_test src/camera_test.cpp )
target_link_libraries( camera_test ${OpenCV_LIBS} )

add_executable( harris_detector src/harris.cpp )
target_link_libraries( harris_detector ${OpenCV_LIBS} )

add_executable( motion_detector src/motion_detector.cpp )
target_link_libraries( motion_detector ${OpenCV_LIBS} )
