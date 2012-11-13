/**
 * @file assignment3.cpp
 * @brief Load world file and find a path 
 */

#ifndef _ASSIGNMENT_3_
#define _ASSIGNMENT_3_


#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <string>

#include <list>
#include <vector>


/** Cell State */
enum GRID_TYPE{
  OBSTACLE = 5,
  FREE = 6,
  PAD = 7
};

/**
 * @class assignment3
 */
class assignment3 {

 public:
  assignment3();
  ~assignment3();

  // 1. Grow map
  void obstaclePadding();
  void planPRM();
  void generateRandomNodes();
  void getNeighborInfo();
  void findPath();
  std::vector<int> BFS( int _start, 
			int _goal );
  bool inClosedList( int _ind );
  bool inQueue( int _ind );
  int getNearestNeighbor( int _x, int _y );
  float nodePointDist( int x, int y, int nodeInd );
  float nodeDist( int i, int j );
  void printPath();

  // Map functions
  void loadMap( char* _mapFilename );
  void printMap();
  void printPlanInfo();
  void resetMap();
  void visualizeMap();

  // Drawing utilities
  void drawObstacleGrid( int i, int j );
  void drawFreeGrid( int i, int j );
  void drawPadGrid( int i, int j );
  void drawStartGrid();
  void drawGoalGrid();
  void drawNodes();
  void drawEdges();
  void drawPath();

  // Access Map index utility
  inline int ref( int _w, int _h );

  int mMapWidth;
  int mMapHeight;
  int mNumGrids;

  int mStartX;
  int mStartY;
  double mStartTheta;

  int mGoalX;
  int mGoalY;

  int mRobotWidth;
  int mRobotHeight;

  int mVGridSize;
  cv::Mat mVMap;
  std::string mWindowName;

  // PRM
  int mNumNodes;
  std::vector<int> mNodesX;
  std::vector<int> mNodesY;
  std::vector< std::vector<int> > mNeighbors;
  float mDistThresh;

  std::vector<int> mClosedList;
  std::list<int> mQueue;
  std::vector<int> mParents;

  std::vector<int> mPathX;
  std::vector<int> mPathY;

  int* mMap;

};

/**
 * @function ref
 * @brief Get the reference to (x,y) in the mMap
 */
inline int assignment3::ref( int _w, int _h ) {
  return mMapWidth*_h + _w;
}

#endif /** _ASSIGNMENT_3_ */
