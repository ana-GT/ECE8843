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

///////////////// GREEDY STUFF ///////////////////
/**
 * @function test_EGreedy
 */
void pickupGuy::test_EGreedy( int _numEpisodes,
			      int _numPlays,
			      float _e ) {
  int action;
  int reward;
  int totalReward;
  int initPosX;
  int initPosY;
  int explorationCount;
  int exploitationCount;

  int e = (int) ( _e*100 );
  int p;

  totalReward = 0;
  explorationCount = 0;
  exploitationCount = 0;
  initPosX = 0; initPosY = 0;
  setCurrentPos( initPosX, initPosY );
  printCurrentPos();
  for( int i = 0; i < _numPlays; ++i ) {

    p = rand() % 100;
    updateState();

    // Exploration
    if( p <= e ) {
      printf("Exploration: ");
      explorationCount++;
      action = RANDOM;
      printf("Action: %d \n", action );
    }
    // Exploitation
    if( p > e ) {
      printf("Exploitation: ");
      exploitationCount++;
      action = getActionHighestReward( mCurrent, 
				       mNorth, mSouth,
				       mEast, mWest );
    }
    
    reward = performAction( action );
    totalReward += reward;
    printCurrentPos();
  }

  printf("[test_EGreedy] Final reward after %d actions: %d \n", _numPlays, totalReward );

}

/**
 * @function getActionHighestReward
 */
int pickupGuy::getActionHighestReward( int _current, 
				       int _north, int _south,
				       int _east, int _west ) {

  int maxReward = -1000;
  int maxInd = -1;
  int reward;

  for( int i = 0; i < mNumActions-1; ++i ) {
    reward = getReward( i, _current, 
			_north, _south,
			_east, _west );
    if( reward >= maxReward ) {
      maxReward = reward;
      maxInd = i;
    }
  }
  
  if( maxInd != PICK_UP ) {
      if( _north == CAN ) {
	maxReward = MOVE;
	maxInd = NORTH;
      }
      if( _south == CAN ) {
	maxReward = MOVE;
      maxInd = SOUTH;
      }
      
      if( _east == CAN ) {
	maxReward = MOVE;
      maxInd = EAST;
      }
      
      if( _west == CAN ) {
	maxReward = MOVE;
	maxInd = WEST;
      }
  } 

  if( maxInd == STAY ) {
    if( _east != WALL ) {
      maxInd = EAST;
    }
  }
  
  printf("Action: %d with reward: %d \n", maxInd, maxReward );
  return maxInd;
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
    x = mPosX; y = mPosY -1;
    if( isValidGrid(x,y) ) {
      setCurrentPos(x,y);
      return MOVE;
    } 
    else { 
      return BUMP_WALL;
    }
  }

  // SOUTH
  else if( _action == SOUTH ) {
    x = mPosX; y = mPosY + 1;
    if( isValidGrid(x,y) ) {
      setCurrentPos(x,y);
      return MOVE;
    }    
    else {
      return BUMP_WALL;
    }
  }

  // EAST
  else if( _action == EAST ) {
    x = mPosX + 1; y = mPosY;
    if( isValidGrid(x,y) ) {
      setCurrentPos(x,y);
      return MOVE;
    }    
    else {
      return BUMP_WALL;
    }

  }

  // WEST
  else if( _action == WEST ) {
    x = mPosX - 1; y = mPosY;
    if( isValidGrid(x,y) ) {
      setCurrentPos(x,y);
      return MOVE;
    }    
    else {
      return BUMP_WALL;
    }

  }

  // STAY
  else if( _action == STAY ) {
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
    return performAction( random );
  }

  // PICK_UP
  else if( _action == PICK_UP ) {
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
 * @function getReward
 */
int pickupGuy::getReward( int _action, int _current,
			  int _north, int _south,
			  int _east, int _west ) {

  // NORTH
  if( _action == NORTH ) {
    if( _north == FREE || _north == CAN ) {
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
    return getReward( random, _current,
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
    printf("[getReward] No recognized action \n");
  }
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

/**
 * @function printNumCans
 */
void pickupGuy::printNumCans() {
  int count = 0;
  for( int i = 0; i < mNumGrids; ++i ) {
    if( mMap[i] == CAN ) {
      count++;
    }
  }

  printf("Num cans: %d \n", count);
}

/**
 * @function printCurrentPos
 */
void pickupGuy::printCurrentPos() {
  printf("Robot current pos: %d %d \n", mPosX, mPosY );
}
