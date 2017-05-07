#pragma once


class PID
{
private:
	int cSock;
	socklen_t cSize;
	struct sockaddr_un sAddr;
	std::string fileName;

public:
	UDSClient(std::string fileName);
	~UDSClient();

	int start();
	int close();
	float recvFloat();
};
