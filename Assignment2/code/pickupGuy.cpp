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
  mNumActions = 7;
  mNumRunActions = 200;
  mNumGridTypes = 3;

  // Initialize Q-Learning stuff
  mDefaultQ = 0.5;
  mGamma = 0.1;

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

}

/**
 * @function loadMap
 * @brief Load map
 */
void pickupGuy::loadMap( char* _mapFile ) {

  //printf("Loading map... \n");
  std::string fullPath( _mapFile );
  std::fstream wstream( fullPath.c_str(), std::ios::in );
  std::string str;
  mMapFilename = _mapFile;

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

  //printf("...Finished loading map \n");
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
  std::vector<int> actionCount(mNumActions);

  int e = (int) ( _e*100 );
  int p;

  for( int episode = 0; episode < _numEpisodes; ++episode ) {

    // Reload map
    loadMap( mMapFilename );

    totalReward = 0;
    initPosX = rand() % mWidth; 
    initPosY = rand() % mHeight;
    std::fill( actionCount.begin(), actionCount.end(), 0 );

    setCurrentPos( initPosX, initPosY );
    
    for( int i = 0; i < _numPlays; ++i ) {
     // printf("\n [Step %d] Location: %d %d  --", i, mPosX, mPosY );      
      p = rand() % 100;
      updateState();
      
      // Exploration
      if( p <= e ) { 
	action = getRandomAction( mCurrent, 
				  mNorth, mSouth,
				  mEast, mWest );
	//printf(" (Random)" );   
      }
      // Exploitation
      if( p > e ) {
	action = getActionHighestReward( mCurrent, 
					 mNorth, mSouth,
					 mEast, mWest );
      }
      
      // Keep count
      actionCount[action] = actionCount[action] + 1;

      reward = performAction( action );
      totalReward += reward;
    } // end i = _numPlays

    printf("Episode [%d] Start Pos: (%d %d) \n Final reward after %d actions: %d with %d cans left \n", episode, initPosX, initPosY,  _numPlays, totalReward, getNumCans() );
    //printf("* North: %d times * South: %d times \n", actionCount[0], actionCount[1] );
    //printf("* East: %d times * West: %d times \n", actionCount[2], actionCount[3] );
    //printf("* Stay: %d times * Pickup: %d times \n", actionCount[4], actionCount[5] );    

  } // end episode = _numEpisodes


}

///////////////////////// Q-LEARNING ///////////////////////////

/**
 * @function test_QLearning
 */
void pickupGuy::test_QLearning( int _numTrainingEpisodes,
				int _numTrainingPlays,
				float _alpha,
				int _numEpisodes,
				int _numPlays,
				float _epsilon ) {
  
  mQTable.resize(0);
  
  // 1. Initialize Q(s, a) arbitrarily (243x7)
  for( int i = 0; i < mNumStates; ++i ) {
    std::vector<float> row;
    for( int j = 0; j < mNumActions; ++j ) {
      row.push_back( mDefaultQ );
    }
    mQTable.push_back( row );
  }

  // 2. ** TRAIN **
  int a; int action;
  int r; int V; 
  int s_t; int s_t_1;
  int initPosX; int initPosY;

  float q; float maxQ;

  int e = (int) ( _epsilon*100 );
  int p;

  std::vector<int> actionCount(mNumActions);

  // Initialize Qs
  int ic, in, is, ie, iw;
  float ir;

  for( int i = 0; i < mNumStates; ++i ) {
    getStateFromIndex(i, ic, in, is, ie, iw );
    for( int j = 0; j < mNumActions; ++j ) {
      ir = getReward( j, ic, in, is, ie, iw );
      mQTable[i][j] = ir;
    }
  }

  for( int episode = 0; episode < _numTrainingEpisodes; ++episode ) {

    // Reload map
    std::fill( actionCount.begin(), actionCount.end(), 0 );
    loadMap( mMapFilename );
    V = 0;
    initPosX = rand() % mWidth; initPosY = rand() % mHeight;
    setCurrentPos( initPosX, initPosY );
    updateState();
    s_t = getStateIndex( mCurrent, mNorth, mSouth, mEast, mWest );

    for( int i = 0; i < _numTrainingPlays; ++i ) {
      
      p = rand() % 100;
      
      // Exploration
      if( p <= e ) {
	action = getRandomAction( mCurrent, 
				  mNorth, mSouth,
				  mEast, mWest ); 
	a = RANDOM;
      }
      // Exploitation
      if( p > e ) {
	bool isRandom;
	action = getActionHighestReward2( mCurrent, 
					  mNorth, mSouth,
					  mEast, mWest, isRandom );
	if( isRandom ) {
	  a = RANDOM; 
	} else {
	  a = action;
	}
      }
      
      r = performAction( action );
      updateState();
      s_t_1 = getStateIndex( mCurrent, mNorth, mSouth, mEast, mWest );
      
      // Save Q(s,a)
      q = mQTable[s_t][a];
      maxQ = getMaxQ( s_t_1 );
      float factor = _alpha*( (float)r + mGamma*maxQ - q );
      //printf("Factor: %f for action %d and maxQ: %f \n", factor, a, maxQ);
      mQTable[s_t][a] = q + factor;
      actionCount[a] = actionCount[a] + 1;
      s_t = s_t_1;
      V += r;
    } // end i = _numTrainingPlays

    printf("Train Episode [%d] Start Pos: (%d %d) \n Final reward after %d actions: %d with %d cans left \n", episode, initPosX, initPosY,  _numPlays, V, getNumCans() );


  } // end episode = _numTrainingEpisodes

  
  // Check
  bool flag;
  std::vector<int> countActions(mNumActions);
  for( int i = 0; i < mNumActions; ++i ) {
    countActions[i] = 0;
  }

  for( int i = 0; i < mNumStates; ++i ) {
    int b =  getActionFromQTable( i );
    countActions[b] = countActions[b] + 1;
  }
 
  for( int i = 0; i < mNumActions; ++i ) {
    printf("Number of states with action %d: %d \n", i, countActions[i]);
    printf("And number of times this action was checked: %d \n", actionCount[i]);
  }
  

  // 3. USE
  for( int episode = 0; episode < _numEpisodes; ++episode ) {

    // Reload map
    loadMap( mMapFilename );
    V = 0;
    initPosX = rand() % mWidth; 
    initPosY = rand() % mHeight;
    setCurrentPos( initPosX, initPosY );
    updateState();

    for( int i = 0; i < _numPlays; ++i ) {

      s_t = getStateIndex( mCurrent, mNorth, mSouth, mEast, mWest );      
      // Choose action from lookup table
      a = getActionFromQTable( s_t );
      if( a == RANDOM ) {
	a =  getRandomAction( mCurrent, mNorth, mSouth, mEast, mWest );
      }
      r = performAction( a );
      updateState();

      V += r;
    } // end i = _numPlays

    printf("Run Episode [%d] Start Pos: (%d %d) \n Final reward after %d actions: %d with %d cans left \n", episode, initPosX, initPosY,  _numPlays, V, getNumCans() );

  } // end episode = _numEpisodes
  

}

/**
 * @function getActionFromQTable
 */
int pickupGuy::getActionFromQTable( int _state ) {

  int maxQ;
  int maxInd;

  for( int i = 0; i < mNumActions; ++i ) {
    if( i == 0 ) {
      maxQ = mQTable[_state][i];
      maxInd = i;
    }
    else {
      if( mQTable[_state][i] > maxQ ) {
	maxQ = mQTable[_state][i];
	maxInd = i;
      } 
    }
  } // end for

  return maxInd;
}

/**
 * @function getMaxQ
 */
float pickupGuy::getMaxQ( int _state ) {

  float maxQ;

  for( int i = 0; i < mNumActions; ++i ) {
    if( i == 0 ) {
      maxQ = mQTable[_state][i];
    }
    else {
      if( mQTable[_state][i] > maxQ ) {
	maxQ = mQTable[_state][i];
      }
    }
  }

  return maxQ;
}


/**
 * @function getActionHighestReward
 */
int pickupGuy::getActionHighestReward( int _current, 
				       int _north, int _south,
				       int _east, int _west ) {

  if( _current == CAN ) {
    //printAction( PICK_UP );
    return PICK_UP;
  }
  else {
    if( _north == CAN ) {
      //printAction( NORTH );
      return NORTH;
    }
    else if( _south == CAN ) {
      //printAction( SOUTH );
      return SOUTH;
    }
    else if( _east == CAN ) {
      //printAction( EAST );
      return EAST;
    }
    else if( _west == CAN ) {
      //printAction( WEST );
      return WEST;
    }
    else {
      /*return getRandomAction( _current, 
			      _north, _south,
			      _east, _west ); */
      return getRandomActionNoPickup( _current, 
				      _north, _south,
				      _east, _west );
    }
  }

}

/**
 * @function getActionHighestReward2
 */
int pickupGuy::getActionHighestReward2( int _current, 
					int _north, int _south,
					int _east, int _west,
					bool &_isRandom ) {

  if( _current == CAN ) {
    _isRandom = false;
    return PICK_UP;
  }
  else {
    if( _north == CAN ) {
      _isRandom = false;
      return NORTH;
    }
    else if( _south == CAN ) {
      _isRandom = false;
      return SOUTH;
    }
    else if( _east == CAN ) {
      _isRandom = false;
      return EAST;
    }
    else if( _west == CAN ) {
      _isRandom = false;
      return WEST;
    }
    else {
      _isRandom = true;
      /*return getRandomAction( _current, 
			      _north, _south,
			      _east, _west ); */
      return getRandomActionNoPickup( _current, 
				      _north, _south,
				      _east, _west );
    }
  }

}

/**
 * @function getRandomAction
 */
int pickupGuy::getRandomAction( int _current, 
				int _north, int _south,
				int _east, int _west ) {

  // All actions - RANDOM obviously
  int action;
  do {
    action= rand() % (mNumActions - 1);
  } while( getReward( action, _current, 
		      _north, _south,
		      _east, _west ) == BUMP_WALL );
  //printAction( action );
  //printf(" (Random)" );   
  return action;
}

/**
 * @function getRandomActionNoPickup
 */
int pickupGuy::getRandomActionNoPickup( int _current, 
					int _north, int  _south,
					int _east, int _west ) {

  // All actions - PICK UP - RANDOM obviously
  int action;
  do {
    action= rand() % (mNumActions - 2);
  } while( getReward( action, _current, 
		      _north, _south,
		      _east, _west ) == BUMP_WALL );
  //printAction( action );
  //printf(" (Random)" );   
  return action;
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

/////////////// DEBUGGING FUNCTIONS /////////////////////

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
 * @function printNumCans
 */
int pickupGuy::getNumCans() {
  int count = 0;
  for( int i = 0; i < mNumGrids; ++i ) {
    if( mMap[i] == CAN ) {
      count++;
    }
  }

  return count;
}

/**
 * @function printCurrentPos
 */
void pickupGuy::printCurrentPos() {
  printf("Robot current pos: %d %d \n", mPosX, mPosY );
}

/**
 * @function printAction
 */
void pickupGuy::printAction( int _action ) {

  if( _action == NORTH ) { printf(" Action: North   "); }
  else if( _action == SOUTH ) { printf(" Action: South   "); }
  else if( _action == EAST ) { printf(" Action: East    "); }
  else if( _action == WEST ) { printf(" Action: West    "); }
  else if( _action == PICK_UP ) { printf(" Action: Pick up "); }
  else if( _action == STAY ) { printf(" Action: Stay    "); }
  else if( _action == RANDOM ) { printf(" Action: Random  "); }
}
