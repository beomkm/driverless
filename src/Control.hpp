#pragma once

#include <string>
#include "Serial.hpp"

enum class Mode {
	AUTO, MANUAL
};

enum class Gear {
	FORWARD, NEUTRAL, BACKWARD
};


class Control
{
private:
	char sArr[14]; //sending packet
	char rArr[13]; //receiving packet
	std::string devicePath = "/dev/ttyUSB0";
	Serial serial;

public:
	Control();
	~Control();

	int waitForUpdate();
	int sendCommand();

	Mode getMode();
	int setMode(Mode mode);

	bool getEstop();
	int setEstop(bool estop);

	Gear getGear();
	int setGear(Gear gear);

	int getSpeed();
	int setSpeed(int speed);

	int getSteer();
	int setSteer(int steer);

	int getBraking();
	int setBraking(int braking);

	int getEncoder();

};
