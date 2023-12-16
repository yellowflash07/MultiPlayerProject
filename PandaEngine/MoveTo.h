#pragma once

#include "Command.h"
#include <glm/glm.hpp>
#include "cMesh.h"

class MoveTo : public Command
{
public:
	MoveTo();
	MoveTo(cMesh* mesh, glm::vec3 to, float speed);
	virtual ~MoveTo() {};

	void SetParams(cMesh* mesh, glm::vec3 to, float speed);
	void SetParams(cMesh* mesh, cMesh* target, float speed);
	virtual bool Execute(double deltaTime) override;

private:
	cMesh* mesh;
	cMesh* target;
	glm::vec3 to;
	float speed;
	bool targetSet;
	bool toSet;
};

