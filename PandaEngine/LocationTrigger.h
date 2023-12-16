#pragma once
#include "Command.h"
#include <glm/glm.hpp>
#include <functional>
#include "cAABB.h"
class LocationTrigger :
    public Command
{
public:
    LocationTrigger();
    virtual ~LocationTrigger() {};
    virtual bool Execute(double deltaTime);
    virtual void SetParams(cAABB* meshToTrigger, glm::vec3 location, float size);

    std::function<void()> OnEnter;
    cAABB* aabb;
private:
    cAABB* meshToTrigger;
};

