#ifndef __LIDAR_MANAGER_H__
#define __LIDAR_MANAGER_H__


// includes
#include <cstdio>
#include <iostream>
#include <unistd.h>
#include "LMS1xx.h"


// defines
#define CRITICAL_AREA       3000
#define LIDAR_MAX_DISTANCE  5000
#define DATA_SIZE           541
#define LIDAR_RESOLUTION    5000
#define LIDAR_FREQUENCY     5000
#define READY_FOR_MEASURE   7

// define of LidarManager
class LidarManager
{

// LidarManager value
private:
  // LMS Lidar device
  LMS1xx    _device;
  scanData  _data;
  int       _status;

  void setUpScanCfg();


public:
  LidarManager();
  // set up Link Lidar Deivce
  void setUpLidar();

  bool connect();
  bool disconnect();
  void run();
  void interpolate();
  void trace();
  void update();
  void start();
  void stop();

};



#endif
