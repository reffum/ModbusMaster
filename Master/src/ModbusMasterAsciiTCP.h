/**
 * Modbus master ASCII TCP 
 * Oleg Gavrilchenko
 * reffum@bk.ru
 **/
#ifndef _MODBUS_MASTER_ASCII_TCP_H_
#define _MODBUS_MASTER_ASCII_TCP_H_

#include "Master.h"
#include "AsciiFdu.h"
#include "Tcp.h"

namespace Modbus
{

	class ModbusMasterAsciiTCP :public Tcp,
								public AsciiFdu,
								public virtual Master
	{
	public:
		ModbusMasterAsciiTCP(){}
		~ModbusMasterAsciiTCP(){}
	};
}
#endif	/* _MODBUS_MASTER_ASCII_TCP_H_ */
