
#include "SerialConnection.h"

namespace AbsFlow { namespace Propeller {

/* Public methods */

SerialConnection::SerialConnection(std::string portName) {
	WCHAR wPortName[256];
	stringToWideChars(portName, wPortName);
	openSerialPort(wPortName);
	setPortParameters(CBR_115200, 8, ONESTOPBIT, NOPARITY);
	setPortTimeouts();
}

SerialConnection::~SerialConnection() {
	CloseHandle(_handle);
}

void SerialConnection::writeBytes(const void* source, int count) {
	DWORD dwBytesWritten = 0;
	if (!WriteFile(_handle, source, count, &dwBytesWritten, NULL) || dwBytesWritten != count){
		throw("Error writing to serial port\n");
	}
}

void SerialConnection::readBytes(void* destination, int count) {
	DWORD dwBytesRead = 0;
	if (!ReadFile(_handle, destination, count, &dwBytesRead, NULL) || dwBytesRead != count) {
		throw("Error reading string from serial port\n");
	}
}

SerialConnection& SerialConnection::operator>>(std::string& s) {
	unsigned char b;
	s.clear();
	while (b = readByte()) {
		s += b;
	}
	return *this;
}

/* Private methods */

void SerialConnection::stringToWideChars(std::string source, WCHAR* destination) {
	MultiByteToWideChar(CP_ACP, 0, source.c_str(), -1, destination, 256);
}


void SerialConnection::openSerialPort(WCHAR* portName) {

	_handle = CreateFileW(
		portName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);

	if (_handle == INVALID_HANDLE_VALUE) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND) {
			throw "Serial port does not exist";
		}
		else {
			throw "Error connecting to propeller";
		}
	}
}

void SerialConnection::setPortParameters(DWORD baudRate, BYTE byteSize, BYTE stopBits, BYTE parity) {

	DCB dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

	if (!GetCommState(_handle, &dcbSerialParams)) {
		throw "Error getting serial port parameters";
	}

	dcbSerialParams.BaudRate = baudRate;
	dcbSerialParams.ByteSize = byteSize;
	dcbSerialParams.StopBits = stopBits;
	dcbSerialParams.Parity = parity;

	if (!SetCommState(_handle, &dcbSerialParams)){
		throw "Error setting serial port parameters";
	}
}

void SerialConnection::setPortTimeouts() {

	COMMTIMEOUTS timeouts = { 0 };
	timeouts.ReadIntervalTimeout = MAXDWORD - 1;
	timeouts.ReadTotalTimeoutConstant = MAXDWORD - 1;
	timeouts.ReadTotalTimeoutMultiplier = MAXDWORD - 1;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;

	if (!SetCommTimeouts(_handle, &timeouts)) {
		throw "Error setting serial port timout configuration";
	}
}

void SerialConnection::printerr() {

	WCHAR lastError[1024];

	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		lastError,
		1024,
		NULL);
}

}}