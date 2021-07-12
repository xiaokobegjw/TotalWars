#include "component.h"
#include "utils/hashedstring.h"

static ComponentId GetIdFromName(const char* componentStr)
{
	void* rawId = HashedString::hash_name(componentStr);
	return reinterpret_cast<ComponentId>(rawId);
}
