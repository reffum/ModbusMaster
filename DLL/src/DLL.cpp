/**
* Modbus Master DLL
* Oleg Gavrilchenko
* reffum@bk.ru
**/
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

