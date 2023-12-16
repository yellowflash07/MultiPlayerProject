#include "LightControl.h"

LightControl::LightControl()
{
}

void LightControl::SetParams(cLight* light, glm::vec3 target, float speed)
{
    this->light = light;
	this->target = target;
	this->speed = speed;

}

bool LightControl::Execute(double deltaTime)
{
	currTime += (float)deltaTime;

	if (currTime < delay)
	{
		return false;
	}

	glm::vec3 lightPos = glm::vec3(light->position.x ,-1, light->position.y);
	glm::vec3 direction = glm::normalize(lightPos - target);
	light->position = glm::vec4(target, 1) + glm::vec4(0, 10, 0, 0);

	light->param2.x = 1.0f;
	light->direction = glm::vec4(direction.x, -1, direction.y,0);

    return false;
}


