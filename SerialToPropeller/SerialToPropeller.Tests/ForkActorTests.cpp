#include "stdafx.h"
#include "CppUnitTest.h"

#include "PropellerSerialConnector.h" 
#include "PropAssert.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace AbsFlow;
	
TEST_CLASS(ForkActorTests)
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
	AbsFlow::Propeller::PropellerSerialConnector propeller;

public:

	TEST_METHOD_INITIALIZE(TestInitialize)
	{
		propeller.open(L"COM4");
	}

	TEST_METHOD_CLEANUP(TestCleanup)
	{
		Propeller::IsTrue(propeller.writeCharReadInt32('D'));
		propeller.close();
	}

	TEST_METHOD(TestInitializeActor)
	{ 	 
		// initialize actor
		propeller.writeChar('I');
		propeller.writeInt32(5);
		propeller.writeInt32(6);
		propeller.writeInt32(7);
		propeller.writeInt32(8);
		propeller.writeInt32(9);
		Propeller::IsTrue(propeller.readInt32());

		// verify initial state of input fifo
		propeller.writeCharCharReadArray('Q', 0, &state, sizeof(state)); 
		Assert::AreEqual(0, state.semid);
		Assert::AreEqual(5, state.depth);
		Assert::AreEqual(0, state.occupancy);
		Assert::AreEqual(0, state.eof);

		Propeller::IsTrue(propeller.writeCharReadInt32('W'));

		// verify initial state of output fifo
		propeller.writeCharCharReadArray('Q', 1, &state, sizeof(state)); 
		Assert::AreEqual(1, state.semid);
		Assert::AreEqual(6, state.depth);
		Assert::AreEqual(0, state.occupancy);
		Assert::AreEqual(0, state.eof);
	}
	 
	TEST_METHOD(TestPut)
	{
		// initialize actor
		propeller.writeChar('I');
		propeller.writeInt32(5);
		propeller.writeInt32(6);
		propeller.writeInt32(7);
		propeller.writeInt32(8);
		propeller.writeInt32(9);
		Propeller::IsTrue(propeller.readInt32());

		propeller.writeCharCharReadArray('Q', 0, &state, sizeof(state)); 
		Assert::AreEqual(0, state.occupancy);

		propeller.writeCharCharReadArray('Q', 1, &state, sizeof(state)); 
		Assert::AreEqual(0, state.occupancy);
		propeller.writeCharCharReadArray('Q', 2, &state, sizeof(state)); 
		Assert::AreEqual(0, state.occupancy);
		propeller.writeCharCharReadArray('Q', 3, &state, sizeof(state)); 
		Assert::AreEqual(0, state.occupancy);
		propeller.writeCharCharReadArray('Q', 4, &state, sizeof(state)); 
		Assert::AreEqual(0, state.occupancy);

		// put one value to the input fifo and verify output fifo occupancy of one
		Propeller::IsTrue(propeller.writeCharInt32ReadInt32('P', 1));
		Propeller::IsTrue(propeller.writeCharReadInt32('W'));
		propeller.writeCharCharReadArray('Q', 1, &state, sizeof(state)); 
		Assert::AreEqual(1, state.occupancy);
		propeller.writeCharCharReadArray('Q', 2, &state, sizeof(state)); 
		Assert::AreEqual(1, state.occupancy);
		propeller.writeCharCharReadArray('Q', 3, &state, sizeof(state)); 
		Assert::AreEqual(1, state.occupancy);
		propeller.writeCharCharReadArray('Q', 4, &state, sizeof(state)); 
		Assert::AreEqual(1, state.occupancy);

		// put second value to the input fifo and verify output fifo occupancy of two
		Propeller::IsTrue(propeller.writeCharInt32ReadInt32('P', 2));
		Propeller::IsTrue(propeller.writeCharReadInt32('W'));
		propeller.writeCharCharReadArray('Q', 1, &state, sizeof(state)); 
		Assert::AreEqual(2, state.occupancy);
		propeller.writeCharCharReadArray('Q', 2, &state, sizeof(state)); 
		Assert::AreEqual(2, state.occupancy);
		propeller.writeCharCharReadArray('Q', 3, &state, sizeof(state)); 
		Assert::AreEqual(2, state.occupancy);
		propeller.writeCharCharReadArray('Q', 4, &state, sizeof(state)); 
		Assert::AreEqual(2, state.occupancy);
	}

	TEST_METHOD(TestTake)
	{
		// initialize actor
		propeller.writeChar('I');
		propeller.writeInt32(5);
		propeller.writeInt32(6);
		propeller.writeInt32(7);
		propeller.writeInt32(8);
		propeller.writeInt32(9);
		Propeller::IsTrue(propeller.readInt32());

		// put three values to input fifo
		Propeller::IsTrue(propeller.writeCharInt32ReadInt32('P', 1));
		Propeller::IsTrue(propeller.writeCharInt32ReadInt32('P', 2));
		Propeller::IsTrue(propeller.writeCharInt32ReadInt32('P', 3));

		// wait for actor to drain input fifo
		Propeller::IsTrue(propeller.writeCharReadInt32('W'));

		// query fifo state and verify occupancy of 3
		propeller.writeCharCharReadArray('Q', 1, &state, sizeof(state)); 
		Assert::AreEqual(3, state.occupancy);
		propeller.writeCharCharReadArray('Q', 2, &state, sizeof(state)); 
		Assert::AreEqual(3, state.occupancy);
		propeller.writeCharCharReadArray('Q', 3, &state, sizeof(state)); 
		Assert::AreEqual(3, state.occupancy);
		propeller.writeCharCharReadArray('Q', 4, &state, sizeof(state)); 
		Assert::AreEqual(3, state.occupancy);

		// Propeller value from fifo and verify new occupancy of 2
		propeller.writeChar('T');
		Propeller::IsTrue(propeller.readInt32());
		Propeller::IsTrue(propeller.readInt32());
		Propeller::IsTrue(propeller.readInt32());
		Propeller::IsTrue(propeller.readInt32());
		propeller.writeChar('L');
		Assert::AreEqual(1, propeller.readInt32());
		Assert::AreEqual(1, propeller.readInt32());
		Assert::AreEqual(1, propeller.readInt32());
		Assert::AreEqual(1, propeller.readInt32());
		propeller.writeCharCharReadArray('Q', 1, &state, sizeof(state));
		Assert::AreEqual(2, state.occupancy);
		propeller.writeCharCharReadArray('Q', 2, &state, sizeof(state));
		Assert::AreEqual(2, state.occupancy);
		propeller.writeCharCharReadArray('Q', 3, &state, sizeof(state));
		Assert::AreEqual(2, state.occupancy);
		propeller.writeCharCharReadArray('Q', 4, &state, sizeof(state));
		Assert::AreEqual(2, state.occupancy);

		// take value from fifo and verify new occupancy of 1
		propeller.writeChar('T');
		Propeller::IsTrue(propeller.readInt32());
		Propeller::IsTrue(propeller.readInt32());
		Propeller::IsTrue(propeller.readInt32());
		Propeller::IsTrue(propeller.readInt32());
		propeller.writeChar('L');
		Assert::AreEqual(2, propeller.readInt32());
		Assert::AreEqual(2, propeller.readInt32());
		Assert::AreEqual(2, propeller.readInt32());
		Assert::AreEqual(2, propeller.readInt32());
		propeller.writeCharCharReadArray('Q', 1, &state, sizeof(state));
		Assert::AreEqual(1, state.occupancy);
		propeller.writeCharCharReadArray('Q', 2, &state, sizeof(state));
		Assert::AreEqual(1, state.occupancy);
		propeller.writeCharCharReadArray('Q', 3, &state, sizeof(state));
		Assert::AreEqual(1, state.occupancy);
		propeller.writeCharCharReadArray('Q', 4, &state, sizeof(state));
		Assert::AreEqual(1, state.occupancy);

		// take value from fifo and verify new occupancy of 0
		propeller.writeChar('T');
		Propeller::IsTrue(propeller.readInt32());
		Propeller::IsTrue(propeller.readInt32());
		Propeller::IsTrue(propeller.readInt32());
		Propeller::IsTrue(propeller.readInt32());
		propeller.writeChar('L');
		Assert::AreEqual(3, propeller.readInt32());
		Assert::AreEqual(3, propeller.readInt32());
		Assert::AreEqual(3, propeller.readInt32());
		Assert::AreEqual(3, propeller.readInt32());
		propeller.writeCharCharReadArray('Q', 1, &state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
		propeller.writeCharCharReadArray('Q', 2, &state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
		propeller.writeCharCharReadArray('Q', 3, &state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
		propeller.writeCharCharReadArray('Q', 4, &state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
	}

	TEST_METHOD(TestEndFlow_EmptyFifo)
	{
		// initialize actor
		propeller.writeChar('I');
		propeller.writeInt32(5);
		propeller.writeInt32(6);
		propeller.writeInt32(7);
		propeller.writeInt32(8);
		propeller.writeInt32(9);
		Propeller::IsTrue(propeller.readInt32());

		propeller.writeCharCharReadArray('Q', 1, &state, sizeof(state)); 
		Propeller::IsFalse(state.eof);
		propeller.writeCharCharReadArray('Q', 2, &state, sizeof(state)); 
		Propeller::IsFalse(state.eof);
		propeller.writeCharCharReadArray('Q', 3, &state, sizeof(state)); 
		Propeller::IsFalse(state.eof);
		propeller.writeCharCharReadArray('Q', 4, &state, sizeof(state)); 
		Propeller::IsFalse(state.eof);

		// send eof to fifo and verify eof is now true
		propeller.writeChar('E');
		propeller.writeCharCharReadArray('Q', 1, &state, sizeof(state));
		Propeller::IsTrue(state.eof);
		propeller.writeCharCharReadArray('Q', 2, &state, sizeof(state));
		Propeller::IsTrue(state.eof);
		propeller.writeCharCharReadArray('Q', 3, &state, sizeof(state));
		Propeller::IsTrue(state.eof);
		propeller.writeCharCharReadArray('Q', 4, &state, sizeof(state));
		Propeller::IsTrue(state.eof);
	}

	TEST_METHOD(TestEndFlow_OccupiedFifo)
	{
		// initialize actor
		propeller.writeChar('I');
		propeller.writeInt32(5);
		propeller.writeInt32(6);
		propeller.writeInt32(7);
		propeller.writeInt32(8);
		propeller.writeInt32(9);
		Propeller::IsTrue(propeller.readInt32());

		Propeller::IsTrue(propeller.writeCharInt32ReadInt32('P', 1));
		Propeller::IsTrue(propeller.writeCharInt32ReadInt32('P', 2));

		// verify that eof is currently false
		propeller.writeCharCharReadArray('Q', 1, &state, sizeof(state)); 
		Propeller::IsFalse(state.eof);
		propeller.writeCharCharReadArray('Q', 2, &state, sizeof(state)); 
		Propeller::IsFalse(state.eof);
		propeller.writeCharCharReadArray('Q', 3, &state, sizeof(state)); 
		Propeller::IsFalse(state.eof);
		propeller.writeCharCharReadArray('Q', 4, &state, sizeof(state)); 
		Propeller::IsFalse(state.eof);

		// send eof to fifo and verify eof is now true
		propeller.writeChar('E');
		propeller.writeCharCharReadArray('Q', 1, &state, sizeof(state)); 
		Propeller::IsTrue(state.eof);
		propeller.writeCharCharReadArray('Q', 2, &state, sizeof(state)); 
		Propeller::IsTrue(state.eof);
		propeller.writeCharCharReadArray('Q', 3, &state, sizeof(state)); 
		Propeller::IsTrue(state.eof);
		propeller.writeCharCharReadArray('Q', 4, &state, sizeof(state)); 
		Propeller::IsTrue(state.eof);
	}


	TEST_METHOD(Test_TakeAfterEndFlow_OccupiedFifo)
	{
		// initialize actor
		propeller.writeChar('I');
		propeller.writeInt32(5);
		propeller.writeInt32(6);
		propeller.writeInt32(7);
		propeller.writeInt32(8);
		propeller.writeInt32(9);
		Propeller::IsTrue(propeller.readInt32());

		Propeller::IsTrue(propeller.writeCharInt32ReadInt32('P', 1));
		Propeller::IsTrue(propeller.writeCharInt32ReadInt32('P', 2));
		propeller.writeChar('E');

		// take two values from fifo and confirm success for each
		propeller.writeChar('T');
		Propeller::IsTrue(propeller.readInt32());
		Propeller::IsTrue(propeller.readInt32());
		Propeller::IsTrue(propeller.readInt32());
		Propeller::IsTrue(propeller.readInt32());
		
		propeller.writeChar('T');
		Propeller::IsTrue(propeller.readInt32());
		Propeller::IsTrue(propeller.readInt32());
		Propeller::IsTrue(propeller.readInt32());
		Propeller::IsTrue(propeller.readInt32());

		// try to take a value from fifo and confirm failure
		propeller.writeChar('T');
		Propeller::IsFalse(propeller.readInt32());
		Propeller::IsFalse(propeller.readInt32());
		Propeller::IsFalse(propeller.readInt32());
		Propeller::IsFalse(propeller.readInt32());	
	}
};