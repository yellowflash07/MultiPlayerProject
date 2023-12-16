#include "FollowCurve.h"
#include <glm/gtx/spline.hpp>
FollowCurve::FollowCurve()
{
}

void FollowCurve::SetParams(cMesh* mesh, std::vector<glm::vec3> curvePoints, bool orienToPath)
{
	this->mesh = mesh;
	this->curvePoints = curvePoints;
	this->orient = orienToPath;
}

bool FollowCurve::Execute(double deltaTime)
{
	currTime += (float)deltaTime;
	if (currTime > 1.f)
	{
		currTime = 0.0f;
		pointIndex += 1;
	}

	if (pointIndex == 0)
	{
		mesh->drawPosition = PointOnCurve(curvePoints[pointIndex],
							curvePoints[pointIndex + 1],
							curvePoints[pointIndex + 2],
							curvePoints[pointIndex + 3], (float)currTime);

		/*glm::vec3 direction = glm::normalize(to - mesh->drawPosition);
		mesh->drawPosition += direction * speed * (float)deltaTime;*/
		//glm::vec3 direction = glm::normalize(to - mesh->drawPosition);
		//mesh->drawPosition += direction * speed * (float)deltaTime;

		if (orient)
		{
			glm::vec3 direction = glm::normalize(mesh->drawPosition - curvePoints[pointIndex + 2]);
			glm::quat targetOrientation = glm::quatLookAt(direction, glm::vec3(0, 1, 0));
			glm::quat resultQuat = glm::slerp(mesh->get_qOrientation(), targetOrientation, currTime);
			mesh->setRotationFromQuat(glm::normalize(resultQuat));
		}
		

	//	return false;
	}
	else
	{
		if (pointIndex < curvePoints.size() - 3)
		{
			mesh->drawPosition = PointOnCurve(curvePoints[pointIndex],
				curvePoints[pointIndex + 1],
				curvePoints[pointIndex + 2],
				curvePoints[pointIndex + 3], (float)currTime);

		/*	glm::vec3 direction = glm::normalize(to - mesh->drawPosition);
			mesh->drawPosition += direction * speed * (float)deltaTime;*/

			if (orient)
			{
				glm::vec3 direction = glm::normalize(mesh->drawPosition - curvePoints[pointIndex + 2] );
				glm::quat targetOrientation = glm::quatLookAt(direction, glm::vec3(0, 1, 0));
				glm::quat resultQuat = glm::slerp(mesh->get_qOrientation(), targetOrientation, currTime);
				mesh->setRotationFromQuat(glm::normalize(resultQuat));
			}


		//	return false;
		}
		
	}

	if (OnComplete)
	{
		OnComplete();
	}
	float distance = glm::distance(mesh->drawPosition, curvePoints[curvePoints.size() - 2]);
	if (distance > 0.1)
	{
		return false;
	}
	std::cout << glm::distance(mesh->drawPosition, curvePoints[curvePoints.size() - 1]) << std::endl;	
	std::cout << "Done" << std::endl;
	return true;
}

//from https://glm.g-truc.net/0.9.4/api/a00203.html
glm::vec3 FollowCurve::PointOnCurve(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t)
{
	return glm::catmullRom(p0,p1,p2,p3,t);
}
