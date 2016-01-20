/**
* This class is neened for create Ascii FDU frame from PDU and create PDU from FDU Ascii
* frame.
* Author	Oleg Gavrilchenko
* E-mail	reffum@bk.ru
**/
#include <cassert>
#include <cctype>
#include <chrono>
#include "AsciiFdu.h"

namespace Modbus
{
	using namespace std;
	using namespace std::chrono;

	/**
	 * Constants
	 **/
	const seconds AsciiStandartTimeout(1);

	AsciiFdu::AsciiFdu()
	{
	}


	AsciiFdu::~AsciiFdu()
	{
	}

	/**
	* Send PDU and do not wait responce.
	**/
	void AsciiFdu::SendPdu(uint8_t id, vector<uint8_t> requestPdu)
	{
		SendFdu(getFdu(id, requestPdu));
	}

	/**
	* Form FDU from PDU, send it, receive responce and return it in respPdu.
	**/
	vector<uint8_t> AsciiFdu::SendPduAndReceive(uint8_t id, vector<uint8_t> requestPdu)
	{
		assert(id != BroadcastID);

		vector<uint8_t> reqFdu = getFdu(id, requestPdu);

		SendFdu(reqFdu);

		vector<uint8_t> respFdu = receiveAsciiResponceFdu();

		assert(respFdu.size() > 3);

		/**
		 * Skip SOF and EOF symbols
		 **/
		respFdu.erase(respFdu.cbegin());
		respFdu.erase(respFdu.cend() - 1, respFdu.cend());

		/**
		 * Check FDU correction.
		 **/
		if ((respFdu.size() % 2) == 1)
		{
			throw InvalidResponceFdu(reqFdu, respFdu);
		}

		vector<uint8_t> respPdu = getPdu(respFdu);

		assert(respPdu.size() > 3);

		/**
		 * Check ID and LRC.
		 **/
		uint8_t lrc = respFdu.back();
		respFdu.pop_back();

		if ((respPdu[0] != id)		||
			(lrc != getLrc(respPdu)	))
		{
			throw InvalidResponcePdu(requestPdu, respPdu);
		}

		/**
		 * Remove ID
		 **/
		respPdu.erase(respFdu.cbegin());

		return respPdu;
	}

	vector<uint8_t> AsciiFdu::receiveAsciiResponceFdu()
	{

	}

	vector<uint8_t> AsciiFdu::getFdu(uint8_t id, vector<uint8_t> requestPdu)
	{
		vector<uint8_t> fdu;

		/**
		* Add ID and LRC to PDU
		**/
		requestPdu.insert(requestPdu.cbegin(), 1, id);

		uint8_t lrc = getLrc(requestPdu);
		requestPdu.push_back(lrc);

		/**
		* Store ASCII FDU
		**/
		fdu.push_back(':');

		for each (uint8_t b in requestPdu)
		{
			fdu.push_back(hbyte2ascii(b));
			fdu.push_back(lbyte2ascii(b));
		}

		fdu.push_back('\r');
		fdu.push_back('\n');

		return fdu;
	}


	/**
	* Convert low and high part of byte to ASCII symbol
	**/
	uint8_t AsciiFdu::lbyte2ascii(const uint8_t byte)
	{
		return byte2ascii(0x0F & byte);
	}

	uint8_t AsciiFdu::hbyte2ascii(const uint8_t byte)
	{
		return byte2ascii(byte / 16);
	}

	uint8_t AsciiFdu::byte2ascii(const uint8_t byte)
	{
		assert(byte < 0xf);

		return (byte < 10) ?
			byte + 0x30 :
			byte - 10 + 0x41;
	}

	/**
	* Convert high and low chars to byte
	**/
	uint8_t AsciiFdu::ascii2byte(const uint8_t l, const uint8_t h)
	{
		assert(isxdigit(l));
		assert(isxdigit(h));

		return ascii2byte(l) + ascii2byte(h) * 16;
	}

	uint8_t AsciiFdu::ascii2byte(const uint8_t ch)
	{
		toupper(ch);

		if (isdigit(ch))
		{
			return ch - 0x30;
		}
		else
		{
			return ch - 0x41 + 10;
		}
	}

	/**
	* Return ASCII LRC check sum
	**/
	uint8_t AsciiFdu::getLrc(vector<uint8_t> v)
	{
		uint8_t lrc = 0;
		for each (uint8_t b in v)
		{
			lrc += b;
		}

		return (uint8_t)(-((int8_t)lrc));
	}

	/**
	* Get ID + PDU + LRC from FDU
	**/
	vector<uint8_t> AsciiFdu::getPdu(vector<uint8_t> fdu)
	{
		vector<uint8_t> pdu;

		for (auto it = fdu.begin(); it != fdu.cend(); it += 2)
		{
			pdu.push_back(ascii2byte(*it, *(it + 1)));
		}

		return pdu;
	}
}
