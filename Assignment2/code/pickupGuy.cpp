/**
 * @file pickupGuy.cpp
 */
#include "pickupGuy.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * @function pickupGuy
 * @brief Constructor
 */
pickupGuy::pickupGuy() {

  mNumGrids = mWidth*mHeight;

  mMap = new int[mNumGrids]; 
  for( int i = 0; i < mNumGrids; ++i ) {
    mMap[i] = FREE;
  }

  mNumStates = 243;
  mPolicy = new int[mNumStates];
  for( int i = 0; i < mNumStates; ++i ) {
    mPolicy[i] = RANDOM;
  }

  mNumActions = 7;
  mNumRunActions = 200;
  mNumGridTypes = 3;

  // Initialize random seed
  srand ( time(NULL) );
}

/**
 * @function pickupGuy
 * @brief Destructor
 */
pickupGuy::~pickupGuy() {

  if( mMap != NULL ) {
    delete [] mMap;
  }

  if( mPolicy != NULL ) {
    delete [] mPolicy;
  }
}

/**
 * @function loadMap
 * @brief Load map
 */
void pickupGuy::loadMap( char* _mapFile ) {

  printf("Loading map... \n");
  std::string fullPath( _mapFile );
  std::fstream wstream( fullPath.c_str(), std::ios::in );
  std::string str;

  while( !wstream.eof() ) {
    int x, y;
    wstream >> x;
    wstream >> y;
    // Fill
    if( isValidGrid(x,y) ) {
      mMap[ ref(x,y) ] = CAN;
    }
    else {
      printf("What the frack is this data? No valid grid! \n");
    }
  
  }

  printf("...Finished loading map \n");
}

/**
 * @function updateState
 */
void pickupGuy::updateState() {

  int x, y;

  // Current
  x = mPosX; y = mPosY;
  if( !isValidGrid( x, y ) ) {
    mCurrent = WALL;
  } else {
    mCurrent = mMap[ref(x,y)];
  }

  // North
  x = mPosX; y = mPosY - 1;
  if( !isValidGrid( x, y ) ) {
    mNorth = WALL;
  } else {
    mNorth = mMap[ref(x,y)];
  }

  // South
  x = mPosX; y = mPosY + 1;
  if( !isValidGrid( x, y ) ) {
    mSouth = WALL;
  } else {
    mSouth = mMap[ref(x,y)];
  }

  // East
  x = mPosX + 1; y = mPosY;
  if( !isValidGrid( x, y ) ) {
    mEast = WALL;
  } else {
    mEast = mMap[ref(x,y)];
  }

  // West
  x = mPosX - 1; y = mPosY;
  if( !isValidGrid( x, y ) ) {
    mWest = WALL;
  } else {
    mWest = mMap[ref(x,y)];
  }
}

/**
 * @function getGreedyPolicy
 */
void pickupGuy::getGreedyPolicy() {

  int cost;
  int maxCost;
  int maxInd;
  printf("Get greedy policy \n");
  for( int c = 0; c < mNumGridTypes; ++c ) {
    for( int n = 0; n < mNumGridTypes; n++ ) {
      for( int s = 0; s < mNumGridTypes; ++s ) {
	for( int e = 0; e < mNumGridTypes; ++e ) {
	  for( int w = 0; w < mNumGridTypes; ++w ) {
	    
	    maxCost = -1000;
	    maxInd = -1000;
	    for( int i = 0; i < mNumActions; ++i ) {
	      
	      cost = evaluateAction( i, c, n, s, e, w );
	      if( cost > maxCost ) {
		maxCost = cost;
		maxInd = i;
	      }
	    }

	    mPolicy[ getStateIndex(c,n,s,e,w) ] = maxInd;

	  } // w
	} // e
      } // s
    } // n
  } // c
}

/**
 * @function getAction
 */
int pickupGuy::getAction( int _current,
			  int _north,
			  int _south,
			  int _east,
			  int _west ) {

  int stateIndex = getStateIndex( _current, 
				  _north, _south, 
				  _east, _west );
  return mPolicy[stateIndex];
}

/**
 * @function performAction
 */
int pickupGuy::performAction( int _action ) {

  int x; int y;

  // NORTH
  if( _action == NORTH ) {
    printf("Going north! \n");
    x = mPosX; y = mPosY -1;
    if( isValidGrid(x,y) ) {
      mPosX = x; 
      mPosY = y;
      return MOVE;
    } 
    else { 
      return BUMP_WALL;
    }
  }

  // SOUTH
  else if( _action == SOUTH ) {
    printf("Going south! \n");
    x = mPosX; y = mPosY + 1;
    if( isValidGrid(x,y) ) {
      mPosX = x; 
      mPosY = y;
      return MOVE;
    }    
    else {
      return BUMP_WALL;
    }
  }

  // EAST
  else if( _action == EAST ) {
    printf("Going east! \n");
    x = mPosX + 1; y = mPosY;
    if( isValidGrid(x,y) ) {
      mPosX = x; 
      mPosY = y;
      return MOVE;
    }    
    else {
      return BUMP_WALL;
    }

  }

  // WEST
  else if( _action == WEST ) {
    printf("Going west! \n");
    x = mPosX - 1; y = mPosY;
    if( isValidGrid(x,y) ) {
      mPosX = x; 
      mPosY = y;
      return MOVE;
    }    
    else {
      return BUMP_WALL;
    }

  }

  // STAY
  else if( _action == STAY ) {
    printf("Staying put! \n");
    x = mPosX; y = mPosY;

    if( isValidGrid(x,y) ) {
      return MOVE;
    }
    else {
      return BUMP_WALL; // cost anyway even if on the same place
    }
  }

  // RANDOM
  else if( _action == RANDOM ) {
    int random = rand()% (mNumActions - 1);
    printf("Random call--  %d \n", random );
    return performAction( random );
  }

  // PICK_UP
  else if( _action == PICK_UP ) {
    printf("Pickup! \n");
    x = mPosX; y = mPosY;

    if( mCurrent == CAN ) {
      // Grid is now free
      mMap[ref(x,y)] = FREE;
      return PICK_CAN;
    }
    else if( mCurrent == FREE ) {
      return PICK_NO_CAN;
    }
    else if ( mCurrent == WALL ){
      return BUMP_WALL;
    }
  }

  else {
    printf("[performAction] No recognized action \n");
  }
}


/**
 * @function evaluateAction
 */
int pickupGuy::evaluateAction( int _action, int _current,
			       int _north, int _south,
			       int _east, int _west ) {

  // NORTH
  if( _action == NORTH ) {
    if( _north == FREE || _north == CAN) {
      return MOVE;
    } 
    else { 
      return BUMP_WALL;
    }
  }

  // SOUTH
  else if( _action == SOUTH ) {
    if( _south == FREE || _south == CAN ) {
      return MOVE;
    }
    else {
      return BUMP_WALL;
    }
  }

  // EAST
  else if( _action == EAST ) {
    if( _east == FREE || _east == CAN ) {
      return MOVE;
    }
    else {
      return BUMP_WALL;
    }
  }

  // WEST
  else if( _action == WEST ) {
    if( _west == FREE || _west == CAN ) {
      return MOVE;
    }
    else {
      return BUMP_WALL;
    }
  }

  // STAY
  else if( _action == STAY ) {
    if( _current == FREE || _current == CAN ) {
      return MOVE;
    }
    else {
      return BUMP_WALL;
    }
  }

  // RANDOM
  else if( _action == RANDOM ) {
    int random = rand()% (mNumActions - 1);
    return evaluateAction( random, _current,
			   _north, _south,
			   _east, _west );
  }

  // PICK_UP
  else if( _action == PICK_UP ) {
    if( _current == CAN ) {
      return PICK_CAN;
    }
    else if( _current == FREE ) {
      return PICK_NO_CAN;
    }
    else {
      return BUMP_WALL;
    }
  }

  else {
    printf("[evaluateAction] No recognized action \n");
  }
}


/**
 * @function testRun
 */
void pickupGuy::testRun() {
  printf("test Run \n");
  int action;
  int cost;
  int totalCost;

  // Initialize position
  int initPosX; int initPosY;
  initPosX = 0;
  initPosY = 0;
  totalCost = 0;

  setCurrentPos( initPosX, initPosY );

  for( int i = 0; i < mNumRunActions; ++i ) {
    updateState();
    action = getAction( mCurrent, 
			mNorth, mSouth, 
			mEast, mWest );
    cost = performAction(action);
    totalCost += cost;
    printf("[%d] Position: %d %d - recent cost: %d total cost so far: %d \n", i, mPosX, 
	   mPosY, cost, totalCost );
  }

  printf("Final cost after %d runs: %d \n", mNumRunActions, totalCost);
}

/**
 * @function printCanLocations
 */
void pickupGuy::printCanLocations() {

  printf( "\n Print Can Locations Info \n" );

  int num = 0;
  for( int i = 0; i < mWidth; ++i ) {
    for( int j = 0; j < mHeight; ++j ) { 

      if( mMap[ref(i,j)] == CAN ) {
	printf("(%d) [%d %d]: Can! \n", num, i, j );
	num++;
      }

    }
  }
}
