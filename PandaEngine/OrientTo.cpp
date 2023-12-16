#include "OrientTo.h"
#include <cmath>
#include <string>
#include <glm/gtx/string_cast.hpp>

OrientTo::OrientTo()
{
}

void OrientTo::SetParams(cMesh* mesh, cMesh* to, float speed, float time)
{
	this->mesh = mesh;
	this->to = to;
	this->speed = speed;
	this->time = time;
	this->targetPositionSet = true;
	this->targetRotationSet = false;
}

void OrientTo::SetParams(cMesh* mesh, glm::vec3 targetRotation, float speed, float time)
{
	this->mesh = mesh;
	this->targetRotation = targetRotation;
	this->speed = speed;
	this->time = time;
	this->targetPositionSet = false;
	this->targetRotationSet = true;
}

bool OrientTo::Execute(double deltaTime)
{
	currTime += deltaTime;

	if (currTime < delay)
	{
		return false;
	}	

	if (isCancelled)
	{
		return true;
	}

	if (OnStart)
	{
		OnStart();
	}

	glm::quat targetOrientation = glm::quat(0,0,0,0);
	if (targetPositionSet)
	{
		glm::vec3 direction = glm::normalize(mesh->drawPosition - to->drawPosition);
		targetOrientation = glm::quatLookAt(direction, glm::vec3(0, 1, 0));
	}
	if(targetRotationSet)
	{
		targetOrientation = glm::quat(glm::radians(targetRotation));
	}

	bool isDone = false;
	glm::quat resultQuat = glm::quat(0,0,0,0);

	if (speed > 0)
	{
		resultQuat = glm::slerp(glm::normalize(mesh->get_qOrientation()), glm::normalize(targetOrientation), speed * (float)deltaTime);
		float length = glm::length(glm::normalize(targetOrientation) - glm::normalize(resultQuat));
		if (length <= 0.001f)
		{
			isDone = true;  // Return false to indicate completion
			return true;  // Return false to indicate completion
		}

		mesh->setRotationFromQuat(glm::normalize(resultQuat));
		// Check if the interpolation is complete based on speed
		//std::cout << "Result: " << length << std::endl;
	}
	
	if (time > 0)
	{
		resultQuat = glm::slerp(glm::normalize(mesh->get_qOrientation()), targetOrientation, (float)deltaTime/ time);
		mesh->setRotationFromQuat(resultQuat);
		
		if (glm::length(targetOrientation - resultQuat) < 0.001f)
		{
			isDone = true;  // Return false to indicate completion
			//return true;  // Return false to indicate completion
		}
	}

	return isDone;
}
