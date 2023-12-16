#include "CommandGroup.h"

CommandGroup::CommandGroup(std::string name) 
{
	this->name = name;
}

CommandGroup::~CommandGroup()
{
}

void CommandGroup::AddSerialCommand(iCommand* command)
{
	serialCommands.push_back(command);
}

void CommandGroup::AddParallelCommand(iCommand* command)
{
	parallelCommands.push_back(command);
}


void CommandGroup::Execute(double deltaTime)
{
	currTime += deltaTime;
	ExecuteSerial(deltaTime);
	for (int i = 0; i < parallelCommands.size(); i++)
	{
		parallelCommands[i]->Execute(deltaTime);
	}
}

float CommandGroup::GetTotalTime()
{
	float totalTime = 0.0f;
	for (int i = 0; i < serialCommands.size(); i++)
	{
		Command* command = (Command*)serialCommands[i];
		totalTime += command->delay;
	}
	for (int i = 0; i < parallelCommands.size(); i++)
	{
		Command* command = (Command*)parallelCommands[i];
		totalTime += command->delay;
	}
	return totalTime;
}


void CommandGroup::ExecuteSerial(double deltaTime)
{
	if (serialIndex < serialCommands.size())
	{
		if (serialCommands[serialIndex]->Execute(deltaTime))
		{
			serialIndex++;
		}
	}
}

