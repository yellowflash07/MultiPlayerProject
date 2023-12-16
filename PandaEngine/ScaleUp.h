#pragma once
#include "Command.h"
#include "cMesh.h"

class ScaleUp :
    public Command
{
public:
	ScaleUp();
	virtual ~ScaleUp() {};
	virtual bool Execute(double deltaTime);
	virtual void SetParams(cMesh* mesh, float speed, float maxScale);

private:
	cMesh* mesh;
	float speed;
	float maxScale;
};

