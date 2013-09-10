#include "stdafx.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace AbsFlow;

TEST_CLASS(MemoryPoolTests)
{

private:

	Propeller::SerialConnection propeller{ "COM4" };

public:

	TEST_METHOD(TestInitializePool)
	{
		int poolStartAddress;
		int nextAllocationAddress;

		Propeller::AssertTrue(propeller << 'I' << 1024);

		Propeller::AreEqual(0, propeller << 'L');
		Propeller::AreEqual(1024, propeller << 'C');
		Propeller::AreEqual(0, propeller << 'S');

		propeller << 'P', propeller >> poolStartAddress;
		propeller << 'N', propeller >> nextAllocationAddress;
		Assert::AreEqual(poolStartAddress, nextAllocationAddress);
	}

	TEST_METHOD(TestFirstAllocation_MultipleOfFour)
	{
		int poolStartAddress;
		int allocatedMemoryAddress;
		int nextAllocationAddress;

		Propeller::AssertTrue(propeller << 'I' << 1024);
		propeller << 'P', propeller >> poolStartAddress;

		propeller << 'A' << 16, propeller >> allocatedMemoryAddress;
		Assert::AreEqual(poolStartAddress, allocatedMemoryAddress);
		Propeller::AreEqual(16, propeller << 'S');

		propeller << 'N', propeller >> nextAllocationAddress;
		Assert::AreEqual(poolStartAddress + 16, nextAllocationAddress);
	}

	TEST_METHOD(TestFirstAllocation_NotMultipleOfFour)
	{
		int poolStartAddress;
		int allocatedMemoryAddress;
		int nextAllocationAddress;

		Propeller::AssertTrue(propeller << 'I' << 1024);
		propeller << 'P', propeller >> poolStartAddress;

		propeller << 'A' << 15, propeller >> allocatedMemoryAddress;
		Propeller::AreEqual(16, propeller << 'S');
		Assert::AreEqual(poolStartAddress, allocatedMemoryAddress);

		propeller << 'N', propeller >> nextAllocationAddress;
		Assert::AreEqual(poolStartAddress + 16, nextAllocationAddress);
	}

	TEST_METHOD(TestFirstAllocation_SingleByte)
	{
		int poolStartAddress;
		int allocatedMemoryAddress;
		int nextAllocationAddress;

		Propeller::AssertTrue(propeller << 'I' << 1024);
		propeller << 'P', propeller >> poolStartAddress;

		propeller << 'A' << 1, propeller >> allocatedMemoryAddress;
		Assert::AreEqual(poolStartAddress, allocatedMemoryAddress);

		propeller << 'N', propeller >> nextAllocationAddress;
		Assert::AreEqual(poolStartAddress + 4, nextAllocationAddress);
	}

	TEST_METHOD(TestMultipleAllocations_MultiplesOfFour)
	{
		int allocationAddress;
		int expectedNextAllocationAddress;
		int nextAllocationAddress;
		int expectedSize = 0;
		
		Propeller::AssertTrue(propeller << 'I' << 1024);
		propeller << 'P', propeller >> expectedNextAllocationAddress;

		for (auto request_size_in_bytes : { 4, 12, 256, 32, 96 }) {

			propeller << 'A' << request_size_in_bytes, propeller >> allocationAddress;
			Assert::AreEqual(expectedNextAllocationAddress, allocationAddress);

			Propeller::AreEqual(expectedSize += request_size_in_bytes, propeller << 'S');

			propeller << 'N', propeller >> nextAllocationAddress;
			expectedNextAllocationAddress += request_size_in_bytes;
			Assert::AreEqual(expectedNextAllocationAddress, nextAllocationAddress);
		}
	}

	TEST_METHOD(TestMultipleAllocations_NotMultiplesOfFour)
	{
		int allocationAddress;
		int expectedNextAllocationAddress;
		int nextAllocationAddress;
		int expectedSize = 0;

		Propeller::AssertTrue(propeller << 'I' << 1024);

		propeller << 'P';
		propeller >> expectedNextAllocationAddress;

		for (auto b : { 2, 11, 291, 17, 93 }) {

			propeller << 'A' << b, propeller >> allocationAddress;
			Assert::AreEqual(expectedNextAllocationAddress, allocationAddress);

			int expectedAllocationBytes = ((b + 3) / 4) * 4;

			Propeller::AreEqual(expectedSize += expectedAllocationBytes, propeller << 'S');

			expectedNextAllocationAddress += expectedAllocationBytes;

			propeller << 'N', propeller >> nextAllocationAddress;
			Assert::AreEqual(expectedNextAllocationAddress, nextAllocationAddress);
		}
	}

	TEST_METHOD(TestFirstAllocation_MultipleOfFour_InsufficientMemory)
	{
		Propeller::AssertTrue(propeller << 'I' << 8);
		Propeller::AssertFalse(propeller << 'A' << 16);
	}

	TEST_METHOD(TestMultipleAllocations_MultiplesOfFour_InsufficientMemory)
	{
		Propeller::AssertTrue(propeller << 'I' << 64);
		Propeller::AreEqual(0, propeller << 'S');

		Propeller::AssertTrue(propeller << 'A' << 16);
		Propeller::AreEqual(16, propeller << 'S');

		Propeller::AssertTrue(propeller << 'A' << 4);
		Propeller::AreEqual(20, propeller << 'S');

		Propeller::AssertTrue(propeller << 'A' << 32);
		Propeller::AreEqual(52, propeller << 'S');

		Propeller::AssertFalse(propeller << 'A' << 16);
		Propeller::AreEqual(52, propeller << 'S');
	}

	TEST_METHOD(TestFirstAllocation_NotMultipleOfFour_InsufficientMemory)
	{
		Propeller::AssertTrue(propeller << 'I' << 8);
		Propeller::AssertFalse(propeller << 'A' << 16);
	}

	TEST_METHOD(TestMultipleAllocations_NotMultiplesOfFour_InsufficientMemory)
	{
		Propeller::AssertTrue(propeller << 'I' << 64);
		Propeller::AreEqual(0, propeller << 'S');

		Propeller::AssertTrue(propeller << 'A' << 7);
		Propeller::AreEqual(8, propeller << 'S');

		Propeller::AssertTrue(propeller << 'A' << 33);
		Propeller::AreEqual(44, propeller << 'S');

		Propeller::AssertTrue(propeller << 'A' << 10);
		Propeller::AreEqual(56, propeller << 'S');

		Propeller::AssertFalse(propeller << 'A' << 16);
		Propeller::AreEqual(56, propeller << 'S');
	}
};