# CMake for Assignment 2 (ECE 8843)
cmake_minimum_required( VERSION 2.8 )
project( assignment2 )


set( src_files assignment2.cpp pickupGuy.cpp )
set( hdr_files pickupGuy.h )

add_executable( assignment2 ${src_files} )
