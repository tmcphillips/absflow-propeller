#include "stdafx.h"
#include "PropellerSerialConnector.h"

namespace AbsFlow { namespace Propeller {

PropellerSerialConnector::PropellerSerialConnector(LPCWSTR portName) :
	_portName(portName)
{
	_hSerial = CreateFileW(
		_portName, 
		GENERIC_READ | GENERIC_WRITE, 
		0, 
		0, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL,
		0);

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

PropellerSerialConnector& PropellerSerialConnector::operator<<(char c) {
	writeChar(c);
	return *this;
}

void PropellerSerialConnector::writeCharChar(char c1, char c2) {
	writeChar(c1);
	writeChar(c2);
}

void PropellerSerialConnector::writeInt32(__int32 value)
{
	char *p = (char*) &value;
	for (int i = 0; i < 4; i++) {
	 writeChar(*p++);
	}
}

void PropellerSerialConnector::writeCharInt32(char c, __int32 i)
{ 
	writeChar(c);
	writeInt32(i);
}

void PropellerSerialConnector::writeCharInt32Int32(char c, __int32 i1, __int32 i2)
{ 
	writeChar(c);
	writeInt32(i1);
	writeInt32(i2);
}

__int32 PropellerSerialConnector::writeCharInt32ReadInt32(char c, __int32 i)
{
	writeChar(c);
	writeInt32(i);
	return readInt32();
}

__int32 PropellerSerialConnector::writeCharCharInt32ReadInt32(char c1, char c2, __int32 i) {
	writeChar(c1);
	writeChar(c2);
	writeInt32(i);
	return readInt32();
}

__int32 PropellerSerialConnector::writeCharInt32Int32ReadInt32(char c, __int32 i1, _int32 i2)
{
	writeChar(c);
	writeInt32(i1);
	writeInt32(i2);
	return readInt32();
}

void PropellerSerialConnector::writeLine(char* text) {

	DWORD dwBytesWritten = 0;

	if(!WriteFile(_hSerial, text, strlen(text), &dwBytesWritten, NULL)){
		throw("Error writing string to serial port\n");
	}

	writeChar('\r');
}

char PropellerSerialConnector::readChar() {

	DWORD dwBytesRead = 0;
	char c = 0;

	if (!ReadFile(_hSerial, &c, 1, &dwBytesRead, NULL)) {
		throw("Error reading string from serial port\n");
	}

	return c;
}

void PropellerSerialConnector::writeCharReadArray(char c, void* array, int size)
{
	writeChar(c);
	readArray(array, size);
}

void PropellerSerialConnector::writeCharCharReadArray(char c1, char c2, void* structure, int size)
{
	writeChar(c1);
	writeChar(c2);
	readArray(structure, size);
}

__int32 PropellerSerialConnector::writeCharReadInt32(char c) {
	writeChar(c);
	return readInt32();
}

__int32 PropellerSerialConnector::writeInt32ReadInt32(int i) {
	writeInt32(i);
	return readInt32();
}

__int32 PropellerSerialConnector::writeInt32Int32ReadInt32(int i1, int i2) {
	writeInt32(i1);
	writeInt32(i2);
	return readInt32();
}

__int32 PropellerSerialConnector::readInt32() {
	__int32 value;
	readArray(&value, 4);
	return value;
}

void PropellerSerialConnector::readArray(void* buffer, int count) {

	DWORD dwBytesRead = 0;

	if (!ReadFile(_hSerial, buffer, count, &dwBytesRead, NULL)) {
		throw("Error reading string from serial port\n");
	}
}

void PropellerSerialConnector::readLine(char* text) {

	DWORD dwBytesRead = 0;
	char* tp = text;

	do {
		if (!ReadFile(_hSerial, tp, 1, &dwBytesRead, NULL)) {
			throw("Error reading string from serial port\n");
		}
	} while (*tp++ != '\r');

	*(tp - 1) = 0;
}

}}