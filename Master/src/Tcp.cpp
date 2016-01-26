/**
* TCP communication.
* Author	Oleg Gavrilchenko
* Email	reffum@bk.ru
**/
#include <crtdbg.h>
#include "Tcp.h"


namespace Modbus
{
	using namespace std;
	using namespace chrono;

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
		_RPTF2(_CRT_WARN, "Tcp::Connect(%s, %hu)", ip, port);

		int iResult;
		//
		// Initialize Winsock 2.2
		//
		WSAData wsaDate;
		if (WSAStartup(MAKEWORD(2, 2), &wsaDate))
		{
			_RPTF0(_CRT_WARN, "WinSock initialization error\n");
			throw ConnectException("WinSock initialization error");
		}

		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == INVALID_SOCKET)
		{
			_RPTF0(_CRT_WARN, "Create socket error");
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

		if (iResult == SOCKET_ERROR)
		{
			_RPTF1(_CRT_WARN, "connect() error with code:%X", WSAGetLastError());

			closesocket(sock);
			WSACleanup();

			throw ConnectException("Connection to TCP slave error.");
		}
	}

	/**
	* Disconnect from Modbus slave
	**/
	void Tcp::Disconnect()
	{
		_RPTF0(_CRT_WARN, "Tcp::Disconnect()");

		shutdown(sock, SD_BOTH);
		closesocket(sock);
		WSACleanup();
	}

	/**
	* Send FDU to to physical layer without responce
	**/
	void Tcp::SendFdu(vector<uint8_t> fdu)
	{
		_RPTF0(_CRT_WARN, "Tcp::SendFdu()");

		int iResult;
		
		do
		{
#ifdef _DEBUG
			_RPTF0(_CRT_WARN, "Try send: ");
			for each (uint8_t ch in fdu)
			{
				_RPT1(_CRT_WARN, " %hhX", ch);
			}
			_RPT0(_CRT_WARN, "\n");
#endif
			iResult = send(sock, (const char*)fdu.data(), fdu.size(), 0);
			if (iResult == SOCKET_ERROR)
			{
				_RPTF1(_CRT_WARN, "send() error with code:%X", WSAGetLastError());
				throw SendException("Send exception.");
			}

			_RPTF1(_CRT_WARN, "send() %d bytes.", iResult);

			fdu.erase(fdu.cbegin(), fdu.cbegin() + iResult);
		} while (!fdu.empty());
	}

	/**
	* Receive one FDU symbol from physical layer with timeout.
	**/
	uint8_t Tcp::ReceiveOneFduSymbol(milliseconds timeout)
	{
		_RPTF1(_CRT_WARN, "Tcp::ReceiveOneFduSymbol(%d)", timeout.count());

		fd_set read_fd_set;
		TIMEVAL tm;
		uint8_t ch;

		FD_ZERO(&read_fd_set);
		FD_SET(sock, &read_fd_set);

		seconds sec = duration_cast<seconds>(timeout);
		microseconds micro = timeout;

		tm.tv_sec = (long)sec.count();
		tm.tv_usec = (long)micro.count();

		int iResult = select(0, &read_fd_set, NULL, NULL, &tm);

		_RPTF1(_CRT_WARN, "Wait complete with result: %d", iResult);

		if (iResult == SOCKET_ERROR)
		{
			_RPTF1(_CRT_WARN, "select() error with code %X", WSAGetLastError());
			throw ReceiveException("Receive exception.");
		}

		if (iResult == 0)
		{
			_RPTF0(_CRT_WARN, "Timeout");
			throw TimeoutException();
		}

		iResult = recv(sock, (char*)&ch, 1, 0);
		if (iResult == SOCKET_ERROR)
		{
			_RPTF1(_CRT_WARN, "recv() error with code %X", WSAGetLastError());
			throw ReceiveException("Receive exception.");
		}

		_RPTF1(_CRT_WARN, "Receive: %hhX", ch);
		return ch;

	}
}