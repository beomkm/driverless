#include <mutex>
#include <string>
#include <thread>
#include <chrono>
#include <iostream>
#include <iomanip>

#include "Serial.hpp"
#include "Control.hpp"
#include "../common/UDSClient.hpp"
#include "PID.hpp"

namespace chro = std::chrono;
namespace tthr = std::this_thread;

int main()
{

	PID pid;

	//UDSClient client("/tmp/gtserver");
	//client.start();

	//PCU와 시리얼 연결 설정
	Control control("/dev/ttyUSB0");
	control.start();

	char command;
	bool loopFlag = true;
	int input;

	std::cout << "Central process on" << std::endl;

	control.setMode(Mode::AUTO);
	float temp;
	int tempPID;
	for(;loopFlag;) {
		/*temp = client.recvFloat();
		tempPID = pid.process(temp);
		std::cout << std::setw(10) << temp;
		std::cout << std::setw(10) << tempPID << std::endl;

		control.setSteer(tempPID);
		*/

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
			case 'p':
				std::cout << "input power : ";
				std::cin >> input;
				control.setSpeed(input);
				break;
		}
	}


	control.end();
	control.stopThread();

	std::cout << "Exit success" << std::endl;
	return 0;
}
