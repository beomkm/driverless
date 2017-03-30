#include "Control.hpp"


Control::Control()
{
}
Control::~Control()
{
}

int Control::waitForUpdate();

Mode Control::getMode()
{
	if(rArr[0] == 0)
		return Mode::MANUAL;
	else
		return Mode::AUTO;
}

int Control::setMode(Mode mode);

bool Control::getEstop()
{
	if(rArr[1] == 0)
		return false;
	else
		return true;
}

int Control::setEstop(int estop);

Gear Control::getGear()
{
	if(rArr[2] == 0)
		return Gear::FORWARD;
	else if(rArr[2] == 1)
		return GEAR::NEUTRAL;
	else
		return GEAR::BACKWARD;
}

int Control::setGear(Gear gear);

int Control::getSpeed()
{
	rArr[3] | rArr[4] 
}
int Control::setSpeed(int speed);

int Control::getSteer();
int Control::setSteer(int steer);

int Control::getBraking();
int Control::setBraking(int breaking);

int Control::getEncoder();


