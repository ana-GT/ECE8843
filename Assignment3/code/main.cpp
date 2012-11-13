/**
 * @file main.cpp
 */

#include <stdio.h>
#include "assignment3.h"

/**
 * @function main
 */
int main( int argc, char** argv ) {

  assignment3 a3;

  if( argc != 2 ) {
    printf("[ERROR] What the Helen of Troy is this? I need a map! Exiting\n");
    return 1;
  }

  a3.loadMap( argv[1] );
  a3.obstaclePadding();
  a3.planPRM();
  a3.findPath();
  a3.printPath();
  a3.visualizeMap();
  return 0;
}
