#include "stdafx.h"
#include "CppUnitTest.h"

#include "PropellerSerialConnector.h" 
#include "PropAssert.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace AbsFlow;
	
TEST_CLASS(MergeActorTests)
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
		propeller->writeChar('I');
		propeller->writeInt32(5);
		propeller->writeInt32(6);
		propeller->writeInt32(7);
		propeller->writeInt32(8);
		Propeller::IsTrue(propeller->readInt32());

		// verify initial state of input fifos
		propeller->writeCharCharReadArray('Q', 1, &state, sizeof(state)); 
		Assert::AreEqual(0, state.semid);
		Assert::AreEqual(5, state.depth);
		Assert::AreEqual(0, state.occupancy);
		Assert::AreEqual(0, state.eof);

		propeller->writeCharCharReadArray('Q', 2, &state, sizeof(state)); 
		Assert::AreEqual(1, state.semid);
		Assert::AreEqual(6, state.depth);
		Assert::AreEqual(0, state.occupancy);
		Assert::AreEqual(0, state.eof);

		propeller->writeCharCharReadArray('Q', 3, &state, sizeof(state)); 
		Assert::AreEqual(2, state.semid);
		Assert::AreEqual(7, state.depth);
		Assert::AreEqual(0, state.occupancy);
		Assert::AreEqual(0, state.eof);

		// verify initial state of output fifo
		propeller->writeCharCharReadArray('Q', 0, &state, sizeof(state)); 
		Assert::AreEqual(3, state.semid);
		Assert::AreEqual(8, state.depth);
		Assert::AreEqual(0, state.occupancy);
		Assert::AreEqual(0, state.eof);
	}
	 
	TEST_METHOD(TestPut)
	{
		// initialize actor
		propeller->writeChar('I');
		propeller->writeInt32(5);
		propeller->writeInt32(6);
		propeller->writeInt32(7);
		propeller->writeInt32(8);
		Propeller::IsTrue(propeller->readInt32());

		propeller->writeCharCharReadArray('Q', 0, &state, sizeof(state)); 
		Assert::AreEqual(0, state.occupancy);

		// put one value to the input fifo and verify output fifo occupancy of one
		Propeller::IsTrue(propeller->writeCharCharInt32ReadInt32('P', 1, 1));
		Propeller::IsTrue(propeller->writeCharReadInt32('W'));
		propeller->writeCharCharReadArray('Q', 0, &state, sizeof(state)); 
		Assert::AreEqual(1, state.occupancy);

		// put second value to the input fifo and verify output fifo occupancy of two
		Propeller::IsTrue(propeller->writeCharCharInt32ReadInt32('P', 2, 10));
		Propeller::IsTrue(propeller->writeCharReadInt32('W'));
		propeller->writeCharCharReadArray('Q', 0, &state, sizeof(state)); 
		Assert::AreEqual(2, state.occupancy);

		// put second value to the input fifo and verify output fifo occupancy of two
		Propeller::IsTrue(propeller->writeCharCharInt32ReadInt32('P', 3, 100));
		Propeller::IsTrue(propeller->writeCharReadInt32('W'));
		propeller->writeCharCharReadArray('Q', 0, &state, sizeof(state)); 
		Assert::AreEqual(3, state.occupancy);
	}

	TEST_METHOD(TestTake)
	{
		// initialize actor
		propeller->writeChar('I');
		propeller->writeInt32(5);
		propeller->writeInt32(6);
		propeller->writeInt32(7);
		propeller->writeInt32(8);
		Propeller::IsTrue(propeller->readInt32());

		// put three values to input fifo
		Propeller::IsTrue(propeller->writeCharCharInt32ReadInt32('P', 1, 1));
		Propeller::IsTrue(propeller->writeCharReadInt32('W'));
		Propeller::IsTrue(propeller->writeCharCharInt32ReadInt32('P', 2, 10));
		Propeller::IsTrue(propeller->writeCharReadInt32('W'));
		Propeller::IsTrue(propeller->writeCharCharInt32ReadInt32('P', 3, 100));
		Propeller::IsTrue(propeller->writeCharReadInt32('W'));

		// query output fifo state and verify occupancy of 3
		propeller->writeCharCharReadArray('Q', 0, &state, sizeof(state)); 
		Assert::AreEqual(3, state.occupancy);

		// Propeller value from fifo and verify new occupancy of 2
		Propeller::IsTrue(propeller->writeCharReadInt32('T'));
		Assert::AreEqual(1, propeller->writeCharReadInt32('L'));
		propeller->writeCharCharReadArray('Q', 0, &state, sizeof(state));
		Assert::AreEqual(2, state.occupancy);

		// Propeller value from fifo and verify new occupancy of 2
		Propeller::IsTrue(propeller->writeCharReadInt32('T'));
		Assert::AreEqual(10, propeller->writeCharReadInt32('L'));
		propeller->writeCharCharReadArray('Q', 0, &state, sizeof(state));
		Assert::AreEqual(1, state.occupancy);

		// Propeller value from fifo and verify new occupancy of 2
		Propeller::IsTrue(propeller->writeCharReadInt32('T'));
		Assert::AreEqual(100, propeller->writeCharReadInt32('L'));
		propeller->writeCharCharReadArray('Q', 0, &state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
	}

	TEST_METHOD(TestEndFlow_EmptyInputFifos)
	{
		// initialize actor
		propeller->writeChar('I');
		propeller->writeInt32(5);
		propeller->writeInt32(6);
		propeller->writeInt32(7);
		propeller->writeInt32(8);
		Propeller::IsTrue(propeller->readInt32());

		propeller->writeCharCharReadArray('Q', 0, &state, sizeof(state)); 
		Propeller::IsFalse(state.eof);

		propeller->writeCharChar('E', 1);
		propeller->writeCharCharReadArray('Q', 0, &state, sizeof(state)); 
		Propeller::IsFalse(state.eof);

		propeller->writeCharChar('E', 2);
		propeller->writeCharCharReadArray('Q', 0, &state, sizeof(state)); 
		Propeller::IsFalse(state.eof);

		propeller->writeCharChar('E', 3);
		propeller->writeCharCharReadArray('Q', 0, &state, sizeof(state)); 
		Propeller::IsTrue(state.eof);

	}

	TEST_METHOD(TestEndFlow_OccupiedFifo)
	{
		// initialize actor
		propeller->writeChar('I');
		propeller->writeInt32(5);
		propeller->writeInt32(6);
		propeller->writeInt32(7);
		propeller->writeInt32(8);
		Propeller::IsTrue(propeller->readInt32());

		// put three values to input fifo
		Propeller::IsTrue(propeller->writeCharCharInt32ReadInt32('P', 1, 1));
		Propeller::IsTrue(propeller->writeCharCharInt32ReadInt32('P', 2, 10));
		Propeller::IsTrue(propeller->writeCharCharInt32ReadInt32('P', 3, 100));

		Propeller::IsTrue(propeller->writeCharReadInt32('W'));

		// verify that eof is currently false
		propeller->writeCharCharReadArray('Q', 0, &state, sizeof(state)); 
		Propeller::IsFalse(state.eof);

		propeller->writeCharChar('E', 1);
		propeller->writeCharCharReadArray('Q', 0, &state, sizeof(state)); 
		Propeller::IsFalse(state.eof);

		propeller->writeCharChar('E', 2);
		propeller->writeCharCharReadArray('Q', 0, &state, sizeof(state)); 
		Propeller::IsFalse(state.eof);

		propeller->writeCharChar('E', 3);
		propeller->writeCharCharReadArray('Q', 0, &state, sizeof(state)); 
		Propeller::IsTrue(state.eof);
	}

	TEST_METHOD(Test_TakeAfterEndFlow_OccupiedFifo)
	{
		// initialize actor
		//propeller->writeChar('I');
		*propeller << 'I';
		propeller->writeInt32(5);
		propeller->writeInt32(6);
		propeller->writeInt32(7);
		propeller->writeInt32(8);
		Propeller::IsTrue(propeller->readInt32());

		Propeller::IsTrue(propeller->writeCharCharInt32ReadInt32('P', 1, 1));
		Propeller::IsTrue(propeller->writeCharCharInt32ReadInt32('P', 2, 10));
		Propeller::IsTrue(propeller->writeCharCharInt32ReadInt32('P', 3, 100));

		Propeller::IsTrue(propeller->writeCharReadInt32('W'));
		propeller->writeCharChar('E', 1);
		propeller->writeCharChar('E', 2);
		propeller->writeCharChar('E', 3);
		
		// take two values from fifo and confirm success for each
		Propeller::IsTrue(propeller->writeCharReadInt32('T'));
		Propeller::IsTrue(propeller->writeCharReadInt32('T'));
		Propeller::IsTrue(propeller->writeCharReadInt32('T'));

		// try to take a value from fifo and confirm failure
		Propeller::IsFalse(propeller->writeCharReadInt32('T'));
	}
};