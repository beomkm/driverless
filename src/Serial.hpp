#pragma once

enum class Newline
{
	GNCR, CRNL, NLCR
};
	
class Serial
{
private:
	int fd;
	char portPath[64];

public:
	const int GNCR = IGNCR;
	const int CRNL = ICRNL;
	const int NLCR = INLCR;

	Serial(char *port, int baud, int bit, bool parity, int newLine);
	~Serial();
	int open();
	int close();
	int readData(char *buf);
	int writeData(char *buf, int size);
		
};

