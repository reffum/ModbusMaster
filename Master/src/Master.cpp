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

	struct WriteSingleDev0RequestPdu
	{
		uint8_t func;
		uint16_t addr;
		uint8_t data;
	};

	struct ReadHoldDev0RequestPdu
	{
		uint8_t func;
		uint8_t count;
		uint16_t addr;
	};

	struct ReadHoldDev0ResponcePdu
	{
		uint8_t func;
		uint8_t count;
		uint8_t data[1];
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
	const uint8_t ReadHoldDev0FuncCode = 0x65;
	const uint8_t WriteSingleDev0FuncCode = 0x64;
	const uint8_t ExceptionFlag = 0x80;

	/**
	 * Modbus PDU sizes.
	 **/

	const int ExceptionPduSize = sizeof(ExceptionPdu);


	Master::Master()
	{
	}


	Master::~Master()
	{
	}

	/**
	* Modbus Write Single in dev0
	**/
	void Master::WriteSingleDev0(uint8_t id, uint16_t regAddr, uint8_t regData)
	{
		_RPTF3(_CRT_WARN, "Master::WriteSingleDev0(%hhX, %hX, %hhX)", id, regAddr, regData);
		
		WriteSingleDev0RequestPdu reqPdu;
		reqPdu.func = WriteSingleDev0FuncCode;
		reqPdu.addr = _byteswap_ushort(regAddr);
		reqPdu.data = regData;

		vector<uint8_t> request((uint8_t*)&reqPdu, (uint8_t*)&reqPdu + sizeof(reqPdu));
#ifdef _DEBUG
		_RPTF0(_CRT_WARN, "Request PDU:");
		for each (uint8_t ch in request)
		{
			_RPT1(_CRT_WARN, "%hhX ", ch);
		}
		_RPT0(_CRT_WARN, "\n");
#endif
		if (id == BroadcastID)
		{
			SendPdu(id, request);
			return;
		}

		vector<uint8_t> responce = SendPduAndReceive(id, request);
#ifdef _DEBUG
		_RPTF0(_CRT_WARN, "Responce:");
		for each (uint8_t ch in responce)
		{
			_RPT1(_CRT_WARN, " %hhX", ch);
		}
		_RPT0(_CRT_WARN, "\n");
#endif

		/**
		* Check modbus exception.
		**/
		if ((responce[0] == (ReadHoldFuncCode | 0x80)) &&
			(responce.size() == ExceptionPduSize))
		{
			ExceptionPdu *erp = (ExceptionPdu*)(responce.data());

			_RPTF1(_CRT_WARN, "Exception code:%hX", erp->code);

			throw ExceptionCode(erp->code);
		}

		/**
		* Check PDU responce correction.
		**/
		if (request != responce)
		{
			throw InvalidResponcePdu(request, responce);
		}

		_RPTF0(_CRT_WARN, "Function complete");

	}

	/**
	* Modbus Read Hold from device 0
	**/
	void Master::ReadHoldDev0(
		uint8_t id,
		uint16_t regStartAddr,
		uint8_t regsNum,
		uint8_t* regsValue
		)
	{
		_RPTF4(_CRT_WARN, "Master::ReadHoldDev0(%hhu, %hX, %hhX, %p)", id, regStartAddr, regsNum, regsValue);

		assert(regsValue);
		
		if (id == BroadcastID)
		{
			throw std::logic_error("Broadcast ReadHold reques.");
		}

		ReadHoldDev0RequestPdu reqPdu;
		reqPdu.func = ReadHoldDev0FuncCode;
		reqPdu.count = regsNum;
		reqPdu.addr = _byteswap_ushort(regStartAddr);

		vector<uint8_t> request((uint8_t*)&reqPdu, (uint8_t*)&reqPdu + sizeof(reqPdu));
#ifdef _DEBUG
		_RPTF0(_CRT_WARN, "Request PDU:");
		for each (uint8_t ch in request)
		{
			_RPT1(_CRT_WARN, "%hhX ", ch);
		}
		_RPT0(_CRT_WARN, "\n");
#endif

		vector<uint8_t> responce = SendPduAndReceive(id, request);

#ifdef _DEBUG
		_RPTF0(_CRT_WARN, "Responce PDU:");
		for each (uint8_t ch in responce)
		{
			_RPT1(_CRT_WARN, "%hhX ", ch);
		}
		_RPT0(_CRT_WARN, "\n");
#endif
		/**
		* Check modbus exception.
		**/
		if ((responce[0] == (ReadHoldFuncCode | 0x80)) &&
			(responce.size() == ExceptionPduSize))
		{
			ExceptionPdu *erp = (ExceptionPdu*)(responce.data());

			_RPTF1(_CRT_WARN, "Exception code:%hX", erp->code);

			throw ExceptionCode(erp->code);
		}

		/**
		* Check responce PDU correction
		**/
		const int expectedRespSize = 2 + regsNum;
		if (expectedRespSize != responce.size())
		{
			_RPTF0(_CRT_WARN, "Responce PDU have incorrect format.");
			throw InvalidResponcePdu(request, responce);
		}

		ReadHoldDev0ResponcePdu *respPdu = (ReadHoldDev0ResponcePdu *)responce.data();
		if (respPdu->func != ReadHoldDev0FuncCode ||
			respPdu->count != regsNum)
		{
			_RPTF0(_CRT_WARN, "Responce PDU have incorrect format.");
			throw InvalidResponcePdu(request, responce);
		}

		/**
		* Copy registers data
		**/
		for (int i = 0; i < regsNum; i++)
		{
			regsValue[i] = respPdu->data[i];
		}

		_RPTF0(_CRT_WARN, "Function complete");
	}

	void Master::ReadHold(
		uint8_t id, 
		uint16_t regsStartAddr, 
		uint16_t regsNumber, 
		uint16_t* regsValue)
	{
		_RPTF4(_CRT_WARN, "Master::ReadHold(%hhu, %hu, %hu, %p)\n", id, regsStartAddr, 
			regsNumber, regsValue);

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

#ifdef _DEBUG
		_RPTF0(_CRT_WARN, "Request PDU:");
		for each (uint8_t ch in request)
		{
			_RPT1(_CRT_WARN, "%hhX ", ch);
		}
		_RPT0(_CRT_WARN, "\n");
#endif

		vector<uint8_t> responce = SendPduAndReceive(id, request);

#ifdef _DEBUG
		_RPTF0(_CRT_WARN, "Responce PDU:");
		for each (uint8_t ch in responce)
		{
			_RPT1(_CRT_WARN, "%hhX ", ch);
		}
		_RPT0(_CRT_WARN, "\n");
#endif
		/**
		 * Check modbus exception.
		 **/
		if ((responce[0] & ExceptionFlag)		&&
			(responce.size() == ExceptionPduSize))
		{
			ExceptionPdu *erp = (ExceptionPdu*)(responce.data());

			_RPTF1(_CRT_WARN, "Exception code:%hX", erp->code);

			throw ExceptionCode(erp->code);
		}

		/**
		 * Check responce PDU correction
		 **/
		const int expectedRespSize = 2 + regsNumber * 2;
		if (expectedRespSize != responce.size())
		{
			_RPTF0(_CRT_WARN, "Responce PDU have incorrect format.");
			throw InvalidResponcePdu(request, responce);
		}

		ReadHoldResponcePdu *respPdu = (ReadHoldResponcePdu *)responce.data();

		if ((respPdu->func != ReadHoldFuncCode)	||
			(respPdu->count != regsNumber*2))
		{
			_RPTF0(_CRT_WARN, "Responce PDU have incorrect format.");
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

		_RPTF3(_CRT_WARN, "Master::WriteSingle(%hhu, %hu, %hu)", id, regAddr, regValue);

		reqPdu.func = WriteSingleFuncCode;
		reqPdu.addr = _byteswap_ushort(regAddr);
		reqPdu.value = _byteswap_ushort(regValue);

		vector<uint8_t> request((uint8_t*)&reqPdu, (uint8_t*)&reqPdu + sizeof(WriteSinglePdu));

#ifdef _DEBUG
		_RPTF0(_CRT_WARN, "Request:");
		for each (uint8_t ch in request)
		{
			_RPT1(_CRT_WARN, " %hhX", ch);
		}
		_RPT0(_CRT_WARN, "\n");
#endif
		
		if (id == BroadcastID)
		{
			SendPdu(id, request);
			return;
		}

		vector<uint8_t> responce = SendPduAndReceive(id, request);

#ifdef _DEBUG
		_RPTF0(_CRT_WARN, "Responce:");
		for each (uint8_t ch in responce)
		{
			_RPT1(_CRT_WARN, " %hhX", ch);
		}
		_RPT0(_CRT_WARN, "\n");
#endif

		/**
		 * Check modbus exception
		 **/
		if ((responce.size() == ExceptionPduSize) && 
			(responce[0] & ExceptionFlag))
		{
			ExceptionPdu *ep = (ExceptionPdu*)(responce.data());

			_RPTF1(_CRT_WARN, "Exception code: %hX", ep->code);

			throw ExceptionCode(ep->code);
		}

		/**
		 * Check PDU responce correction.
		 **/
		if (request != responce)
		{
			_RPT0(_CRT_WARN, "Invalid responce PDU");
			throw InvalidResponcePdu(request, responce);
		}
		_RPTF0(_CRT_WARN, "Function complete");
	}

}