#pragma once

#include <string>
#include <memory>

class EventManager;

class BaseApp
{
public:
	BaseApp();

	virtual ~BaseApp();

	virtual bool init();

	virtual std::string getResPath() { return ""; };

	void update(float dt);

private:
	bool init_setting();

	void init_log_streams();

	virtual bool init_lua_manager();

	void registerLuaFunc();
private:
	std::shared_ptr<EventManager> m_pEventManger;
};

extern std::shared_ptr<BaseApp> g_pApp;

