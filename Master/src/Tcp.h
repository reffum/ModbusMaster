/**
 * TCP communication.
 * Author	Oleg Gavrilchenko
 * Email	reffum@bk.ru
 **/
#ifndef _TCP_H_
#define _TCP_H_

#include <cstdint>
#include <winsock2.h>
#include "Master.h"

namespace Modbus
{
	class Tcp : public virtual Master
	{
	public:
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
		* Send FDU to to physical layer without responce
		**/
		void SendFdu(std::vector<uint8_t>);

		/**
		* Receive one FDU symbol from physical layer with timeout.
		**/
		uint8_t ReceiveOneFduSymbol(std::chrono::milliseconds timeout);

	private:
		SOCKET sock;
	};

}

#endif	/*_TCP_H_  */
