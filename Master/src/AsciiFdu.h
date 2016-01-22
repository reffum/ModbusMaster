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
		AsciiFdu();
		~AsciiFdu();

		/**
		* Form FDU from PDU, send it, receive responce and return it in respPdu.
		**/
		virtual std::vector<uint8_t> SendPduAndReceive(uint8_t id, std::vector<uint8_t> requestPdu);

		/**
		* Send PDU and do not wait responce.
		**/
		void SendPdu(uint8_t id, std::vector<uint8_t> requestPdu);

	private:
		std::vector<uint8_t> receiveAsciiResponceFdu();

		/**
		 * Convert low and high part of byte to ASCII symbol
		 **/
		uint8_t lbyte2ascii(const uint8_t);
		uint8_t hbyte2ascii(const uint8_t);
		uint8_t byte2ascii(const uint8_t);

		/**
		 * Convert high and low chars to byte
		 **/
		uint8_t ascii2byte(const uint8_t l, const uint8_t h);
		uint8_t ascii2byte(const uint8_t ch);

		/**
		 * Check that is this a ASCII symbol.
		 **/
		bool isAscii(uint8_t);

		/**
		 * Return ASCII LRC check sum
		 **/
		uint8_t getLrc(std::vector<uint8_t>);

		/**
		 * Get ASCII FDU
		 **/
		std::vector<uint8_t> getFdu(uint8_t id, std::vector<uint8_t> requestPdu);

		/**
		 * Get ID + PDU + LRC from FDU
		 **/
		std::vector<uint8_t> getPdu(std::vector<uint8_t>);
	};

}
#endif /* _ASCII_FDU_H_ */
