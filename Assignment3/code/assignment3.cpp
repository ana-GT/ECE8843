/**
 * @file assignment3.cpp
 * @author A. Huaman
 * @date 2012-11-12
 */


#include "assignment3.h"
#include <stdio.h>
#include <fstream>
#include <iostream>

#include <stdlib.h>
#include <time.h>
#include <math.h>


/**
 * @function assignment3
 * @brief Constructor
 */
assignment3::assignment3() {

  // Map dimensions are given: 20x20
  mMapWidth = 20;
  mMapHeight = 20;
  mNumGrids = mMapWidth*mMapHeight;

  mMap = new int[mNumGrids];

  // Visual stuff
  mVGridSize = 20;
  mWindowName = "View Map";

  // PRM Stuff
  mNumNodes = 350;
  mDistThresh = 2.0;

  // Rand
  srand( time(NULL) );
}

/**
 * @function ~assignment3
 * @brief Destructor
 */
assignment3::~assignment3() {

}

/**
 * @function obstaclePadding
 * @brief Pads the obstacles by width x height of the robot
 */
void assignment3::obstaclePadding() {

  int maxPad;
  if( mRobotWidth > mRobotHeight ) { maxPad = mRobotWidth; }
  else { maxPad = mRobotHeight; }

  for( int j = 0; j < mMapHeight; ++j ) {
    for( int i = 0; i < mMapWidth; ++i ) {
      
      // Find obstacles
      if( mMap[ref(i,j)] == OBSTACLE ) {
	// Check straight neighbors
	if( mMap[ref(i, j-1)] == FREE ) { mMap[ref(i,j-1)] = PAD; }
	if( mMap[ref(i+1, j)] == FREE ) { mMap[ref(i+1,j)] = PAD; }
	if( mMap[ref(i, j+1)] == FREE ) { mMap[ref(i,j+1)] = PAD; }
	if( mMap[ref(i-1, j)] == FREE ) { mMap[ref(i-1,j)] = PAD; }

	// Check diagonal neighbors
	if( mMap[ref(i+1, j-1)] == FREE ) { mMap[ref(i+1,j-1)] = PAD; }
	if( mMap[ref(i+1, j+1)] == FREE ) { mMap[ref(i+1,j+1)] = PAD; }
	if( mMap[ref(i-1, j+1)] == FREE ) { mMap[ref(i-1,j+1)] = PAD; }
	if( mMap[ref(i-1, j-1)] == FREE ) { mMap[ref(i-1,j-1)] = PAD; }

      }

    }
  }
}

/**
 * @function planPRM
 */
void assignment3::planPRM() {
  generateRandomNodes();
  getNeighborInfo();
}

/**
 * @function generateRandomNodes
 */
void assignment3::generateRandomNodes() {

  float x; float y;
  int gx; int gy;

  printf( "* Start generate random Nodes \n" );
  for( int i = 0; i < mNumNodes; ++i ) {
    do {
      gx = rand() % mMapWidth;
      gy = rand() % mMapHeight;
    } while( mMap[ref(gx, gy)] != FREE );

    mNodesX.push_back(gx);
    mNodesY.push_back(gy);
  }

  printf("* End generate random Nodes \n");
}

/**
 * @function getNeighborInfo
 */
void assignment3::getNeighborInfo() {
  
  mNeighbors.resize(0);

  for( int i = 0; i < mNumNodes; ++i ) {
    std::vector<int> neighbors;
    for( int j = 0; j < mNumNodes; ++j ) {
      if( nodeDist(i, j) < mDistThresh  && i != j ) {
	neighbors.push_back(j);
      } 
    }
    mNeighbors.push_back( neighbors );
  }
}

/**
 * @function findPath
 */
void assignment3::findPath() {

  // Find nearest nodes to start and goal
  int startNode; int goalNode;
  startNode = getNearestNeighbor( mStartX, mStartY );
  goalNode = getNearestNeighbor( mGoalX, mGoalY );

  printf("Nearest neighbor Start: %d (%d, %d) \n", startNode, mNodesX[startNode],  mNodesY[startNode] );
  printf("Nearest neighbor Goal: %d (%d, %d) \n", goalNode, mNodesX[goalNode],  mNodesY[goalNode] );

  // Now find the shortest path (well not shortest)
  std::vector<int> path;
  path = BFS( startNode, goalNode );
  // Now join the extremes
  mPathX.resize( path.size() + 2 );
  mPathY.resize( path.size() + 2 );
  
  // Put start
  mPathX[0] = mStartX;
  mPathY[0] = mStartY;

  // Put goal
  mPathX[mPathX.size() - 1] = mGoalX;
  mPathY[mPathY.size() - 1] = mGoalY;

  // Add the rest of points
  for( int i = 0; i < path.size(); ++i ) {
    int ind = path[path.size() - 1 - i];
    mPathX[1+i] = mNodesX[ind];
    mPathY[1+i] = mNodesY[ind];
  }

}

/**
 * @function BFS
 */
std::vector<int> assignment3::BFS( int _start, 
				   int _goal ) {
  
  std::vector<int> path;

  mClosedList.resize(0);
  mQueue.resize(0);
  mParents.resize(mNumNodes);

  int node;
  int neighbor;
  bool flag;

  mQueue.push_back( _start );

  printf("Start BFS while \n");
  flag = true;
  while( !mQueue.empty() && flag ) {

    // Get first guy
    node = mQueue.front();
    mQueue.pop_front();

    // Push it to closedList
    mClosedList.push_back(node);

    // Get all its neighbors
    for( int i = 0; i < mNeighbors[node].size(); ++i ) {
      neighbor = mNeighbors[node][i];
      // If not in closed list
      if( !inClosedList(neighbor) && !inQueue(neighbor) ) {
	// Set parent
	mParents[neighbor] = node;
	// Add to FIFO
	mQueue.push_back( neighbor );
	if( neighbor == _goal ) {
	  flag = false;
	  printf("FLAG IS FALSE! \n");
	}
      } // if
    } // for

  } // while
  
  printf("End BFS while \n");

  // Retrieve path
  if( flag == false ) {
    printf("Found goal yay! :D \n");

    node = _goal;
    while( node != _start ) {

      path.push_back(node);
      node = mParents[node];
    }
    path.push_back(node); // start
  }

  return path;
}

/**
 * @function inClosedList
 */
bool assignment3::inClosedList( int _ind ) {
  
  for( int i = 0; i < mClosedList.size(); ++i ) {
    if( mClosedList[i] == _ind ) {
      return true;
    }
  }

  return false;
}

/**
 * @function inQueue
 */
bool assignment3::inQueue( int _ind ) {

  int q;

  for( std::list<int>::iterator it = mQueue.begin(); 
       it != mQueue.end(); 
       it++ ) {
    q = *it;
    if( q == _ind ) {
      return true;
    }
  }

  return false;
}

/**
 * @function getNearestNeighbor
 */
int assignment3::getNearestNeighbor( int _x, int _y ) {

  float minDist = 1000;
  float dist;
  int minInd = -1;


  for( int i = 0; i < mNumNodes; ++i ) {
    dist = nodePointDist( _x, _y, i );
    if( dist < minDist ) {
      minDist = dist;
      minInd = i;
    }
  }

  return minInd;
}

/**
 * @function nodePointDist
 */
float assignment3::nodePointDist( int x, int y, int nodeInd ) {
  
  return sqrt( pow( (x - mNodesX[nodeInd]),2) + 
	       pow( (y - mNodesY[nodeInd]),2) );
}

/**
 * @function nodeDist
 * @brief 
 */
float assignment3::nodeDist( int i, int j ) {
  return sqrt( pow( (mNodesX[i] - mNodesX[j]),2) + 
	       pow( (mNodesY[i] - mNodesY[j]),2) );
}

/**
 * @function loadMap
 */
void assignment3::loadMap( char* _mapFilename ) {

  resetMap();
  std::string fullPath( _mapFilename );
  std::fstream wstream( fullPath.c_str(), std::ios::in );
  std::string str;
  int x, y;
  int width, height;


  while( !wstream.eof() ) {
    wstream >> str;

    // Origin
    if( str == "Origin" ) {
      wstream >> mStartX;
      wstream >> mStartY;
      wstream >> mStartTheta;
      wstream >> mRobotWidth;
      wstream >> mRobotHeight;
    }

    // Goal
    else if( str == "Goal" ){
      wstream >> mGoalX;
      wstream >> mGoalY;
    }
    
    // Obstacle
    else if( str == "Obstacle" ) {
      wstream >> x;
      wstream >> y;
      wstream >> width;
      wstream >> height;
      for( int i = x; i < x + width; ++i ) {
	for( int j = y; j < y + height; ++j ) {
	  mMap[ref(i,j)] = OBSTACLE;
	}
      }

    }
  } // end while

  printf("Done! \n");
}

/**
 * @function printMap
 */
void assignment3::printMap() {
 
  printf( " **** Map **** \n" );
  for( int j = 0; j < mMapHeight; ++j ) {
    for( int i = 0; i < mMapWidth; ++i ) {
      printf( "%d  ", mMap[ref(i,j)] );
    }
    printf("\n");
  }
 
}

/**
 * @function visualizeMap
 */
void assignment3::visualizeMap() {

  // Create map
  mVMap = cv::Mat( mMapHeight*mVGridSize, 
		   mMapWidth*mVGridSize,
		   CV_8UC3 );

  // Draw Free and Obstacle grids
  for( int j = 0; j < mMapHeight; j++ ) {
    for( int i = 0; i < mMapWidth; ++i ) {
      if( mMap[ref(i,j)] == FREE ) {
	drawFreeGrid(i,j);
      }
      else if( mMap[ref(i,j)] == OBSTACLE ) {
	drawObstacleGrid(i,j);
      }
      else if( mMap[ref(i,j)] == PAD ) {
	drawPadGrid(i,j);
      }
    }
  }

  // Draw Start and Goal
  drawStartGrid();
  drawGoalGrid();

  // Draw Nodes
  drawNodes();

  // Draw Neighbors
  drawEdges();
  cv::imwrite( "roadmap.png", mVMap );

  // Draw Path
  drawPath();
  cv::imwrite( "path.png", mVMap );

  // show
  cv::namedWindow( mWindowName, CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO );
  cv::imshow( mWindowName, mVMap );

  // Press a key to keep with the program
  int key;
  while(  true ) {
    key = cv::waitKey(30);
    if( key != -1 ) {
      break;
    }
  }

}

/**
 * @function drawObstacleGrid
 */
void assignment3::drawObstacleGrid( int i, int j ) {
  cv::rectangle( mVMap, 
		 cv::Point( i*mVGridSize, j*mVGridSize ),
		 cv::Point( (i+1)*mVGridSize, (j+1)*mVGridSize ),
		 cv::Scalar( 0, 0, 0 ),
		 -1, 8 );
}

/**
 * @function drawFreeGrid
 */
void assignment3::drawFreeGrid( int i, int j ) {
  cv::rectangle( mVMap, 
		 cv::Point( i*mVGridSize, j*mVGridSize ),
		 cv::Point( (i+1)*mVGridSize, (j+1)*mVGridSize ),
		 cv::Scalar( 255, 255, 255 ),
		 -1, 8 );
}


/**
 * @function drawPadGrid
 */
void assignment3::drawPadGrid( int i, int j ) {
  cv::rectangle( mVMap, 
		 cv::Point( i*mVGridSize, j*mVGridSize ),
		 cv::Point( (i+1)*mVGridSize, (j+1)*mVGridSize ),
		 cv::Scalar( 50, 50, 50 ),
		 -1, 8 );
}

/**
 * @function drawGoalGrid
 */
void assignment3::drawGoalGrid( ) {
  cv::rectangle( mVMap, 
		 cv::Point( mGoalX*mVGridSize, mGoalY*mVGridSize ),
		 cv::Point( (mGoalX+1)*mVGridSize, (mGoalY+1)*mVGridSize ),
		 cv::Scalar( 0, 0, 255 ),
		 -1, 8 );
}

/**
 * @function drawGoalGrid
 */
void assignment3::drawStartGrid( ) {
  cv::rectangle( mVMap, 
		 cv::Point( mStartX*mVGridSize, mStartY*mVGridSize ),
		 cv::Point( (mStartX+1)*mVGridSize, (mStartY+1)*mVGridSize ),
		 cv::Scalar( 0, 255, 0 ),
		 -1, 8 );
}

/**
 * @function drawNodes
 */
void assignment3::drawNodes() {

  for( int i = 0; i < mNodesX.size(); ++i ) {
    cv::circle( mVMap, 
		cv::Point( (mNodesX[i] + 0.5)*mVGridSize, (mNodesY[i] + 0.5 )*mVGridSize ),
		mVGridSize / 4,
		cv::Scalar(255, 0, 0),
		-1, 8 );
  }
}


/**
 * @function drawEdges
 */
void assignment3::drawEdges() {

  for( int i = 0; i < mNodesX.size(); ++i ) {
    for( int j = 0; j < mNeighbors[i].size(); ++j ) {
      cv::line( mVMap, 
		cv::Point( (mNodesX[i] + 0.5)*mVGridSize, 
			   (mNodesY[i] + 0.5)*mVGridSize ), 
		cv::Point( (mNodesX[mNeighbors[i][j]] + 0.5)*mVGridSize, 
			   (mNodesY[mNeighbors[i][j]] + 0.5)*mVGridSize ),
		cv::Scalar(255, 0, 255),
		2, 8 );
    }
  }
}

/**
 * @function drawPath
 */
void assignment3::drawPath() {
  
  for( int i = 0; i < mPathX.size() - 1; ++i ) {
    cv::line( mVMap, 
	      cv::Point( (mPathX[i] + 0.5)*mVGridSize,
			 (mPathY[i] + 0.5)*mVGridSize ),
	      cv::Point( (mPathX[i+1] + 0.5)*mVGridSize,
			 (mPathY[i+1] + 0.5)*mVGridSize ),
	      cv::Scalar( 0, 100, 0),
	      2, 8 );
  }
}

/** 
 * @function printPlanInfo
 */
void assignment3::printPlanInfo() {
  printf("* Start Location: (%d %d) \n", mStartX, mStartY);
  printf("* Goal Location: (%d %d) \n", mGoalX, mGoalY );
  printf("* Robot dimensions: (%d %d) \n", mRobotWidth, mRobotHeight );
}

/**
 * @function resetMap
 */
void assignment3::resetMap() {

  for( int i = 0; i < mMapWidth; ++i ) {
    for( int j = 0; j < mMapHeight; ++j ) {
      mMap[ ref(i,j) ] = FREE;
    }
  }
}

/**
 * @function printPath
 */
void assignment3::printPath() {

  printf("Path from (%d %d) to (%d %d) \n", mStartX, mStartY, mGoalX, mGoalY );
  for( int i = 0; i < mPathX.size(); ++i ) {
    printf("[Waypoint %d] (%d %d) \n", i, mPathX[i], mPathY[i]);
  }
}
