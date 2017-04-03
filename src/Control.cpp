#include "Control.hpp"
#include "Serial.hpp"

Control::Control()
{
	serial = Serial("/dev/ttyUSB0", 115200, 8, false);
	serial.open();
}
Control::~Control()
{
	serial.close();
}

int Control::waitForUpdate()
{

	serial.readByte(rArr);

	char magic;
	char temp[13];
	int flag = 1;


	for(;flag;) {
		read(fd, &magic, 1);
		if (magic != 'S')
			continue;

		read(fd, &magic, 1);
		if (magic != 'T')
			continue;

		read(fd, &magic, 1);
		if (magic != 'X')
			continue;

		for(int i=0; i<13; i++) {
			read(fd, &temp[i], 1);
		}

		read(fd, &magic, 1);
		if (magic != 0x0D)
			continue;

		read(fd, &magic, 1);
		if (magic != 0x0A)
			continue;

		flag = 0;
	}
}

int Control::sendCommand()
{
	return serial.writeData(sArr, 14);
}

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
		sArr[3] = 0;
	else
		sArr[3] = 1;

	return 0;
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
		sArr[4] = 0;
	else
		sArr[4] = 1;

	return 0;
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
		sArr[5] = 0;
	else if(gear == Gear::NEUTRAL)
		sArr[5] = 1;
	else
		sArr[5] = 2;

	return 0;
}

int Control::getSpeed()
{
	return (rArr[6]&0xFF) | (rArr[7]<<8&0xFF00);
}
int Control::setSpeed(int speed)
{
	sArr[6] = speed&0xFF00;
	sArr[7] = speed&0xFF;

	return 0;
}

int Control::getSteer()
{
	return (rArr[8]&0xFF) | rArr[9]<<8;
}

int Control::setSteer(int steer)
{
	sArr[8] = speed&0xFF00;
	sArr[9] = speed&0xFF;

	return 0;
}

int Control::getBraking()
{
	return rArr[10];
}

int Control::setBraking(int breaking)
{
	sArr[10] = braking;

	return 0;
}

int Control::getEncoder()
{
	return (rArr[11]&0xFF) | (rArr[12]<<8&0xFF00) |
			(rArr[13]<<16&0xFF0000) | (rArr[14]<<24&0xFF000000);
}
