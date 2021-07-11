#pragma once

#include <memory>
#include <strstream>

#include "FastDelegate.h"
#include "concurrentqueue.h"
#include "utils/templates.h"

class IEventData;

typedef unsigned long EventType;
typedef std::shared_ptr<IEventData> IEventDataPtr;
typedef fastdelegate::FastDelegate1<IEventDataPtr> EventListenerDelegate;
typedef concurrentqueue<IEventDataPtr> ThreadSafeEventQueue;


//---------------------------------------------------------------------------------------------------------------------
// Macro for event registration
//---------------------------------------------------------------------------------------------------------------------
extern GenericObjectFactory<IEventData, EventType> g_eventFactory;
#define REGISTER_EVENT(eventClass) g_eventFactory.Register<eventClass>(eventClass::sk_EventType)
#define CREATE_EVENT(eventType) g_eventFactory.Create(eventType)

class IEventData 
{
public:
	virtual ~IEventData(void) {}
	virtual const EventType& VGetEventType(void) const = 0;
	virtual float GetTimeStamp(void) const = 0;
	virtual void VSerialize(std::ostrstream& out) const = 0;
    virtual void VDeserialize(std::istrstream& in) = 0;
	virtual IEventDataPtr VCopy(void) const = 0;
    virtual const char* GetName(void) const = 0;

    //GCC_MEMORY_WATCHER_DECLARATION();
};

class BaseEventData : public IEventData
{
    const float m_timeStamp;

public:
	explicit BaseEventData(const float timeStamp = 0.0f) : m_timeStamp(timeStamp) { }

	// Returns the type of the event
	virtual const EventType& VGetEventType(void) const = 0;

	float GetTimeStamp(void) const { return m_timeStamp; }

	// Serializing for network input / output
	virtual void VSerialize(std::ostrstream &out) const	{ }
    virtual void VDeserialize(std::istrstream& in) { }
};

class IEventManager
{
public:

	enum eConstants { kINFINITE = 0xffffffff };

	explicit IEventManager(const char* pName, bool setAsGlobal);
	virtual ~IEventManager(void);

    // Registers a delegate function that will get called when the event type is triggered.  Returns true if 
    // successful, false if not.
    virtual bool VAddListener(const EventListenerDelegate& eventDelegate, const EventType& type) = 0;

	// Removes a delegate / event type pairing from the internal tables.  Returns false if the pairing was not found.
	virtual bool VRemoveListener(const EventListenerDelegate& eventDelegate, const EventType& type) = 0;

	// Fire off event NOW.  This bypasses the queue entirely and immediately calls all delegate functions registered 
    // for the event.
	virtual bool VTriggerEvent(const IEventDataPtr& pEvent) const = 0;

	// Fire off event.  This uses the queue and will call the delegate function on the next call to VTick(), assuming
    // there's enough time.
	virtual bool VQueueEvent(const IEventDataPtr& pEvent) = 0;
	virtual bool VThreadSafeQueueEvent(const IEventDataPtr& pEvent) = 0;

	// Find the next-available instance of the named event type and remove it from the processing queue.  This 
    // may be done up to the point that it is actively being processed ...  e.g.: is safe to happen during event
	// processing itself.
	//
	// if allOfType is true, then all events of that type are cleared from the input queue.
	//
	// returns true if the event was found and removed, false otherwise
	virtual bool VAbortEvent(const EventType& type, bool allOfType = false) = 0;

	// Allow for processing of any queued messages, optionally specify a processing time limit so that the event 
    // processing does not take too long. Note the danger of using this artificial limiter is that all messages 
    // may not in fact get processed.
	//
	// returns true if all messages ready for processing were completed, false otherwise (e.g. timeout )
	virtual bool VUpdate(unsigned long maxMillis = kINFINITE) = 0;

    // Getter for the main global event manager.  This is the event manager that is used by the majority of the 
    // engine, though you are free to define your own as long as you instantiate it with setAsGlobal set to false.
    // It is not valid to have more than one global event manager.
	static IEventManager* Get(void);

};
