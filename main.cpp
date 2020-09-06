#include "utest.h"
#include "class.h"

#include <string>
#include <vector>

void test_stringEquality()
{
	std::string s = "Hello";
	UASSERT(s == "Hello");
}

void test_vector()
{
	std::vector<int> v{1,2,3,4};
	UASSERT(v.at(2) == 3);
}

void test_failure()
{
	int x = 10;
	UASSERT(x == 9);
}

int main()
{
	UTest t;

	test_stringEquality();
	test_vector();
	test_failure();

	TestClass c;
	c.runTests();

	return 0;
}
