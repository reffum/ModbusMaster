/**
* TCP communication.
* Author	Oleg Gavrilchenko
* Email	reffum@bk.ru
**/
#include <crtdbg.h>
#include "Exceptions.h"
#include "Tcp.h"


namespace Modbus
{

	Tcp::Tcp() :sock(INVALID_SOCKET)
	{
	}


	Tcp::~Tcp()
	{
	}

	/**
	* Connect to Modbus slave.
	* Exception: ConnectException.
	**/
	void Tcp::Connect(const char* ip, uint16_t port)
	{
		int iResult;
		//
		// Initialize Winsock 2.2
		//
		WSAData wsaDate;
		if (WSAStartup(MAKEWORD(2, 2), &wsaDate))
		{
			_RPTF0(_CRT_ERROR, "WinSock initialization error\n");
			throw ConnectException("WinSock initialization error");
		}

		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == INVALID_SOCKET)
		{
			_RPTF0(_CRT_ERROR, "Create socket error");
			WSACleanup();
			throw ConnectException("Create socket error.");
		}

		//
		// Set IP address and TCP port
		//
		sockaddr_in sAddr;
		sAddr.sin_family = AF_INET;
		sAddr.sin_port = htons((u_short)port);
		sAddr.sin_addr.S_un.S_addr = inet_addr(ip);

		//
		// Open TCP connection
		//
		iResult = connect(sock, (sockaddr*)&sAddr, sizeof(sAddr));


	}
}