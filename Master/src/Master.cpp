/**
* MODBUS Master base functional class
* Author	Oleg Gavrilchenko
* E-mail	reffum@bk.ru
**/
#include <cassert>
#include <cstdlib>
#include "Master.h"

namespace Modbus
{

	/**
	 * Modbus PDU structures.
	 **/
#ifdef _MSC_VER
#pragma pack(1)
#endif
	struct ExceptionResponcePdu
	{
		uint8_t func;
		uint8_t code;
	};

	struct ReadHoldReqPdu
	{
		uint8_t func;
		uint16_t addr;
		uint16_t count;
	};

	struct ReadHoldRespPdu
	{
		uint8_t func;
		uint8_t count;
		uint16_t data[1];
	};

#ifdef _MSC_VER
#pragma pack()
#endif

	/**
	 * Modbus function codes.
	 **/
	const uint8_t ReadHoldFuncCode = 3;

	/**
	 * Modbus PDU sizes.
	 **/
	const int ReadHoldMaxRegisters = 125;
	const int ReadHoldMaxResponceSize = ReadHoldMaxRegisters * 2 + 2;
	const int ExceptionRespPduSize = sizeof(ExceptionResponcePdu);


	Master::Master()
	{
	}


	Master::~Master()
	{
	}

	void Master::ReadHold(
		uint8_t id, 
		uint16_t regsStartAddr, 
		uint16_t regsNumber, 
		uint16_t* regsValue)
	{
		assert(regsValue);
		assert(regsNumber <= ReadHoldMaxRegisters);

		ReadHoldReqPdu reqPdu;

		uint8_t responce[ReadHoldMaxResponceSize];
		int respSize;

		// Store Request PDU
		reqPdu.func = ReadHoldFuncCode;
		reqPdu.addr = _byteswap_ushort(regsStartAddr);
		reqPdu.count = _byteswap_ushort(regsNumber);

		processPdu((uint8_t*)&reqPdu, sizeof(reqPdu), responce, respSize);
		
		/**
		 * Check modbus exception.
		 **/
		if ((responce[0] == (ReadHoldFuncCode | 0x80)) &&
			(respSize == ExceptionRespPduSize))
		{
			ExceptionResponcePdu *erp = (ExceptionResponcePdu*)responce;
			throw ExceptionCode(erp->code);
		}

		/**
		 * Check responce PDU correction
		 **/
		const int expectedRespSize = 2 + regsNumber * 2;
		const ReadHoldRespPdu *respPdu = (ReadHoldRespPdu *)responce;

		if ((expectedRespSize != respSize)		||
			(respPdu->func != ReadHoldFuncCode)	||
			(respPdu->count != regsNumber*2))
		{
			throw InvalidResponcePdu(
				(uint8_t*)&reqPdu, 
				sizeof(reqPdu), 
				(uint8_t*)responce, 
				respSize);
		}

		/**
		 * Copy registers data 
		 **/
		for (int i = 0; i < regsNumber; i++)
		{
			regsValue[i] = _byteswap_ushort(respPdu->data[i]);
		}
	}

}