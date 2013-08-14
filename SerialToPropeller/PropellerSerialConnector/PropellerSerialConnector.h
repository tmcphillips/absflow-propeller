#pragma once

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AbsFlow { namespace Propeller {

	class PropellerSerialConnector
{
	public:

		// constructor and destructor
		PropellerSerialConnector(LPCWSTR portName);
		~PropellerSerialConnector(void);

		// write methods
		void writeChar(char c);
		void writeCharChar(char c1, char c2);
		void writeCharInt32(char c, __int32 i);
		void writeCharInt32Int32(char c, __int32 i1, __int32 i2);
		void writeInt32(__int32 i);
		void writeLine(char* text);

		// read methods
		char readChar();
		void readArray(void* array, int count);
		__int32 readInt32();
		void readLine(char* text);

		// write-then-read methods
		__int32 writeCharReadInt32(char c);
		__int32 writeCharInt32ReadInt32(char c, __int32 i);
		__int32 writeCharCharInt32ReadInt32(char c1, char c2, __int32 i);
		__int32 writeCharInt32Int32ReadInt32(char c, __int32 i1, __int32 i2);		
		__int32 writeInt32ReadInt32(int i);
		__int32 writeInt32Int32ReadInt32(int i1, int i2);
		void writeCharReadArray(char c, void* array, int size);
		void writeCharCharReadArray(char c1, char c2, void* array, int size);

		PropellerSerialConnector& operator<<(char c);

	private:
	 
		LPCWSTR _portName;
		HANDLE _hSerial;

		void printerr();
};

	}
}