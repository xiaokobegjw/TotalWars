#include "ScriptExports.h"
#include "ScriptEvent.h"
#include "LuaStateManager.h"
#include "log.h"
#include "utils/macros.h"
#include "utils/time_utils.h"
#include <set>

class ScriptEventListener
{
    EventType m_eventType;
	LuaPlus::LuaObject m_scriptCallbackFunction;

public:
	explicit ScriptEventListener(const EventType& eventType, const LuaPlus::LuaObject& scriptCallbackFunction);
    ~ScriptEventListener(void);
    EventListenerDelegate GetDelegate(void) { return fastdelegate::MakeDelegate(this, &ScriptEventListener::ScriptEventDelegate); }
	void ScriptEventDelegate(IEventDataPtr pEventPtr);
};

class ScriptEventListenerMgr
{
	typedef std::set<ScriptEventListener*> ScriptEventListenerSet;
	ScriptEventListenerSet m_listeners;

public:
	~ScriptEventListenerMgr(void);
	void AddListener(ScriptEventListener* pListener);
	void DestroyListener(ScriptEventListener* pListener);
};

class InternalScriptExports
{
	static ScriptEventListenerMgr* s_pScriptEventListenerMgr;

public:
	// initialization
	static bool Init(void);
	static void Destroy(void);

	// resource loading
	static bool LoadAndExecuteScriptResource(const char* scriptResource);
	
	// actors
	static int CreateActor(const char* actorArchetype, LuaPlus::LuaObject luaPosition, LuaPlus::LuaObject luaYawPitchRoll);

	// event system
	static unsigned long RegisterEventListener(EventType eventType, LuaPlus::LuaObject callbackFunction);
	static void RemoveEventListener(unsigned long listenerId);
	static bool QueueEvent(EventType eventType, LuaPlus::LuaObject eventData);
	static bool TriggerEvent(EventType eventType, LuaPlus::LuaObject eventData);
	
	// process system
	static void AttachScriptProcess(LuaPlus::LuaObject scriptProcess);

    // math
    static float GetYRotationFromVector(LuaPlus::LuaObject vec3);
    static float WrapPi(float wrapMe);
    static LuaPlus::LuaObject GetVectorFromRotation(float angleRadians);

    // misc
    static void LuaLog(LuaPlus::LuaObject text);
    static unsigned long GetTickCount(void);

	// physics
	static void ApplyForce(LuaPlus::LuaObject normalDir, float force, int actorId);
	static void ApplyTorque(LuaPlus::LuaObject axis, float force, int actorId);

private:
	static std::shared_ptr<ScriptEvent> BuildEvent(EventType eventType, LuaPlus::LuaObject& eventData);
};

ScriptEventListenerMgr* InternalScriptExports::s_pScriptEventListenerMgr = NULL;




//---------------------------------------------------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------------------------------------------------
ScriptEventListenerMgr::~ScriptEventListenerMgr(void)
{
	for (auto it = m_listeners.begin(); it != m_listeners.end(); ++it)
	{
		ScriptEventListener* pListener = (*it);
		delete pListener;
	}
	m_listeners.clear();
}

//---------------------------------------------------------------------------------------------------------------------
// Adds a new listener
//---------------------------------------------------------------------------------------------------------------------
void ScriptEventListenerMgr::AddListener(ScriptEventListener* pListener)
{
	m_listeners.insert(pListener);
}

//---------------------------------------------------------------------------------------------------------------------
// Destroys a listener
//---------------------------------------------------------------------------------------------------------------------
void ScriptEventListenerMgr::DestroyListener(ScriptEventListener* pListener)
{
	ScriptEventListenerSet::iterator findIt = m_listeners.find(pListener);
	if (findIt != m_listeners.end())
	{
		m_listeners.erase(findIt);
		delete pListener;
	}
	else
	{
		errorstream<<("Couldn't find script listener in set; this will probably cause a memory leak");
	}
}


//---------------------------------------------------------------------------------------------------------------------
// Event Listener
//---------------------------------------------------------------------------------------------------------------------
ScriptEventListener::ScriptEventListener(const EventType& eventType, const LuaPlus::LuaObject& scriptCallbackFunction)
:   m_scriptCallbackFunction(scriptCallbackFunction)
{
    m_eventType = eventType;
}

ScriptEventListener::~ScriptEventListener(void)
{
    IEventManager* pEventMgr = IEventManager::Get();
    if (pEventMgr)
        pEventMgr->VRemoveListener(GetDelegate(), m_eventType);
}

void ScriptEventListener::ScriptEventDelegate(IEventDataPtr pEvent)
{
    assert(m_scriptCallbackFunction.IsFunction());  // this should never happen since it's validated before even creating this object

    // call the Lua function
	std::shared_ptr<ScriptEvent> pScriptEvent = std::static_pointer_cast<ScriptEvent>(pEvent);
	LuaPlus::LuaFunction<int> Callback = m_scriptCallbackFunction;
	Callback(pScriptEvent->GetEventData());
}


//---------------------------------------------------------------------------------------------------------------------
// Initializes the script export system
//---------------------------------------------------------------------------------------------------------------------
bool InternalScriptExports::Init(void)
{
	assert(s_pScriptEventListenerMgr == NULL);
	s_pScriptEventListenerMgr = new ScriptEventListenerMgr;
	
	return true;
}

//---------------------------------------------------------------------------------------------------------------------
// Destroys the script export system
//---------------------------------------------------------------------------------------------------------------------
void InternalScriptExports::Destroy(void)
{
	assert(s_pScriptEventListenerMgr != NULL);
	SAFE_DELETE(s_pScriptEventListenerMgr);
}

//---------------------------------------------------------------------------------------------------------------------
// Instantiates a C++ ScriptListener object, inserts it into the manager, and returns a handle to it.  The script 
// should maintain the handle if it needs to remove the listener at some point.  Otherwise, the listener will be 
// destroyed when the program exits.
//---------------------------------------------------------------------------------------------------------------------
unsigned long InternalScriptExports::RegisterEventListener(EventType eventType, LuaPlus::LuaObject callbackFunction)
{
	assert(s_pScriptEventListenerMgr);

	if (callbackFunction.IsFunction())
	{
		// create the C++ listener proxy and set it to listen for the event
		ScriptEventListener* pListener = new ScriptEventListener(eventType, callbackFunction);
		s_pScriptEventListenerMgr->AddListener(pListener);
		IEventManager::Get()->VAddListener(pListener->GetDelegate(), eventType);
		
		// convert the pointer to an unsigned long to use as the handle
		unsigned long handle = reinterpret_cast<unsigned long>(pListener);
		return handle;
	}

	errorstream<< ("Attempting to register script event listener with invalid callback function");
	return 0;
}

//---------------------------------------------------------------------------------------------------------------------
// Removes a script listener.
//---------------------------------------------------------------------------------------------------------------------
void InternalScriptExports::RemoveEventListener(unsigned long listenerId)
{
	assert(s_pScriptEventListenerMgr);
	assert(listenerId != 0);
	
	// convert the listenerId back into a pointer
	ScriptEventListener* pListener = reinterpret_cast<ScriptEventListener*>(listenerId);
	s_pScriptEventListenerMgr->DestroyListener(pListener);  // the destructor will remove the listener
}

//---------------------------------------------------------------------------------------------------------------------
// Queue's an event from the script.  Returns true if the event was sent, false if not.
//---------------------------------------------------------------------------------------------------------------------
bool InternalScriptExports::QueueEvent(EventType eventType, LuaPlus::LuaObject eventData)
{
	std::shared_ptr<ScriptEvent> pEvent(BuildEvent(eventType, eventData));
    if (pEvent)
    {
	    IEventManager::Get()->VQueueEvent(pEvent);
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------------------------------------------------
// Sends an event from the script.  Returns true if the event was sent, false if not.
//---------------------------------------------------------------------------------------------------------------------
bool InternalScriptExports::TriggerEvent(EventType eventType, LuaPlus::LuaObject eventData)
{
	std::shared_ptr<ScriptEvent> pEvent(BuildEvent(eventType, eventData));
    if (pEvent)
	    return IEventManager::Get()->VTriggerEvent(pEvent);
    return false;
}

std::shared_ptr<ScriptEvent> InternalScriptExports::BuildEvent(EventType eventType, LuaPlus::LuaObject& eventData)
{
	// create the event from the event type
	std::shared_ptr<ScriptEvent> pEvent(ScriptEvent::CreateEventFromScript(eventType));
	if (!pEvent)
        return std::shared_ptr<ScriptEvent>();

	// set the event data that was passed in
	if (!pEvent->SetEventData(eventData))
	{
		return std::shared_ptr<ScriptEvent>();
	}
	
	return pEvent;
}


int InternalScriptExports::CreateActor(const char* actorArchetype, LuaPlus::LuaObject luaPosition, LuaPlus::LuaObject luaYawPitchRoll)
{
	return -1;
}


void InternalScriptExports::LuaLog(LuaPlus::LuaObject text)
{
    if (text.IsConvertibleToString())
    {
		actionstream << "Lua:" << text.ToString();
    }
    else
    {
		actionstream << "Lua:" <<"<"<< text.TypeName()<<">";
    }
}

unsigned long InternalScriptExports::GetTickCount(void)
{
    return getTimeMs();
}

//---------------------------------------------------------------------------------------------------------------------
// This function registers all the ScriptExports functions with the scripting system.  It is called in 
// Application::Init().
//---------------------------------------------------------------------------------------------------------------------
void ScriptExports::Register(void)
{
	LuaPlus::LuaObject globals = LuaStateManager::Get()->GetGlobalVars();

	// init	
	InternalScriptExports::Init();

	// actors
	globals.RegisterDirect("CreateActor", &InternalScriptExports::CreateActor);

	// event system
	globals.RegisterDirect("RegisterEventListener", &InternalScriptExports::RegisterEventListener);
	globals.RegisterDirect("RemoveEventListener", &InternalScriptExports::RemoveEventListener);
	globals.RegisterDirect("QueueEvent", &InternalScriptExports::QueueEvent);
	globals.RegisterDirect("TriggerEvent", &InternalScriptExports::TriggerEvent);
	
	// misc
	globals.RegisterDirect("Log", &InternalScriptExports::LuaLog);
    globals.RegisterDirect("GetTickCount", &InternalScriptExports::GetTickCount);
}

//---------------------------------------------------------------------------------------------------------------------
// This function unregisters all the ScriptExports functions and gives any underlying systems a chance to destroy 
// themselves.  It is called in the Application destructor.
//---------------------------------------------------------------------------------------------------------------------
void ScriptExports::Unregister(void)
{
	InternalScriptExports::Destroy();
}
