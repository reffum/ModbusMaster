/**
* This class is neened for create Ascii FDU frame from PDU and create PDU from FDU Ascii
* frame.
* Author	Oleg Gavrilchenko
* E-mail	reffum@bk.ru
**/
#include <cassert>
#include "AsciiFdu.h"

namespace Modbus
{

	AsciiFdu::AsciiFdu()
	{
	}


	AsciiFdu::~AsciiFdu()
	{
	}

	/**
	 * Form FDU from PDU, send it, receive responce and return it in respPdu.
	 **/
	void AsciiFdu::processPdu(
		uint8_t* reqPdu,
		int reqPduSize,
		void* respPdu,
		int &respPduSize)
	{
		assert(reqPdu);
		assert(respPdu);
		assert(reqPduSize*MaxFduSize + 3 <= MaxFduSize);
		int i, j;

		/**
		 * Create FDU
		 */
		ReqFdu[0] = ':';
		for (i = 0, j = 1; i < reqPduSize; i++, j = j + 2)
		{
			ReqFdu[j] = hchar(reqPdu[i]);
			ReqFdu[j + 1] = lchar(reqPdu[i]);
		}

		ReqFdu[j + 2] = '\r';
		ReqFdu[j + 3] = '\n';
		
		int respSize;
		SendRequestAndReceiveResponce(
			ReqFdu, 
			j+3,
			RespFdu,
			respSize
		);
		
	}

	uint8_t AsciiFdu::byte2ascii(uint8_t byte)
	{
		return 0;
	}

	uint8_t AsciiFdu::lchar(uint8_t)
	{
		return 0;
	}

	uint8_t AsciiFdu::hchar(uint8_t)
	{
		return 0;
	}

}