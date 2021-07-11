#pragma once

#include <string>
#include "interfaces.h"
#include "math2d/vector2d.h"
#include "3rdParty/LuaPlus/LuaPlus.h"

class LuaStateManager
{
    static LuaStateManager* s_pSingleton;
    LuaPlus::LuaState* m_pLuaState;
    std::string m_lastError;

public:
    // Singleton functions
    static bool Create(lua_State* L = nullptr);

    static void Destroy(void);
    
	static LuaStateManager* Get(void) { assert(s_pSingleton); return s_pSingleton; }

    // IScriptManager interface
	bool Init(lua_State* L = nullptr);

    LuaPlus::LuaObject GetGlobalVars(void);
    
	LuaPlus::LuaState* GetLuaState(void) const;

private:
    void SetError(int errorNum);
    void ClearStack(void);

    // private constructor & destructor; call the static Create() and Destroy() functions instead
    explicit LuaStateManager(void);
    virtual ~LuaStateManager(void);
};

