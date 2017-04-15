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
	Serial serial;

public:
	Control(std::string devPath);
	~Control();

	int start();
	int end();

	int waitForUpdate();
	int sendCommand();

	std::string toString();

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

	char getAlive();

};
