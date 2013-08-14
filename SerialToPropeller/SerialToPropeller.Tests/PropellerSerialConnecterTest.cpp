#include "stdafx.h"
#include "CppUnitTest.h"


#include "PropellerSerialConnector.h"  

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AbsFlow { namespace Propeller {
		
	TEST_CLASS(SerialToPropellerTests)
	{
	public:
		
		TEST_METHOD(TestEcho)
		{

			char* messageToSend = "Hello!";
			PropellerSerialConnector connector( L"COM4");
			char receivedMessage[200];

			connector.writeLine(messageToSend);
			connector.readLine(receivedMessage);

			Assert::AreEqual(messageToSend, receivedMessage);
		}

	};

}}