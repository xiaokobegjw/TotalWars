#pragma once

#include "BaseApp.h"

class TotalWarsApp :public BaseApp
{
public:
	TotalWarsApp();

	~TotalWarsApp();

	bool init() override;

	std::string getResPath() override;

private:
	bool init_lua_manager() override;
};