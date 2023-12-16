#pragma once
#include "Command.h"
#include "MoveTo.h"
#include "OrientTo.h"
#include "FollowObject.h"
#include "FollowCurve.h"
#include "LocationTrigger.h"
#include "Random.h"
#include "LightControl.h"
#include "ScaleUp.h"

class CommandFactory
{
public:
	CommandFactory();
	~CommandFactory();

	iCommand* CreateCommand(std::string name, std::string friendlyName);
private:
	int GenerateUniqueId();
};

