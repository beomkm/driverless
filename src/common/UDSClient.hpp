#pragma once

#include <string>
#include <sys/un.h>

class UDSClinet
{
private:
	int cSock;
	socklen_t cSize;
	struct sockaddr_un sAddr;
	std::string fileName;

public:
	UDSClinet(std::string fileName);
	~UDSClinet();

	int start();
	int close();
	float recvFloat();
};
