#pragma once

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AbsFlow { namespace Propeller {

	static void IsTrue(int value) 
	{
		Assert::AreNotEqual(0, value);
	}

	static void IsFalse(int value) 
	{
		Assert::AreEqual(0, value);
	}

	static void AreEqual(char expected, char actual) {
		Assert::AreEqual((int)expected, (int)actual);
	}

}}