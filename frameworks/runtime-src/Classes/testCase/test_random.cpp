#include "unittest/test.h"
#include "utils/random_utils.h"
#include "debug.h"

class TestRandom :public TestBase {
public:
	TestRandom() { TestManager::registerTestModule(this); }
	const char *getName() { return "TestRandom"; }

	void runTests();

	void testAllRandoms();
};

void TestRandom::runTests()
{
	TEST(testAllRandoms);
}

void TestRandom::testAllRandoms()
{
	int r1 = random_int(0, 10);
	float f1 = random_float(0, 1);
	double d1 = random_double(0, 0.88);
	float f2 = random_float();

}

static TestRandom g_test_instance;