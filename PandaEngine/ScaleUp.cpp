#include "ScaleUp.h"

ScaleUp::ScaleUp()
{
}


void ScaleUp::SetParams(cMesh* mesh, float speed, float maxScale)
{
	this->mesh = mesh;
	this->speed = speed;
	this->maxScale = maxScale;
}

bool ScaleUp::Execute(double deltaTime)
{

	if (mesh->drawScale.x < maxScale)
	{
		mesh->drawScale += glm::vec3(speed) * (float)deltaTime;
		return false;
	}

	return true;
}

