#include "LuaStateManager.h"
#include "utils/macros.h"
#include "log.h"

LuaStateManager* LuaStateManager::s_pSingleton = NULL;

bool LuaStateManager::Create(lua_State* L)
{
    if (s_pSingleton)
    {
        errorstream << "Overwriting LuaStateManager singleton";
        SAFE_DELETE(s_pSingleton);
    }

    s_pSingleton = new LuaStateManager;
    if (s_pSingleton)
        return s_pSingleton->Init(L);

    return false;
}

void LuaStateManager::Destroy(void)
{
    assert(s_pSingleton);
    SAFE_DELETE(s_pSingleton);
}

LuaStateManager::LuaStateManager(void)
{
    m_pLuaState = NULL;
}

LuaStateManager::~LuaStateManager(void)
{
    if (m_pLuaState)
    {
        LuaPlus::LuaState::Destroy(m_pLuaState);
        m_pLuaState = NULL;
    }
}

bool LuaStateManager::Init(lua_State* L)
{
	if (L)
	{
		m_pLuaState = LuaPlus::LuaState::Create(L);
	}
	else
	{
		m_pLuaState = LuaPlus::LuaState::Create();
	}
    
    if (m_pLuaState == nullptr)
        return false;

 
    return true;
}


void LuaStateManager::SetError(int errorNum)
{
    // Note: If we get an error, we're hosed because LuaPlus throws an exception.  So if this function
    // is called and the error at the bottom triggers, you might as well pack it in.

    LuaPlus::LuaStackObject stackObj(m_pLuaState,-1);
    const char* errStr = stackObj.GetString();
    if (errStr)
    {
        m_lastError = errStr;
        ClearStack();
    }
    else
        m_lastError = "Unknown Lua parse error";

    errorstream << (m_lastError);
}

void LuaStateManager::ClearStack(void)
{
    m_pLuaState->SetTop(0);
}

LuaPlus::LuaObject LuaStateManager::GetGlobalVars(void)
{
    assert(m_pLuaState);
    return m_pLuaState->GetGlobals();
}

LuaPlus::LuaState* LuaStateManager::GetLuaState(void) const
{
    return m_pLuaState;
}