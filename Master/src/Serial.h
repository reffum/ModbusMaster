/**
 * Serial Modbus communication.
 * Author	Oleg Gavrilchenko
 * E-mail	reffum@bk.ru
 **/
#ifndef _SERIAL_H_
#define _SERIAL_H_

#include "Master.h"

namespace Modbus
{

	class Serial : public virtual Master
	{
	public:
		Serial();
		~Serial();

		void Connect();

		void SendRequestAndReceiveResponce(
				uint8_t* req,
				int reqSize,
				uint8_t* resp,
				int &respSize
		);
	};

}
#endif	/* _SERIAL_H_ */
