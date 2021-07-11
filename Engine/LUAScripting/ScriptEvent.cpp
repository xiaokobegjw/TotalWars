#include "ScriptEvent.h"
#include "LuaStateManager.h"
#include "log.h"
#include <cassert>

ScriptEvent::CreationFunctions ScriptEvent::s_creationFunctions;

//---------------------------------------------------------------------------------------------------------------------
// Returns the event data after building it (if necessary)
//---------------------------------------------------------------------------------------------------------------------
LuaPlus::LuaObject ScriptEvent::GetEventData(void)
{
	if (!m_eventDataIsValid)
	{
		VBuildEventData();
		m_eventDataIsValid = true;
	}
	
	return m_eventData;
}


//---------------------------------------------------------------------------------------------------------------------
// This function is called when an event is sent from the script.  It sets the m_eventData member and calls 
// VBuildEventFromScript().
//---------------------------------------------------------------------------------------------------------------------
bool ScriptEvent::SetEventData(LuaPlus::LuaObject eventData)
{
	m_eventData = eventData;
	m_eventDataIsValid = VBuildEventFromScript();
	return m_eventDataIsValid;
}


//---------------------------------------------------------------------------------------------------------------------
// This function is called to register an event type with the script to link them.  The simplest way to do this is 
// with the REGISTER_SCRIPT_EVENT() macro, which calls this function.
//---------------------------------------------------------------------------------------------------------------------
void ScriptEvent::RegisterEventTypeWithScript(const char* key, EventType type)
{
	// get or create the EventType table
	LuaPlus::LuaObject eventTypeTable = LuaStateManager::Get()->GetGlobalVars().GetByName("EventType");
	if (eventTypeTable.IsNil())
		eventTypeTable = LuaStateManager::Get()->GetGlobalVars().CreateTable("EventType");

	// error checking
	assert(eventTypeTable.IsTable());
	assert(eventTypeTable[key].IsNil());
	
	// add the entry
	eventTypeTable.SetNumber(key, (double)type);
}


//---------------------------------------------------------------------------------------------------------------------
// This function is called to map an event creation function pointer with the event type.  This allows an event to be
// created by only knowing its type.  This is required to allow scripts to trigger the instantiation and queueing of 
// events.
//---------------------------------------------------------------------------------------------------------------------
void ScriptEvent::AddCreationFunction(EventType type, CreateEventForScriptFunctionType pCreationFunctionPtr)
{
	assert(s_creationFunctions.find(type) == s_creationFunctions.end());
	assert(pCreationFunctionPtr != NULL);
	s_creationFunctions.insert(std::make_pair(type, pCreationFunctionPtr));
}

//---------------------------------------------------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------------------------------------------------
ScriptEvent* ScriptEvent::CreateEventFromScript(EventType type)
{
	CreationFunctions::iterator findIt = s_creationFunctions.find(type);
	if (findIt != s_creationFunctions.end())
	{
		CreateEventForScriptFunctionType func = findIt->second;
		return func();
	}
	else
	{
		errorstream<< "Couldn't find event type";
		return NULL;
	}
}


//---------------------------------------------------------------------------------------------------------------------
// Default implementation for VBuildEventData() sets the event data to nil.
//---------------------------------------------------------------------------------------------------------------------
void ScriptEvent::VBuildEventData(void)
{
	m_eventData.AssignNil(LuaStateManager::Get()->GetLuaState());
}
