
#include "StdIncl.h"

#include "Test.h"
#include "Tests/Walker.h"
#include "Tests/SimpleCarTest.h"

#include "Tests/BipedTest.h"


Test* CreatePSOWalker1()
{
	return new Walker(1, true, true, true);
}

Test* CreatePSOWalker2()
{
	return new Walker(2, true, false, true);
}

Test* CreatePSOWalker4K()
{
	return new Walker(4, true);
}

Test* CreatePSOWalker4()
{
	return new Walker(4, false);
}

Test* CreatePSOWalker10()
{
	return new Walker(10, false);
}

TestFactory g_testEntries[] =
{
	{"Simple Car", SimpleCarTest::Create },
	{"1-lp walker (knees)", CreatePSOWalker1},
	{"2-lp walker (knees)", CreatePSOWalker2},
	{"4-lp walker (knees)", CreatePSOWalker4K},
	{"4-lp walker", CreatePSOWalker4},
	{"10-lp walker", CreatePSOWalker10},
	{"Biped model", BipedTest::Create},
	{0, 0	}
};

