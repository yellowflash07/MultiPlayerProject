#include "CommandFactory.h"

CommandFactory::CommandFactory()
{
}

CommandFactory::~CommandFactory()
{
}

iCommand* CommandFactory::CreateCommand(std::string name, std::string friendlyName)
{
    iCommand* command = nullptr;
    if (name == "MoveTo")
    {
		command = new MoveTo();
	}

    if (name == "OrientTo")
    {
		command = new OrientTo();
	}

    if (name == "FollowObject")
    {
		command = new FollowObject();
	}

    if (name == "FollowCurve")
    {
        command = new FollowCurve();
    }

    if (name == "LocationTrigger")
    {
		command = new LocationTrigger();
	}

    if (name == "LightControl")
    {
		command = new LightControl();
	}

    if (name == "ScaleUp")
    {
		command = new ScaleUp();
	}

    command->SetName(friendlyName);
    command->SetUniqueId(GenerateUniqueId());

    return command;
}

int CommandFactory::GenerateUniqueId()
{
    Random random;
    return random.GetRandomInt(100000, 999999);
}
