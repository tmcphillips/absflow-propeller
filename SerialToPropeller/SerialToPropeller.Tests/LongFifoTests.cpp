#include "stdafx.h"
#include "CppUnitTest.h"

#include "SerialConnection.h" 
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
	AbsFlow::Propeller::SerialConnection propeller;

public:

	TEST_METHOD_INITIALIZE(TestInitialize)
	{
		propeller.open(L"COM4");
	}

	TEST_METHOD_CLEANUP(TestCleanup)
	{
	}

	TEST_METHOD(TestInitializeFifo)
	{
		// initialize fifo
		Propeller::AssertTrue(propeller << 'I' << 5);
			
		// verify initial state
		propeller << 'Q';
		propeller.readChars(&state, sizeof(state));
		Assert::AreEqual(0, state.semid);
		Assert::AreEqual(5, state.depth);
		Assert::AreEqual(0, state.occupancy);
		Assert::AreEqual(0, state.eof);
	}
	 
	TEST_METHOD(TestPut)
	{
		// initialize fifo and verify initial occupancy of zero
		Propeller::AssertTrue(propeller << 'I' << 5);
		propeller << 'Q';
		propeller.readChars(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);

		// put one value to the fifo and very occupancy of one
		Propeller::AssertTrue(propeller << 'P' << 1);
		propeller << 'Q';
		propeller.readChars(&state, sizeof(state));
		Assert::AreEqual(1, state.occupancy);

		// put second value to the fifo and very occupancy of two
		Propeller::AssertTrue(propeller << 'P' << 2);
		propeller << 'Q';
		propeller.readChars(&state, sizeof(state));
		Assert::AreEqual(2, state.occupancy);
	}

	TEST_METHOD(TestTake)
	{
		// initialize fifo
		Propeller::AssertTrue(propeller << 'I' << 5);

		// put three values to fifo
		Propeller::AssertTrue(propeller << 'P' << 1);		
		Propeller::AssertTrue(propeller << 'P' << 2);		
		Propeller::AssertTrue(propeller << 'P' << 3);

		// query fifo state and verify occupancy of 3
		propeller << 'Q';
		propeller.readChars(&state, sizeof(state));
		Assert::AreEqual(3, state.occupancy);

		// Propeller value from fifo and verify new occupancy of 2
		Propeller::AssertTrue(propeller << 'T');
		Propeller::AreEqual(1, propeller << 'L');
		propeller << 'Q';
		propeller.readChars(&state, sizeof(state));
		Assert::AreEqual(2, state.occupancy);

		// take value from fifo and verify new occupancy of 1
		Propeller::AssertTrue(propeller << 'T');
		Propeller::AreEqual(2, propeller << 'L');
		propeller << 'Q';
		propeller.readChars(&state, sizeof(state));
		Assert::AreEqual(1, state.occupancy);

		// take value from fifo and verify new occupancy of 0
		Propeller::AssertTrue(propeller << 'T');
		Propeller::AreEqual(3, propeller << 'L');
		propeller << 'Q';
		propeller.readChars(&state, sizeof(state));;
		Assert::AreEqual(0, state.occupancy);
	}

	TEST_METHOD(TestEndFlow_EmptyFifo)
	{
		// initialize fifo and verify eof is false
		Propeller::AssertTrue(propeller << 'I' << 5);
		propeller << 'Q';
		propeller.readChars(&state, sizeof(state));
		Propeller::IsFalse(state.eof);

		// send eof to fifo and verify eof is now true
		propeller.writeChar('E');
		propeller << 'Q';
		propeller.readChars(&state, sizeof(state));
		Propeller::IsTrue(state.eof);
	}

	TEST_METHOD(TestEndFlow_OccupiedFifo)
	{
		// initialize fifo and put two values in it 
		Propeller::AssertTrue(propeller << 'I' << 5);
		Propeller::AssertTrue(propeller << 'P' << 1);
		Propeller::AssertTrue(propeller << 'P' << 2);

		// verify that eof is currently false
		propeller << 'Q';
		propeller.readChars(&state, sizeof(state));
		Propeller::IsFalse(state.eof);

		// send eof to fifo and verify eof is now true
		propeller.writeChar('E');
		propeller << 'Q';
		propeller.readChars(&state, sizeof(state));
		Propeller::IsTrue(state.eof);
	}


	TEST_METHOD(Test_TakeAfterEndFlow_OccupiedFifo)
	{
		// initialize fifo, put two values in it, and send eof signal 
		Propeller::AssertTrue(propeller << 'I' << 5);
		Propeller::AssertTrue(propeller << 'P' << 1);
		Propeller::AssertTrue(propeller << 'P' << 2);
		propeller.writeChar('E');

		// take two values from fifo and confirm success for each
		Propeller::AssertTrue(propeller << 'T');
		Propeller::AssertTrue(propeller << 'T');

		// try to take a value from fifo and confirm failure
		Propeller::AssertFalse(propeller << 'T');
	}
};