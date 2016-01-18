// ConsoleTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Master.h"
#include "AsciiFdu.h"
#include "Serial.h"

using namespace Modbus;

namespace Modbus
{
	class MyClass : public Serial, 
					public AsciiFdu, 
					public virtual Master
	{
	};
}

int _tmain(int argc, _TCHAR* argv[])
{
	MyClass* m = new MyClass;
	uint16_t buff[12];

	m->Connect();
	m->ReadHold(0, 0, 1, buff);

	return 0;
}

