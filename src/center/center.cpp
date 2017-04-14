#include <thread>
#include <chrono>
#include <mutex>
#include <iostream>

#include "Serial.hpp"
#include "Control.hpp"

namespace chro = std::chrono;
namespace tthr = std::this_thread;


int main(void)
{
	Control control;
	control.start();

	

	return 0;
}
