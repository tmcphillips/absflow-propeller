#include "stdafx.h"
#include "CppUnitTest.h"

#include "PropellerSerialConnector.h" 
#include "PropAssert.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace AbsFlow;
	
TEST_CLASS(LongFifoTests)
{

private:

	struct fifo_state {
		__int32 semid;
		__int32 depth;
		__int32 occupancy;
		__int32 eof;
	}; 

	fifo_state state;
	Propeller::PropellerSerialConnector *propeller;

public:

	TEST_METHOD_INITIALIZE(TestInitialize)
	{
		propeller =  new Propeller::PropellerSerialConnector( L"COM4");
	}

	TEST_METHOD_CLEANUP(TestCleanup)
	{
		delete propeller;
	}

	TEST_METHOD(TestInitializeFifo)
	{
		// initialize fifo
		propeller->writeCharInt32('I', 5);
			
		// verify initial state
		propeller->writeCharReadArray('Q', &state, sizeof(state)); 
		//Assert::AreEqual(1, state.semid);
		Assert::AreEqual(5, state.depth);
		Assert::AreEqual(0, state.occupancy);
		Assert::AreEqual(0, state.eof);
	}
	 
	TEST_METHOD(TestPut)
	{
		// initialize fifo and verify initial occupancy of zero
		propeller->writeCharInt32('I', 5);
		propeller->writeCharReadArray('Q', &state, sizeof(state)); 
		Assert::AreEqual(0, state.occupancy);

		// put one value to the fifo and very occupancy of one
		Propeller::IsTrue(propeller->writeCharInt32ReadInt32('P', 1));
		propeller->writeCharReadArray('Q', &state, sizeof(state)); 
		Assert::AreEqual(1, state.occupancy);

		// put second value to the fifo and very occupancy of two
		Propeller::IsTrue(propeller->writeCharInt32ReadInt32('P', 2));
		propeller->writeCharReadArray('Q', &state, sizeof(state)); 
		Assert::AreEqual(2, state.occupancy);
	}

	TEST_METHOD(TestTake)
	{
		// initialize fifo
		propeller->writeCharInt32('I', 5);

		// put three values to fifo
		Propeller::IsTrue(propeller->writeCharInt32ReadInt32('P', 1));
		Propeller::IsTrue(propeller->writeCharInt32ReadInt32('P', 2));
		Propeller::IsTrue(propeller->writeCharInt32ReadInt32('P', 3));

		// query fifo state and verify occupancy of 3
		propeller->writeCharReadArray('Q', &state, sizeof(state)); 
		Assert::AreEqual(3, state.occupancy);

		// Propeller value from fifo and verify new occupancy of 2
		Propeller::IsTrue(propeller->writeCharReadInt32('T'));
		Assert::AreEqual(1, propeller->writeCharReadInt32('L'));
		propeller->writeCharReadArray('Q', &state, sizeof(state)); 
		Assert::AreEqual(2, state.occupancy);

		// take value from fifo and verify new occupancy of 1
		Propeller::IsTrue(propeller->writeCharReadInt32('T'));
		Assert::AreEqual(2, propeller->writeCharReadInt32('L'));
		propeller->writeCharReadArray('Q', &state, sizeof(state));
		Assert::AreEqual(1, state.occupancy);

		// take value from fifo and verify new occupancy of 0
		Propeller::IsTrue(propeller->writeCharReadInt32('T'));
		Assert::AreEqual(3, propeller->writeCharReadInt32('L'));
		propeller->writeCharReadArray('Q', &state, sizeof(state)); 
		Assert::AreEqual(0, state.occupancy);
	}

	TEST_METHOD(TestEndFlow_EmptyFifo)
	{
		// initialize fifo and verify eof is false
		propeller->writeCharInt32('I', 5);
		propeller->writeCharReadArray('Q', &state, sizeof(state)); 
		Propeller::IsFalse(state.eof);

		// send eof to fifo and verify eof is now true
		propeller->writeChar('E');
		propeller->writeCharReadArray('Q', &state, sizeof(state)); 
		Propeller::IsTrue(state.eof);
	}

	TEST_METHOD(TestEndFlow_OccupiedFifo)
	{
		// initialize fifo and put two values in it 
		propeller->writeCharInt32('I', 5);
		Propeller::IsTrue(propeller->writeCharInt32ReadInt32('P', 1));
		Propeller::IsTrue(propeller->writeCharInt32ReadInt32('P', 2));

		// verify that eof is currently false
		propeller->writeCharReadArray('Q', &state, sizeof(state));
		Propeller::IsFalse(state.eof);

		// send eof to fifo and verify eof is now true
		propeller->writeChar('E');
		propeller->writeCharReadArray('Q', &state, sizeof(state));
		Propeller::IsTrue(state.eof);
	}


	TEST_METHOD(Test_TakeAfterEndFlow_OccupiedFifo)
	{
		// initialize fifo, put two values in it, and send eof signal 
		propeller->writeCharInt32('I', 5);
		Propeller::IsTrue(propeller->writeCharInt32ReadInt32('P', 1));
		Propeller::IsTrue(propeller->writeCharInt32ReadInt32('P', 2));
		propeller->writeChar('E');

		// take two values from fifo and confirm success for each
		Propeller::IsTrue(propeller->writeCharReadInt32('T'));
		Propeller::IsTrue(propeller->writeCharReadInt32('T'));

		// try to take a value from fifo and confirm failure
		Propeller::IsFalse(propeller->writeCharReadInt32('T'));
	}
};