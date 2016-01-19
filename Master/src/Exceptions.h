/**
* Modbus C++ exceptions.
* Author	Oleg Gavrilchenko
* Email	reffum@bk.ru
**/
#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

#include <exception>

namespace Modbus
{
	class ConnectException : public std::exception
	{
	public:
		ConnectException(const char *);
		ConnectException(const std::string& );
	};
}

#endif	/* _EXCEPTIONS_H_ */
