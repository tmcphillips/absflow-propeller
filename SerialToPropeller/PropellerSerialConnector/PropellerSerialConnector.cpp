#include "stdafx.h"
#include "PropellerSerialConnector.h"


PropellerSerialConnector::PropellerSerialConnector(LPCWSTR portName) :
	_portName(portName)
{
	_hSerial = CreateFileW(_portName, 
		GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (_hSerial == INVALID_HANDLE_VALUE) {
		if(GetLastError() == ERROR_FILE_NOT_FOUND) {
			throw "Serial port does not exist";
		} else {
			throw "Error connecting to propeller";
		}
	}

	DCB dcbSerialParams = {0};
	dcbSerialParams.DCBlength=sizeof(dcbSerialParams);
	if (!GetCommState(_hSerial, &dcbSerialParams)) {
		throw "Error getting serial port state";
	}
	dcbSerialParams.BaudRate=CBR_115200;
	dcbSerialParams.ByteSize=8;
	dcbSerialParams.StopBits=ONESTOPBIT;
	dcbSerialParams.Parity=NOPARITY;
	if (!SetCommState(_hSerial, &dcbSerialParams)){
		throw "Error setting serial port state";
	}	

	COMMTIMEOUTS timeouts={0};
	timeouts.ReadIntervalTimeout=MAXDWORD -1;
	timeouts.ReadTotalTimeoutConstant=MAXDWORD -1;
	timeouts.ReadTotalTimeoutMultiplier=MAXDWORD -1;
	timeouts.WriteTotalTimeoutConstant=50;
	timeouts.WriteTotalTimeoutMultiplier=10;
	if(!SetCommTimeouts(_hSerial, &timeouts)){
		throw "Error setting serial port timout configuration";
	}
}


PropellerSerialConnector::~PropellerSerialConnector(void)
{
	CloseHandle(_hSerial);
}

void PropellerSerialConnector::printerr() {

	WCHAR lastError[1024];

	FormatMessageW(	FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					GetLastError(),
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					lastError,
					1024,
					NULL);
}

void PropellerSerialConnector::writeChar(char c) {

	DWORD dwBytesWritten = 0;

	if(!WriteFile(_hSerial, &c, 1, &dwBytesWritten, NULL)){
		throw("Error writing character to serial port\n");
	}
}

void PropellerSerialConnector::writeChars(char c1, char c2) {
	writeChar(c1);
	writeChar(c2);
}

void PropellerSerialConnector::writeChars(char c1, char c2, char c3) {
	writeChar(c1);
	writeChar(c2);
	writeChar(c3);
}

void PropellerSerialConnector::writeChars(char c1, char c2, char c3, char c4) {
	writeChar(c1);
	writeChar(c2);
	writeChar(c3);
	writeChar(c4);
}

void PropellerSerialConnector::writeChars(char* message) {

	DWORD dwBytesWritten = 0;

	if(!WriteFile(_hSerial, message, strlen(message), &dwBytesWritten, NULL)){
		throw("Error writing string to serial port\n");
	}
}

void PropellerSerialConnector::writeString(char* message) {

	DWORD dwBytesWritten = 0;

	if(!WriteFile(_hSerial, message, strlen(message), &dwBytesWritten, NULL)){
		throw("Error writing string to serial port\n");
	}
}

int PropellerSerialConnector::readChar() {

	DWORD dwBytesRead = 0;
	int c = 0;

	if (!ReadFile(_hSerial, &c, 1, &dwBytesRead, NULL)) {
		throw("Error reading string from serial port\n");
	}

	return c;
}

int PropellerSerialConnector::writeThenRead(char c) 
{
	writeChar(c);
	return readChar();
}

int PropellerSerialConnector::writeThenRead(char c1, char c2)
{
	writeChar(c1);
	writeChar(c2);
	return readChar();
}

void PropellerSerialConnector::writeThenRead(char c, void* buffer, int count)
{
	writeChar(c);
	readChars(buffer, count);
}


void PropellerSerialConnector::readChars(void* buffer, int count) {

	DWORD dwBytesRead = 0;

	if (!ReadFile(_hSerial, buffer, count, &dwBytesRead, NULL)) {
		throw("Error reading string from serial port\n");
	}
}

void PropellerSerialConnector::readString(char* messageBuffer) {

	DWORD dwBytesRead = 0;
	char* nextChar = messageBuffer;
	char lastRead = 0;

	do {
		if (!ReadFile(_hSerial, nextChar, 1, &dwBytesRead, NULL)) {
			throw("Error reading string from serial port\n");
		}
	} while (*nextChar++ != '\r');

	*nextChar = 0;
}