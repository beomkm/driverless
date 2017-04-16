#include <mutex>
#include <string>
#include <thread>
#include <chrono>
#include <iostream>

#include "Serial.hpp"
#include "Control.hpp"

namespace chro = std::chrono;
namespace tthr = std::this_thread;

bool controlFlag = true;
int tempSteer = 0;

void controlHandler()
{
	Control control("/dev/ttyUSB0");
	control.start();
	char alive;

	for(;controlFlag;) {
		control.waitForUpdate();
		alive = control.getAlive();
		system("clear");
		std::cout << control.toString() << std::endl;
		control.setMode(Mode::AUTO);
		control.setSteer(tempSteer);
		control.sendCommand();
	}
	control.end();
}

int main(void)
{
	char command;
	bool loopFlag = true;
	int input;

	std::cout << "Central process on" << std::endl;

	std::thread controlThread(controlHandler);

	for(;loopFlag;) {
		command = getchar();
		switch(command) {
			case 'q':
				loopFlag = false;
				break;
			case 's':
				std::cout << "input steer : ";
				std::cin >> input;
				tempSteer = input;
				break;
		}
	}

	controlFlag = false;

	controlThread.join();
	std::cout << "Exit success" << std::endl;
	exit(0);
}
