#include "stdafx.h"
#include "CppUnitTest.h"

#include "PropellerSerialConnector.h"  


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SerialToPropellerTests
{		
	TEST_CLASS(FifoTests)
	{

	private:

		struct fifo_state {
			char semid;
			char depth;
			char occupancy;
			char eof;
		}; 

		fifo_state state;
		PropellerSerialConnector *propeller;

	public:

		TEST_METHOD_INITIALIZE(TestInitialize)
		{
			propeller =  new PropellerSerialConnector( L"COM4");
		}

		TEST_METHOD_CLEANUP(TestCleanup)
		{
			propeller->writeChar('D');
			delete propeller;
		}

		TEST_METHOD(TestInitializeFifo)
		{
			// initialize fifo
			propeller->writeChars('I', 5);
			
			// verify initial state
			propeller->writeThenRead('Q', &state, 4); 
			Assert::AreEqual(0, (int)state.semid);
			Assert::AreEqual(5, (int)state.depth);
			Assert::AreEqual(0, (int)state.occupancy);
			Assert::AreEqual(0, (int)state.eof);
		}

		TEST_METHOD(TestPut)
		{
			// initialize fifo and verify initial occupancy of zero
			propeller->writeChars('I', 5);
			propeller->writeThenRead('Q', &state, 4); 
			Assert::AreEqual(0, (int)state.occupancy);

			// put one value to the fifo and very occupancy of one
			Assert::AreNotEqual(0, propeller->writeThenRead('P', 1));
			propeller->writeThenRead('Q', &state, 4); 
			Assert::AreEqual(1, (int)state.occupancy);

			// put second value to the fifo and very occupancy of two
			Assert::AreNotEqual(0, propeller->writeThenRead('P', 2));
			propeller->writeThenRead('Q', &state, 4); 
			Assert::AreEqual(2, (int)state.occupancy);
		}

		TEST_METHOD(TestTake)
		{
			// initialize fifo
			propeller->writeChars('I', 5);

			// put three values to fifo
			Assert::AreNotEqual(0, propeller->writeThenRead('P', 1));
			Assert::AreNotEqual(0, propeller->writeThenRead('P', 2));
			Assert::AreNotEqual(0, propeller->writeThenRead('P', 3));

			// query fifo state and verify occupancy of 3
			propeller->writeThenRead('Q', &state, 4); 
			Assert::AreEqual(3, (int)state.occupancy);

			// take value from fifo and verify new occupancy of 2
			Assert::AreNotEqual(0, (int)propeller->writeThenRead('T'));
			Assert::AreEqual(1, propeller->writeThenRead('L'));
			propeller->writeThenRead('Q', &state, 4); 
			Assert::AreEqual(2, (int)state.occupancy);

			// take value from fifo and verify new occupancy of 1
			Assert::AreNotEqual(0, (int)propeller->writeThenRead('T'));
			Assert::AreEqual(2, propeller->writeThenRead('L'));
			propeller->writeThenRead('Q', &state, 4);
			Assert::AreEqual(1, (int)state.occupancy);

			// take value from fifo and verify new occupancy of 0
			Assert::AreNotEqual(0, (int)propeller->writeThenRead('T'));
			Assert::AreEqual(3, propeller->writeThenRead('L'));
			propeller->writeThenRead('Q', &state, 4); 
			Assert::AreEqual(0, (int)state.occupancy);
		}

		TEST_METHOD(TestEndFlow_EmptyFifo)
		{
			// initialize fifo and verify eof is false
			propeller->writeChars('I', 5);
			propeller->writeThenRead('Q', &state, 4); 
			Assert::AreEqual(0, (int)state.eof);

			// send eof to fifo and verify eof is now true
			propeller->writeChar('E');
			propeller->writeThenRead('Q', &state, 4); 
			Assert::AreNotEqual(0, (int)state.eof);		
		}

		TEST_METHOD(TestEndFlow_OccupiedFifo)
		{
			// initialize fifo and put two values in it 
			propeller->writeChars('I', 5);
			Assert::AreNotEqual(0, propeller->writeThenRead('P', 1));
			Assert::AreNotEqual(0, propeller->writeThenRead('P', 2));

			// verify that eof is currently false
			propeller->writeThenRead('Q', &state, 4);
			Assert::AreEqual(0, (int)state.eof);

			// send eof to fifo and verify eof is now true
			propeller->writeChar('E');			
			propeller->writeThenRead('Q', &state, 4);
			Assert::AreNotEqual(0, (int)state.eof);			
		}


		TEST_METHOD(Test_TakeAfterEndFlow_OccupiedFifo)
		{
			// initialize fifo, put two values in it, and send eof signal 
			propeller->writeChars('I', 5);
			Assert::AreNotEqual(0, propeller->writeThenRead('P', 1));
			Assert::AreNotEqual(0, propeller->writeThenRead('P', 2));
			propeller->writeChar('E');

			// take two values from fifo and confirm success for each
			Assert::AreNotEqual(0, (int)propeller->writeThenRead('T'));
			Assert::AreNotEqual(0, (int)propeller->writeThenRead('T'));

			// try to take a value from fifo and confirm failure
			Assert::AreEqual(0, (int)propeller->writeThenRead('T'));
		}
	};
}