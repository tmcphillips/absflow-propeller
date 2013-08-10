#pragma once

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

class PropellerSerialConnector
{
	public:
	
		PropellerSerialConnector(LPCWSTR portName);
		void writeChar(char c);
		void writeChars(char c1, char c2);
		void writeChars(char c1, char c2, char c3);
		void writeChars(char c1, char c2, char c3, char c4);
		void writeString(char* string);
		void writeChars(char* string);
		int readChar();
		int writeThenRead(char c);
		int writeThenRead(char c1, char c2);
		void writeThenRead(char c, void* buffer, int count);
		void readChars(void* buffer, int count);
		void readString(char* messageBuffer);
		~PropellerSerialConnector(void);

	private:
	 
		LPCWSTR _portName;
		HANDLE _hSerial;

		void printerr();
};

