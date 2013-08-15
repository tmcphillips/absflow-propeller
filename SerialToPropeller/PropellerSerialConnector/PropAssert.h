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

	static void AreEqual(int expected, PropellerSerialConnector& propeller) {
		int actual;
		propeller >> actual;
		Assert::AreEqual(expected, actual);
	}

	static void AssertTrue(PropellerSerialConnector& propeller) {	
		int result;
		propeller >> result;
		Propeller::IsTrue(result);
	}

	static void AssertFalse(PropellerSerialConnector& propeller) {
		int result;
		propeller >> result;
		Propeller::IsFalse(result);
	}

}}