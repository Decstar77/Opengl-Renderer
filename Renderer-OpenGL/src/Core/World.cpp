#include "World.h"

namespace cm
{




	void World::RegisterWorldObject(WorldObject *object)
	{
		// @NOTE: This will get more complex later
		Assert(object != nullptr);
		this->objects.push_back(object);
	}

}
