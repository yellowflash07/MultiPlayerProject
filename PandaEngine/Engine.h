#pragma once
#include "../PandaEngine/GraphicsCommon.h"
#include "../Basic Shader Manager/cShaderManager.h"
#include "MeshManager.h"
#include "cLightManager.h"
#include "cLightHelper.h"
#include "PhysicsManager.h"
#include "Camera.h"
#include "AudioManager.h"

class Engine
{
public:
	Engine();
	~Engine();

	bool Initialize();
	void Update();
	void SetShaderPath(std::string filePath);
	void SetModelPath(std::string filePath);
	void SetAudioPath(std::string filePath);
	cMesh* LoadMesh(std::string filePath, std::string friendlyName, bool dontDraw = false);
	PhysicsBody* AddPhysicsBody(std::string friendlyMeshName);
	bool LoadDefaultShaders();
	void LoadDefaultLights();
	void LoadSave();
	double deltaTime;
	bool IsRunning;
	void ShutDown();
	void SetCameraDefaults(glm::vec3 position, glm::vec3 target, glm::vec3 upVector, float near=0.1f, float far=1000.0f);
	void SetCameraTarget(glm::vec3 target);
	GLFWwindow* window;
	cLightManager* lightManager;
	MeshManager* meshManager;
	cAudioManager* audioManager;
	PhysicsManager* physicsManager;

private:
	cShaderManager* shaderManager;
	GLuint shaderProgramID;
	glm::vec3 cameraEye;
	glm::vec3 cameraTarget;
	glm::vec3 upVector;
	float near;
	float far;
	double lastTime;
	int frameCount;
	//Camera* camera;
};