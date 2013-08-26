#pragma once

#include <windows.h>
#include <string>

namespace AbsFlow { namespace Propeller {

	class SerialConnection
	{
	public:

		// constructor and destructor
		SerialConnection(std::string portName);
		~SerialConnection();

		// efficient multibyte serial I/O methods
		void readBytes(void* destination, int count);
		void writeBytes(const void* source, int count);

		// shortcut methods for single-byte I/O
		inline unsigned char readByte() { unsigned int b; readBytes(&b, 1); return b; }
		inline void writeByte(unsigned char b) { writeBytes(&b, 1); }

		// put operators
		inline SerialConnection& operator<<(unsigned char u) { writeByte(u); return *this; }
		inline SerialConnection& operator<<(char c) { writeByte(c); return *this; }
		inline SerialConnection& operator<<(__int16 i) { writeBytes(&i, 2); return *this; }
		inline SerialConnection& operator<<(__int32 i) { writeBytes(&i, 4); return *this; }
		inline SerialConnection& operator<<(std::string& s) { writeBytes(s.c_str(), s.size() + 1); return *this; }

		// get operators
		inline SerialConnection& operator>>(unsigned char& u) { u = readByte(); return *this; }
		inline SerialConnection& operator>>(char& c)  { c = readByte(); return *this; }
		inline SerialConnection& operator>>(__int16& i) { readBytes(&i, 2); return *this; }
		inline SerialConnection& operator>>(__int32& i) { readBytes(&i, 4); return *this; }
		SerialConnection& operator>>(std::string& s);

	private:
	 
		// private fields
		HANDLE _handle;

		// private methods
		void stringToWideChars(std::string source, WCHAR* destination);
		void openSerialPort(WCHAR* portName);
		void setPortParameters(DWORD baudRate, BYTE byteSize, BYTE stopBits, BYTE parity);
		void setPortTimeouts();
		void printerr();
};

}}