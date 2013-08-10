#include "stdafx.h"
#include "CppUnitTest.h"

#include "PropellerSerialConnector.h"  


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SerialToPropellerTests
{		
	TEST_CLASS(SerialToPropellerTests)
	{
	public:
		
		TEST_METHOD(TestEcho)
		{

			char* messageToSend = "Hello!\r";
			PropellerSerialConnector connector( L"COM4");
			char receivedMessage[200];

			connector.writeString(messageToSend);
			connector.readString(receivedMessage);

			Assert::AreEqual(messageToSend, receivedMessage);
		}

	};
}