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
  pg.printNumCans();
  pg.test_EGreedy(1, 20, 0.01);

  pg.printNumCans();
  return 0;
}
