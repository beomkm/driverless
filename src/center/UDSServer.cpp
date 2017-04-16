#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/un.h>

#include "UDSServer.hpp"

UDSServer::UDSServer(std::string filename)
{
    this->fileName = fileName;
}

UDSServer::~UDSServer()
{
}

UDSServer::start()
{
    access(fileName.c_str(), F_OK);
    unlink(fileName.c_str());
    sSock = socket(PF_FILE, SOCK_STREAM, 0);
    memset(&sAddr, 0, sizeof(sAddr));
    sAddr.sun_family = AF_UNIX;
    strcpy(sAddr.sun_path, fileName.c_str());
    bind(sSock, (struct sockaddr*)&sAddr, sizeof(sAddr));
    listen(sSock, 5);

    cSize = sizeof(cAddr);
    cSock = accept(sSock, (struct sockaddr*)&cAddr, &cSize);
    close(cSock);
    close(sSock);
}

UDSServer::close()
{

}
