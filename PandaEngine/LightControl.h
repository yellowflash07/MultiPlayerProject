#pragma once
#include "Command.h"
#include <glm/glm.hpp>
#include "cLightManager.h"
class LightControl :
    public Command
{
public:
	LightControl();
	virtual ~LightControl() {};
	virtual bool Execute(double deltaTime);
	virtual void SetParams(cLight* light, glm::vec3 target, float speed);
private:
	cLight* light;
	glm::vec3 target;
	float speed;
};

