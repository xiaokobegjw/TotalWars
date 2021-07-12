#pragma once

#include <memory>

class Actor;
class Component;

typedef unsigned int ActorId;
typedef unsigned int ComponentId;

typedef std::shared_ptr<Actor> StrongActorPtr;
typedef std::weak_ptr<Actor> WeakActorPtr;
typedef std::shared_ptr<Component> StrongActorComponentPtr;
typedef std::weak_ptr<Component> WeakActorComponentPtr;