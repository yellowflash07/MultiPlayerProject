#include "Random.h"
#include <stdlib.h>
#include <time.h>

Random::Random()
{
    srand((time(nullptr)));
}

Random::~Random()
{
}

float Random::GetRandomFloat(float min, float max)
{
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = max - min;
    float r = random * diff;
    return min + r;
}

int Random::GetRandomInt(int min, int max)
{
	return rand() % max + min;
}
