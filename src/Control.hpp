#pragma once

enum class Mode {
	AUTO, MANUAL
};

enum class Gear {
	FORWOARD, NEUTRAL, BACKWARD
};


class ControlERP
{
private:
	char sArr[14]; //sending packet
	char rArr[18]; //receiving packet

public:
	Control();
	~Control();

	int waitForUpdate();

	Mode getMode();
	int setMode(Mode mode);

	bool getEstop();
	int setEstop(int estop);

	Gear getGear();
	int setGear(Gear gear);

	int getSpeed();
	int setSpeed(int speed);

	int getSteer();
	int setSteer(int steer);

	int getBraking();
	int setBraking(int breaking);

	int getEncoder();

};

