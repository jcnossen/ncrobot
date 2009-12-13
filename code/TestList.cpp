
#include "StdIncl.h"

#include "Test.h"
#include "Tests/Walker.h"
#include "Tests/SimpleCarTest.h"

#include "Tests/BipedTest.h"

TestFactory<SimpleCarTest> simpleCarTestFactory("Simple car test");

class WalkerTestFactory : TestFactoryBase {
public:
	int getNumTests() { return 4; }

	const char* getTestName(int i) { 
		const char *names[] = {
			"1-lp walker (knees)",
			"2-lp walker (knees)",
			"4-lp walker (knees)",
			"4-lp walker",
			"10-lp walker"
		};

		return names[i];
	}

	Test* createTest(int i) {

		switch (i) {
			case 0: return new Walker(1, true);
			case 1: return new Walker(2, true);
			case 2: return new Walker(4, true);
			case 3: return new Walker(4, false);
			case 4: return new Walker(10, false);
		}

		return 0;
	}
};


