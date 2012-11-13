cmake_minimum_required( VERSION 2.8 )
project( assignment3 )

include_directories( .
  /usr/local/include
  /usr/local/include/opencv
  /usr/include/eigen3
)

link_directories( /usr/local/lib )

set( OpenCV_LIBS opencv_core
  opencv_highgui
  opencv_imgproc )

add_executable( assignment3 assignment3.cpp main.cpp )
target_link_libraries( assignment3 ${OpenCV_LIBS} )