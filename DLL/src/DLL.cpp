/**
* Modbus Master DLL
* Oleg Gavrilchenko
* reffum@bk.ru
**/
#include <cassert>
#include "ModbusMaster.h"
#include "ModbusMasterAsciiTCP.h"

using namespace Modbus;

/*************************************************************************
 * Private variables 
 ************************************************************************/
static ModbusMasterAsciiTCP mbMaster;


/**
* Open TCP modbus connection.
* ip	-	ip address
* port	-	TCP port
* Return:	MODBUS_MASTER_SUCCESS
*			MODBUS_MASTER_TCP_ERROR
**/
LIBSPEC int ModbusTCP_Open(const char* ip, int port)
{
	_RPTF2(_CRT_WARN, "ModbusTCP_Open(%s, %d)", ip, port);
	try
	{
		mbMaster.Connect(ip, port);
	}
	catch (ConnectException e)
	{
		_RPTF3(_CRT_WARN, "ModbusTCP_Open(%s, %hu) error with message:%s", ip, port, e.what());
		return MODBUS_MASTER_TCP_ERROR;
	}

	return MODBUS_MASTER_SUCCESS;
}

/**
* Close TCP modbus connection
* Return: MODBUS_MASTER_SUCCESS
**/
LIBSPEC int ModbusTCP_Close()
{
	_RPTF0(_CRT_WARN, "ModbusTCP_Close()");

	mbMaster.Disconnect();
	return MODBUS_MASTER_SUCCESS;
}

/**
* Write single regiser.
* id		Slave ID
* addr		register address
* value	register value
* Return:	MODBUS_MASTER_SUCCESS
*			MODBUS_MASTER_TCP_ERROR
*			MODBUS_MASTER_TIMEOUT_ERROR
*			< 0 Modbus exception with code by absolute value.
**/
LIBSPEC int PUT(uint8_t id, uint16_t addr, uint16_t value)
{
	_RPTF3(_CRT_WARN, "PUT(%hhd, %hX, %hX)", id, addr, value);

	try
	{
		if (id == 0)
			mbMaster.WriteSingleDev0(id, addr, (uint8_t)value);
		else
			mbMaster.WriteSingle(id, addr, value);

		_RPTF0(_CRT_WARN, "Function complete");
		return MODBUS_MASTER_SUCCESS;
	}
	catch (ExceptionCode e)
	{
		_RPTF1(_CRT_WARN, "Exception code: %X", e.code);
		return -(e.code);
	}
	catch (SendException e)
	{
		_RPTF1(_CRT_WARN, "Send exception: %s", e.what());
		return MODBUS_MASTER_TCP_ERROR;
	}
	catch (ReceiveException e)
	{
		_RPTF1(_CRT_WARN, "Receive exception: %s", e.what());
		return MODBUS_MASTER_TCP_ERROR;
	}
	catch (InvalidResponcePdu e)
	{
		_RPTF0(_CRT_WARN, "Invalid responce PDU");
#ifdef _DEBUG
		_RPT0(_CRT_WARN, "Request PDU: ");
		for each (uint8_t b in e.reqPdu)
		{
			_RPT1(_CRT_WARN, "%hhX", b);
		}
		_RPT0(_CRT_WARN, "\n");

		_RPT0(_CRT_WARN, "Responce PDU: ");
		for each (uint8_t b in e.respPdu)
		{
			_RPT1(_CRT_WARN, "%hhX", b);
		}
		_RPT0(_CRT_WARN, "\n");
#endif
		return MODBUS_MASTER_RESPONCE_ERROR;
	}
	catch (InvalidResponceFdu e)
	{
		_RPTF0(_CRT_WARN, "Invalid responce FDU");
#ifdef _DEBUG
		_RPT0(_CRT_WARN, "Request FDU: ");
		for each (uint8_t b in e.reqFdu)
		{
			_RPT1(_CRT_WARN, "%hhX", b);
		}
		_RPT0(_CRT_WARN, "\n");

		_RPT0(_CRT_WARN, "Responce FDU: ");
		for each (uint8_t b in e.respFdu)
		{
			_RPT1(_CRT_WARN, "%hhX", b);
		}
		_RPT0(_CRT_WARN, "\n");
#endif
		return MODBUS_MASTER_RESPONCE_ERROR;
	}
	catch (TimeoutException)
	{
		_RPTF0(_CRT_WARN, "Timeout");
		return MODBUS_MASTER_TIMEOUT_ERROR;
	}
}

/**
* Read hold registers
* id		Slave ID
* addr		Registers start address
* count	count of read registers
* buffer	data buffer for registers value
* Return:	MODBUS_MASTER_SUCCESS
*			MODBUS_MASTER_TCP_ERROR
*			MODBUS_MASTER_TIMEOUT_ERROR
*			< 0 - Modbus exception with code by absolute value.
**/
LIBSPEC int TCP_ReadHold(uint8_t id, uint16_t addr, uint16_t count, uint16_t* buffer)
{
	static uint8_t byteBuffer[Master::ReadHoldMaxRegisters];

	_RPTF4(_CRT_WARN, "TCP_ReadHold(%hhu, %hX, %hX, %p)", id, addr, count, buffer);
	assert(buffer);

	if (count > Master::ReadHoldMaxRegisters)
	{
		return MODBUS_MASTER_INVALID_ARGUMENT;
	}

	try{
		if (id > 0)
			mbMaster.ReadHold(id, addr, count, buffer);
		else
		{
			/**
			 * For device 0 data width is 1 byte and store at hight byte of buffer
			 **/
			mbMaster.ReadHoldDev0(id, addr, (uint8_t)count, byteBuffer);

			for (int i = 0; i < count; i++)
				buffer[i] = (byteBuffer[i] << 8);
		}

		_RPTF0(_CRT_WARN, "Function complete");

		return MODBUS_MASTER_SUCCESS;
	}
	catch (ExceptionCode e)
	{
		_RPTF1(_CRT_WARN, "Exception code: %X", e.code);
		return -(e.code);
	}
	catch (SendException e)
	{
		_RPTF1(_CRT_WARN, "Send exception: %s", e.what());
		return MODBUS_MASTER_TCP_ERROR;
	}
	catch (ReceiveException e)
	{
		_RPTF1(_CRT_WARN, "Receive exception: %s", e.what());
		return MODBUS_MASTER_TCP_ERROR;
	}
	catch (InvalidResponcePdu e)
	{
		_RPTF0(_CRT_WARN, "Invalid responce PDU");
#ifdef _DEBUG
		_RPT0(_CRT_WARN, "Request PDU: ");
		for each (uint8_t b in e.reqPdu)
		{
			_RPT1(_CRT_WARN, "%hhX", b);
		}
		_RPT0(_CRT_WARN, "\n");

		_RPT0(_CRT_WARN, "Responce PDU: ");
		for each (uint8_t b in e.respPdu)
		{
			_RPT1(_CRT_WARN, "%hhX", b);
		}
		_RPT0(_CRT_WARN, "\n");
#endif
		return MODBUS_MASTER_RESPONCE_ERROR;
	}
	catch (InvalidResponceFdu e)
	{
		_RPTF0(_CRT_WARN, "Invalid responce FDU");
#ifdef _DEBUG
		_RPT0(_CRT_WARN, "Request FDU: ");
		for each (uint8_t b in e.reqFdu)
		{
			_RPT1(_CRT_WARN, "%hhX", b);
		}
		_RPT0(_CRT_WARN, "\n");

		_RPT0(_CRT_WARN, "Responce FDU: ");
		for each (uint8_t b in e.respFdu)
		{
			_RPT1(_CRT_WARN, "%hhX", b);
		}
		_RPT0(_CRT_WARN, "\n");
#endif
		return MODBUS_MASTER_RESPONCE_ERROR;
	}
	catch (TimeoutException)
	{
		_RPTF0(_CRT_WARN, "Timeout");
		return MODBUS_MASTER_TIMEOUT_ERROR;
	}
}

/**
* Read 1 register
* id		Slave ID
* addr		Registers start address
* value	Register value
* Return:	MODBUS_MASTER_SUCCESS
*			MODBUS_MASTER_TCP_ERROR
*			MODBUS_MASTER_TIMEOUT_ERROR
*			< 0 - Modbus exception with code by absolute value.
**/
LIBSPEC int GET(uint8_t id, uint16_t addr, uint16_t& value)
{
	_RPTF0(_CRT_WARN, "GET()");
	uint16_t data;
	
	int iResult = TCP_ReadHold(id, addr, 1, &data);
	value = data;
	return iResult;
}