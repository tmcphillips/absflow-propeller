#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace AbsFlow;
	
TEST_CLASS(NondetermMergeActorTests)
{

private:

	struct fifo_state {
		__int32 semid;
		__int32 depth;
		__int32 occupancy;
		__int32 eof;
		__int32 next_write;
		__int32 next_read;
		__int32 buffer_start;
		__int32 buffer_end;
	}; 

	fifo_state state;
	Propeller::SerialConnection propeller{ "COM4" };

public:

	TEST_METHOD_CLEANUP(TestCleanup)
	{
		Propeller::AssertTrue(propeller << 'D');
	}

	TEST_METHOD(TestInitializeActor)
	{ 	 
		// initialize actor
		Propeller::AssertTrue(propeller << 'I' << 5 << 6 << 7);

		// verify initial state of input fifos
		propeller << 'Q' << 1;
		propeller.readBytes(&state, sizeof(state)); 
		Assert::AreEqual(0, state.semid);
		Assert::AreEqual(5, state.depth);
		Assert::AreEqual(0, state.occupancy);
		Assert::AreEqual(0, state.eof);

		propeller << 'Q' << 2;
		propeller.readBytes(&state, sizeof(state)); 
		Assert::AreEqual(1, state.semid);
		Assert::AreEqual(6, state.depth);
		Assert::AreEqual(0, state.occupancy);
		Assert::AreEqual(0, state.eof);

		// verify initial state of output fifo
		propeller << 'Q' << 0;
		propeller.readBytes(&state, sizeof(state)); 
		Assert::AreEqual(2, state.semid);
		Assert::AreEqual(7, state.depth);
		Assert::AreEqual(0, state.occupancy);
		Assert::AreEqual(0, state.eof);
	}
	 
	TEST_METHOD(TestPut)
	{
		// initialize actor
		Propeller::AssertTrue(propeller << 'I' << 5 << 6 << 7);

		// query output fifo state and verify occupancy of 0
		propeller << 'Q' << 0;
		propeller.readBytes(&state, sizeof(state)); 
		Assert::AreEqual(0, state.occupancy);

		// put one value to the input fifo and verify output fifo occupancy of one
		Propeller::AssertTrue(propeller << 'P' << 1 << 1);
		Propeller::AssertTrue(propeller << 'W');

		// query output fifo state and verify occupancy of 0
		propeller << 'Q' << 0;
		propeller.readBytes(&state, sizeof(state)); 
		Assert::AreEqual(1, state.occupancy);

		// put second value to the input fifo and verify output fifo occupancy of two
		Propeller::AssertTrue(propeller << 'P' << 2 << 10);
		Propeller::AssertTrue(propeller << 'W');

		// query output fifo state and verify occupancy of 2
		propeller << 'Q' << 0;
		propeller.readBytes(&state, sizeof(state)); 
		Assert::AreEqual(2, state.occupancy);
	}

	TEST_METHOD(TestTake)
	{
		// initialize actor
		Propeller::AssertTrue(propeller << 'I' << 5 << 6 << 7);

		// put three values to input fifo
		Propeller::AssertTrue(propeller << 'P' << 1 << 1);

		Propeller::AssertTrue(propeller << 'P' << 2 << 10);
		Propeller::AssertTrue(propeller << 'W');

		// query output fifo state and verify occupancy of 2
		propeller << 'Q' << 0;
		propeller.readBytes(&state, sizeof(state)); 
		Assert::AreEqual(2, state.occupancy);

		// take value from fifo
		__int32 value;
		Propeller::AssertTrue(propeller << 'T');
		propeller >> value;
		Assert::AreEqual(1, value);

		// take value from fifo
		Propeller::AssertTrue(propeller << 'T');
		propeller >> value;
		Assert::AreEqual(10, value);

		// query output fifo state and verify occupancy of 0
		propeller << 'Q' << 0;
		propeller.readBytes(&state, sizeof(state)); 
		Assert::AreEqual(0, state.occupancy);
	}

	TEST_METHOD(TestEndFlow_EmptyInputFifos)
	{
		// initialize actor
		Propeller::AssertTrue(propeller << 'I' << 5 << 6 << 7);

		propeller << 'Q' << 0;
		propeller.readBytes(&state, sizeof(state)); 
		Propeller::IsFalse(state.eof);

		propeller << 'E' << 1;
		propeller << 'Q' << 0;
		propeller.readBytes(&state, sizeof(state)); 
		Propeller::IsFalse(state.eof);

		propeller << 'E' << 2;
		propeller << 'Q' << 0;
		propeller.readBytes(&state, sizeof(state)); 
		Propeller::IsTrue(state.eof);
	}

	TEST_METHOD(TestEndFlow_OccupiedFifo)
	{
		// initialize actor
		Propeller::AssertTrue(propeller << 'I' << 5 << 6 << 7);

		// put three values to input fifo
		// put one value to each of the three inputs
		Propeller::AssertTrue(propeller << 'P' << 1 << 1);
		Propeller::AssertTrue(propeller << 'P' << 2 << 10);

		// wait for the actor to consume all inputs
		Propeller::AssertTrue(propeller << 'W');

		// verify that eof on the actor output is currently false
		propeller << 'Q' << 0;
		propeller.readBytes(&state, sizeof(state)); 
		Propeller::IsFalse(state.eof);

		propeller << 'E' << 1;
		propeller << 'Q' << 0;
		propeller.readBytes(&state, sizeof(state)); 
		Propeller::IsFalse(state.eof);

		propeller << 'E' << 2;
		propeller << 'Q' << 0;
		propeller.readBytes(&state, sizeof(state)); 
		Propeller::IsTrue(state.eof);
	}

	TEST_METHOD(Test_TakeAfterEndFlow_OccupiedFifo)
	{
		// initialize actor
		Propeller::AssertTrue(propeller << 'I' << 5 << 6 << 7);
		
		// put one value to each of the three inputs
		Propeller::AssertTrue(propeller << 'P' << 1 << 1);
		Propeller::AssertTrue(propeller << 'P' << 2 << 10);

		// wait for the actor to consume all inputs
		Propeller::AssertTrue(propeller << 'W');

		// end flow on each input
		propeller << 'E' << 1;
		propeller << 'E' << 2;
		
		__int32 value;

		// take three values from actor output and confirm success for each
		Propeller::AssertTrue(propeller << 'T');
		propeller >> value;
		Propeller::AssertTrue(propeller << 'T');
		propeller >> value;

		// try to take a value from output and confirm failure
		Propeller::AssertFalse(propeller << 'T');
	}
};