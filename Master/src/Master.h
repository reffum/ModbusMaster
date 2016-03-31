/**
 * Description: MODBUS Master base functional class. It contnait PDU-level
 *				MODBUS function implementation.
 * Author:		Oleg Gavrilchenko
 * E-mail:		reffum@bk.ru
 **/
#ifndef _MASTER_H_
#define _MASTER_H_

#include <cstdint>
#include <vector>
#include <bitset>

namespace Modbus
{
	class Master
	{
	public:
		/**
		 * Constants
		 **/
		static const int NumberOfExceptionStatusOutputs = 8;
		static const uint8_t IDBroadcast = 0xFF;
		static const int PDU_MAX_SIZE = 253;
		static const int ExceptionResponcePDUSize = 2;

		/**
		 * Types
		 **/
		typedef std::vector<uint8_t>::const_iterator PDUConstIterator;

		/**
		 * Modbus function codes
		 **/
		enum class FunctionCodes : uint8_t
		{
			ReadCoils = 0x01,
			ReadDiscreteInputs = 0x02,
			ReadHoldingRegisters = 0x03,
			ReadInputRegisters = 0x04,
			WriteSingleCoil = 0x05,
			WriteSingleRegister = 0x06,
			ReadExceptionStatus = 0x07,
			Diagnostic = 0x08
		};

		/**
		 * Diagnostic sub-functions
		 **/
		enum class DiagnosticSubFunctions : uint16_t
		{
			ReturnQueryData = 0x00,
			RestartCommunicationsOption = 0x01,
			ReturnDiagnosticRegister = 0x02,
			ChangeASCIIInputDelimeter = 0x03,
			ForceListenOnlyMode = 0x04,
			ClearCountersAndDiagnosticRegister = 0x0A,
			ReturnBusMessageCount = 0x0B,
			ReturnBusCommunicationError = 0x0C,
			ReturnBusExceptionErrorCount = 0x0D,
			ReturnServerMessageCount = 0x0E,
			ReturnServerNoResponceCount = 0x0F,
			ReturnSrverNAKCount = 0x10,
			ReturnServerBusyCount = 0x11,
			ReturnBusCharacterOverrun = 0x12,
			ClearOverrunCounterAndFlag = 0x14
		};

		enum class CommStatus
		{
			LastCommandStillProcessed,
			LastCommandComplete
		};

		enum class CommLogEventType
		{
			ReceiveEvent,
			SendEvent,
			ListenOnlyMode,
			CommRestart
		};

		/**
		 * Data structures
		 **/
		struct CommEventCounter
		{
			CommStatus status;	
			uint16_t eventCount;	
		};

		struct CommEvent
		{
			CommLogEventType type;
			union
			{
				struct ReceiveEvent
				{
					bool CommunicationError;
					bool CharacterOverrun;
					bool CurrentlyInListenOnlyMode;
					bool BroadcastReceived;
				};

				struct SendEvent
				{
					bool ReadExceptionSend;
					bool ServerAbortExceptionSend;
					bool ServerBusyExceptionSend;
					bool ServerProgramNAKExceptionSend;
					bool WriteTimeoutErrorOccurred;
					bool CurrentlyInListenOnlyMode;
				};
			};
		};

		struct CommEventLog
		{
			CommStatus status;

			uint16_t eventCount;
			uint16_t messageCount;

			std::vector<CommEvent> eventLog;
		};

		struct ServerID
		{
			std::vector<uint8_t> serverID;
			bool RunIndicatorStatus;
			std::vector<uint8_t> additionData;
		};

		struct FileReadSubRequest
		{
			uint16_t FileNumber;
			uint16_t StartingRecord;
			uint16_t Length;
		};

		struct FileWriteSubRequest
		{
			uint16_t FileNumber;
			uint16_t StartingRecord;
			std::vector<uint8_t> data;
		};

		typedef std::vector<std::vector<uint8_t>> FileRecords;

		Master();
		virtual ~Master();

		/**
		 * MODBUS functions. Each function can raise EPDUFrameError, ETimeout and EExceptionCode
		 * exceptions, as well as other exceptions that depends on FDU and phisical 
		 * protocols. For examlple, frame error, tcp connectoin failure etc.
		 * Each functioin also contain ID parameter, because it present at all
		 * FDU protocols.
		 * If request PDU more than PDU_MAX_SIZE, logic_error raised.
		 **/

		/**
		 * Read Coils(01)
		 * addr:		Starting address
		 * quantity:	Quantity of colils(1-2000)
		 * Return:		bit vector with coils status.
		 * Exceptions:	invalid_argument if quantity is out of range 1-2000.
		 *				invalid_argument if id is broadcast
		 **/
		std::vector<bool> ReadCoils(const uint8_t id, const uint16_t addr, const unsigned quantity);

		/**
		 * Read Discrete Inputs(02)
		 * addr:		Starting address
		 * quantity:	Quantity of inputs(1-2000)
		 * Return:		bit vector with inputs status.
		 * Exceptions:	invalid_argument if quantity is out of range 1-2000.
		 *				invalid_argument if id is broadcast
		 * */
		std::vector<bool> ReadDiscreteInputs(const uint8_t id, const uint16_t addr, const unsigned quantity);

		/**
		 * Read Holding Registers(03)
		 * addr:		start address
		 * quantity:	quantity of registers(1-125)
		 * Return:		vector of registers.
		 * Exceptions:	invalid_argument if quantity is out of range(1-125)
		 *				invalid_argument if id is broadcast
		 **/
		std::vector<uint16_t> ReadHoldingRegisters(const uint8_t id, const uint16_t addr, const unsigned quantity);

		/**
		 * Read Input Registers(04)
		 * addr:		starting address
		 * quantity:	quantity of input registers(1-125)
		 * Return:		vector of registers
		 * Exceptions:	invalid_argument if quantity is out of range(1-125)
		 *				invalid_argument if id is broadcast
		 **/
		std::vector<uint16_t> ReadInputRegisters(const uint8_t id, const uint16_t addr, const unsigned quantity);

		/**
		 * Write Single Coil(05)
		 * addr:		Coil address
		 * value:		Coil value
		 **/
		void WriteSingleCoil(const uint8_t id, const uint16_t addr, const bool value);

		/**
		 * Write Single Register(06)
		 * addr:		Address
		 * value:		Value
		 **/
		void WriteSingleRegister(const uint8_t id, const uint16_t addr, const uint16_t value);

		/**
		 * Read Exception Status(07)(Serial only)
		 * Return:	exception status
		 * Exception: invalid_argument if id is broadcast.
		 **/
		std::bitset<NumberOfExceptionStatusOutputs>
			ReadExceptionStatus(const uint8_t id);

		/**
		 * Diagnostics functions(08)(Serial only)
		 * This is a common diagnostic request. Below are functoins for
		 * standart diagnostic sub-funstions.
		 * subFunstion:		sub-function code
		 * data:			function data
		 * Return:			data from responce
		 * Exception:		invalid_argument if PDU size if more then maximum.
		 **/
		std::vector<uint8_t> Diagnostic(const uint8_t id, const uint16_t subFunction, const std::vector<uint8_t> data);

		/**
		 * Return query data(00)
		 * data:	Any test data.
		 * Return:	true - OK, false - error
		 **/
		bool ReturnQueryData(const uint8_t id, const std::vector<uint8_t> data);

		/**
		 * Restart Communications Options(01)
		 * clearLog:	if true, communication event log must also be cleared
		 **/
		void RestartCommunicationOptions(const uint8_t id, const bool clearLog);

		/**
		 * Return Diagnostic Register(02)
		 * Return:	Diagnostic register
		 **/
		uint16_t ReturnDiagnosticRegister(const uint8_t id);

		/**
		 * Change ASCII delimeter(03)
		 * delim:	new ASCII delimeter
		 **/
		void ChangeASCIIDelimeter(uint8_t id, char delim);

		/**
		 * Force Listen Only Mode(04)
		 **/
		void ForceListenOnlyMode(uint8_t id);

		/**
		* Clear Counters and Diagnostic Register(0A)
		**/
		void ClearCountersAndDiagnosticRegister(uint8_t id);

		/**
		 * Return bus message count(0B)
		 * Return: bus message count
		 **/
		uint64_t ReturnBusMessageCount(uint8_t id);

		/**
		 * Return Bus Communication Error Count(0C)
		 * Return: CRC error count
		 **/
		uint64_t ReturnBusCommunicationErrorCount(uint8_t id);

		/**
		 * Return Bus Exception Error Count(0D)
		 **/
		uint64_t ReturnBusExceptionErrorCount(uint8_t id);
		 
		/**
		 * Return Server Message Count(0E)
		 **/
		uint64_t ReturnServerMessageCount(uint8_t id);

		/**
		 * Return Server No Responce Count(0F)
		 **/
		uint64_t ReturnServerNoResponceCount(uint8_t id);

		/**
		 * Return Server NAK Count(10)
		 **/
		uint64_t ReturnServerNAKCount(uint8_t id);

		/**
		 * Return Server Busy Count(11)
		 **/
		uint64_t ReturnServerBusyCount(uint8_t id);

		/**
		 * Return Bus Character Overrun Count(12)
		 **/
		uint64_t ReturnBusCharacterOverrunCount(uint8_t id);

		/**
		 * Clear Overrurn Counter and Flag(14)
		 **/
		void ClearOverrunCounterAndFlag(uint8_t id);

		/**
		 * Get Comm Event Counter(0B)
		 * Return:	Comm event counter structure.
		 **/
		CommEventCounter GetCommEventCounter(uint8_t id, bool& status);

		/**
		 * Get Comm Event Log(0C) (Serial Only)
		 **/
		CommEventLog GetCommventLog(uint8_t id);

		/**
		 * Write Multiple Coils(0F)
		 * addr:		Start address
		 * coils:		coils output state and quantity
		 * Exception:	invalid_argument if quantity of coils out of range(1-1968)
		 **/
		void WriteMultipleCoils(uint8_t id, uint16_t addr, std::vector<bool> coils);

		/**
		 * Write Muliple Registers(10)
		 * addr:		Start address
		 * regs:		registers value(and quatity)
		 * Exception:	logic_error if quantity of registers out of range(1-123)
		 **/
		void WriteMultipleRegisters(uint8_t id, uint16_t addr, std::vector<uint16_t> regs);

		/**
		 * Report Server ID(11)(Serial Line Only)
		 **/
		ServerID ReportServerID(uint8_t id);

		/**
		 * Read File Record(14)
		 * Exception:	logic_error if PDU size more than PDU_MAX_SIZE.
		 **/
		FileRecords ReadFileRecord(uint8_t id, std::vector<FileReadSubRequest> requests);

		/**
		 * Write File Record(15)
		 **/
		void WriteFileRecord(uint8_t id, std::vector<FileWriteSubRequest> requests);

		/**
		 * Mask Write Register(16)
		 **/
		void MaskWriteResister(uint8_t id, uint16_t addr, uint16_t OrMask, uint16_t AndMask);

		/**
		 * Read/Write Multiple Registers(17)
		 **/
		std::vector<uint16_t> ReadWriteMultipleRegisters(uint8_t id,
			uint16_t writeAddr,
			uint16_t readAddr,
			std::vector<uint16_t> writeData
			);

		/**
		 * Read FIFO Queue(18)
		 **/
		std::vector<uint16_t> ReadFIFOQueue(uint8_t id, uint16_t FIFOPointerAddress);

		/**
		 * Encapsulated interface Transport(2B)
		 * MEIType:		MEI Type
		 * data:		protocol specific data
		 * Return:		protocol specific data
		 **/
		std::vector<uint8_t> EncaplulatedInterfaceTransport(
			uint8_t id,
			uint16_t MEIType,
			std::vector<uint8_t> data
			);

		/**
		 * Read Device identification(2B/0E)
		 **/

	protected:
		/**
		 * Send PDU to remote device.
		 * This function must be implemented FDU protocols.
		 * Return:	responce PDU
		 **/
		virtual std::vector<uint8_t> SendPDU(uint8_t id, std::vector<uint8_t> request) = 0;

		/**
		 * Send PDU and no wait responce.
		 **/
		virtual void SendPDU(std::vector<uint8_t> request);

	private:
		/**
		 * Check responce PDU for exceptions
		 * Raise EException if PDU is MODBUS exception. 
		 * funcCode		Function code in request PDU.
		 **/
		void checkForException(const std::vector<uint8_t> request, const std::vector<uint8_t> responce);

		/**
		* Return Exception Function code for MODBUS function code
		**/
		uint8_t exceptionCodeFor(FunctionCodes code);
	};
}

#endif	/* _MASTER_H_ */
