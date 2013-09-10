#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace AbsFlow;

TEST_CLASS(SerialConnectionTests)
{

private:

	Propeller::SerialConnection propeller{ "COM4" };

public:

	TEST_METHOD(TestStringPutGet) {
		std::string stringToSend = "Hello!";
		std::string receivedString;
		propeller << 'S' << stringToSend;
		propeller >> receivedString;
		Assert::AreEqual(stringToSend, receivedString);
	}

	TEST_METHOD(TestCharPutGet) {
		char c;
		propeller << 'A';
		propeller << 'x';
		propeller >> c;
		Assert::AreEqual('x', c);
	}

	TEST_METHOD(Test_PutInt32_GetInt32) {
		__int32 x;
		propeller << 'L' << 42;
		propeller >> x;
		Assert::AreEqual(42, x);
	}

	TEST_METHOD(TestInt32PutGet) {
		__int32 i;
		propeller << 'L';
		propeller << 42;
		propeller >> i;
		Assert::AreEqual(42, i);
	}

	TEST_METHOD(TestPutTwoInt32GetTwoInt32) {
		__int32 x, y;
		propeller << 'T';
		propeller << 5 << 7;
		propeller >> x >> y;
		Assert::AreEqual(5, x);
		Assert::AreEqual(7, y);
	}
};