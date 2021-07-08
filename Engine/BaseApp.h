#pragma once

#include <memory>

class BaseApp
{
public:
	BaseApp();

	virtual ~BaseApp();

	virtual bool init();

	void update(float dt);
};

extern std::shared_ptr<BaseApp> g_pApp;

