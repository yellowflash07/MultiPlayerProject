#pragma once
#include "Command.h"
#include "cMesh.h"

class FollowObject :
    public Command
{
public:
	FollowObject();
	virtual ~FollowObject() {};

	void SetParams(cMesh* mesh, cMesh* target, glm::vec3 offset, float minSpeed, float maxSpeed);
	virtual bool Execute(double deltaTime) override;
	void StopFollowIn(float time);
private:
	cMesh* mesh;
	cMesh* target;
	float minSpeed;
	float maxSpeed;
	float currSpeed;
	float currTime;
	glm::vec3 offset;
	float stopFollowTime;
	glm::vec3 lerp(glm::vec3 start, glm::vec3 end, float percent);
};

