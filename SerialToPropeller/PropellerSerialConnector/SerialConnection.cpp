#include "stdafx.h"
#include "SerialConnection.h"

namespace AbsFlow { namespace Propeller {

SerialConnection::SerialConnection(LPCWSTR portName) {

	_hSerial = CreateFileW(
		portName, 
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



SerialConnection::~SerialConnection() {
	CloseHandle(_hSerial);
}

void SerialConnection::printerr() {

	WCHAR lastError[1024];

	FormatMessageW(	FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					GetLastError(),
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					lastError,
					1024,
					NULL);
}

void SerialConnection::writeChar(char c) {

	DWORD dwBytesWritten = 0;

	if(!WriteFile(_hSerial, &c, 1, &dwBytesWritten, NULL)){
		throw("Error writing character to serial port\n");
	}
}

void SerialConnection::writeInt32(__int32 value) {
	char *p = (char*) &value;
	for (int i = 0; i < 4; i++) {
		writeChar(*p++);
	}
}

void SerialConnection::writeString(std::string& s) {
	for (auto c = s.begin(); c != s.end(); c++) {
		writeChar(*c);
	}
	writeChar(0);
}

char SerialConnection::readChar() {

	DWORD dwBytesRead = 0;
	char c = 0;

	if (!ReadFile(_hSerial, &c, 1, &dwBytesRead, NULL)) {
		throw("Error reading string from serial port\n");
	}

	return c;
}

__int32 SerialConnection::readInt32() {
	__int32 value;
	readChars(&value, 4);
	return value;
}

void SerialConnection::readChars(void* buffer, int count) {

	DWORD dwBytesRead = 0;

	if (!ReadFile(_hSerial, buffer, count, &dwBytesRead, NULL)) {
		throw("Error reading string from serial port\n");
	}
}

SerialConnection& SerialConnection::operator<<(char c) {
	writeChar(c);
	return *this;
}

SerialConnection& SerialConnection::operator<<(__int32 i) {
	writeInt32(i);
	return *this;
}

SerialConnection&  SerialConnection::operator<<(std::string& s) {
	writeString(s);
	return *this;
}

SerialConnection& SerialConnection::operator>>(char& c) {
	c = readChar();
	return *this;
}

SerialConnection& SerialConnection::operator>>(__int32& i) {
	i = readInt32();
	return *this;
}

SerialConnection& SerialConnection::operator>>(std::string& s) {

	DWORD dwBytesRead = 0;
	char c;
	s.clear();

	while (true) {

		if (!ReadFile(_hSerial, &c, 1, &dwBytesRead, NULL)) {
			throw("Error reading string from serial port\n");
		}

		if (c) {
			s += c;
		}
		else {
			break;
		}
	}

	return *this;
}


}}