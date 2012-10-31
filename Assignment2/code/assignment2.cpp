/**
 * @file assignment2.cpp
 */

#include "pickupGuy.h"

/**
 * @function main
 */
int main( int argc, char* argv [] ) {

  pickupGuy pg;
 
  if( argc < 2 ) {
    printf("You need to enter the name of a map to load! Exiting \n");
    return -1;
  }
  pg.loadMap( argv[1] );
  pg.printCanLocations();
  pg.getGreedyPolicy();
  pg.testRun();

  return 0;
}
