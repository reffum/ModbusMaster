/**
 * MODBUS Master base functional class
 * Author	Oleg Gavrilchenko
 * E-mail	reffum@bk.ru
 **/
#ifndef _MASTER_H_
#define _MASTER_H_

#include <cstdint>
#include <vector>
#include <exception>

namespace Modbus
{
	/**
	 * Exceptions
	 **/
	class ExceptionCode
	{
	public:
		ExceptionCode(uint8_t c) :code(c){}
		uint8_t code;
	};

	class ConnectException : public std::exception
	{
	public:
		ConnectException(const char *);
		ConnectException(const std::string&);
	};

	class InvalidResponcePdu
	{
	public:
		InvalidResponcePdu(std::vector<uint8_t> req, std::vector<uint8_t> resp)
			:reqPdu(req), respPdu(resp){}

		std::vector<uint8_t> reqPdu;
		std::vector<uint8_t> respPdu;
	};

	class InvalidResponceFdu
	{
	public:
		InvalidResponceFdu(std::vector<uint8_t> req, std::vector<uint8_t> resp)
			:reqPdu(req), respPdu(resp){}

		std::vector<uint8_t> reqPdu;
		std::vector<uint8_t> respPdu;
	};

	class Master
	{
	public:
		/**
		 * Constants
		 **/
		const int BroadcastID = 255;

		Master();
		~Master();

		/**
		* Modbus request Read Hold.
		**/
		void ReadHold(uint8_t id, uint16_t regsStartAddr, uint16_t regsNumber, uint16_t* regsValue);

		/**
		* Modbus request Write Single.
		**/
		void WriteSingle(uint8_t id, uint16_t regAddr, uint16_t regValue);


		/**
		 * Send PDU, receive responce and return it in respPdu.
		 **/
		virtual std::vector<uint8_t> SendPduAndReceive(uint8_t id, std::vector<uint8_t> requestPdu) = 0;

		/**
		 * Send PDU and do not wait responce.
		 **/
		virtual void SendPdu(uint8_t id, std::vector<uint8_t> requestPdu) = 0;


		/**
		 * Send FDU to to physical layer without responce
		 **/
		virtual void SendFdu(std::vector<uint8_t>) = 0;
	};
}

#endif	/* _MASTER_H_ */
