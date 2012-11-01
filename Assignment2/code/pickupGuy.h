/**
 * @file pickupGuy.h
 */

#ifndef _PICKUP_GUY_
#define _PICKUP_GUY_

#include <stdio.h>

/** 7 actions */
enum ACTIONS{
  STAY = 0,
  NORTH = 1,
  SOUTH = 2,
  EAST = 3,
  WEST = 4,
  PICK_UP = 5,
  RANDOM = 6
};

/** PENALTIES */
enum PENALTIES {
  PICK_CAN = 10,
  PICK_NO_CAN = -2,
  MOVE = -1,
  BUMP_WALL = -5
};

/** GRID TYPE */
enum GRID_TYPE {
  FREE = 0,
  CAN = 1,
  WALL = 2
};

/**
 * @class pickupGuy
 */
class pickupGuy {
 public:

  // Functions
  pickupGuy();
  ~pickupGuy();
  void loadMap( char* _mapFile );
  void updateState();

  // Greedy behavior
  void test_EGreedy( int _numEpisodes,
		     int _numPlays,
		     float _e );
  int getActionHighestReward( int _current, 
			      int _north, int _south,
			      int _east, int _west );

  // Policy
  int getAction( int _current,
		 int _north,
		 int _south,
		 int _east,
		 int _west );
  int performAction( int _action );
  int getReward( int _action, int _current,
		 int _north, int _south,
		 int _east, int _west );
  

  // Debugging functions
  void printCanLocations();
  void printNumCans();
  void printCurrentPos();

  // Inline functions
  inline bool setCurrentPos( int _x, int _y );
  inline bool isValidGrid( int _x, int _y );
  inline int ref( int _x, int _y );
  inline int getStateIndex( int _current,
			    int _north, int _south,
			    int _east, int _west );

  void testRun();

  // Member variables

  // Geometric position
  int mPosX; int mPosY;

  // States (CAN, WALL or FREE)
  int mCurrent;
  int mNorth;
  int mSouth;
  int mEast;
  int mWest;
  

  static const int mWidth = 10;
  static const int mHeight = 10;
  int *mPolicy;
  int *mMap; 
  int mNumGrids;
  int mNumStates;
  int mNumActions;
  int mNumRunActions;
  int mNumGridTypes;
};

/**
 * @function ref
 */
inline int pickupGuy::ref( int _x, int _y ) {
  return mWidth*_y + _x; 
}

/**
 * @function isValidGrid
 */
inline bool pickupGuy::isValidGrid( int _x, int _y ) {

  if( _x < 0 || _x >= mWidth || _y < 0 || _y >= mHeight ) {
    return false;
  }
  else {
    return true;
  }
}

/**
 * @function setCurrentPos
 */
inline bool pickupGuy::setCurrentPos( int _x, int _y ) {

  mPosX = _x;
  mPosY = _y;
}

/**
 * @function getStateIndex
 */
inline int pickupGuy::getStateIndex( int _current,
				     int _north, int _south,
				     int _east, int _west ) {

  return 81*_current + 27*_north + 9*_south + 3*_east + _west;
}
				  

#endif /** _PICKUP_GUY_ */
