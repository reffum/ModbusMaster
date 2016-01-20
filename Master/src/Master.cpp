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
	using namespace std;
	/**
	 * Modbus PDU structures.
	 **/
#ifdef _MSC_VER
#pragma pack(1)
#endif
	struct ExceptionPdu
	{
		uint8_t func;
		uint8_t code;
	};

	struct ReadHoldRequestPdu
	{
		uint8_t func;
		uint16_t addr;
		uint16_t count;
	};

	struct ReadHoldResponcePdu
	{
		uint8_t func;
		uint8_t count;
		uint16_t data[1];
	};

	struct WriteSinglePdu
	{
		uint8_t func;
		uint16_t addr;
		uint16_t value;
	};

#ifdef _MSC_VER
#pragma pack()
#endif
	/**
	 * Modbus function codes.
	 **/
	const uint8_t ReadHoldFuncCode = 3;
	const uint8_t WriteSingleFuncCode = 6;

	/**
	 * Modbus PDU sizes.
	 **/
	const int ReadHoldMaxRegisters = 125;
	const int ReadHoldMaxResponceSize = ReadHoldMaxRegisters * 2 + 2;
	const int ExceptionPduSize = sizeof(ExceptionPdu);


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
		
		if (id == BroadcastID)
		{
			throw std::logic_error("Broadcast ReadHold reques.");
		}

		ReadHoldRequestPdu reqPdu;

		// Store Request PDU
		reqPdu.func = ReadHoldFuncCode;
		reqPdu.addr = _byteswap_ushort(regsStartAddr);
		reqPdu.count = _byteswap_ushort(regsNumber);

		vector<uint8_t> request((uint8_t*)&reqPdu, (uint8_t*)&reqPdu + sizeof(reqPdu));
		vector<uint8_t> responce = SendPduAndReceive(id, request);
		
		/**
		 * Check modbus exception.
		 **/
		if ((responce[0] == (ReadHoldFuncCode | 0x80)) &&
			(responce.size() == ExceptionPduSize))
		{
			ExceptionPdu *erp = (ExceptionPdu*)(responce.data());
			throw ExceptionCode(erp->code);
		}

		/**
		 * Check responce PDU correction
		 **/
		const int expectedRespSize = 2 + regsNumber * 2;
		if (expectedRespSize != responce.size())
		{
			throw InvalidResponcePdu(request, responce);
		}

		ReadHoldResponcePdu *respPdu = (ReadHoldResponcePdu *)responce.data();

		if ((respPdu->func != ReadHoldFuncCode)	||
			(respPdu->count != regsNumber*2))
		{
			throw InvalidResponcePdu(request, responce);
		}

		/**
		 * Copy registers data 
		 **/
		for (int i = 0; i < regsNumber; i++)
		{
			regsValue[i] = _byteswap_ushort(respPdu->data[i]);
		}
	}

	/**
	* Modbus request Write Single
	**/
	void Master::WriteSingle(uint8_t id, uint16_t regAddr, uint16_t regValue)
	{
		WriteSinglePdu reqPdu;
		reqPdu.func = WriteSingleFuncCode;
		reqPdu.addr = _byteswap_ushort(regAddr);
		reqPdu.value = _byteswap_ushort(regValue);

		vector<uint8_t> request((uint8_t*)&reqPdu, (uint8_t*)&reqPdu + sizeof(WriteSinglePdu));
		
		if (id == BroadcastID)
		{
			SendPdu(id, request);
			return;
		}

		vector<uint8_t> responce = SendPduAndReceive(id, request);

		/**
		 * Check modbus exception
		 **/
		if ((responce.size() == ExceptionPduSize) && 
			(responce[0] == (WriteSingleFuncCode | 0x80)))
		{
			ExceptionPdu *ep = (ExceptionPdu*)(responce.data());
			throw ExceptionCode(ep->code);
		}

		/**
		 * Check PDU responce correction.
		 **/
		if (request != responce)
		{
			throw InvalidResponcePdu(request, responce);
		}
	}

}