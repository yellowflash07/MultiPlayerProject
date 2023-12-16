#pragma once

class Random
{
public:
	Random();
	~Random();
	float GetRandomFloat(float min, float max);
	int GetRandomInt(int min, int max);
};