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
			PropellerSerialConnector propeller;
			propeller.open(L"COM4");
			char receivedMessage[200];

			propeller.writeLine(messageToSend);
			propeller.readLine(receivedMessage);

			Assert::AreEqual(messageToSend, receivedMessage);
		}

	};

}}