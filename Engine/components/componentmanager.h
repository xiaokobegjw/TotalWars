#pragma once

#include <map>
#include <memory>
#include "component.h"

class ComponentManager
{
	typedef std::map<ComponentId, StrongActorComponentPtr> ActorComponents;

public:

	void update(float dt);

	template <class ComponentType>
	std::weak_ptr<ComponentType> GetComponent(ComponentId id)
	{
		ActorComponents::iterator findIt = m_components.find(id);
		if (findIt != m_components.end())
		{
			StrongActorComponentPtr pBase(findIt->second);
			shared_ptr<ComponentType> pSub(static_pointer_cast<ComponentType>(pBase));  // cast to subclass version of the pointer
			weak_ptr<ComponentType> pWeakSub(pSub);  // convert strong pointer to weak pointer
			return pWeakSub;  // return the weak pointer
		}
		else
		{
			return weak_ptr<ComponentType>();
		}
	}

	template <class ComponentType>
	std::weak_ptr<ComponentType> GetComponent(const char *name)
	{
		ComponentId id = ActorComponent::GetIdFromName(name);
		ActorComponents::iterator findIt = m_components.find(id);
		if (findIt != m_components.end())
		{
			StrongActorComponentPtr pBase(findIt->second);
			shared_ptr<ComponentType> pSub(static_pointer_cast<ComponentType>(pBase));  // cast to subclass version of the pointer
			weak_ptr<ComponentType> pWeakSub(pSub);  // convert strong pointer to weak pointer
			return pWeakSub;  // return the weak pointer
		}
		else
		{
			return weak_ptr<ComponentType>();
		}
	}

	const ActorComponents* GetComponents() { return &m_components; }

	void AddComponent(StrongActorComponentPtr pComponent);

private:
	ActorComponents m_components;
};