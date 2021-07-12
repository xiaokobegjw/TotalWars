#pragma once

#include "interfaces.h"
#include "3rdParty/LuaPlus/LuaPlus.h"

class ComponentManager;

class Actor
{
	friend class ActorFactory;
public:
	Actor();

	~Actor();

	void init(LuaPlus::LuaObject);

	void update(float dt);

private:
	ActorId m_id;					// unique id for the actor
	std::shared_ptr<ComponentManager> m_componentManger;
};