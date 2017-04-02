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
	if(rArr[3] == 0)
		return Mode::MANUAL;
	else
		return Mode::AUTO;
}

int Control::setMode(Mode mode)
{
	if(mode == Mode::MANUAL)
		wArr[3] = 0;
	else
		wArr[3] = 1;
}

bool Control::getEstop()
{
	if(rArr[4] == 0)
		return false;
	else
		return true;
}

int Control::setEstop(bool estop)
{
	if(estop == 0)
		wArr[4] = 0;
	else
		wArr[4] = 1;
}

Gear Control::getGear()
{
	if(rArr[5] == 0)
		return Gear::FORWARD;
	else if(rArr[5] == 1)
		return GEAR::NEUTRAL;
	else
		return GEAR::BACKWARD;
}

int Control::setGear(Gear gear)
{
	if(gear == Gear::FORWARD)
		wArr[5] = 0;
	else if(gear == Gear::NEUTRAL)
		wArr[5] = 1;
	else
		wArr[5] = 2;
}

int Control::getSpeed()
{
	return (rArr[6]&0xFF) | (rArr[7]<<8&0xFF00);
}
int Control::setSpeed(int speed)
{
	wArr[6] = speed&0xFF00;
	wArr[7] = speed&0xFF;
}

int Control::getSteer()
{
	return (rArr[8]&0xFF) | rArr[9]<<8;
}

int Control::setSteer(int steer)
{
	wArr[8] = speed&0xFF00;
	wArr[9] = speed&0xFF;
}

int Control::getBraking()
{
	return rArr[10];
}

int Control::setBraking(int breaking)
{
	wArr[10] = braking;
}

int Control::getEncoder()
{
	return (rArr[11]&0xFF) | (rArr[12]<<8&0xFF00) |
			(rArr[13]<<16&0xFF0000) | (rArr[14]<<24&0xFF000000);
}
