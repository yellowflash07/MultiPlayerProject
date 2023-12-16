#pragma once
#include "Command.h"
#include "MoveTo.h"
#include "OrientTo.h"
#include "FollowObject.h"
#include "FollowCurve.h"

class CommandGroup
{
	public:
	CommandGroup(std::string name);
	~CommandGroup();

	void AddSerialCommand(iCommand* command);
	void AddParallelCommand(iCommand* command);
	std::string name;

	void Execute(double deltaTime);
	float GetTotalTime();
private:
	std::vector<iCommand*> serialCommands;
	std::vector<iCommand*> parallelCommands;
	std::vector<iCommand*> parallelRotations;
	void ExecuteSerial(double deltaTime);
	int serialIndex = 0;
	int parallelRotationIndex = 0;
	float currTime = 0.0f;
};

