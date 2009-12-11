
#include "StdIncl.h"

#include "Test.h"
#include "Tests/Walker.h"
#include "Tests/SimpleCarTest.h"

#include "Tests/BipedTest.h"


Test* CreatePSOWalker2()
{
	return new Walker(2);
}

Test* CreatePSOWalker4()
{
	return new Walker(4);
}

Test* CreatePSOWalker10()
{
	return new Walker(10);
}

TestEntry g_testEntries[] =
{
	{"Simple Car", SimpleCarTest::Create },
	{"2-leg walker", CreatePSOWalker2},
	{"4-leg walker", CreatePSOWalker4},
	{"10-leg walker", CreatePSOWalker10},
	{"Biped model", BipedTest::Create},
	{0, 0	}
};

