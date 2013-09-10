#include "stdafx.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace AbsFlow;

TEST_CLASS(MemoryPoolTests)
{

private:

	AbsFlow::Propeller::SerialConnection propeller;

public:

	MemoryPoolTests() : propeller("COM4") {}

	TEST_METHOD(TestInitializePool)
	{
		int poolStartAddress;
		int nextAllocationAddress;

		Propeller::AssertTrue(propeller << 'I' << 1024);

		Propeller::AreEqual(0, propeller << 'L');
		Propeller::AreEqual(1024, propeller << 'C');

		propeller << 'P';
		propeller >> poolStartAddress;

		propeller << 'N';
		propeller >> nextAllocationAddress;

		Assert::AreEqual(poolStartAddress, nextAllocationAddress);
	}

	TEST_METHOD(TestFirstAllocation_MultipleOfFour)
	{
		int poolStartAddress;
		int allocatedMemoryAddress;
		int nextAllocationAddress;

		Propeller::AssertTrue(propeller << 'I' << 1024);

		propeller << 'P';
		propeller >> poolStartAddress;

		propeller << 'A' << 16;
		propeller >> allocatedMemoryAddress;

		Assert::AreEqual(poolStartAddress, allocatedMemoryAddress);

		propeller << 'N';
		propeller >> nextAllocationAddress;
		Assert::AreEqual(poolStartAddress + 16, nextAllocationAddress);
	}

	TEST_METHOD(TestFirstAllocation_NotMultipleOfFour)
	{
		int poolStartAddress;
		int allocatedMemoryAddress;
		int nextAllocationAddress;

		Propeller::AssertTrue(propeller << 'I' << 1024);

		propeller << 'P';
		propeller >> poolStartAddress;

		propeller << 'A' << 15;
		propeller >> allocatedMemoryAddress;

		Assert::AreEqual(poolStartAddress, allocatedMemoryAddress);

		propeller << 'N';
		propeller >> nextAllocationAddress;
		Assert::AreEqual(poolStartAddress + 16, nextAllocationAddress);
	}

	TEST_METHOD(TestFirstAllocation_SingleByte)
	{
		int poolStartAddress;
		int allocatedMemoryAddress;
		int nextAllocationAddress;

		Propeller::AssertTrue(propeller << 'I' << 1024);

		propeller << 'P';
		propeller >> poolStartAddress;

		propeller << 'A' << 1;
		propeller >> allocatedMemoryAddress;

		Assert::AreEqual(poolStartAddress, allocatedMemoryAddress);

		propeller << 'N';
		propeller >> nextAllocationAddress;
		Assert::AreEqual(poolStartAddress + 4, nextAllocationAddress);
	}

	TEST_METHOD(TestMultipleAllocations_MultiplesOfFour)
	{
		int allocationAddress;
		int expectedNextAllocationAddress;
		int nextAllocationAddress;
		
		Propeller::AssertTrue(propeller << 'I' << 1024);

		propeller << 'P';
		propeller >> expectedNextAllocationAddress;

		for (auto b : { 4, 12, 256, 32, 96 }) {

			propeller << 'A' << b;
			propeller >> allocationAddress;
			Assert::AreEqual(expectedNextAllocationAddress, allocationAddress);

			expectedNextAllocationAddress += b;

			propeller << 'N';
			propeller >> nextAllocationAddress;

			Assert::AreEqual(expectedNextAllocationAddress, nextAllocationAddress);
		}
	}

	TEST_METHOD(TestMultipleAllocations_NotMultiplesOfFour)
	{
		int allocationAddress;
		int expectedNextAllocationAddress;
		int nextAllocationAddress;

		Propeller::AssertTrue(propeller << 'I' << 1024);

		propeller << 'P';
		propeller >> expectedNextAllocationAddress;

		for (auto b : { 2, 11, 291, 17, 93 }) {

			propeller << 'A' << b;
			propeller >> allocationAddress;
			Assert::AreEqual(expectedNextAllocationAddress, allocationAddress);

			expectedNextAllocationAddress += ((b + 3) / 4) * 4;

			propeller << 'N';
			propeller >> nextAllocationAddress;

			Assert::AreEqual(expectedNextAllocationAddress, nextAllocationAddress);
		}
	}
};