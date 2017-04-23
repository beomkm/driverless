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


}

int main(void)
{
	Control control("/dev/ttyUSB0");
	control.start();

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
				control.setSteer(input);
				break;
		}
	}

	control.end();
	control.stopThread();

	std::cout << "Exit success" << std::endl;
	return 0;
}
