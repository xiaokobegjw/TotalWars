#include "Events.h"

const EventType EvtData_ScriptEventTest_FromLua::sk_EventType(0x53fbab61);

bool EvtData_ScriptEventTest_FromLua::VBuildEventFromScript(void)
{
	if (m_eventData.IsInteger())
	{
		m_num = m_eventData.GetInteger();
		return true;
	}

	return false;
}


void RegisterScriptEvents(void)
{
	REGISTER_SCRIPT_EVENT(EvtData_ScriptEventTest_FromLua, EvtData_ScriptEventTest_FromLua::sk_EventType);
}
