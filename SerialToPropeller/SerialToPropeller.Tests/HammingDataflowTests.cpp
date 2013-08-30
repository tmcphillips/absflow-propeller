#include "stdafx.h"
#include "CppUnitTest.h"
#include <vector>

#include "SerialConnection.h" 
#include "PropAssert.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace AbsFlow;

TEST_CLASS(HammingDataflowTests)
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

	HammingDataflowTests() : propeller("COM4") {}

	TEST_METHOD_CLEANUP(TestCleanup)
	{
		propeller << 'W';
	}

	TEST_METHOD(Test_HammingNumbersThrough20)
	{
		propeller << 'I' << 100;
		propeller << 'S' << 20;

		std::vector<__int32> hamming_numbers;
		__int32 success, value;
		while (true) {
			propeller << 'R';
			propeller >> success >> value;
			if (!success) break;
			hamming_numbers.push_back(value);
		}

		auto v = getHammingNumbers(20);
		Assert::IsTrue(hamming_numbers == v);
	}


private:
	
	std::vector<__int32> getHammingNumbers(int max) {

		std::vector<__int32> v {};
		for (int i = 1; i <= max; i++) {
			if (isHammingNumber(i)) {
				v.push_back(i);
			}
		}
		return v;
	}

	bool isHammingNumber(int n) {
		return removeFactor(removeFactor(removeFactor(n, 5), 3), 2) == 1;
	}

	int removeFactor(int n, int factor) {
		if (!n) return n;
		while (n % factor == 0) n /= factor;
		return n;
	}

};