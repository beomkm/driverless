#pragma once

#include <termios.h>

enum class Newline
{
	GNCR, CRNL, NLCR
};

class Serial
{
private:
	int fd;
	char portPath[64];
	struct termios config;

public:

	Serial(const char *port, int baud, int bits, bool parity,
		Newline newLine = Newline::CRNL);
	~Serial();
	int open();
	int close();
	char readByte();
	int writeData(char *buf, int size);

};
