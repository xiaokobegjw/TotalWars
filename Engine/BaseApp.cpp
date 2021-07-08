#include "BaseApp.h"
#include "unittest/test.h"

std::shared_ptr<BaseApp> g_pApp = nullptr;

BaseApp::BaseApp()
{

}


BaseApp::~BaseApp()
{

}

bool BaseApp::init()
{
	run_tests();

	return true;
}

void BaseApp::update(float dt)
{

}