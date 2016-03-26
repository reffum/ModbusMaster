/**
* Description:	MODBUS master exceptions
* Author:		Oleg Gavrilchenko
* E-mail:		reffum@bk.ru
**/
#ifndef _MB_EXCEPTIONS_H_
#define _MB_EXCEPTIONS_H_

#include <exception>
#include <cstdint>
#include <vector>

namespace Modbus
{
	/**
	 * PDU frame error
	 **/
	class EPDUFrameError : public std::exception
	{
	public:
		EPDUFrameError(std::vector<uint8_t> request, 
			std::vector<uint8_t> responce);

		std::vector<uint8_t> GetRequest() const;
		std::vector<uint8_t> GetResponce() const;
	};

	/**
	 * Timeout exception
	 **/
	class ETimeout : public std::exception
	{

	};

	/**
	 * This exception is raised, when MODBUS Exception Code 
	 * is taking.
	 **/
	class EException : public std::exception
	{
	public:
		EException(uint8_t ecode);

		uint8_t GetExceptionCode();

		/**
		 * MODBUS standart exception codes.
		 **/
		static const uint8_t ILLEGAL_FUNCTION = 0x01;
		static const uint8_t ILLEGAL_DATA_ADDRESS = 0x02;
		static const uint8_t ILLEGAL_DATA_VALUE = 0x03;
		static const uint8_t SERVER_DEVICE_FAILURE = 0x04;
		static const uint8_t ACKNOWLEDGE = 0x05;
		static const uint8_t SERVER_DEVICE_BUSY = 0x06;
		static const uint8_t MEMORY_PARITY_ERROR = 0x08;
		static const uint8_t GATEWAY_PATH_UNAVIABLE = 0x0A;
		static const uint8_t TARGET_DEVICE_FAILED_TO_RESPONCE = 0x0B;
	};
}


#endif	/* _MB_EXCEPTIONS_H_ */
