#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace AbsFlow;

TEST_CLASS(FilterThenForkActorTests)
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
	AbsFlow::Propeller::SerialConnection propeller;

public:

	FilterThenForkActorTests() : propeller("COM4") {}

	TEST_METHOD_CLEANUP(TestCleanup)
	{
		Propeller::AssertTrue(propeller << 'D');
	}

	TEST_METHOD(TestInitializeActor)
	{
		// initialize actor
		Propeller::AssertTrue(propeller << 'I' << 5 << 6 << 7 << 8 << 9 << 12 << 0);

		// verify initial state of input fifos
		propeller << 'Q' << 0;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.semid);
		Assert::AreEqual(5, state.depth);
		Assert::AreEqual(0, state.occupancy);
		Assert::AreEqual(0, state.eof);

		// verify initial state of input fifos
		propeller << 'Q' << 1;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(1, state.semid);
		Assert::AreEqual(6, state.depth);
		Assert::AreEqual(0, state.occupancy);
		Assert::AreEqual(0, state.eof);

		propeller << 'Q' << 2;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(2, state.semid);
		Assert::AreEqual(7, state.depth);
		Assert::AreEqual(0, state.occupancy);
		Assert::AreEqual(0, state.eof);

		propeller << 'Q' << 3;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(3, state.semid);
		Assert::AreEqual(8, state.depth);
		Assert::AreEqual(0, state.occupancy);
		Assert::AreEqual(0, state.eof);

		propeller << 'Q' << 4;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(4, state.semid);
		Assert::AreEqual(9, state.depth);
		Assert::AreEqual(0, state.occupancy);
		Assert::AreEqual(0, state.eof);
	}

	TEST_METHOD(TestPut_AllBelowCutoff)
	{
		// initialize actor
		Propeller::AssertTrue(propeller << 'I' << 5 << 6 << 7 << 8 << 9 << 12 << 0);

		propeller << 'Q' << 0;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
		propeller << 'Q' << 1;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
		propeller << 'Q' << 2;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
		propeller << 'Q' << 3;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
		propeller << 'Q' << 4;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);

		// put one value to the input fifo and verify output fifo occupancy of one
		Propeller::AssertTrue(propeller << 'P' << 1);
		Propeller::AssertTrue(propeller << 'W');
		propeller << 'Q' << 1;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(1, state.occupancy);
		propeller << 'Q' << 2;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(1, state.occupancy);
		propeller << 'Q' << 3;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(1, state.occupancy);
		propeller << 'Q' << 4;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(1, state.occupancy);

		// put second value to the input fifo and verify output fifo occupancy of two
		Propeller::AssertTrue(propeller << 'P' << 2);
		Propeller::AssertTrue(propeller << 'W');
		propeller << 'Q' << 1;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(2, state.occupancy);
		propeller << 'Q' << 2;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(2, state.occupancy);
		propeller << 'Q' << 3;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(2, state.occupancy);
		propeller << 'Q' << 4;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(2, state.occupancy);
	}

	TEST_METHOD(TestPut_AllAboveCutoff)
	{
		// initialize actor
		Propeller::AssertTrue(propeller << 'I' << 5 << 6 << 7 << 8 << 9 << 12 << 0);

		propeller << 'Q' << 0;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
		propeller << 'Q' << 1;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
		propeller << 'Q' << 2;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
		propeller << 'Q' << 3;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
		propeller << 'Q' << 4;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);

		// put one value to the input fifo and verify output fifo occupancy of one
		Propeller::AssertTrue(propeller << 'P' << 13);
		Propeller::AssertTrue(propeller << 'W');
		propeller << 'Q' << 1;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
		propeller << 'Q' << 2;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
		propeller << 'Q' << 3;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
		propeller << 'Q' << 4;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);

		// put second value to the input fifo and verify output fifo occupancy of two
		Propeller::AssertTrue(propeller << 'P' << 14);
		Propeller::AssertTrue(propeller << 'W');
		propeller << 'Q' << 1;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
		propeller << 'Q' << 2;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
		propeller << 'Q' << 3;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
		propeller << 'Q' << 4;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
	}

	TEST_METHOD(TestTake_AllBelowCutoff)
	{
		// initialize actor
		Propeller::AssertTrue(propeller << 'I' << 5 << 6 << 7 << 8 << 9 << 12 << 0);

		// put three values to input fifo
		Propeller::AssertTrue(propeller << 'P' << 1);
		Propeller::AssertTrue(propeller << 'P' << 2);
		Propeller::AssertTrue(propeller << 'P' << 3);

		// wait for actor to drain input fifo
		Propeller::AssertTrue(propeller << 'W');

		// query fifo state and verify occupancy of 3
		propeller << 'Q' << 1;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(3, state.occupancy);
		propeller << 'Q' << 2;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(3, state.occupancy);
		propeller << 'Q' << 3;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(3, state.occupancy);
		propeller << 'Q' << 4;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(3, state.occupancy);

		// Propeller value from fifo and verify new occupancy of 2
		Propeller::AssertTrue(propeller << 'T' << 1);
		Propeller::AreEqual(1, propeller);
		Propeller::AssertTrue(propeller << 'T' << 2);
		Propeller::AreEqual(1, propeller);
		Propeller::AssertTrue(propeller << 'T' << 3);
		Propeller::AreEqual(1, propeller);
		Propeller::AssertTrue(propeller << 'T' << 4);
		Propeller::AreEqual(1, propeller);


		propeller << 'Q' << 1;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(2, state.occupancy);
		propeller << 'Q' << 2;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(2, state.occupancy);
		propeller << 'Q' << 3;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(2, state.occupancy);
		propeller << 'Q' << 4;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(2, state.occupancy);

		// take value from fifo and verify new occupancy of 1
		Propeller::AssertTrue(propeller << 'T' << 1);
		Propeller::AreEqual(2, propeller);
		Propeller::AssertTrue(propeller << 'T' << 2);
		Propeller::AreEqual(2, propeller);
		Propeller::AssertTrue(propeller << 'T' << 3);
		Propeller::AreEqual(2, propeller);
		Propeller::AssertTrue(propeller << 'T' << 4);
		Propeller::AreEqual(2, propeller);

		propeller << 'Q' << 1;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(1, state.occupancy);
		propeller << 'Q' << 2;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(1, state.occupancy);
		propeller << 'Q' << 3;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(1, state.occupancy);
		propeller << 'Q' << 4;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(1, state.occupancy);

		// take value from fifo and verify new occupancy of 0
		Propeller::AssertTrue(propeller << 'T' << 1);
		Propeller::AreEqual(3, propeller);
		Propeller::AssertTrue(propeller << 'T' << 2);
		Propeller::AreEqual(3, propeller);
		Propeller::AssertTrue(propeller << 'T' << 3);
		Propeller::AreEqual(3, propeller);
		Propeller::AssertTrue(propeller << 'T' << 4);
		Propeller::AreEqual(3, propeller);

		propeller << 'Q' << 1;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
		propeller << 'Q' << 2;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
		propeller << 'Q' << 3;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
		propeller << 'Q' << 4;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
	}

	TEST_METHOD(TestTake_AboveAndBelowCutoff_NoShutdownOnOverage)
	{
		// initialize actor
		Propeller::AssertTrue(propeller << 'I' << 5 << 6 << 7 << 8 << 9 << 12 << 0);

		// put three values to input fifo
		Propeller::AssertTrue(propeller << 'P' << 1);
		Propeller::AssertTrue(propeller << 'P' << 21);
		Propeller::AssertTrue(propeller << 'P' << 2);
		Propeller::AssertTrue(propeller << 'P' << 13);
		Propeller::AssertTrue(propeller << 'P' << 15);
		Propeller::AssertTrue(propeller << 'P' << 12);

		// wait for actor to drain input fifo
		Propeller::AssertTrue(propeller << 'W');

		// query fifo state and verify occupancy of 3
		propeller << 'Q' << 1;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(3, state.occupancy);
		propeller << 'Q' << 2;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(3, state.occupancy);
		propeller << 'Q' << 3;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(3, state.occupancy);
		propeller << 'Q' << 4;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(3, state.occupancy);

		// Propeller value from fifo and verify new occupancy of 2
		Propeller::AssertTrue(propeller << 'T' << 1);
		Propeller::AreEqual(1, propeller);
		Propeller::AssertTrue(propeller << 'T' << 2);
		Propeller::AreEqual(1, propeller);
		Propeller::AssertTrue(propeller << 'T' << 3);
		Propeller::AreEqual(1, propeller);
		Propeller::AssertTrue(propeller << 'T' << 4);
		Propeller::AreEqual(1, propeller);

		propeller << 'Q' << 1;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(2, state.occupancy);
		propeller << 'Q' << 2;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(2, state.occupancy);
		propeller << 'Q' << 3;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(2, state.occupancy);
		propeller << 'Q' << 4;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(2, state.occupancy);

		// take value from fifo and verify new occupancy of 1
		Propeller::AssertTrue(propeller << 'T' << 1);
		Propeller::AreEqual(2, propeller);
		Propeller::AssertTrue(propeller << 'T' << 2);
		Propeller::AreEqual(2, propeller);
		Propeller::AssertTrue(propeller << 'T' << 3);
		Propeller::AreEqual(2, propeller);
		Propeller::AssertTrue(propeller << 'T' << 4);
		Propeller::AreEqual(2, propeller);

		propeller << 'Q' << 1;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(1, state.occupancy);
		propeller << 'Q' << 2;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(1, state.occupancy);
		propeller << 'Q' << 3;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(1, state.occupancy);
		propeller << 'Q' << 4;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(1, state.occupancy);

		// take value from fifo and verify new occupancy of 0
		Propeller::AssertTrue(propeller << 'T' << 1);
		Propeller::AreEqual(12, propeller);
		Propeller::AssertTrue(propeller << 'T' << 2);
		Propeller::AreEqual(12, propeller);
		Propeller::AssertTrue(propeller << 'T' << 3);
		Propeller::AreEqual(12, propeller);
		Propeller::AssertTrue(propeller << 'T' << 4);
		Propeller::AreEqual(12, propeller);

		propeller << 'Q' << 1;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
		propeller << 'Q' << 2;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
		propeller << 'Q' << 3;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
		propeller << 'Q' << 4;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
	}

	TEST_METHOD(TestTake_AboveAndBelowCutoff_ShutdownOnOverage)
	{
		// initialize actor
		Propeller::AssertTrue(propeller << 'I' << 5 << 6 << 7 << 8 << 9 << 12 << 1);

		// put three values to input fifo
		Propeller::AssertTrue(propeller << 'P' << 1);
		Propeller::AssertTrue(propeller << 'P' << 7);
		Propeller::AssertTrue(propeller << 'P' << 12);
		Propeller::AssertTrue(propeller << 'P' << 13);
		Propeller::AssertTrue(propeller << 'P' << 3);
		Propeller::AssertTrue(propeller << 'P' << 4);

		// wait for actor to drain input fifo
		Propeller::AssertTrue(propeller << 'W');

		// query fifo state and verify occupancy of 3
		propeller << 'Q' << 1;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(3, state.occupancy);
		propeller << 'Q' << 2;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(3, state.occupancy);
		propeller << 'Q' << 3;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(3, state.occupancy);
		propeller << 'Q' << 4;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(3, state.occupancy);

		// Propeller value from fifo and verify new occupancy of 2
		Propeller::AssertTrue(propeller << 'T' << 1);
		Propeller::AreEqual(1, propeller);
		Propeller::AssertTrue(propeller << 'T' << 2);
		Propeller::AreEqual(1, propeller);
		Propeller::AssertTrue(propeller << 'T' << 3);
		Propeller::AreEqual(1, propeller);
		Propeller::AssertTrue(propeller << 'T' << 4);
		Propeller::AreEqual(1, propeller);

		propeller << 'Q' << 1;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(2, state.occupancy);
		propeller << 'Q' << 2;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(2, state.occupancy);
		propeller << 'Q' << 3;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(2, state.occupancy);
		propeller << 'Q' << 4;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(2, state.occupancy);

		// take value from fifo and verify new occupancy of 1
		Propeller::AssertTrue(propeller << 'T' << 1);
		Propeller::AreEqual(7, propeller);
		Propeller::AssertTrue(propeller << 'T' << 2);
		Propeller::AreEqual(7, propeller);
		Propeller::AssertTrue(propeller << 'T' << 3);
		Propeller::AreEqual(7, propeller);
		Propeller::AssertTrue(propeller << 'T' << 4);
		Propeller::AreEqual(7, propeller);

		propeller << 'Q' << 1;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(1, state.occupancy);
		propeller << 'Q' << 2;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(1, state.occupancy);
		propeller << 'Q' << 3;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(1, state.occupancy);
		propeller << 'Q' << 4;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(1, state.occupancy);

		// take value from fifo and verify new occupancy of 0
		Propeller::AssertTrue(propeller << 'T' << 1);
		Propeller::AreEqual(12, propeller);
		Propeller::AssertTrue(propeller << 'T' << 2);
		Propeller::AreEqual(12, propeller);
		Propeller::AssertTrue(propeller << 'T' << 3);
		Propeller::AreEqual(12, propeller);
		Propeller::AssertTrue(propeller << 'T' << 4);
		Propeller::AreEqual(12, propeller);

		propeller << 'Q' << 1;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
		propeller << 'Q' << 2;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
		propeller << 'Q' << 3;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
		propeller << 'Q' << 4;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
	}


	TEST_METHOD(TestEndFlow_EmptyFifo)
	{
		// initialize actor
		Propeller::AssertTrue(propeller << 'I' << 5 << 6 << 7 << 8 << 9 << 12 << 0);

		propeller << 'Q' << 1;
		propeller.readBytes(&state, sizeof(state));
		Propeller::IsFalse(state.eof);

		propeller << 'Q' << 2;
		propeller.readBytes(&state, sizeof(state));
		Propeller::IsFalse(state.eof);

		propeller << 'Q' << 3;
		propeller.readBytes(&state, sizeof(state));
		Propeller::IsFalse(state.eof);

		propeller << 'Q' << 4;
		propeller.readBytes(&state, sizeof(state));
		Propeller::IsFalse(state.eof);

		// send eof to fifo and verify eof is now true
		propeller << 'E';

		propeller << 'Q' << 1;
		propeller.readBytes(&state, sizeof(state));
		Propeller::IsTrue(state.eof);

		propeller << 'Q' << 2;
		propeller.readBytes(&state, sizeof(state));
		Propeller::IsTrue(state.eof);

		propeller << 'Q' << 3;
		propeller.readBytes(&state, sizeof(state));
		Propeller::IsTrue(state.eof);

		propeller << 'Q' << 4;
		propeller.readBytes(&state, sizeof(state));
		Propeller::IsTrue(state.eof);
	}

	TEST_METHOD(TestEndFlow_OccupiedFifo)
	{
		// initialize actor
		Propeller::AssertTrue(propeller << 'I' << 5 << 6 << 7 << 8 << 9 << 12 << 0);

		Propeller::AssertTrue(propeller << 'P' << 1);
		Propeller::AssertTrue(propeller << 'P' << 2);

		// verify that eof is currently false
		propeller << 'Q' << 1;
		propeller.readBytes(&state, sizeof(state));
		Propeller::IsFalse(state.eof);

		propeller << 'Q' << 2;
		propeller.readBytes(&state, sizeof(state));
		Propeller::IsFalse(state.eof);

		propeller << 'Q' << 3;
		propeller.readBytes(&state, sizeof(state));
		Propeller::IsFalse(state.eof);

		propeller << 'Q' << 4;
		propeller.readBytes(&state, sizeof(state));
		Propeller::IsFalse(state.eof);

		// send eof to fifo and verify eof is now true
		propeller << 'E';

		propeller << 'Q' << 1;
		propeller.readBytes(&state, sizeof(state));
		Propeller::IsTrue(state.eof);

		propeller << 'Q' << 2;
		propeller.readBytes(&state, sizeof(state));
		Propeller::IsTrue(state.eof);

		propeller << 'Q' << 3;
		propeller.readBytes(&state, sizeof(state));
		Propeller::IsTrue(state.eof);

		propeller << 'Q' << 4;
		propeller.readBytes(&state, sizeof(state));
		Propeller::IsTrue(state.eof);
	}


	TEST_METHOD(Test_TakeAfterEndFlow_OccupiedFifo)
	{
		// initialize actor
		Propeller::AssertTrue(propeller << 'I' << 5 << 6 << 7 << 8 << 9 << 12 << 0);

		Propeller::AssertTrue(propeller << 'P' << 1);
		Propeller::AssertTrue(propeller << 'P' << 2);
		propeller << 'E';

		// take two values from fifo and confirm success for each
		Propeller::AssertTrue(propeller << 'T' << 1);
		Propeller::AreEqual(1, propeller);
		Propeller::AssertTrue(propeller << 'T' << 2);
		Propeller::AreEqual(1, propeller);
		Propeller::AssertTrue(propeller << 'T' << 3);
		Propeller::AreEqual(1, propeller);
		Propeller::AssertTrue(propeller << 'T' << 4);
		Propeller::AreEqual(1, propeller);

		Propeller::AssertTrue(propeller << 'T' << 1);
		Propeller::AreEqual(2, propeller);
		Propeller::AssertTrue(propeller << 'T' << 2);
		Propeller::AreEqual(2, propeller);
		Propeller::AssertTrue(propeller << 'T' << 3);
		Propeller::AreEqual(2, propeller);
		Propeller::AssertTrue(propeller << 'T' << 4);
		Propeller::AreEqual(2, propeller);

		// try to take a value from fifo and confirm failure
		__int32 discard;
		Propeller::AssertFalse(propeller << 'T' << 1);
		propeller >> discard;
		Propeller::AssertFalse(propeller << 'T' << 2);
		propeller >> discard;
		Propeller::AssertFalse(propeller << 'T' << 3);
		propeller >> discard;
		Propeller::AssertFalse(propeller << 'T' << 4);
		propeller >> discard;
	}
};