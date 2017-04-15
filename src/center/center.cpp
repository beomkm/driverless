#include <thread>
#include <chrono>
#include <mutex>
#include <iostream>
#include <string>
#include <unistd.h>

#include "Serial.hpp"
#include "Control.hpp"

namespace chro = std::chrono;
namespace tthr = std::this_thread;

bool controlFlag = true;

void controlHandler()
{
	Control control("/dev/ttyUSB0");
	control.start();
	char alive;

	for(;controlFlag;) {
		control.waitForUpdate();
		alive = control.getAlive();
	}
	control.end();

	
}


int main(void)
{
	char command;
	bool loopFlag = true;

	std::cout << "Central process on" << std::endl;

	std::thread controlThread(controlHandler);

	for(;loopFlag;) {
		command = getchar();
		switch(command) {
			case 'q':
				loopFlag = false;
				break;
		}
	}

	controlFlag = false;

	controlThread.join();
	std::cout << "Exit success" << std::endl;
	exit(0);
}
