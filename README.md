UTest
====

Simple, small, header-only testing library. I wrote this to test simple stuff I write (for which I don't need many thousands of lines worth of unit testing libraries).

## Usage

Some things I should mention:
- `main()` function is not provided
- There's only one function (macro) that you need to know: `UASSERT(condition)`
- Only `FAILED` tests are printed out (this can be changed if you want, though I don't see any reason for that)

Example:

```
#include "utest.h"

void test_someFuncName() { UASSERT(1 == 1); }

int main()
{
	UTest t; //this is necessary

	//call your test functions here
	test_someFuncName();
}
```

## LICENSE

Do whatever you want.

MIT
