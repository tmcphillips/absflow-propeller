#pragma once

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AbsFlow { namespace Propeller {

class SerialConnection
{
	public:

		SerialConnection(LPCWSTR portName);
		~SerialConnection();

		// write methods
		void writeChar(char c);
		void writeInt32(__int32 i);
		void writeString(std::string& s);

		// read methods
		char readChar();
		void readChars(void* array, int count);
		__int32 readInt32();

		SerialConnection& operator<<(char c);
		SerialConnection& operator<<(__int32 i);
		SerialConnection& operator<<(std::string& s);
		SerialConnection& operator>>(char& c);
		SerialConnection& operator>>(__int32& i);
		SerialConnection& operator>>(std::string& s);

	private:
	 
		HANDLE _hSerial;
		void printerr();
};

}}