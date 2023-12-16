#ifndef _cMesh_HG_
#define _cMesh_HG_

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>
#include "../cVAOManager/sModelDrawInfo.h"

class cMesh
{
public:
	cMesh();	
	~cMesh();		

	std::string meshName;
	std::string friendlyName;		
	glm::vec3 drawPosition;
	glm::vec3 eulerRotation;

	void setRotationFromEuler(glm::vec3 newEulerAngleXYZ)
	{
		eulerRotation = newEulerAngleXYZ;
		this->m_qOrientation = glm::quat(eulerRotation);
	}

	void setRotationFromQuat(glm::quat newQuat)
	{
		eulerRotation = glm::eulerAngles(newQuat);
		this->m_qOrientation = newQuat;
	}

	void Rotate(glm::vec3 EulerAngleXYZ_Adjust)
	{
		glm::quat qChange = glm::quat(EulerAngleXYZ_Adjust);
		this->m_qOrientation *= qChange;
	}

	glm::quat get_qOrientation(void)
	{
		return this->m_qOrientation;
	}

	glm::vec3 drawScale;
	void setUniformDrawScale(float scale);
	bool bIsVisible;

	bool bUseDebugColours;
	glm::vec4 wholeObjectDebugColourRGBA;
	glm::vec4 color;
	bool bIsWireframe;
	bool bDoNotLight;
	bool isSkyBox;
	bool hasVertexColors;
	bool isReflective;
	static const int NUM_OF_TEXTURES = 4;
	std::string texture[NUM_OF_TEXTURES];
	float textureRatio[NUM_OF_TEXTURES];
	std::string maskTexture;

	glm::vec2 UV_Offset;

	float transperancy = 1.0f;
	std::vector<cMesh*> vec_pChildMeshes;
	unsigned int getUniqueID(void);

	glm::vec3 maxExtents_XYZ;	// bounding box maximums
	glm::vec3 minExtents_XYZ;	// bounding box minimums

	void calcExtents(void);

	glm::vec3 GetCenter(void)
	{
		return (this->maxExtents_XYZ + this->minExtents_XYZ) / 2.0f;
	}

	sModelDrawInfo modelDrawInfo;

	float explosionOffset;

	bool isChild = false;
	void AddChild(cMesh* child);
	bool hideParent = false;
private:
	unsigned int m_UniqueID;
	static const unsigned int FIRST_UNIQUE_ID = 1000;
	static unsigned int m_nextUniqueID;
	glm::quat m_qOrientation;
};

#endif

