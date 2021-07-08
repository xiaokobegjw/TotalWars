#pragma once

#include "test.h"

class TestSettings : public TestBase {
public:
	TestSettings() { TestManager::registerTestModule(this); }
	const char *getName() { return "TestSettings"; }

	void runTests();

	void testAllSettings();
	void testDefaults();
	void testFlagDesc();

	static const char *config_text_before;
	static const std::string config_text_after;
};