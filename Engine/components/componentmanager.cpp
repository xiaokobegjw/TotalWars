#include "componentmanager.h"
#include "debug.h"

void ComponentManager::update(float dt)
{
	for (auto iter = m_components.begin();iter != m_components.end();iter++)
	{
		iter->second->VUpdate(dt);
	}
}

void ComponentManager::AddComponent(StrongActorComponentPtr pComponent)
{
	std::pair<ActorComponents::iterator, bool> success = m_components.insert(std::make_pair(pComponent->VGetId(), pComponent));
	SANITY_CHECK(success.second);
}
