#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <ctime>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include "Serial.hpp"

Serial::Serial(char *port, int baud, int size, bool parity,
				Newline newline=Newline::CRNL)
{
	strncpy(portPath, port, 64);
	memset(&config, 0, sizeof(struct termios));

	int baudBit;
	int sizeBit;
	int parityBit;
	int lineBit;

	switch(baud) {
		case 9600:
			baudBit = B9600;
			break;
		case 19200:
			baudBit = B19200;
			break;
		case 38400:
			baudBit = B38400;
			break;
		case 57600:
			baudBit = B57600;
			break;
		case 115200:
			baudBit = B115200;
			break;
		default:
			perror("Unsupported baudrate");
			exit(1);
	}

	switch(size) {
		case 5:
			sizeBit = CS5;
			break;
		case 6:
			sizeBit = CS6;
			break;
		case 7:
			sizeBit = CS7;
			break;
		case 8:
			sizeBit = CS8;
			break;
		default:
			perror("Unsupported bitsize");
			exit(1);
	}

	if(parity) {
		parityBit = INPCK | PARMRK;
	}
	else {
		parityBit = IGNPAR;
	}

	switch(newline) {
		case Newline::GNCR:
			lineBit = IGNCR;
			break;
		case Newline::CRNL:
			lineBit = ICRNL;
			break;
		case Newline::NLCR:
			lineBit = INLCR;
			break;
		case
	}

	config.c_cflag = baudBit | sizeBit | CLOCAL | CREAD;
	config.c_iflag = parityBit | lineBit;
	//not use output option
	config.c_oflag = 0;
	//not use signal bits (raw)
	config.c_lflag = ~(ICANON | ECHO | ECHOE | ISIG);
	config.c_cc[VTIME] = 0;
	//block until receive 1 byte
	config.c_cc[VMIN] = 1;

}

Serial::~Serial()
{
	close();
}

int Serial::open()
{
	fd = open(portPath, O_RDWR | O_NOCTTY | O_SYNC);
	if(fd == -1) {
		perror("Failed to open port.");
		exit(1);
	}

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &config);

	return 0;
}

int Serial::close()
{
	return close(fd);
}

char Serial::readByte()
{
	char
	return read(fd, buf, 1);
}

Serial::writeData(char *buf, int size)
{
	return write(fd, buf, size);
}
