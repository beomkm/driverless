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

bool pidPrintFlag = false;

int cmdHandler();

Control control("/dev/ttyUSB0");
PID pid;

int main()
{

	UDSClient client("/tmp/gtserver");
	client.start();

	//PCU와 시리얼 연결 설정

	control.start();


	bool loopFlag = true;


	std::thread cmdThread(cmdHandler);

	std::cout << "Central process on" << std::endl;

	control.setMode(Mode::AUTO);
	//control.setSpeed(15);
	float temp;
	int tempPID;
	for(;loopFlag;) {
		temp = client.recvFloat();
		tthr::sleep_for(chro::milliseconds(10));
		temp = 0;
		if(control.getMode() == Mode::AUTO) {
			tempPID = pid.process(temp);
		}
		else {
			pid.init();
			if(!control.isBusy()) 
				control.setSteer(0);

		}
		if(pidPrintFlag) {
			std::cout << std::setw(10) << temp;
			std::cout << std::setw(10) << tempPID << std::endl;
		}

		if(!control.isBusy()) 
			control.setSteer(-tempPID);

	}


	control.end();
	control.stopThread();
	cmdThread.join();

	std::cout << "Exit success" << std::endl;
	return 0;
}


int cmdHandler()
{
	char command;
	int input;
	while(1) {
		command = getchar();

		switch(command) {
			//프로그램 종료
			case 'q':
				exit(0);
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
			case '1':
				control.addAction(Gear::FORWARD, 0, 0, 40);
				control.addAction(Gear::FORWARD, -2000,30 , 240);
				control.addAction(Gear::FORWARD, -2000,12 , 240);
				control.addAction(Gear::FORWARD, 0, 0, 40);
				break;
			case '2':
				control.addAction2(Gear::FORWARD, 2000, 10, 100);
				control.addAction(Gear::FORWARD, 0, 0, 1);
				break;
			case 'f':
				std::cout << "1:state 2:pidprint ";
				std::cin >> input;
				switch(input) {
				case 1:
					control.printFlag = !control.printFlag;
					break;
				case 2:
					pidPrintFlag = !pidPrintFlag;
					break;
				}
				break;
			case 'c':
				std::cout << "1:Kp 2:Ki 3:Kd 4:coef 5:cycletime 6:alph ";
				std::cin >> input;
				float ivalue;
				switch(input) {
				case 1:
					std::cout << "current : " << pid.Kp  << std::endl;
					std::cout << "input Kp" << std::endl;
					std::cin >> ivalue;
					pid.Kp = ivalue;
					break;
				case 2:
					std::cout << "current : " << pid.Ki  << std::endl;
					std::cout << "input Ki" << std::endl;
					std::cin >> ivalue;
					pid.Ki = ivalue;
					break;
				case 3:
					std::cout << "current : " << pid.Kd  << std::endl;
					std::cout << "input Kd" << std::endl;
					std::cin >> ivalue;
					pid.Kd = ivalue;
					break;
				case 4:
					std::cout << "current : " << pid.coef  << std::endl;
					std::cout << "input coef" << std::endl;
					std::cin >> ivalue;
					pid.coef = ivalue;
					break;
				case 5:
					std::cout << "current : " << pid.cycle_time  << std::endl;
					std::cout << "input cycletime" << std::endl;
					std::cin >> ivalue;
					pid.cycle_time = ivalue;
					break;
				case 6:
					std::cout << "current : " << pid.alpha  << std::endl;
					std::cout << "input alpha" << std::endl;
					std::cin >> ivalue;
					pid.alpha = ivalue;
					break;
				}
				break;
				
			default:
				break;
		}
	}

	return 0;
}
