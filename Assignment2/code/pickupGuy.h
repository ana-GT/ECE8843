/**
 * @file pickupGuy.h
 */

#ifndef _PICKUP_GUY_
#define _PICKUP_GUY_

#include <stdio.h>
#include <vector>

/** 7 actions */
enum ACTIONS{
  NORTH = 0,
  SOUTH = 1,
  EAST = 2,
  WEST = 3,
  STAY = 4,
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

  // Q-Learning
  void test_QLearning( int _numTrainingEpisodes,
		       int _numTrainingPlays,
		       float _alpha,
		       int _numEpisodes,
		       int _numPlays,
		       float _epsilon = 0.05 );
  int getActionFromQTable( int _state );
  float getMaxQ( int _state );

  // General stuff
  int getActionHighestReward( int _current, 
			      int _north, int _south,
			      int _east, int _west );

  int getActionHighestReward2( int _current, 
			       int _north, int _south,
			       int _east, int _west,
			       bool &_isRandom ); 

  int getRandomAction( int _current, 
		       int _north, int _south,
		       int _east, int _west );

  int getRandomActionNoPickup( int _current, 
			       int _north, int _south,
			       int _east, int _west );

  int performAction( int _action );
  int getReward( int _action, int _current,
		 int _north, int _south,
		 int _east, int _west );
  

  // Debugging functions
  void printCanLocations();
  void printNumCans();
  int getNumCans();
  void printCurrentPos();
  void printAction( int _action );

  // Inline functions
  inline bool setCurrentPos( int _x, int _y );
  inline bool isValidGrid( int _x, int _y );
  inline int ref( int _x, int _y );
  inline int getStateIndex( int _current,
			    int _north, int _south,
			    int _east, int _west );
  inline int getStateFromIndex( int _state,
				int &_current,
				int &_north, int &_south,
				int &_east, int &_west );

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

  int *mMap; 
  int mNumGrids;
  int mNumStates;
  int mNumActions;
  int mNumRunActions;
  int mNumGridTypes;

  // Q-Learning stuff
  std::vector< std::vector<float> > mQTable;
  float mDefaultQ;
  float mGamma;

  char* mMapFilename;
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

/**
 * @function getStateFromIndex
 */
inline int pickupGuy::getStateFromIndex( int _state,
					 int &_current,
					 int &_north, int &_south,
					 int &_east, int &_west ) {

  int temp;
  _current = _state / 81;
  temp = _state % 81;

  _north = temp / 27;
  temp = temp % 27;

  _south = temp / 9;
  temp = temp % 9;

  _east = temp / 3;
  temp = temp % 3;

  _west = temp;
}				  

#endif /** _PICKUP_GUY_ */
