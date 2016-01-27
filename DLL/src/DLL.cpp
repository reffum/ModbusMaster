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
* Constants
************************************************************************/
const int MaxTimeoutMs = 10000;
const int MinTimeoutMs = 100;

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
	_RPTF2(_CRT_WARN, "ModbusTCP_Open(%s, %d)\n", ip, port);
	try
	{
		mbMaster.Connect(ip, port);
	}
	catch (ConnectException e)
	{
		_RPTF3(_CRT_WARN, "ModbusTCP_Open(%s, %hu) error with message:%s\n", ip, port, e.what());
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
	_RPTF0(_CRT_WARN, "ModbusTCP_Close()\n");

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
	_RPTF3(_CRT_WARN, "PUT(%hhd, %hX, %hX)\n", id, addr, value);

	try
	{
		if (id == 0)
			mbMaster.WriteSingleDev0(id, addr, (uint8_t)value);
		else
			mbMaster.WriteSingle(id, addr, value);

		_RPTF0(_CRT_WARN, "Function complete\n");
		return MODBUS_MASTER_SUCCESS;
	}
	catch (ExceptionCode e)
	{
		_RPTF1(_CRT_WARN, "Exception code: %X\n", e.code);
		return -(e.code);
	}
	catch (SendException e)
	{
		_RPTF1(_CRT_WARN, "Send exception: %s\n", e.what());
		return MODBUS_MASTER_TCP_ERROR;
	}
	catch (ReceiveException e)
	{
		_RPTF1(_CRT_WARN, "Receive exception: %s\n", e.what());
		return MODBUS_MASTER_TCP_ERROR;
	}
	catch (InvalidResponcePdu e)
	{
		_RPTF0(_CRT_WARN, "Invalid responce PDU\n");
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
		_RPTF0(_CRT_WARN, "Invalid responce FDU\n");
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
		_RPTF0(_CRT_WARN, "Timeout\n");
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

	_RPTF4(_CRT_WARN, "TCP_ReadHold(%hhu, %hX, %hX, %p)\n", id, addr, count, buffer);
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

		_RPTF0(_CRT_WARN, "Function complete\n");

		return MODBUS_MASTER_SUCCESS;
	}
	catch (ExceptionCode e)
	{
		_RPTF1(_CRT_WARN, "Exception code: %X\n", e.code);
		return -(e.code);
	}
	catch (SendException e)
	{
		_RPTF1(_CRT_WARN, "Send exception: %s\n", e.what());
		return MODBUS_MASTER_TCP_ERROR;
	}
	catch (ReceiveException e)
	{
		_RPTF1(_CRT_WARN, "Receive exception: %s\n", e.what());
		return MODBUS_MASTER_TCP_ERROR;
	}
	catch (InvalidResponcePdu e)
	{
		_RPTF0(_CRT_WARN, "Invalid responce PDU\n");
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
		_RPTF0(_CRT_WARN, "Invalid responce FDU\n");
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
		_RPTF0(_CRT_WARN, "Timeout\n");
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
	_RPTF0(_CRT_WARN, "GET()\n");
	uint16_t data;
	
	int iResult = TCP_ReadHold(id, addr, 1, &data);
	value = data;
	return iResult;
}

/**
* Set Modbus timeout
* Timeout	timeout in ms(100ms..10s)
* Return:  MODBUS_MASTER_SUCCESS
*			MODBUS_MASTER_INVALID_ARGUMENT
**/
LIBSPEC int TCP_SetTimeout(unsigned timeout)
{
	_RPTF1(_CRT_WARN, "TCP_SetTimeout(&d)\n", timeout);

	if (timeout > MaxTimeoutMs ||
		timeout < MinTimeoutMs)
		return MODBUS_MASTER_INVALID_ARGUMENT;

	mbMaster.SetTimeout(std::chrono::milliseconds(timeout));
	return MODBUS_MASTER_SUCCESS;
}

/**
* Get current Modbus timeout
* Returen:	timeout in ms
**/
LIBSPEC unsigned TCP_GetTimeout(void)
{
	return (unsigned)mbMaster.GetTimeout().count();
}