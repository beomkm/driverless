#include <cstring>
#include <string>

#include "Control.hpp"
#include "Serial.hpp"

Control::Control(std::string devPath)
:serial(devPath.c_str(), 115200, 8, false)
{
	sArr[0] = 'S';
	sArr[1] = 'T';
	sArr[2] = 'X';
	sArr[11] = 0; //alive
	sArr[12] = 0x0D;
	sArr[13] = 0x0A;
	setMode(Mode::MANUAL);
	setEstop(false);
	setGear(Gear::FORWARD);
	setSpeed(0);
	setSteer(0);
	setBraking(0);

}
Control::~Control()
{
	end();
}

int Control::start()
{
	serial.open();
	return 0;
}

int Control::end()
{
	serial.close();
	return 0;
}

int Control::waitForUpdate()
{
	char magic;
	char temp[13];
	int flag = 1;

	for(;flag;) {
		magic = serial.readByte();
		if(magic != 'S')
			continue;

		magic = serial.readByte();
		if(magic != 'T')
			continue;

		magic = serial.readByte();
		if(magic != 'X')
			continue;

		for(int i=0; i<13; i++) {
			temp[i] = serial.readByte();
		}

		magic = serial.readByte();
		if(magic != 0x0D)
			continue;

		magic = serial.readByte();
		if(magic != 0x0A)
			continue;

		flag = 0;
	}

	memcpy(rArr, temp, 13);

	return 0;
}

int Control::sendCommand()
{
	sArr[11] += 1;
	return serial.writeData(sArr, 14);
}

std::string Control::toString()
{
	std::string ret;
	Mode mode = getMode();
	bool estop = getEstop();
	Gear gear = getGear();
	int speed = getSpeed();
	int steer = getSteer();
	int braking = getBraking();
	int encoder = getEncoder();

	ret += "AorM : ";
	if(mode == Mode::AUTO) 	ret += "Auto\n";
	else 					ret += "Manual\n";

	ret += "ESTOP : ";
	if(estop) 	ret += "On\n";
	else 		ret += "Off\n";

	ret += "GEAR : ";
	if(gear == Gear::FORWARD)		ret += "forward drive\n";
	else if(gear == Gear::NEUTRAL) 	ret += "neutral\n";
	else							ret += "backward drive\n";

	ret += "SPEED : " + std::to_string(speed) + "\n";
	ret += "STEER : " + std::to_string(steer) + "\n";
	ret += "BRAKE : " + std::to_string(braking) + "\n";
	ret += "ENC : "   + std::to_string(encoder) + "\n";

	return ret;
}

Mode Control::getMode()
{
	if(rArr[0] == 0)
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
	if(rArr[1] == 0)
		return false;
	else
		return true;
}

int Control::setEstop(bool estop)
{
	if(estop == false)
		sArr[4] = 0;
	else
		sArr[4] = 1;

	return 0;
}

Gear Control::getGear()
{
	if(rArr[2] == 0)
		return Gear::FORWARD;
	else if(rArr[2] == 1)
		return Gear::NEUTRAL;
	else
		return Gear::BACKWARD;
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
	return (rArr[3]&0xFF) | (rArr[4]<<8&0xFF00);
}
int Control::setSpeed(int speed)
{
	sArr[6] = (speed&0xFF00)>>8;
	sArr[7] = speed&0xFF;

	return 0;
}

int Control::getSteer()
{
	return (rArr[5]&0xFF) | rArr[6]<<8;
}

int Control::setSteer(int steer)
{
	sArr[8] = (steer&0xFF00)>>8;
	sArr[9] = steer&0xFF;

	return 0;
}

int Control::getBraking()
{
	return rArr[7];
}

int Control::setBraking(int braking)
{

	sArr[10] = braking;

	return 0;
}

int Control::getEncoder()
{
	return (rArr[8]&0xFF) | (rArr[9]<<8&0xFF00) |
			(rArr[10]<<16&0xFF0000) | (rArr[11]<<24&0xFF000000);
}

char Control::getAlive()
{
	return rArr[12];
}
