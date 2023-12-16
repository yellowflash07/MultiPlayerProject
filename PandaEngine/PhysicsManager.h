#pragma once
#include <vector>
#include "MeshManager.h"
#include "PhysicsShapes.h"
#include "cAABB.h"
#include <map>

struct Plane
{
	glm::vec3 n;  // Normal vector
	float d;      // Distance from the origin along the normal vector
};

struct CollisionEvent
{
	glm::vec3 collisionPoint;
	std::string objectName;
};

struct PhysicsBody
{
	cMesh* mesh;
	glm::vec3 acceleration;
	glm::vec3 velocity;
	glm::vec3 angularVelocity;
	glm::vec3 angularAcceleration;
	float inverseMass;

	void setShape(PhysicsShapes::sSphere* pSphereProps);
	void setShape(PhysicsShapes::sPlane* pPlaneProps);
	void setShape(PhysicsShapes::sTriangle* pTriangleProps);
	void setShape(PhysicsShapes::sCapsule* pCapsuleProps);
	void setShape(PhysicsShapes::sMeshOfTriangles_Indirect* pTriangleMeshProps);
	void setShape(PhysicsShapes::sMeshOfTriangles_LocalVertices* pTriangleMeshProps);
	void setShape(PhysicsShapes::sAABB* aabb);

	std::map<unsigned int, cAABB*> aabbsMap;
	std::vector<cAABB*> aabbs;
	std::vector<cAABB*> activeAABBs;
	std::vector<std::pair<cAABB*, cAABB*>> aabbPairs;
//	std::pair<cAABB*, cAABB*> aabbPair;
	cAABB* activeAABB;
	PhysicsShapes::eShape shapeType;
	void* shape = NULL;

	void UpdateAABBs()
	{
		for (size_t i = 0; i < aabbs.size(); i++)
		{
			cAABB* pAABB = aabbs[i];

			glm::vec3 minBoxDimensions = pAABB->minBoxDimensions;
			glm::vec3 maxBoxDimensions = pAABB->maxBoxDimensions;
			pAABB->minXYZ.x = mesh->minExtents_XYZ.x + minBoxDimensions.x;
			pAABB->minXYZ.y = mesh->minExtents_XYZ.y + minBoxDimensions.y;
			pAABB->minXYZ.z = mesh->minExtents_XYZ.z + minBoxDimensions.z;

			pAABB->maxXYZ.x = pAABB->minXYZ.x + maxBoxDimensions.x;
			pAABB->maxXYZ.y = pAABB->minXYZ.y + maxBoxDimensions.y;
			pAABB->maxXYZ.z = pAABB->minXYZ.z + maxBoxDimensions.z;

			pAABB->minXYZ *= pAABB->scale;
			pAABB->maxXYZ *= pAABB->scale;

			/*for (cAABB* aabb : aabbs)
			{
				aabb->minXYZ += globe1->drawPosition;
				aabb->maxXYZ += globe1->drawPosition;
			}*/
			aabbs[i]->UpdateAABBPosition(mesh->drawPosition);
		}
	}

	std::vector<CollisionEvent*> collisionEvents;

};

class PhysicsManager
{
public:
	PhysicsManager();
	PhysicsManager(MeshManager* meshManager);
	~PhysicsManager();

	void Update(float deltaTime);
	void CheckIntersections(float deltaTime);
	void AddMesh(PhysicsBody* physicsBody);
	void GenerateAABBs(PhysicsBody* body,int numberOfAABBs, int scaleExtents = 1);
private:
	MeshManager* meshManager;
	std::vector<PhysicsBody*> bodies;
	glm::vec3 ClosestPtPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c);
	bool CheckAABBOverlap(PhysicsBody* pBodyA, PhysicsBody* pBodyB);
	

	//intersection tests
	bool m_Sphere_Sphere_IntersectionTest(PhysicsBody* pSphereA, PhysicsBody* pShpereB);
	bool m_Sphere_Plane_IntersectionTest(PhysicsBody* pSphere, PhysicsBody* pPlane);
	bool m_Sphere_Triangle_IntersectionTest(PhysicsBody* pSphere, PhysicsBody* pTriangle);
	bool m_Sphere_AABB_IntersectionTest(PhysicsBody* pSphere, PhysicsBody* pAABB);
	bool m_Sphere_Capsule_IntersectionTest(PhysicsBody* pSphere, PhysicsBody* pCapsule);
	bool m_Sphere_TriMeshIndirect_IntersectionTest(PhysicsBody* pSphere, PhysicsBody* pTriMesh);
	bool m_Sphere_TriMeshLocal_IntersectionTest(PhysicsBody* pSphere, PhysicsBody* pTriMesh);
	bool m_AABB_TriMeshIndirect_IntersectionTest(PhysicsBody* pAABB, PhysicsBody* pTriMesh);

	bool PointInAABB(glm::vec3 point, cAABB* aabb);
	int TestTriangleAABB(glm::vec3 v0, glm::vec3  v1, glm::vec3  v2, cAABB* b);
	int TestAABBPlane(cAABB* b, Plane p);

};

