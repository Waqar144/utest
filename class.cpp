#include "class.h"
#include "utest.h"

void TestClass::runTests()
{
	int x = 0;
	UASSERT(x == 1);

	TestClass t;
	UASSERT(t.a == 0);
	UASSERT(t.b == 1);
}
