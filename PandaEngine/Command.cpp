#include "Command.h"

Command::Command()
{
}

Command::Command(std::string name)
{
	this->name = name;
}


bool Command::Execute(double deltaTime)
{
	return false;
}

int Command::UniqueId()
{
	return uniqueId;
}

std::string Command::Name()
{
	return name;
}

void Command::SetName(std::string name)
{
	this->name = name;
}

void Command::SetUniqueId(int id)
{
		this->uniqueId = id;
}
