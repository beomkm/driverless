#include <mutex>
#include <string>
#include <thread>
#include <chrono>
#include <iostream>

#include "Serial.hpp"
#include "Control.hpp"

namespace chro = std::chrono;
namespace tthr = std::this_thread;

int main()
{
	//PCU와 시리얼 연결 설정
	Control control("/dev/ttyUSB0");
	control.start();

	char command;
	bool loopFlag = true;
	int input;

	std::cout << "Central process on" << std::endl;

	for(;loopFlag;) {
		command = getchar();
		switch(command) {
			//프로그램 종료
			case 'q':
				loopFlag = false;
				break;
			//PCU제어 테스트
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
