#pragma once

#include <string>
#include <sys/un.h>

class UDSServer
{
private:
	int sSock;
	int cSock;
	int cSize;
	struct sockaddr_un sAddr;
	struct sockaddr_un cAddr;
	std::string fileName;

public:
	UDSServer(std::string fileName);
	~UDSServer();

	int start();
	int close();
};
