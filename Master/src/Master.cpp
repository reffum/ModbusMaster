/**
* MODBUS Master base functional class
* Author	Oleg Gavrilchenko
* E-mail	reffum@bk.ru
**/
#include <cassert>
#include <stdexcept>
#include "Master.h"
#include "mb_exceptions.h"

using namespace std;

namespace Modbus
{
	static uint8_t high_byte(uint16_t word)
	{
		return (uint8_t)(word >> 8);
	}

	static uint8_t low_byte(uint16_t word)
	{
		return (uint8_t)(word & 0x00FF);
	}

	/**
	* Read Coils(01)
	* addr:			Starting address
	* quantity:		Quantity of colils(1-2000)
	* Return:		bit vector with coils status.
	* Exceptions:	invalid_argument if quantity is out of range 1-2000.
	**/
	vector<bool> Master::ReadCoils(uint8_t id, uint16_t addr, unsigned quantity)
	{
		vector<uint8_t> request, responce;

		if ((quantity > 2000) || (quantity < 1))
		{
			throw invalid_argument("Quantity of coils out of range");
		}

		if (id == IDBroadcast)
		{
			throw invalid_argument("Read Coils with broadcast ID");
		}

		/**
		 * Send request PDU
		 **/
		request.push_back((uint8_t)FunctionCodes::ReadCoils);
		request.push_back(high_byte(addr));
		request.push_back(low_byte(addr));
		request.push_back(high_byte(quantity));
		request.push_back(low_byte(quantity));

		responce = SendPDU(id, request);

		/**
		 * Check responce PDU
		 **/

		/**
		 * Compute expected PDU size
		 **/
		int N = ((quantity % 8) == 0) ? quantity / 8 : (quantity / 8) + 1;
		int pduSize = N + 2;

		if ((responce.size() != pduSize) ||
			(responce[0] != request[0]) ||
			(responce[1] != N))
		{
			throw EPDUFrameError(request, responce);
		}

		/**
		 * Parse and form coils status
		 **/
		vector<bool> coils;

		for (vector<uint8_t>::const_iterator i = responce.cbegin() + 2;
			i != responce.cend();
			i++)
		{
			uint8_t byte = *i;
			for (int j = 0; j < 7; j++)
			{
				bool b = 0x01 & (byte >> j);
				coils.push_back(b);
			}
		}

		return coils;
	}
}