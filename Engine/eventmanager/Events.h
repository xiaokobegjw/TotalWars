#pragma once

#include "interfaces.h"
#include "LUAScripting/ScriptEvent.h"

class EvtData_ScriptEventTest_FromLua : public ScriptEvent
{
	int m_num;

public:
	static const EventType sk_EventType;

	EvtData_ScriptEventTest_FromLua(void) { m_num = 0; }
	EvtData_ScriptEventTest_FromLua(int num) { m_num = num; }

	virtual const EventType& VGetEventType(void) const { return sk_EventType; }

	virtual IEventDataPtr VCopy(void) const
	{
		std::shared_ptr<EvtData_ScriptEventTest_FromLua> pCopy(new EvtData_ScriptEventTest_FromLua(m_num));
		pCopy->m_eventData = m_eventData;
		return pCopy;
	}

	virtual const char* GetName(void) const { return "EvtData_ScriptEventTest_FromLua"; }

	int GetNum(void) { return m_num; }

protected:
	//virtual void VBuildEventData(void);
	virtual bool VBuildEventFromScript(void);

	EXPORT_FOR_SCRIPT_EVENT(EvtData_ScriptEventTest_FromLua);
};

void RegisterScriptEvents(void);