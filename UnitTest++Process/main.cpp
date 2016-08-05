#include "../UnitTest++/src/UnitTest++.h"
#include "../UnitTest++/src/TestReporterStdout.h"


int main(int, char const *[])
{
	int result = UnitTest::RunAllTests();
	//system("pause");
    return result;
}
