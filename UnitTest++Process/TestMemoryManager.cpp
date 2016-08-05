#include "../EDMemoryManager/OverloadNew.h"
#include "../UnitTest++/src/UnitTest++.h"
#include "../EDMemoryManager/MemoryManager.h"

SUITE(MemoryManagerTestSuit)
{
	struct MemoryManagerFixture
	{
		void CheckAddress(void *pointer)
		{
			CHECK(pointer != nullptr);
			//CHECK_EQUAL(0, ((size_t)pointer) % 8);
		}
	};

	TEST(testNotEnoughMemoryToAllocate)
	{
		unsigned char *data = new unsigned char[INT_MAX-1];
		CHECK(nullptr == data);
	}

	TEST_FIXTURE(MemoryManagerFixture, testSingleAllocationAndAlignment)
	{
		int *testPointer = new int(55);
		CheckAddress(testPointer);

		*testPointer *= 2;
		CHECK_EQUAL(55*2, *testPointer);

		delete testPointer;
	}

	TEST_FIXTURE(MemoryManagerFixture, testArrayAllocationAndAlignment)
	{
		int testArrayElements = 1024;

		// Test allocation
		int **testArray = new int*[testArrayElements];
		CheckAddress(testArray);
		for(int testArrayIndex = 0; testArrayIndex < testArrayElements; ++testArrayIndex)
		{
			testArray[testArrayIndex] = new int(testArrayIndex);
			CheckAddress(testArray[testArrayIndex]);
		}

		// Test read/write
		for(int testArrayIndex = 0; testArrayIndex < testArrayElements; ++testArrayIndex)
		{
			*testArray[testArrayIndex] *= 2;
			CHECK_EQUAL(testArrayIndex*2, *testArray[testArrayIndex]);
		}

		// Test deallocation
		for(int testArrayIndex = 0; testArrayIndex < testArrayElements; ++testArrayIndex)
		{
			delete testArray[testArrayIndex];
		}

		delete[] testArray;
	}
}