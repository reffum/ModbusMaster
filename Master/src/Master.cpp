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

	/**
	 * Return high and low bytes of word
	 **/
	static uint8_t high_byte(uint16_t word)
	{
		return (uint8_t)(word >> 8);
	}

	static uint8_t low_byte(uint16_t word)
	{
		return (uint8_t)(word & 0x00FF);
	}

	/**
	 * Return word from low and high bytes
	 **/
	static uint16_t get_word(const uint8_t h, const uint8_t l)
	{
		return ((uint16_t)h << 8) | l;
	}

	/**
	 * Return Exception Function code for MODBUS function code
	 **/
	uint8_t Master::exceptionCodeFor(FunctionCodes code)
	{
		return ((uint8_t)code & 0x80);
	}

	/**
	* Read Coils(01)
	* addr:			Starting address
	* quantity:		Quantity of colils(1-2000)
	* Return:		bit vector with coils status.
	* Exceptions:	invalid_argument if quantity is out of range 1-2000.
	*				invalid_argument if id is broadcast
	**/
	vector<bool> Master::ReadCoils(const uint8_t id, const uint16_t addr, const unsigned quantity)
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

		checkForException(request, responce);

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

	/**
	* Read Discrete Inputs(02)
	* addr:		Starting address
	* quantity:	Quantity of inputs(1-2000)
	* Return:		bit vector with inputs status.
	* Exceptions:	invalid_argument if quantity is out of range 1-2000.
	*				invalid_argument if id is broadcast
	* */
	vector<bool> Master::ReadDiscreteInputs(const uint8_t id, const uint16_t addr, const unsigned quantity)
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
		request.push_back((uint8_t)FunctionCodes::ReadDiscreteInputs);
		request.push_back(high_byte(addr));
		request.push_back(low_byte(addr));
		request.push_back(high_byte(quantity));
		request.push_back(low_byte(quantity));

		responce = SendPDU(id, request);

		checkForException(request, responce);

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

	/**
	* Read Holding Registers(03)
	* addr:		start address
	* quantity:	quantity of registers(1-125)
	* Return:		vector of registers.
	* Exceptions:	invalid_argument if quantity is out of range(1-125)
	*				invalid_argument if id is broadcast
	**/
	vector<uint16_t> Master::ReadHoldingRegisters(const uint8_t id, const uint16_t addr, const unsigned quantity)
	{
		vector<uint8_t> request, responce;
		const uint8_t funcCode = (uint8_t)FunctionCodes::ReadHoldingRegisters;
		vector<uint16_t> registers;

		/**
		 * Check parameters
		 **/
		if (quantity < 1 || quantity > 125)
		{
			throw invalid_argument("Quantaty out of range.");
		}

		if (id == IDBroadcast)
		{
			throw invalid_argument("Read Hold with broadcast ID.");
		}

		/**
		 * Request PDU
		 **/
		request.push_back(funcCode);
		request.push_back(high_byte(addr));
		request.push_back(low_byte(addr));
		request.push_back(high_byte(quantity));
		request.push_back(low_byte(quantity));

		responce = SendPDU(id, request);

		checkForException(request, responce);

		/**
		 * Check responce PDU.
		 **/
		const int expectedSize = 2 + quantity * 2;
		if (responce.size() != expectedSize ||
			responce[0] != funcCode ||
			responce[1] != quantity * 2)
		{
			throw EPDUFrameError(request, responce);
		}

		/**
		 * Parse responce and return registers value.
		 **/
		for (unsigned i = 2; i < responce.size(); i += 2)
		{
			uint8_t hbyte = responce[i];
			uint8_t lbyte = responce[i + 1];

			registers.push_back(get_word(hbyte, lbyte));
		}

		return registers;
	}

	/**
	* Read Input Registers(04)
	* addr:		starting address
	* quantity:	quantity of input registers(1-125)
	* Return:		vector of registers
	* Exceptions:	invalid_argument if quantity is out of range(1-125)
	*				invalid_argument if id is broadcast
	**/
	vector<uint16_t> Master::ReadInputRegisters(const uint8_t id, const uint16_t addr, const unsigned quantity)
	{
		vector<uint8_t> request, responce;
		const uint8_t funcCode = (uint8_t)FunctionCodes::ReadInputRegisters;
		vector<uint16_t> registers;

		/**
		* Check parameters
		**/
		if (quantity < 1 || quantity > 125)
		{
			throw invalid_argument("Quantaty out of range.");
		}

		if (id == IDBroadcast)
		{
			throw invalid_argument("Read Hold with broadcast ID.");
		}

		/**
		* Request PDU
		**/
		request.push_back(funcCode);
		request.push_back(high_byte(addr));
		request.push_back(low_byte(addr));
		request.push_back(high_byte(quantity));
		request.push_back(low_byte(quantity));

		responce = SendPDU(id, request);

		checkForException(request, responce);

		/**
		* Check responce PDU.
		**/
		const int expectedSize = 2 + quantity * 2;
		if (responce.size() != expectedSize ||
			responce[0] != funcCode ||
			responce[1] != quantity * 2)
		{
			throw EPDUFrameError(request, responce);
		}

		/**
		* Parse responce and return registers value.
		**/
		for (unsigned i = 2; i < responce.size(); i += 2)
		{
			uint8_t hbyte = responce[i];
			uint8_t lbyte = responce[i + 1];

			registers.push_back(get_word(hbyte, lbyte));
		}

		return registers;
	}

	/**
	* Write Single Coil(05)
	* addr:		Coil address
	* value:	Coil value
	**/
	void Master::WriteSingleCoil(const uint8_t id, const uint16_t addr, const bool value)
	{
		uint8_t funcCode = (uint8_t)FunctionCodes::WriteSingleCoil;
		vector<uint8_t> request, responce;

		request.push_back(funcCode);
		request.push_back(high_byte(addr));
		request.push_back(low_byte(addr));
		request.push_back(value ? 0xFF : 0x00);
		request.push_back(0);

		if (id == IDBroadcast)
		{
			SendPDU(request);
		}
		else
		{
			responce = SendPDU(id, request);

			checkForException(request, responce);

			if (responce != request)
				throw EPDUFrameError(request, responce);
		}
	}

	/**
	* Write Single Register(06)
	* addr:		Address
	* value:	Value
	**/
	void Master::WriteSingleRegister(const uint8_t id, const uint16_t addr, const uint16_t value)
	{
		uint8_t funcCode = (uint8_t)FunctionCodes::WriteSingleRegister;
		vector<uint8_t> request, responce;

		request.push_back(funcCode);
		request.push_back(high_byte(addr));
		request.push_back(low_byte(addr));
		request.push_back(high_byte(value));
		request.push_back(low_byte(value));

		if (id == IDBroadcast)
		{
			SendPDU(request);
		}
		else
		{
			responce = SendPDU(id, request);

			checkForException(request, responce);

			if (responce != request)
				throw EPDUFrameError(request, responce);
		}
	}

	/**
	* Read Exception Status(07)(Serial only)
	* Return:	exception status
	**/
	bitset<Master::NumberOfExceptionStatusOutputs> Master::ReadExceptionStatus(const uint8_t id)
	{
		vector<uint8_t> request, responce;
		uint8_t funcCode = (uint8_t)FunctionCodes::ReadExceptionStatus;

		if (id == IDBroadcast)
		{
			throw invalid_argument("Read with broadcast ID.");
		}

		request.push_back(funcCode);

		responce = SendPDU(id, request);

		checkForException(request, responce);

		/**
		 * Check responce correction.
		 **/
		const int expectedSize = 2;
		if (responce[0] != request[0] ||
			responce.size() != expectedSize)
		{
			throw EPDUFrameError(request, responce);
		}

		return responce[1];
	}

	/**
	* Diagnostics functions(08)(Serial only)
	* This is a common diagnostic request. Below are functoins for
	* standart diagnostic sub-funstions.
	* subFunstion:		sub-function code
	* data:			function data
	* Return:			data from responce
	**/
	vector<uint8_t> Master::Diagnostic(const uint8_t id, const uint16_t subFunction, const vector<uint8_t> data)
	{
		vector<uint8_t> request, responce;

		/**
		 * Check parameters
		 **/
		const int requestPDUSize = 3 + data.size() * 2;
		if (requestPDUSize > PDU_MAX_SIZE)
		{
			throw invalid_argument("PDU size is more than maximum size.");
		}

		if (data.size() % 2 != 0)
		{
			throw invalid_argument("Data should be even value.");
		}

		uint8_t funcCode = (uint8_t)FunctionCodes::Diagnostic;

		request.push_back(funcCode);
		request.push_back(high_byte(subFunction));
		request.push_back(low_byte(subFunction));

		request.insert(request.cend(), data.cbegin(), data.cend());

		if (id == IDBroadcast)
		{
			SendPDU(request);

			/**
			 * Return empty vector.
			 **/
			return vector < uint8_t >();
		}
		else
		{
			responce = SendPDU(id, request);

			checkForException(request, responce);
			
			/**
			 * Return responce data
			 **/
			return vector<uint8_t>(responce.cbegin() + 2, responce.cend());
		}
	}

	/**
	* Return query data(00)
	* data:	Any test data.
	* Return:	true - OK, false - error
	**/
	bool Master::ReturnQueryData(const uint8_t id, const vector<uint8_t> data)
	{
		/**
		 * Check parameters
		 **/
		if (id == IDBroadcast)
		{
			throw invalid_argument("Return Query Data with broadcast ID.");
		}

		return data == Diagnostic(id, (uint16_t)DiagnosticSubFunctions::ReturnQueryData, data);
	}

	/**
	* Restart Communications Options(01)
	* clearLog:	if true, communication event log must also be cleared
	**/
	void Master::RestartCommunicationOptions(const uint8_t id, const bool clearLog)
	{
		/**
		* Check parameters
		**/
		if (id == IDBroadcast)
		{
			throw invalid_argument("Return Query Data with broadcast ID.");
		}

		vector<uint8_t> requestData, responceData;

		if (clearLog)
		{
			requestData = { 0xFF, 0x00 };
		}
		else
			requestData = { 0, 0 };

		responceData = Diagnostic(id, (uint16_t)DiagnosticSubFunctions::RestartCommunicationsOption, requestData);

		if (responceData != requestData)
		{
			throw EPDUFrameError(requestData, responceData);
		}
	}

	/**
	* Return Diagnostic Register(02)
	* Return:	Diagnostic register
	**/
	uint16_t Master::ReturnDiagnosticRegister(const uint8_t id)
	{
		/**
		* Check parameters
		**/
		if (id == IDBroadcast)
		{
			throw invalid_argument("Return Query Data with broadcast ID.");
		}

		vector<uint8_t> responceData = Diagnostic(id, (uint16_t)DiagnosticSubFunctions::ReturnDiagnosticRegister, { 0, 0 });
		
		/**
		 * Check responce data
		 **/
		if (responceData.size() != 2)
		{
			throw EDiagnostic({ 0, 0 }, responceData);
		}

		return get_word(responceData[0], responceData[1]);
	}

	/**
	* Change ASCII delimeter(03)
	* delim:	new ASCII delimeter
	**/
	void Master::ChangeASCIIDelimeter(const uint8_t id, const unsigned char delim)
	{
		vector<uint8_t> requestData, responceData;
		/**
		* Check parameters
		**/
		if (id == IDBroadcast)
		{
			throw invalid_argument("Return Query Data with broadcast ID.");
		}

		requestData = { delim, 0 };

		responceData = Diagnostic(
			id, 
			(uint16_t)DiagnosticSubFunctions::ChangeASCIIInputDelimeter, 
			{ delim, 0 }
		);

		if (responceData != requestData)
		{
			throw EDiagnostic(requestData, responceData);
		}
	}

	/**
	* Force Listen Only Mode(04)
	**/
	void Master::ForceListenOnlyMode(const uint8_t id)
	{
		vector<uint8_t> requestData = { 0, 0 };

		/**
		* Check parameters
		**/
		if (id == IDBroadcast)
		{
			throw invalid_argument("Return Query Data with broadcast ID.");
		}

		/**
		 * No responce returned.
		 **/
		Diagnostic(id, (uint16_t)DiagnosticSubFunctions::ForceListenOnlyMode, requestData);
	}

	/**
	* Clear Counters and Diagnostic Register(0A)
	**/
	void Master::ClearCountersAndDiagnosticRegister(const uint8_t id)
	{
		vector<uint8_t> requestData = { 0, 0 };
		/**
		* Check parameters
		**/
		if (id == IDBroadcast)
		{
			throw invalid_argument("Return Query Data with broadcast ID.");
		}

		vector<uint8_t> responceData = Diagnostic(
			id,
			(uint16_t)DiagnosticSubFunctions::ClearCountersAndDiagnosticRegister,
			requestData
			);
		if (requestData != responceData)
		{
			throw EDiagnostic(requestData, responceData);
		}
	}

	/**
	* Return bus message count(0B)
	* Return: bus message count
	**/
	uint64_t Master::ReturnBusMessageCount(const uint8_t id)
	{
		return getCounter(id, DiagnosticSubFunctions::ReturnBusMessageCount);
	}

	/**
	* Return Bus Communication Error Count(0C)
	* Return: CRC error count
	**/
	uint64_t Master::ReturnBusCommunicationErrorCount(const uint8_t id)
	{
		return getCounter(id, DiagnosticSubFunctions::ReturnBusCommunicationError);
	}

	/**
	* Return Bus Exception Error Count(0D)
	**/
	uint64_t Master::ReturnBusExceptionErrorCount(const uint8_t id)
	{
		return getCounter(id, DiagnosticSubFunctions::ReturnBusExceptionErrorCount);
	}

	/**
	* Return Server Message Count(0E)
	**/
	uint64_t Master::ReturnServerMessageCount(const uint8_t id)
	{
		return getCounter(id, DiagnosticSubFunctions::ReturnServerMessageCount);
	}

	/**
	* Return Server No Responce Count(0F)
	**/
	uint64_t Master::ReturnServerNoResponceCount(const uint8_t id)
	{
		return getCounter(id, DiagnosticSubFunctions::ReturnServerNoResponceCount);
	}

	/**
	* Return Server NAK Count(10)
	**/
	uint64_t Master::ReturnServerNAKCount(const uint8_t id)
	{
		return getCounter(id, DiagnosticSubFunctions::ReturnSrverNAKCount);
	}

	/**
	* Return Server Busy Count(11)
	**/
	uint64_t Master::ReturnServerBusyCount(const uint8_t id)
	{
		return getCounter(id, DiagnosticSubFunctions::ReturnServerBusyCount);
	}

	/**
	* Return Bus Character Overrun Count(12)
	**/
	uint64_t Master::ReturnBusCharacterOverrunCount(const uint8_t id)
	{
		return getCounter(id, DiagnosticSubFunctions::ReturnBusCharacterOverrun);
	}

	/**
	* Clear Overrurn Counter and Flag(14)
	**/
	void Master::ClearOverrunCounterAndFlag(const uint8_t id)
	{
		vector<uint8_t> requestData = { 0, 0 }, responceData;
		Diagnostic(id, (uint16_t)DiagnosticSubFunctions::ClearOverrunCounterAndFlag, requestData);

		if (requestData != responceData)
		{
			throw EDiagnostic(requestData, responceData);
		}
	}

	/**
	* Get Comm Event Counter(0B)
	* Return:	Comm event counter structure.
	**/
	Master::CommEventCounter Master::GetCommEventCounter(const uint8_t id)
	{
		const vector<uint8_t> requestPDU = { (uint8_t)FunctionCodes::GetCommEventCounter };

		if (id == IDBroadcast)
		{
			throw invalid_argument("Read request for broadcast ID.");
		}

		vector<uint8_t> responcePDU = SendPDU(id, requestPDU);

		checkForException(requestPDU, responcePDU);

		/**
		 * Check responce PDU correction
		 **/
		const int responcePDUSize = 5;
		if (responcePDU.size() != responcePDUSize)
		{
			EPDUFrameError(requestPDU, responcePDU);
		}

		uint16_t statusWord;
		uint16_t counter;

		statusWord = ((uint16_t)responcePDU[1] << 8) | responcePDU[2];
		counter = ((uint16_t)responcePDU[3] << 8) | responcePDU[4];

		const uint16_t StatusWordTrue = 0xFFFF;
		const uint16_t StatusWordFalse = 0x0000;

		CommEventCounter cec;

		if (statusWord == StatusWordTrue)
			cec.status = CommStatus::LastCommandStillProcessed;
		else if (statusWord == StatusWordFalse)
			cec.status = CommStatus::LastCommandComplete;
		else
			throw EPDUFrameError(requestPDU, responcePDU);

		cec.eventCount = counter;

		return cec;
	}

	/**
	* Get Comm Event Log(0C) (Serial Only)
	**/
	Master::CommEventLog Master::GetCommEventLog(const uint8_t id)
	{
		if (id == IDBroadcast)
		{
			throw invalid_argument("Read request with broadcast ID.");
		}

		const vector<uint8_t> requestPDU = { (uint8_t)FunctionCodes::GetCommEventLog };

		vector<uint8_t> responcePDU = SendPDU(id, requestPDU);

		checkForException(requestPDU, responcePDU);

		/**
		 * Check responce PDU correction.
		 **/
		const int MinResponcePDUSize = 6;
		if (responcePDU.size() < MinResponcePDUSize)
			throw EPDUFrameError(requestPDU, responcePDU);

		uint8_t N = responcePDU[1];

		if (responcePDU.size() != N)
			throw EPDUFrameError(requestPDU, responcePDU);

		CommEventLog commEventLog;


	}


	/**
	 * Private functions
	 **/

	/**
	* Get counters function
	**/
	uint64_t Master::getCounter(const uint8_t id, const DiagnosticSubFunctions subFunction)
	{
		vector<uint8_t> requestData = { 0, 0 };
		/**
		* Check parameters
		**/
		if (id == IDBroadcast)
		{
			throw invalid_argument("Return counter with broadcast ID.");
		}

		vector<uint8_t> responceData = Diagnostic(
			id,
			(uint16_t)subFunction,
			requestData
			);

		if (responceData.size() == 0)
		{
			EDiagnostic(requestData, responceData);
		}

		/**
		* Responce data contain message count, and it size is variable.
		* It can contain more than 8 bytes. In this case, the function return low 8 byte
		* only .
		**/
		uint64_t counter = 0;
		for (unsigned i = 0; i < responceData.size() && i < 8; i++)
		{
			counter += ((uint64_t)responceData[i] << i * 8);
		}

		return counter;
	}

	/**
	* Check responce PDU for exceptions
	* Raise EException if PDU is MODBUS exception.
	* funcCode		Function code in request PDU.
	**/
	void Master::checkForException(const vector<uint8_t> request, const vector<uint8_t> responce)
	{
		if (responce.size() == ExceptionResponcePDUSize)
		{
			uint8_t functionCode = responce[0];
			if (functionCode == exceptionCodeFor(FunctionCodes::ReadDiscreteInputs))
			{
				throw EException(responce[1]);
			}

			throw EPDUFrameError(request, responce);
		}
	}
}