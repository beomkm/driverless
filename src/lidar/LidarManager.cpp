#include "LidarManager.h"

LidarManager::LidarManager()
{
  _status = 0;
}

void LidarManager::setUpScanCfg()
{
  scanCfg c = _device.getScanCfg();

	c.angleResolution = LIDAR_RESOLUTION;
	c.scaningFrequency = LIDAR_FREQUENCY;

	_device.setScanCfg(c);

	scanDataCfg cc;
	cc.deviceName = false;
	cc.encoder = 0;
	cc.outputChannel = 3;
	cc.remission = true;
	cc.resolution = 0;
	cc.position = false;
	cc.outputInterval = 1;

	_device.setScanDataCfg(cc);
}

void LidarManager::setUpLidar()
{
  // set up paise 1.
  if( connect() == false ) std::cout << "Connection Fail!" << std::endl;

  // set up paise 2.
  _device.login();

  // set up paise 3.
	stop();

  // set up paise 4.
  setUpScanCfg();

}


bool LidarManager::connect()
{
  _device.connect("192.168.0.1");
  return _device.isConnected();
}

bool LidarManager::disconnect()
{
  _device.disconnect();
  return !(_device.isConnected());
}

void LidarManager::run()
{
  std::cout << "Receive data sample ..." << std::endl;
  _device.getData(_data);
  printf("***********");
  int temp;
  for(int j=0; j<541; j++) {
    temp = _data.dist1[j];

    printf("%d ", temp);
  }

  printf("\n");
}

void LidarManager::interpolate()
{

}

void LidarManager::trace()
{

}

void LidarManager::update()
{

}

void LidarManager::start()
{
  _device.startMeas();

  while (_status != READY_FOR_MEASURE)
	{
		_status = _device.queryStatus();
		std::cout << "status : " << _status << std::endl;
		sleep(1);
	}
  _device.scanContinous(1);
}

void LidarManager::stop()
{
  _device.scanContinous(0);
  _device.stopMeas();
}
