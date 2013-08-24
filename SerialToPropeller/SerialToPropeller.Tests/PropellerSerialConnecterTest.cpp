#include "stdafx.h"
#include "CppUnitTest.h"

#include "SerialConnection.h"
#include "PropAssert.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace AbsFlow;

TEST_CLASS(SerialToPropellerTests)
{
private:

	Propeller::SerialConnection propeller;

public:

	TEST_METHOD_INITIALIZE(TestInitialize) {
		propeller.open(L"COM4");
	}

	TEST_METHOD_CLEANUP(TestCleanup) {
	}

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
		propeller << 'L';
		propeller.writeInt32(42);
		x = propeller.readInt32();
		Assert::AreEqual(42, x);
	}

	TEST_METHOD(TestInt32PutGet) {
		__int32 i;
		propeller << 'L';
		propeller << 42;
		propeller >> i;
		Assert::AreEqual(42, i);
	}

	TEST_METHOD(TestTwoInt32PutGet) {
		__int32 x, y;
		propeller << 'T';
		propeller << 5;
		propeller << 7;
		propeller >> x;
		propeller >> y;
		Assert::AreEqual(5, x);
		Assert::AreEqual(7, y);
	}


	TEST_METHOD(Test_WriteInt32_ReadInt32_TwoInts) {
		__int32 x, y;
		propeller << 'T';
		propeller.writeInt32(5);
		propeller.writeInt32(7);
		x = propeller.readInt32();
		y = propeller.readInt32();
		Assert::AreEqual(5, x);
		Assert::AreEqual(7, y);
	}
};