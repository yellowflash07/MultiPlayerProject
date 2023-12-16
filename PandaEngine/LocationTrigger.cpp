#include "LocationTrigger.h"

LocationTrigger::LocationTrigger()
{
}

bool LocationTrigger::Execute(double deltaTime)
{
	if (isCancelled)
	{
		return true;
	}

	cAABB* pAABB1 = meshToTrigger;
	cAABB* pAABB2 = aabb;
	if (pAABB1->maxXYZ.x < pAABB2->minXYZ.x || pAABB1->minXYZ.x > pAABB2->maxXYZ.x)
	{
		return false;
	}
	if (pAABB1->maxXYZ.y < pAABB2->minXYZ.y || pAABB1->minXYZ.y > pAABB2->maxXYZ.y)
	{
		return false;
	}
	if (pAABB1->maxXYZ.z < pAABB2->minXYZ.z || pAABB1->minXYZ.z > pAABB2->maxXYZ.z)
	{
		return false;
	}

	if (OnEnter)
	{
		OnEnter();
	}

	return true;
}

void LocationTrigger::SetParams(cAABB* meshToTrigger, glm::vec3 location, float size)
{
	 this->meshToTrigger = meshToTrigger;
	 aabb = new cAABB();
	 aabb->minXYZ = location - glm::vec3(size);
	 aabb->maxXYZ = location + glm::vec3(size);
}

