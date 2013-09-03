#include "stdafx.h"
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace AbsFlow;

TEST_CLASS(OrderedMergeActorTests)
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

	static const int A = 0;
	static const int B = 1;
	static const int C = 2;

public:

	OrderedMergeActorTests() : propeller("COM4") {}

	TEST_METHOD_CLEANUP(TestCleanup)
	{
		Propeller::AssertTrue(propeller << 'D');
	}

	TEST_METHOD(TestInitializeActor)
	{
		// initialize actor
		Propeller::AssertTrue(propeller << 'I' << 5 << 6 << 7);

		// verify initial state of input fifos
		propeller << 'Q' << A;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.semid);
		Assert::AreEqual(5, state.depth);
		Assert::AreEqual(0, state.occupancy);
		Assert::AreEqual(0, state.eof);

		propeller << 'Q' << B;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(1, state.semid);
		Assert::AreEqual(6, state.depth);
		Assert::AreEqual(0, state.occupancy);
		Assert::AreEqual(0, state.eof);

		// verify initial state of output fifo
		propeller << 'Q' << C;
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

		// query output fifo state and verify initial occupancy of 0
		propeller << 'Q' << C;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);

		// put first value to input a
		Propeller::AssertTrue(propeller << 'P' << A << 1);
		Propeller::AssertTrue(propeller << 'W');

		// query output fifo state and verify occupancy is still 0
		propeller << 'Q' << C;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);

		// put second value to input b
		Propeller::AssertTrue(propeller << 'P' << B << 10);
		Propeller::AssertTrue(propeller << 'W');

		// query output fifo state and verify occupancy of 1
		propeller << 'Q' << C;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(1, state.occupancy);
	}

	TEST_METHOD(TestPutThenEndFlow)
	{
		// initialize actor
		Propeller::AssertTrue(propeller << 'I' << 5 << 6 << 7);

		// query output fifo state and verify initial occupancy of 0
		propeller << 'Q' << C;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);

		// put first value to input a
		Propeller::AssertTrue(propeller << 'P' << A << 1);
		Propeller::AssertTrue(propeller << 'W');

		// query output fifo state and verify occupancy is still 0
		propeller << 'Q' << C;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);

		// Signal end of flow on b
		propeller << 'E' << B;

		// query output fifo state and verify occupancy of 1
		propeller << 'Q' << C;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(1, state.occupancy);
	}

	TEST_METHOD(TestPutThenEndFlow_ThreeValuesToOneInput)
	{
		// initialize actor
		Propeller::AssertTrue(propeller << 'I' << 5 << 6 << 7);

		// query output fifo state and verify initial occupancy of 0
		propeller << 'Q' << C;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);

		// put three values to input a
		Propeller::AssertTrue(propeller << 'P' << A << 1);
		Propeller::AssertTrue(propeller << 'P' << A << 10);
		Propeller::AssertTrue(propeller << 'P' << A << 100);

		// query output fifo state and verify occupancy is 0
		propeller << 'Q' << C;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);

		// Signal end of flow on b
		propeller << 'E' << B;

		// query output fifo state and verify occupancy of 3
		propeller << 'Q' << C;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(3, state.occupancy);
	}


	TEST_METHOD(TestTake)
	{
		// initialize actor
		Propeller::AssertTrue(propeller << 'I' << 5 << 6 << 7);

		// put three values to input fifo
		Propeller::AssertTrue(propeller << 'P' << A << 1);
		Propeller::AssertTrue(propeller << 'P' << B << 10);
		Propeller::AssertTrue(propeller << 'W');

		// query output fifo state and verify occupancy of 2
		propeller << 'Q' << C;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(1, state.occupancy);

		// take value from fifo
		Propeller::AssertTrue(propeller << 'T');
		Propeller::AreEqual(1, propeller);

		// query output fifo state and verify occupancy of 0
		propeller << 'Q' << C;
		propeller.readBytes(&state, sizeof(state));
		Assert::AreEqual(0, state.occupancy);
	}

	TEST_METHOD(TestEndFlow_EmptyInputFifos)
	{
		// initialize actor
		Propeller::AssertTrue(propeller << 'I' << 5 << 6 << 7);

		propeller << 'Q' << C;
		propeller.readBytes(&state, sizeof(state));
		Propeller::IsFalse(state.eof);

		propeller << 'E' << A;
		propeller << 'Q' << C;
		propeller.readBytes(&state, sizeof(state));
		Propeller::IsFalse(state.eof);

		propeller << 'E' << B;
		propeller << 'Q' << C;
		propeller.readBytes(&state, sizeof(state));
		Propeller::IsTrue(state.eof);
	}

	TEST_METHOD(TestEndFlow_OccupiedFifo)
	{
		// initialize actor
		Propeller::AssertTrue(propeller << 'I' << 5 << 6 << 7);

		// put three values to input fifo
		// put one value to each of the three inputs
		Propeller::AssertTrue(propeller << 'P' << A << 1);
		Propeller::AssertTrue(propeller << 'P' << B << 10);

		// wait for the actor to consume all inputs
		Propeller::AssertTrue(propeller << 'W');

		// verify that eof on the actor output is currently false
		propeller << 'Q' << C;
		propeller.readBytes(&state, sizeof(state));
		Propeller::IsFalse(state.eof);

		propeller << 'E' << A;
		propeller << 'Q' << C;
		propeller.readBytes(&state, sizeof(state));
		Propeller::IsFalse(state.eof);

		propeller << 'E' << B;
		propeller << 'Q' << C;
		propeller.readBytes(&state, sizeof(state));
		Propeller::IsTrue(state.eof);
	}

	TEST_METHOD(Test_TakeAfterEndFlow_OccupiedFifo)
	{
		// initialize actor
		Propeller::AssertTrue(propeller << 'I' << 5 << 6 << 7);

		// put one value to each of the three inputs
		Propeller::AssertTrue(propeller << 'P' << A << 1);
		Propeller::AssertTrue(propeller << 'P' << B << 10);

		// wait for the actor to consume all inputs
		Propeller::AssertTrue(propeller << 'W');

		// end flow on each input
		propeller << 'E' << A;
		propeller << 'E' << B;

		// take three values from actor output and confirm success for each
		__int32 discard;
		Propeller::AssertTrue(propeller << 'T');
		propeller >> discard;
		Propeller::AssertTrue(propeller << 'T');
		propeller >> discard;

		// try to take a value from output and confirm failure
		Propeller::AssertFalse(propeller << 'T');
	}

	TEST_METHOD(TestTwoStreams_NoDuplicates)
	{
		// initialize actor
		Propeller::AssertTrue(propeller << 'I' << 10 << 10 << 20);

		// put values to input a
		for (__int32 i : {1, 3, 7, 8, 10, 12, 15}) {
			Propeller::AssertTrue(propeller << 'P' << A << i);
		}

		// put values to input b
		for (__int32 i : {2, 4, 5, 6, 9, 11, 13, 14, 16}) {
			Propeller::AssertTrue(propeller << 'P' << B << i);
		}

		// end flow on each input
		propeller << 'E' << A;
		propeller << 'E' << B;

		// take value from fifo
		std::vector<__int32> v;
		__int32 success, value;
		while (true) {
			propeller << 'T';
			propeller >> success >> value;
			if (!success) break;
			v.push_back(value);
		}

		Assert::IsTrue(v == std::vector<__int32>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}));
	}

	TEST_METHOD(TestTwoStreams_DuplicatesWithinStreams)
	{
		// initialize actor
		Propeller::AssertTrue(propeller << 'I' << 10 << 10 << 20);

		// put values to input a
		for (__int32 i : {1, 3, 7, 7, 8, 10, 10, 10, 12, 15}) {
			Propeller::AssertTrue(propeller << 'P' << A << i);
		}

		// put values to input b
		for (__int32 i : {2, 4, 4, 5, 6, 9, 11, 13, 13, 13, 14, 15, 16, 16, 16}) {
			Propeller::AssertTrue(propeller << 'P' << B << i);
		}

		// end flow on each input
		propeller << 'E' << A;
		propeller << 'E' << B;

		// take value from fifo
		std::vector<__int32> v;
		__int32 success, value;
		while (true) {
			propeller << 'T';
			propeller >> success >> value;
			if (!success) break;
			v.push_back(value);
		}

		Assert::IsTrue(v == std::vector<__int32>({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 }));
	}


	TEST_METHOD(TestTwoStreams_DuplicatesAcrossStreams)
	{
		// initialize actor
		Propeller::AssertTrue(propeller << 'I' << 10 << 10 << 20);

		// put values to input a
		for (__int32 i : {1, 3, 7, 8, 10, 12, 16}) {
			Propeller::AssertTrue(propeller << 'P' << A << i);
		}

		// put values to input b
		for (__int32 i : {2, 3, 4, 5, 6, 9, 10, 11, 13, 14, 15, 16}) {
			Propeller::AssertTrue(propeller << 'P' << B << i);
		}

		// end flow on each input
		propeller << 'E' << A;
		propeller << 'E' << B;

		// take value from fifo
		std::vector<__int32> v;
		__int32 success, value;
		while (true) {
			propeller << 'T';
			propeller >> success >> value;
			if (!success) break;
			v.push_back(value);
		}

		Assert::IsTrue(v == std::vector<__int32>({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 }));
	}
	
	TEST_METHOD(TestTwoStreams_DuplicatesWithinAndAcrossStreams)
	{
		// initialize actor
		Propeller::AssertTrue(propeller << 'I' << 20 << 20 << 20);

		// put values to input a
		for (__int32 i : {1, 3, 3, 7, 8, 10, 10, 10, 12, 15}) {
			Propeller::AssertTrue(propeller << 'P' << A << i);
		}

		// put values to input b
		for (__int32 i : {2, 3, 4, 5, 5, 5, 6, 7, 7, 9, 10, 11, 11, 13, 14, 15, 15, 15, 16}) {
			Propeller::AssertTrue(propeller << 'P' << B << i);
		}

		// end flow on each input
		propeller << 'E' << A;
		propeller << 'E' << B;

		// take value from fifo
		std::vector<__int32> v;
		__int32 success, value;
		while (true) {
			propeller << 'T';
			propeller >> success >> value;
			if (!success) break;
			v.push_back(value);
		}

		Assert::IsTrue(v == std::vector<__int32>({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 }));
	}

};