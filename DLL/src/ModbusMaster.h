/**
 * Modbus Master DLL
 * Oleg Gavrilchenko
 * reffum@bk.ru
 **/
#ifndef _MODBUS_MASTER_H_
#define _MODBUS_MASTER_H_

#include <cstdint>

#if defined(DLL_EXPORTS)
#define LIBSPEC	extern "C" __declspec(dllexport) 
#else
#define LIBSPEC	extern "C" __declspec(dllimport) 
#endif

/**
 * Error codes
 **/
#define MODBUS_MASTER_SUCCESS			0
#define MODBUS_MASTER_FAIL				1
#define MODBUS_MASTER_TCP_ERROR			2
#define MODBUS_MASTER_TIMEOUT_ERROR		3
#define MODBUS_MASTER_RESPONCE_ERROR	4
#define MODBUS_MASTER_INVALID_ARGUMENT	5


/**
 * Open TCP modbus connection. 
 * ip	-	ip address
 * port	-	TCP port
 * Return:	MODBUS_MASTER_SUCCESS
 *			MODBUS_MASTER_TCP_ERROR
 **/
LIBSPEC int ModbusTCP_Open(const char* ip, int port);

/**
 * Close TCP modbus connection
 * Return: MODBUS_MASTER_SUCCESS
 **/
LIBSPEC int ModbusTCP_Close();

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
LIBSPEC int PUT(uint8_t id, uint16_t addr, uint16_t value);

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
LIBSPEC int GET(uint8_t id, uint16_t addr, uint16_t& value);

/**
* Read hold registers
* id		Slave ID
* addr		Registers start address
* count		count of read registers
* buffer	data buffer for registers value
* Return:	MODBUS_MASTER_SUCCESS
*			MODBUS_MASTER_TCP_ERROR
*			MODBUS_MASTER_TIMEOUT_ERROR
*			< 0 - Modbus exception with code by absolute value.
**/
LIBSPEC int TCP_ReadHold(uint8_t id, uint16_t addr, uint16_t count, uint16_t* buffer);

#endif /* _MODBUS_MASTER_H_ */
