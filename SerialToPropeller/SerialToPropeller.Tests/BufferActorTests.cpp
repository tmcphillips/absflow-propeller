#include "stdafx.h"
#include "CppUnitTest.h"

#include "PropellerSerialConnector.h" 
#include "PropAssert.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace AbsFlow;
	
TEST_CLASS(BufferActorTests)
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
	Propeller::PropellerSerialConnector *propeller;

public:

	TEST_METHOD_INITIALIZE(TestInitialize)
	{
		propeller =  new Propeller::PropellerSerialConnector( L"COM4");
	}

	TEST_METHOD_CLEANUP(TestCleanup)
	{
		Propeller::IsTrue(propeller->writeCharReadInt32('D'));
		delete propeller;
	}

	TEST_METHOD(TestInitializeActor)
	{ 	 
		// initialize actor
		Propeller::IsTrue(propeller->writeCharInt32Int32ReadInt32('I', 5, 7));

		// verify initial state of input fifo
		propeller->writeCharCharReadArray('Q', 'i', &state, sizeof(state)); 
		Assert::AreEqual(0, state.semid);
		Assert::AreEqual(5, state.depth);
		Assert::AreEqual(0, state.occupancy);
		Assert::AreEqual(0, state.eof);

		Propeller::IsTrue(propeller->writeCharReadInt32('W'));

		// verify initial state of output fifo
		propeller->writeCharCharReadArray('Q', 'o', &state, sizeof(state)); 
		Assert::AreEqual(1, state.semid);
		Assert::AreEqual(7, state.depth);
		Assert::AreEqual(0, state.occupancy);
		Assert::AreEqual(0, state.eof);
	}
	 
	TEST_METHOD(TestPut)
	{
		Propeller::IsTrue(propeller->writeCharInt32Int32ReadInt32('I', 5, 7));

		propeller->writeCharCharReadArray('Q', 'i', &state, sizeof(state)); 
		Assert::AreEqual(0, state.occupancy);

		propeller->writeCharCharReadArray('Q', 'o', &state, sizeof(state)); 
		Assert::AreEqual(0, state.occupancy);

		// put one value to the input fifo and verify output fifo occupancy of one
		Propeller::IsTrue(propeller->writeCharInt32ReadInt32('P', 1));
		Propeller::IsTrue(propeller->writeCharReadInt32('W'));
		propeller->writeCharCharReadArray('Q', 'o', &state, sizeof(state)); 
		Assert::AreEqual(1, state.occupancy);

		// put second value to the input fifo and verify output fifo occupancy of two
		Propeller::IsTrue(propeller->writeCharInt32ReadInt32('P', 2));
		Propeller::IsTrue(propeller->writeCharReadInt32('W'));
		propeller->writeCharCharReadArray('Q', 'o', &state, sizeof(state)); 
		Assert::AreEqual(2, state.occupancy);
	}

	TEST_METHOD(TestTake)
	{
		// initialize fifo
		Propeller::IsTrue(propeller->writeCharInt32Int32ReadInt32('I', 5, 7));

		propeller->writeCharCharReadArray('Q', 'i', &state, sizeof(state));

		// put three values to input fifo
		Propeller::IsTrue(propeller->writeCharInt32ReadInt32('P', 1));
		Propeller::IsTrue(propeller->writeCharInt32ReadInt32('P', 2));
		Propeller::IsTrue(propeller->writeCharInt32ReadInt32('P', 3));

		// wait for actor to drain input fifo
		Propeller::IsTrue(propeller->writeCharReadInt32('W'));

		// query fifo state and verify occupancy of 3
		propeller->writeCharCharReadArray('Q', 'o', &state, sizeof(state)); 
		Assert::AreEqual(3, state.occupancy);

		// Propeller value from fifo and verify new occupancy of 2
		Propeller::IsTrue(propeller->writeCharReadInt32('T'));
		Assert::AreEqual(1, propeller->writeCharReadInt32('L'));
		propeller->writeCharCharReadArray('Q', 'o', &state, sizeof(state));
		Assert::AreEqual(2, state.occupancy);

		// take value from fifo and verify new occupancy of 1
		Propeller::IsTrue(propeller->writeCharReadInt32('T'));
		Assert::AreEqual(2, propeller->writeCharReadInt32('L'));
		propeller->writeCharCharReadArray('Q', 'o', &state, sizeof(state));
		Assert::AreEqual(1, state.occupancy);

		// take value from fifo and verify new occupancy of 0
		Propeller::IsTrue(propeller->writeCharReadInt32('T'));
		Assert::AreEqual(3, propeller->writeCharReadInt32('L'));
		propeller->writeCharCharReadArray('Q', 'o', &state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
	}

	TEST_METHOD(TestEndFlow_EmptyFifo)
	{
		// initialize fifo and verify eof is false
		Propeller::IsTrue(propeller->writeCharInt32Int32ReadInt32('I', 5, 7));

		propeller->writeCharCharReadArray('Q', 'o', &state, sizeof(state)); 
		Propeller::IsFalse(state.eof);

		// send eof to fifo and verify eof is now true
		propeller->writeChar('E');
		propeller->writeCharCharReadArray('Q', 'o', &state, sizeof(state));
		Propeller::IsTrue(state.eof);
	}

	TEST_METHOD(TestEndFlow_OccupiedFifo)
	{
		// initialize fifo and put two values in it 
		Propeller::IsTrue(propeller->writeCharInt32Int32ReadInt32('I', 5, 7));

		Propeller::IsTrue(propeller->writeCharInt32ReadInt32('P', 1));
		Propeller::IsTrue(propeller->writeCharInt32ReadInt32('P', 2));

		// verify that eof is currently false
		propeller->writeCharCharReadArray('Q', 'o', &state, sizeof(state));
		Propeller::IsFalse(state.eof);

		// send eof to fifo and verify eof is now true
		propeller->writeChar('E');
		propeller->writeCharCharReadArray('Q', 'o', &state, sizeof(state));
		Propeller::IsTrue(state.eof);
	}


	TEST_METHOD(Test_TakeAfterEndFlow_OccupiedFifo)
	{
		// initialize fifo, put two values in it, and send eof signal 
		Propeller::IsTrue(propeller->writeCharInt32Int32ReadInt32('I', 5, 7));

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