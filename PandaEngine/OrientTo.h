#pragma once

#include "Command.h"
#include "cMesh.h"

class OrientTo
	: public Command
{
public:
	OrientTo();
	virtual ~OrientTo() {};

	void SetParams(cMesh* mesh, cMesh* to, float speed, float time);
	void SetParams(cMesh* mesh, glm::vec3 targetRotation, float speed, float time);
	virtual bool Execute(double deltaTime) override;

private:
	cMesh* mesh;
	cMesh* to;
	glm::vec3 targetRotation;
	float speed;
	float time;
	float currTime;
	bool targetRotationSet;
	bool targetPositionSet;
};

