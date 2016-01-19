/**
 * TCP communication.
 * Author	Oleg Gavrilchenko
 * Email	reffum@bk.ru
 **/
#ifndef _TCP_H_
#define _TCP_H_

#include <cstdint>
#include <winsock.h>

namespace Modbus
{
	class Tcp
	{
	public:
		/**
		 * Constants.
		 **/
		static const int NO_TIMEOUT = -1;


		Tcp();
		~Tcp();

		/**
		 * Connect to Modbus slave.
		 * Exception: ConnectException.
		 **/
		void Connect(const char* ip, uint16_t port);

		/**
		 * Disconnect from Modbus slave
		 **/
		void Disconnect();

		/**
		 * Send data to Modbus slave
		 * Exception: SendException.
		 **/
		void Send(uint8_t* data, int len);

		/**
		 * Receive data from Modbus slave
		 * Exception: ReceiveException.
		 **/
		void Receive(uint8_t* data, int len, const int timeout = NO_TIMEOUT);

	private:
		SOCKET sock;
	};

}

#endif	/*_TCP_H_  */
