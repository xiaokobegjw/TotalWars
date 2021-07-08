#pragma once

#include <exception>
#include <vector>

#include "filesys.h"

class TestFailedException : public std::exception {
};

// Runs a unit test and reports results
#define TEST(fxn, ...) {                                                      \
	uint64_t t1 = getTimeMs();                                            \
	try {                                                                     \
		fxn(__VA_ARGS__);                                                     \
		rawstream << "[PASS] ";                                               \
	} catch (TestFailedException &e) {                                        \
		rawstream << "[FAIL] ";                                               \
		num_tests_failed++;                                                   \
	} catch (std::exception &e) {                                             \
		rawstream << "Caught unhandled exception: " << e.what() << std::endl; \
		rawstream << "[FAIL] ";                                               \
		num_tests_failed++;                                                   \
	}                                                                         \
	num_tests_run++;                                                          \
	uint64_t tdiff = getTimeMs() - t1;                                    \
	rawstream << #fxn << " - " << tdiff << "ms" << std::endl;                 \
}

// Asserts the specified condition is true, or fails the current unit test
#define UASSERT(x)                                              \
	if (!(x)) {                                                 \
		rawstream << "Test assertion failed: " #x << std::endl  \
			<< "    at " << fs::GetFilenameFromPath(__FILE__)   \
			<< ":" << __LINE__ << std::endl;                    \
		throw TestFailedException();                            \
	}

// Asserts the specified condition is true, or fails the current unit test
// and prints the format specifier fmt
#define UTEST(x, fmt, ...)                                               \
	if (!(x)) {                                                          \
		char utest_buf[1024];                                            \
		snprintf(utest_buf, sizeof(utest_buf), fmt, __VA_ARGS__);        \
		rawstream << "Test assertion failed: " << utest_buf << std::endl \
			<< "    at " << fs::GetFilenameFromPath(__FILE__)            \
			<< ":" << __LINE__ << std::endl;                             \
		throw TestFailedException();                                     \
	}

// Asserts the comparison specified by CMP is true, or fails the current unit test
#define UASSERTCMP(T, CMP, actual, expected) {                            \
	T a = (actual);                                                       \
	T e = (expected);                                                     \
	if (!(a CMP e)) {                                                     \
		rawstream                                                         \
			<< "Test assertion failed: " << #actual << " " << #CMP << " " \
			<< #expected << std::endl                                     \
			<< "    at " << fs::GetFilenameFromPath(__FILE__) << ":"      \
			<< __LINE__ << std::endl                                      \
			<< "    actual:   " << a << std::endl << "    expected: "     \
			<< e << std::endl;                                            \
		throw TestFailedException();                                      \
	}                                                                     \
}

#define UASSERTEQ(T, actual, expected) UASSERTCMP(T, ==, actual, expected)

// UASSERTs that the specified exception occurs
#define EXCEPTION_CHECK(EType, code) {    \
	bool exception_thrown = false;        \
	try {                                 \
		code;                             \
	} catch (EType &e) {                  \
		exception_thrown = true;          \
	}                                     \
	UASSERT(exception_thrown);            \
}


class TestBase {
public:
	bool testModule();
	std::string getTestTempDirectory();
	std::string getTestTempFile();

	virtual void runTests() = 0;
	virtual const char *getName() = 0;

	uint32_t num_tests_failed;
	uint32_t num_tests_run;

private:
	std::string m_test_dir;
};

class TestManager {
public:
	static std::vector<TestBase *> &getTestModules()
	{
		static std::vector<TestBase *> m_modules_to_test;
		return m_modules_to_test;
	}

	static void registerTestModule(TestBase *module)
	{
		getTestModules().push_back(module);
	}
};

bool run_tests();
