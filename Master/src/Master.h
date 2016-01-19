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
		InvalidResponcePdu(uint8_t* req, int reqSize,
			uint8_t* resp, int respSize)
		{
			for (int i = 0; i < reqSize; i++)
				reqPdu.push_back(req[i]);
			for (int i = 0; i < respSize; i++)
				respPdu.push_back(resp[i]);
		}

		std::vector<uint8_t> reqPdu;
		std::vector<uint8_t> respPdu;
	};

	class InvalidResponceFdu
	{
	public:
		InvalidResponceFdu(uint8_t* req, int reqSize,
			uint8_t* resp, int respSize)
		{
			for (int i = 0; i < reqSize; i++)
				reqPdu.push_back(req[i]);
			for (int i = 0; i < respSize; i++)
				respPdu.push_back(resp[i]);
		}

		std::vector<uint8_t> reqPdu;
		std::vector<uint8_t> respPdu;
	};


	class Master
	{
	public:
		Master();
		~Master();


		/**
		 * Form FDU from PDU, send it, receive responce and return it in respPdu.
		 **/
		virtual void SendPdu(
			uint8_t id,
			uint8_t* reqPdu,
			int reqPduSize,
			void* respPdu,
			int &respPduSize
			) = 0;

		virtual void SendRequestAndReceiveResponce(
			uint8_t* req,
			int reqSize,
			uint8_t* resp,
			int &respSize
			) = 0;


		/**
		* Modbus request Read Hold.
		**/
		void ReadHold(uint8_t id, uint16_t regsStartAddr, uint16_t regsNumber, uint16_t* regsValue);

		/**
		 * Modbus request Write Single
		 **/
		void WriteSingle(uint8_t id, uint16_t regAddr, uint16_t regValue);
	};
}

#endif	/* _MASTER_H_ */
