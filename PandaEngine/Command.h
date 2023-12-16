#pragma once

#include "iCommand.h"
#include "Random.h"
#include <iostream>
#include <functional>
class Command :
	public iCommand
{
public:
	Command();
	Command(std::string name);
	virtual ~Command() {};

	virtual bool Execute(double deltaTime);
	virtual int UniqueId();
	virtual std::string Name();
	virtual void SetName(std::string name);
	virtual void SetUniqueId(int id);
	void Cancel()
	{
		this->isCancelled = true;
	}
	float delay;
	float currTime;
	std::function<void()> OnStart;
	std::function<void()> OnComplete;
protected:
	std::string name;
	bool isCancelled;
	int uniqueId;
};

