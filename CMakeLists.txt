cmake_minimum_required(VERSION 2.8)
project( vision_proj )

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

find_package(PkgConfig REQUIRED)
pkg_search_module(Tesseract REQUIRED tesseract)
pkg_search_module(Leptonica REQUIRED lept)

include_directories( ${Tesseract_INCLUDE_DIRS} )

include_directories( ${Leptonica_INCLUDE_DIRS} )

add_executable( canny src/canny.cpp )
target_link_libraries( canny ${OpenCV_LIBS} )

add_executable( harris_detector src/harris.cpp )
target_link_libraries( harris_detector ${OpenCV_LIBS} )

add_executable( motion_detector src/motion_detector.cpp )
target_link_libraries( motion_detector ${OpenCV_LIBS} )

add_executable( ocr src/ocr_tesseract.cpp )
target_link_libraries( ocr ${OpenCV_LIBS} ${Tesseract_LIBRARIES} ${Leptonica_LIBRARIES} )
