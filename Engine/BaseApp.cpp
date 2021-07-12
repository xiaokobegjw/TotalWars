#include "BaseApp.h"
#include "debug.h"
#include "log.h"
#include "settings.h"
#include "utils/macros.h"
#include "unittest/test.h"
#include "LUAScripting/LuaStateManager.h"
#include "LUAScripting/ScriptExports.h"
#include "eventmanager/EventManagerImpl.h"
#include "eventmanager/Events.h"

std::shared_ptr<BaseApp> g_pApp = nullptr;
FileLogOutput file_log_output;

BaseApp::BaseApp()
{

}


BaseApp::~BaseApp()
{

}

bool BaseApp::init()
{
	if (!init_setting())
		return false;

	init_log_streams();

	if (!init_lua_manager())
		return false;

	registerLuaFunc();

	RegisterScriptEvents();

	m_pEventManger = std::make_shared<EventManager>("GameCodeApp Event Mgr", true);
	if (!m_pEventManger)
	{
		errorstream << "Failed to create EventManager.";
		return false;
	}

	if (g_settings->getBool("unittest"))
	{
		run_tests();
	}

	return true;
}

void BaseApp::update(float dt)
{
	IEventManager::Get()->VUpdate(20);
}

bool BaseApp::init_setting()
{
	Settings::createLayer(SL_GLOBAL);

	auto resPath = getResPath();

	bool r = g_settings->readConfigFile((resPath + "setting.txt").c_str());
	if (!r)
	{
		return false;
	}

	return true;
}

void BaseApp::init_log_streams()
{
	g_logger.registerThread("Main");
	g_logger.addOutputMaxLevel(&stderr_output, LL_ACTION);

	std::string log_filename = g_settings->get("logfile");

	g_logger.removeOutput(&file_log_output);
	std::string conf_loglev = g_settings->get("debug_log_level");

	// Old integer format
	if (std::isdigit(conf_loglev[0])) {
		warningstream << "Deprecated use of debug_log_level with an "
			"integer value; please update your configuration." << std::endl;
		static const char *lev_name[] =
		{ "", "error", "action", "info", "verbose" };
		int lev_i = atoi(conf_loglev.c_str());
		if (lev_i < 0 || lev_i >= (int)ARRLEN(lev_name)) {
			warningstream << "Supplied invalid debug_log_level!"
				"  Assuming action level." << std::endl;
			lev_i = 2;
		}
		conf_loglev = lev_name[lev_i];
	}

	if (log_filename.empty() || conf_loglev.empty())  // No logging
		return;

	LogLevel log_level = Logger::stringToLevel(conf_loglev);
	if (log_level == LL_MAX) {
		warningstream << "Supplied unrecognized debug_log_level; "
			"using maximum." << std::endl;
	}

	file_log_output.setFile(log_filename,
		g_settings->getU64("debug_log_size_max") * 1000000);
	g_logger.addOutputMaxLevel(&file_log_output, log_level);
}

bool BaseApp::init_lua_manager()
{
	// Rez up the Lua State manager now, and run the initial script - discussed in Chapter 5, page 144.
	if (!LuaStateManager::Create())
	{
		errorstream << ("Failed to initialize Lua");
		return false;
	}

	return true;
}

void BaseApp::registerLuaFunc()
{
	ScriptExports::Register();
}