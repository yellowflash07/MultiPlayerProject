#pragma once
#include "../PandaEngine/GraphicsCommon.h"
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> 
class Camera
{
public:
	Camera(glm::vec3 position,
		glm::vec3 target, 
		glm::vec3 upVector, 
		float near = 0.1f, 
		float far = 1000.0f);
	~Camera();

	void Update(GLFWwindow* window, double deltaTime);
	void ProcessMouseMovement(double xpos, double ypos);
	void ProcessKeyboardInput(GLFWwindow* window, double deltaTime);
	GLuint shaderProgramID;
	void SetPosition(glm::vec3 position) { cameraEye = position; }
	void Follow(glm::vec3 followPos, glm::vec3 followTarget);
	glm::vec3 GetCameraRotation();

	glm::vec3 cameraEye;
	glm::vec3 cameraTarget;
	glm::vec3 upVector;
	bool isFollowing = false;
	bool camControl = true;
private:

	float near;
	float far;
	float yaw;
	float pitch;
	float speed;
	bool stopUpdates; 
	glm::mat4 matView;

	glm::vec3 followPos;
	glm::vec3 followTarget;
};

