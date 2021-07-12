#pragma once

#include "interfaces.h"
#include "3rdParty/LuaPlus/LuaPlus.h"

class Component
{
protected:
	StrongActorPtr m_pOwner;

public:
	virtual ~Component(void) { m_pOwner.reset(); }

	// These functions are meant to be overridden by the implementation classes of the components.
	virtual bool VInit(LuaPlus::LuaObject pData) = 0;

	virtual void VPostInit(void) { }
	
	virtual void VUpdate(float deltaMs) { }
	
	virtual void VOnChanged(void) { }

	// This function should be overridden by the interface class.
	virtual ComponentId VGetId(void) const { return GetIdFromName(VGetName()); }
	
	virtual const char *VGetName() const = 0;
	
	static ComponentId GetIdFromName(const char* componentStr);

private:
	void SetOwner(StrongActorPtr pOwner) { m_pOwner = pOwner; }
};