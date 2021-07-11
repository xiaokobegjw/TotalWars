#include "TotalWarsApp.h"
#include "filesys.h"
#include "log.h"
#include "LUAScripting/LuaStateManager.h"
#include "scripting/lua-bindings/manual/CCLuaEngine.h"

#include "cocos2d.h"

USING_NS_CC;

TotalWarsApp::TotalWarsApp()
{

}

TotalWarsApp::~TotalWarsApp()
{

}

bool TotalWarsApp::init()
{
	return BaseApp::init();
}

std::string TotalWarsApp::getResPath()
{
	static std::string resPath;
	if (resPath.empty())
	{
		resPath = FileUtils::getInstance()->fullPathForFilename("temp");
		resPath = fs::GetDirectoryFromPath(resPath.c_str());
	}

	return resPath;
}

bool TotalWarsApp::init_lua_manager()
{
	auto engine = LuaEngine::getInstance();
	lua_State* L = engine->getLuaStack()->getLuaState();

	if (!LuaStateManager::Create(L))
	{
		errorstream << ("Failed to initialize Lua");
		return false;
	}

	return true;
}

