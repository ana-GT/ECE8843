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
  pg.test_EGreedy(5, 200, 0.05);

  //pg.printNumCans();

  /*pg.test_QLearning( 100, 2000, 0.01, 
		     5, 200, 
		     0.05 );*/
  
  return 0;
}
