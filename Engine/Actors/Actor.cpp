#include "Actor.h"
#include "components/componentmanager.h"

Actor::Actor()
{

}

Actor::~Actor()
{

}

void Actor::init(LuaPlus::LuaObject)
{

}

void Actor::update(float dt)
{
	if (m_componentManger)
	{
		m_componentManger->update(dt);
	}
}

