#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <math.h>
#include <map>
#include <string>
#include <iostream>
#include <vector>
#include "../cVAOManager/sModelDrawInfo.h"
class cAABB
{
public:
	unsigned int uniqueID;
	glm::vec3 minXYZ;
	glm::vec3 maxXYZ;	// AKA the "lengths" or whatever
	glm::vec3 minBoxDimensions;
	glm::vec3 maxBoxDimensions;
	int scale = 1;
	std::string overlappingMeshName;
	glm::vec3 getCentreXYZ(void)
	{
		return (this->minXYZ + this->maxXYZ) / 2.0f;
	}
	glm::vec3 getExtentsXYZ(void)
	{
		// TODO: Calculate the extent
		return maxXYZ - minXYZ;	// Does this work?? 
	}

	glm::vec3 getHalfLengths(void)
	{
		return this->getExtentsXYZ() / 2.0f;
	}

	bool isOverlapping ;

	// I'll place the index calculation here 
	// It's based on the lowest point on the AABB
	unsigned int getLocationIndex_OG(void)
	{
		// Note that the casting to int will "round down" by removing the fractions
		unsigned int theX = (unsigned int) floor(this->minXYZ.x / getExtentsXYZ().x);
		unsigned int theY = (unsigned int) floor(this->minXYZ.y / getExtentsXYZ().y);
		unsigned int theZ = (unsigned int) floor(this->minXYZ.z / getExtentsXYZ().z);

		// Shift the numbers into the columns we want:
		// xxx,yyy,zzz
		// For 32 bits, we'll multiply by 1000

		unsigned int theIndex = (theX * 1000 * 1000) +
		                        (theY * 1000) +
		                        (theZ);

		// If you are using negative numbers, you can use the 1st 'digit" as a sign:
		// Instead of xxx,yyy,zzz, you get
		//            +xx,+yy,+zz (where "+" is the "sign"
		// Let's say 0 is positive and 1 is negative
		if (this->minXYZ.x < 0)
		{
			theIndex += 100'000'000;	// Place a "1" at that location +__,___,___
		}
		if (this->minXYZ.y < 0)
		{
			theIndex += 100'000;	// Place a "1" at that location ___,+___,___
		}
		if (this->minXYZ.z < 0)
		{
			theIndex += 100;	// Place a "1" at that location ___,___,+___
		}
		return theIndex;
	}

	// Calls the static function
	unsigned int getLocationIndex(void)
	{
		return cAABB::static_getLocationIndex(this->minXYZ, this->getExtentsXYZ());
	}

	// Maybe we want "child" AABBs (like for an oct-tree)
	std::map< unsigned int /*index*/, cAABB* > vecChild_pAABBs;


	static unsigned int static_getLocationIndex(glm::vec3 minXYZ, glm::vec3 extentXYZ)
	{
		// Note that the casting to int will "round down" by removing the fractions
		unsigned int theX = (unsigned int)floor(minXYZ.x / extentXYZ.x);
		unsigned int theY = (unsigned int)floor(minXYZ.y / extentXYZ.y);
		unsigned int theZ = (unsigned int)floor(minXYZ.z / extentXYZ.z);

		// Shift the numbers into the columns we want:
		// xxx,yyy,zzz
		// For 32 bits, we'll multiply by 1000

		unsigned int theIndex = (theX * 1000 * 1000) +
			(theY * 1000) +
			(theZ);

		// If you are using negative numbers, you can use the 1st 'digit" as a sign:
		// Instead of xxx,yyy,zzz, you get
		//            +xx,+yy,+zz (where "+" is the "sign"
		// Let's say 0 is positive and 1 is negative
		if (minXYZ.x < 0)
		{
			theIndex += 100'000'000;	// Place a "1" at that location +__,___,___
		}
		if (minXYZ.y < 0)
		{
			theIndex += 100'000;	// Place a "1" at that location ___,+___,___
		}
		if (minXYZ.z < 0)
		{
			theIndex += 100;	// Place a "1" at that location ___,___,+___
		}
		return theIndex;
	}

	static glm::vec3 static_getLocationFromIndex(unsigned int theIndex, glm::vec3 extentXYZ)
	{
		// Extract the sign information from the index
		int xSign = (theIndex / 100'000'000) % 10 == 1 ? -1 : 1;
		int ySign = (theIndex / 100'000) % 10 == 1 ? -1 : 1;
		int zSign = (theIndex / 100) % 10 == 1 ? -1 : 1;

		// Extract the indices without the sign information
		unsigned int xIndex = (theIndex % 100'000'000) / (1000 * 1000);
		unsigned int yIndex = (theIndex % (1000 * 1000)) / 1000;
		unsigned int zIndex = theIndex % 1000;

		// Calculate the actual (x, y, z) values
		float x = xSign * xIndex * extentXYZ.x;
		float y = ySign * yIndex * extentXYZ.y;
		float z = zSign * zIndex * extentXYZ.z;

		return glm::vec3(x, y, z);
	}

	std::vector< glm::vec3 > vecVerticesInside;
	std::vector< sTriangle > vecTrianglesInside;

	bool IsPointInside(glm::vec3 point)
	{
		return (point.x > minXYZ.x && point.x < maxXYZ.x &&
			point.y > minXYZ.y && point.y < maxXYZ.y &&
			point.z > minXYZ.z && point.z < maxXYZ.z);

	}

	void UpdateAABBPosition(glm::vec3 position)
	{
		this->minXYZ += position;
		this->maxXYZ += position;
	}

};


