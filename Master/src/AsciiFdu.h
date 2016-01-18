/**
 * This class is neened for create Ascii FDU frame from PDU and create PDU from FDU Ascii 
 * frame.
 * Author	Oleg Gavrilchenko
 * E-mail	reffum@bk.ru
 **/
#ifndef _ASCII_FDU_H_
#define _ASCII_FDU_H_

#include <cstdint>
#include "Master.h"

namespace Modbus
{
	class AsciiFdu : public virtual Master
	{
	public:
		static const int MaxFduSize = 1024;

		AsciiFdu();
		~AsciiFdu();

		/**
		* Form FDU from PDU, send it, receive responce and return it in respPdu.
		**/
		virtual void processPdu(
			uint8_t* reqPdu,
			int reqPduSize,
			void* respPdu,
			int &respPduSize
		);

		uint8_t byte2ascii(uint8_t);
		uint8_t lchar(uint8_t);
		uint8_t hchar(uint8_t);

		/**
		 * FDU buffers 
		 **/
		uint8_t ReqFdu[MaxFduSize];
		uint8_t RespFdu[MaxFduSize];

	};

}
#endif /* _ASCII_FDU_H_ */
