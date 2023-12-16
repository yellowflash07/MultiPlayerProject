#include "PhysicsManager.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

PhysicsManager::PhysicsManager()
{
}

PhysicsManager::PhysicsManager(MeshManager* meshManager)
{
	this->meshManager = meshManager;
}

PhysicsManager::~PhysicsManager()
{
}

void PhysicsManager::Update(float deltaTime)
{


	CheckIntersections(deltaTime);
}

void PhysicsManager::CheckIntersections(float deltaTime)
{
	for (PhysicsBody* pObject : this->bodies)
	{
		// Infinite mass? 
		if (pObject->inverseMass > 0.0f)
		{
			// Velocity change is based on the acceleration over this time frame 
			// This part: (Accel * DeltaTime)
			glm::vec3 deltaVelocityThisFrame = pObject->acceleration * (float)deltaTime;

			// Update the velocity based on this delta velocity
			// Then this part: NewVelocity = LastVel + ...
			pObject->velocity += deltaVelocityThisFrame;

			// Position change is based on the velocity over this time frame
			// This part: (Vel * DeltaTime)	
			glm::vec3 deltaPosition = pObject->velocity * (float)deltaTime;

			pObject->mesh->drawPosition.x += deltaPosition.x;
			pObject->mesh->drawPosition.y += deltaPosition.y;
			pObject->mesh->drawPosition.z += deltaPosition.z;

			// Update the AABBs
			for (cAABB* pAABB : pObject->aabbs)
			{
				pAABB->minXYZ += deltaPosition;
				pAABB->maxXYZ += deltaPosition;

			}
		}

	}

	//broad phase
	for (PhysicsBody* pObjectA : this->bodies)
	{

		for (PhysicsBody* pObjectB : this->bodies)
		{
			// Are "A" and "B" the same object
			if (pObjectA == pObjectB)
			{
				// Yup, so skip this
				continue;
			}

			// Check if the two AABBs overlap
			if (CheckAABBOverlap(pObjectA, pObjectB))
			{
				/*std::cout << "Overlapping" << std::endl;
				std::cout << pObjectA->mesh->friendlyName << std::endl;
				std::cout << pObjectA->activeAABB->vecTrianglesInside.size() << std::endl;
				std::cout << pObjectB->mesh->friendlyName << std::endl;
				std::cout << pObjectB->activeAABB->vecTrianglesInside.size() << std::endl;*/
				// Narrow phase
				switch (pObjectA->shapeType)
				{
					case PhysicsShapes::SPHERE:
						switch (pObjectB->shapeType)
						{
							case PhysicsShapes::SPHERE:
								// Sphere - Sphere
								this->m_Sphere_Sphere_IntersectionTest(pObjectA, pObjectB);
								break;
							case PhysicsShapes::MESH_OF_TRIANGLES_INDIRECT:
								// Sphere - Mesh triangle (indirect)
								if (this->m_Sphere_TriMeshIndirect_IntersectionTest(pObjectA, pObjectB))
								{
									std::cout << "Hazzah!" << std::endl;
								}
								break;
						}
						break;
					case PhysicsShapes::AABB:
						switch (pObjectB->shapeType)
						{
							case PhysicsShapes::MESH_OF_TRIANGLES_INDIRECT:
								// Mesh triangle (indirect) - Mesh triangle (indirect)
								if (this->m_AABB_TriMeshIndirect_IntersectionTest(pObjectA, pObjectB))
								{
									//std::cout << "AABB-Tri T_T!!" << std::endl;
								}
								break;
						}
						break;
				}
			}
		}
	}

	// See which object is colliding with which object...
	//for (PhysicsBody* pObjectA : this->bodies)
	//{

	//	for (PhysicsBody* pObjectB : this->bodies)
	//	{
	//		// Are "A" and "B" the same object
	//		if (pObjectA == pObjectB)
	//		{
	//			// Yup, so skip this
	//			continue;
	//		}
	//		// What's the collision? 
	//		switch (pObjectA->shapeType)
	//		{
	//			case PhysicsShapes::SPHERE:
	//				switch (pObjectB->shapeType)
	//				{
	//					case PhysicsShapes::SPHERE:
	//						// Sphere - Sphere
	//						this->m_Sphere_Sphere_IntersectionTest(pObjectA, pObjectB);
	//						break;
	//					case PhysicsShapes::MESH_OF_TRIANGLES_INDIRECT:
	//						// Sphere - Mesh triangle (indirect)
	//						if (this->m_Sphere_TriMeshIndirect_IntersectionTest(pObjectA, pObjectB))
	//						{
	//							std::cout << "Hazzah!" << std::endl;
	//						}
	//						break;
	//					case PhysicsShapes::MESH_OF_TRIANGLES_LOCAL_VERTICES:
	//						// Shpere - Mesh (local vertices)
	//						break;
	//				}//switch (pObjectB->shapeType)
	//				break;
	//		}

	//	}
	//}
	return;
}

void PhysicsManager::AddMesh(PhysicsBody* physicsBody)
{
	bodies.push_back(physicsBody);
}

void PhysicsManager::GenerateAABBs(PhysicsBody* body, int numberOfAABBs, int scaleExtents)
{
	std::cout << "Generating AAABs for " << body->mesh->friendlyName << std::endl;
	std::vector<cAABB*> aabbs;
	
	sModelDrawInfo drawInfo;
	meshManager->GetModelDrawInfo(body->mesh->friendlyName, drawInfo);

	body->mesh->calcExtents();

	//populate the aabb map with it's id
	glm::vec3 extent = (body->mesh->maxExtents_XYZ - body->mesh->minExtents_XYZ);
	glm::vec3 center = (body->mesh->maxExtents_XYZ + body->mesh->minExtents_XYZ) * 0.5f;

	glm::vec3 boxDimensions = extent / glm::vec3(numberOfAABBs);
	for (int x =0; x != numberOfAABBs; x++)
	{
		for (int y = 0; y != numberOfAABBs; y++)
		{
			for (int z = 0; z != numberOfAABBs; z++)
			{
				cAABB* pAABB = new cAABB();

				pAABB->minBoxDimensions = glm::vec3(x * boxDimensions.x, y * boxDimensions.y, z * boxDimensions.z);
				pAABB->maxBoxDimensions = glm::vec3(boxDimensions.x, boxDimensions.y, boxDimensions.z);

				pAABB->minXYZ.x = body->mesh->minExtents_XYZ.x + x * boxDimensions.x;
				pAABB->minXYZ.y = body->mesh->minExtents_XYZ.y + y * boxDimensions.y;
				pAABB->minXYZ.z = body->mesh->minExtents_XYZ.z + z * boxDimensions.z;

				pAABB->maxXYZ.x = pAABB->minXYZ.x + boxDimensions.x;
				pAABB->maxXYZ.y = pAABB->minXYZ.y + boxDimensions.y;
				pAABB->maxXYZ.z = pAABB->minXYZ.z + boxDimensions.z;


				unsigned int AABB_ID = cAABB::static_getLocationIndex(pAABB->minXYZ, extent);

				body->aabbsMap[AABB_ID] = pAABB;

				// Count the number of vertices within this AABB
			/*	for(int i = 0; i < drawInfo.numberOfVertices; i++)
				{
					glm::vec3 vertex = glm::vec3(drawInfo.pVertices[i].x, drawInfo.pVertices[i].y, drawInfo.pVertices[i].z);
					if (vertex.x >= pAABB->minXYZ.x && vertex.x <= pAABB->maxXYZ.x &&
						vertex.y >= pAABB->minXYZ.y && vertex.y <= pAABB->maxXYZ.y &&
						vertex.z >= pAABB->minXYZ.z && vertex.z <= pAABB->maxXYZ.z) 
					{
						pAABB->vecVerticesInside.push_back(vertex);
					}
				}*/
				//store the triangles that are inside the aabb
				for (int i = 0; i < drawInfo.numberOfTriangles; i ++)
				{
					glm::vec3 v1 = drawInfo.pTriangles[i].v1;
					glm::vec3 v2 = drawInfo.pTriangles[i].v2;
					glm::vec3 v3 = drawInfo.pTriangles[i].v3;					

					//if any of the vertices are inside the aabb, add the triangle to the aabb
					if (pAABB->IsPointInside(v1) || pAABB->IsPointInside(v2) || pAABB->IsPointInside(v3))
					{
						pAABB->vecTrianglesInside.push_back(drawInfo.pTriangles[i]);
					}
				}

				if (pAABB->vecTrianglesInside.empty())
				{
					continue;
				}

				pAABB->minXYZ *= scaleExtents;
				pAABB->maxXYZ *= scaleExtents;
				pAABB->scale = scaleExtents;
				body->aabbs.push_back(pAABB);
			}
		}
	}
	std::cout << "Generated " << body->aabbs.size() << " AABBs" << std::endl;
}

bool PhysicsManager::CheckAABBOverlap(PhysicsBody* pBodyA, PhysicsBody* pBodyB)
{
	// Temporary vector to store currently overlapping AABB pairs
	std::vector<std::pair<cAABB*, cAABB*>> overlappingPairsTemp;
	bool foundPairs = false;
	//std::pair<cAABB*, cAABB*> aabbPair;
	//int count = 0;
	for (cAABB* pAABB1 : pBodyA->aabbs)
	{
		for (cAABB* pAABB2 : pBodyB->aabbs)
		{	

			if (pAABB1->vecTrianglesInside.empty() || pAABB2->vecTrianglesInside.empty())
			{
				continue;
			}

			// Check for overlap along each axis
			if (pAABB1->maxXYZ.x < pAABB2->minXYZ.x || pAABB1->minXYZ.x > pAABB2->maxXYZ.x)
			{
				// No overlap on the x-axis
				continue;
			}
			if (pAABB1->maxXYZ.y < pAABB2->minXYZ.y || pAABB1->minXYZ.y > pAABB2->maxXYZ.y) 
			{
				continue;
			}
			if (pAABB1->maxXYZ.z < pAABB2->minXYZ.z || pAABB1->minXYZ.z > pAABB2->maxXYZ.z) 
			{
				continue;
			}

		//	aabbPair.first = pAABB1;
			//aabbPair.second = pAABB2;

			overlappingPairsTemp.push_back(std::make_pair(pAABB1, pAABB2));			
			//pBodyB->aabbPairs.push_back(aabbPair);

			pBodyA->activeAABB = pAABB1;
			pBodyB->activeAABB = pAABB2;

			pBodyA->activeAABB->overlappingMeshName = pBodyB->mesh->friendlyName;
			pBodyB->activeAABB->overlappingMeshName = pBodyA->mesh->friendlyName;
			// Overlap on all axes means AABBs are intersecting
			//count++;
			pAABB1->isOverlapping = true;
			pAABB2->isOverlapping = true;
		
			foundPairs = true;
		}
	}


	pBodyA->aabbPairs = overlappingPairsTemp;
	pBodyB->aabbPairs = overlappingPairsTemp;
//	std::cout << "Overlapping" << pBodyA->aabbPairs.size() << std::endl;
	return foundPairs;
}

glm::vec3 PhysicsManager::ClosestPtPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
		glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	glm::vec3 bc = c - b;

	// Compute parametric position s for projection P' of P on AB,
	// P' = A + s*AB, s = snom/(snom+sdenom)
	float snom = glm::dot(p - a, ab), sdenom = glm::dot(p - b, a - b);

	// Compute parametric position t for projection P' of P on AC,
	// P' = A + t*AC, s = tnom/(tnom+tdenom)
	float tnom = glm::dot(p - a, ac), tdenom = glm::dot(p - c, a - c);

	if (snom <= 0.0f && tnom <= 0.0f) return a; // Vertex region early out

	// Compute parametric position u for projection P' of P on BC,
	// P' = B + u*BC, u = unom/(unom+udenom)
	float unom = glm::dot(p - b, bc), udenom = glm::dot(p - c, b - c);

	if (sdenom <= 0.0f && unom <= 0.0f) return b; // Vertex region early out
	if (tdenom <= 0.0f && udenom <= 0.0f) return c; // Vertex region early out


	// P is outside (or on) AB if the triple scalar product [N PA PB] <= 0
	glm::vec3 n = glm::cross(b - a, c - a);
	float vc = glm::dot(n, glm::cross(a - p, b - p));
	// If P outside AB and within feature region of AB,
	// return projection of P onto AB
	if (vc <= 0.0f && snom >= 0.0f && sdenom >= 0.0f)
		return a + snom / (snom + sdenom) * ab;

	// P is outside (or on) BC if the triple scalar product [N PB PC] <= 0
	float va = glm::dot(n, glm::cross(b - p, c - p));
	// If P outside BC and within feature region of BC,
	// return projection of P onto BC
	if (va <= 0.0f && unom >= 0.0f && udenom >= 0.0f)
		return b + unom / (unom + udenom) * bc;

	// P is outside (or on) CA if the triple scalar product [N PC PA] <= 0
	float vb = glm::dot(n, glm::cross(c - p, a - p));
	// If P outside CA and within feature region of CA,
	// return projection of P onto CA
	if (vb <= 0.0f && tnom >= 0.0f && tdenom >= 0.0f)
		return a + tnom / (tnom + tdenom) * ac;

	// P must project inside face region. Compute Q using barycentric coordinates
	float u = va / (va + vb + vc);
	float v = vb / (va + vb + vc);
	float w = 1.0f - u - v; // = vc / (va + vb + vc)
	return u * a + v * b + w * c;
}

bool PhysicsManager::m_Sphere_Sphere_IntersectionTest(PhysicsBody* pSphereA, PhysicsBody* pShpereB)
{
	// Do we have a mesh manager? 
	if (!this->meshManager)
	{
		return false;
	}

	// Does the physics object have a mesh object associated? 
	if (!pSphereA->mesh || !pShpereB->mesh)
	{
		return false;
	}

	PhysicsShapes::sSphere* sphereA = (PhysicsShapes::sSphere*)(pSphereA->shape);
	PhysicsShapes::sSphere* sphereB = (PhysicsShapes::sSphere*)(pShpereB->shape);


	float distance = glm::distance(pSphereA->mesh->drawPosition, pShpereB->mesh->drawPosition);
	float radiusSum = sphereA->radius + sphereB->radius;

	if (distance < radiusSum)
	{

		// impact speed
		glm::vec3 v = (pSphereA->velocity - pShpereB->velocity);
		glm::vec3 normal = (pSphereA->mesh->drawPosition - pShpereB->mesh->drawPosition);
		
		normal = glm::normalize(normal);

		float vn = glm::dot(v, normal);

		//// sphere intersecting but moving away from each other already
		if (vn > 0.0f)
			return false;

		// collision impulse
		float i = (-(1.0f) * vn) / (pSphereA->inverseMass + pShpereB->inverseMass);
		glm::vec3 impulse = normal * i;

		// change in momentum
		pSphereA->velocity += impulse * pSphereA->inverseMass;
		pShpereB->velocity -= impulse * pShpereB->inverseMass;
		pSphereA->mesh->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		pShpereB->mesh->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

		return true;
	}

}

bool PhysicsManager::m_Sphere_TriMeshIndirect_IntersectionTest(PhysicsBody* sphere, PhysicsBody* triMesh)
{
	// Do we have a mesh manager? 
	if (!this->meshManager)
	{
		return false;
	}

	// Does the physics object have a mesh object associated? 
	if (!triMesh->mesh)
	{
		return false;
	}

	// Get the info about this shape, specifically
	PhysicsShapes::sMeshOfTriangles_Indirect* pTriangleMesh =
		(PhysicsShapes::sMeshOfTriangles_Indirect*)(triMesh->shape);

	PhysicsShapes::sSphere* pSphere = (PhysicsShapes::sSphere*)(sphere->shape);

	sModelDrawInfo theMeshDrawInfo;

	// Find the raw mesh information from the VAO manager
	if (!this->meshManager->GetModelDrawInfo(pTriangleMesh->meshName, theMeshDrawInfo))
	{
		// Didn't find it
		return false;
	}

	int numberOfTriangles = triMesh->activeAABB->vecTrianglesInside.size();

	//	glm::vec3 closestPointToTriangle = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	float closestDistanceSoFar = FLT_MAX;
	glm::vec3 closestTriangleVertices[3] = { glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) };
	unsigned int indexOfClosestTriangle = INT_MAX;


	// We now have the mesh object location and the detailed mesh information 
	// Which triangle is closest to this sphere (right now)
	for (unsigned int index = 0; index != numberOfTriangles; index ++)
	{
		glm::vec3 verts[3];

		glm::vec3 v1 = triMesh->activeAABB->vecTrianglesInside[index].v1;
		glm::vec3 v2 = triMesh->activeAABB->vecTrianglesInside[index].v2;
		glm::vec3 v3 = triMesh->activeAABB->vecTrianglesInside[index].v3;

		//glm::mat4 matModel = glm::mat4(1.0f);

		//// Translation
		//glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
		//						glm::vec3(	triMesh->mesh->drawPosition.x,
		//									triMesh->mesh->drawPosition.y,
		//									triMesh->mesh->drawPosition.z));

		//// Rotation matrix generation
		//glm::mat4 matRotation = glm::mat4(triMesh->mesh->get_qOrientation());

		//glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
		//	glm::vec3(triMesh->mesh->drawScale.x,
		//			triMesh->mesh->drawScale.y,
		//			triMesh->mesh->drawScale.z));

		//		// Combine all these transformation
		//matModel = matModel * matTranslate;

		//matModel = matModel * matRotation;

		//matModel = matModel * matScale;

		//glm::vec4 vertsWorld[3];
		//vertsWorld[0] = (matModel * glm::vec4(verts[0], 1.0f));
		//vertsWorld[1] = (matModel * glm::vec4(verts[1], 1.0f));
		//vertsWorld[2] = (matModel * glm::vec4(verts[2], 1.0f));

		// And make sure you multiply the normal by the inverse transpose
		// OR recalculate it right here! 

		// ******************************************************

		glm::vec3 thisTriangleClosestPoint = this->ClosestPtPointTriangle(sphere->mesh->drawPosition,v1, v2, v3);

		// Is this the closest so far
		float distanceToThisTriangle = glm::distance(thisTriangleClosestPoint, sphere->mesh->drawPosition);

		if (distanceToThisTriangle < closestDistanceSoFar)
		{
			// this one is closer
			closestDistanceSoFar = distanceToThisTriangle;
			// Make note of the triangle index
			indexOfClosestTriangle = index;
			// 
			closestTriangleVertices[0] = v1;
			closestTriangleVertices[1] = v2;
			closestTriangleVertices[2] = v3;
		}


	} 

	if (closestDistanceSoFar < pSphere->radius)
	{
		//glm::vec3 sphereDirection = sphere->velocity;
		//// Normalize... 
		//sphereDirection = glm::normalize(sphereDirection);

		//// Calcualte the current normal from the TRANSFORMED vertices
		//glm::vec3 edgeA = closestTriangleVertices[1] - closestTriangleVertices[0];
		//glm::vec3 edgeB = closestTriangleVertices[2] - closestTriangleVertices[0];

		//glm::vec3 triNormal = glm::normalize(glm::cross(edgeA, edgeB));
		//glm::vec3 reflectionVec = glm::reflect(sphereDirection, triNormal);

		//// Update the  velocity based on this reflection vector
		//float sphereSpeed = glm::length(sphere->velocity);
		//glm::vec3 newVelocity = reflectionVec * sphereSpeed;

		sphere->velocity = glm::vec3(0,0,0);

		return true;
	}

	// Didn't hit
	return false;
}

bool PhysicsManager::m_AABB_TriMeshIndirect_IntersectionTest(PhysicsBody* pAABB, PhysicsBody* pTriMesh)
{

	for (int i = 0; i < pAABB->aabbPairs.size(); i++)
	{
		cAABB* aabb = pAABB->aabbPairs[i].first;
		cAABB* triAABB = pAABB->aabbPairs[i].second;

		for (sTriangle tri: triAABB->vecTrianglesInside)
		{
			glm::vec3 v1 = tri.v1;
			glm::vec3 v2 = tri.v2;
			glm::vec3 v3 = tri.v3;


			int result = TestTriangleAABB(v1, v2, v3, aabb);

			if (result != 0)
			{
				//pAABB->velocity = glm::vec3(0.0f);
				CollisionEvent* collisionEvent = new CollisionEvent();	
				collisionEvent->collisionPoint = v1;
				collisionEvent->objectName = pTriMesh->mesh->friendlyName;

				pAABB->collisionEvents.push_back(collisionEvent);
				//glm::vec3 direction = pAABB->velocity;
				//// Normalize... 
				//direction = glm::normalize(direction);

				//// Calcualte the current normal from the vertices
				//glm::vec3 edgeA = v1 - v2;
				//glm::vec3 edgeB = v3 - v1;

				//glm::vec3 triNormal = glm::normalize(glm::cross(edgeA, edgeB));
				//glm::vec3 reflectionVec = glm::reflect(direction, triNormal);


				// Set the part of velocity perpendicular to the plane to zero

				// Update the  velocity based on this reflection vector
				//float sphereSpeed = glm::length(sphere->velocity);
				//glm::vec3 newVelocity = reflectionVec * sphereSpeed;
				return true;
			}
		}
	}
	for (int i = 0; i < pAABB->aabbPairs.size(); i++)
	{
		pAABB->collisionEvents.clear();
	}
	return false;
}

void PhysicsBody::setShape(PhysicsShapes::sSphere* pSphereProps)
{
	this->shape = (void*)pSphereProps;
}

void PhysicsBody::setShape(PhysicsShapes::sPlane* pPlaneProps)
{
	this->shape = (void*)pPlaneProps;
}

void PhysicsBody::setShape(PhysicsShapes::sTriangle* pTriangleProps)
{
	this->shape = (void*)pTriangleProps;
}

void PhysicsBody::setShape(PhysicsShapes::sCapsule * pCapsuleProps)
{
	this->shape = (void*)pCapsuleProps;
}

void PhysicsBody::setShape(PhysicsShapes::sMeshOfTriangles_Indirect * pTriangleMeshProps)
{
	this->shape = (void*)pTriangleMeshProps;
}

void PhysicsBody::setShape(PhysicsShapes::sMeshOfTriangles_LocalVertices * pTriangleMeshProps)
{
	this->shape = (void*)pTriangleMeshProps;
}

void PhysicsBody::setShape(PhysicsShapes::sAABB* aabb)
{
	this->shape = (void*)aabb;
}

int PhysicsManager::TestTriangleAABB(glm::vec3 v0, glm::vec3  v1, glm::vec3  v2, cAABB* b)
{
	float p0, p1, p2, r;

	// Compute box center and extents (if not already given in that format)
	glm::vec3 c = (b->minXYZ + b->maxXYZ) * 0.5f;
	float e0 = (b->maxXYZ.x - b->minXYZ.x) * 0.5f;
	float e1 = (b->maxXYZ.y - b->minXYZ.y) * 0.5f;
	float e2 = (b->maxXYZ.z - b->minXYZ.z) * 0.5f;

	// Translate triangle as conceptually moving AABB to origin
	v0 = v0 - c;
	v1 = v1 - c;
	v2 = v2 - c;

	// Compute edge vectors for triangle
	glm::vec3 f0 = v1 - v0, f1 = v2 - v1, f2 = v0 - v2;

	// Test axes a00..a22 (category 3)
	// Test axis a00
	p0 = v0.z * v1.y - v0.y * v1.z;
	p2 = v2.z * (v1.y - v0.y) - v2.z * (v1.z - v0.z);
	r = e1 * std::abs(f0.z) + e2 * std::abs(f0.y);
	if (std::max(-std::max(p0, p2), std::min(p0, p2)) > r) return 0; // Axis is a separating axis

	// Test axis a02
	p0 = v0.x * f0.y - v0.y * f0.x;
	p1 = v1.x * f0.y - v1.y * f0.x;
	r = e0 * std::abs(f0.y) + e1 * std::abs(f0.x);
	if (std::max(-std::max(p0, p1), std::min(p0, p1)) > r) return 0;

	// Test axis a10
	p0 = v0.y * f1.z - v0.z * f1.y;
	p1 = v1.y * f1.z - v1.z * f1.y;
	r = e0 * std::abs(f1.z) + e2 * std::abs(f1.y);
	if (std::max(-std::max(p0, p1), std::min(p0, p1)) > r) return 0;

	// Test axis a12
	p0 = v0.x * f1.z - v0.z * f1.x;
	p1 = v1.x * f1.z - v1.z * f1.x;
	r = e0 * std::abs(f1.z) + e2 * std::abs(f1.x);
	if (std::max(-std::max(p0, p1), std::min(p0, p1)) > r) return 0;

	// Test axis a20
	p0 = v0.y * f2.z - v0.z * f2.y;
	p1 = v1.y * f2.z - v1.z * f2.y;
	r = e1 * std::abs(f2.z) + e2 * std::abs(f2.y);
	if (std::max(-std::max(p0, p1), std::min(p0, p1)) > r) return 0;

	// Test axis a21
	p0 = v0.x * f2.z - v0.z * f2.x;
	p1 = v1.x * f2.z - v1.z * f2.x;
	r = e0 * std::abs(f2.z) + e2 * std::abs(f2.x);
	if (std::max(-std::max(p0, p1), std::min(p0, p1)) > r) return 0;

	// Test axis a22
	p0 = v0.x * f0.y - v0.y * f0.x;
	p1 = v1.x * f0.y - v1.y * f0.x;
	r = e0 * std::abs(f0.y) + e1 * std::abs(f0.x);
	if (std::max(-std::max(p0, p1), std::min(p0, p1)) > r) return 0;

// Test the three axes corresponding to the face normals of AABB b (category 1).
// Exit if...
// ... [-e0, e0] and [min(v0.x,v1.x,v2.x), max(v0.x,v1.x,v2.x)] do not overlap
	if (std::max({ v0.x, v1.x, v2.x }) < -e0 || std::min({ v0.x, v1.x, v2.x }) > e0) return 0;
	// ... [-e1, e1] and [min(v0.y,v1.y,v2.y), max(v0.y,v1.y,v2.y)] do not overlap
	if (std::max({ v0.y, v1.y, v2.y }) < -e1 || std::min({ v0.y, v1.y, v2.y }) > e1) return 0;
	// ... [-e2, e2] and [min(v0.z,v1.z,v2.z), max(v0.z,v1.z,v2.z)] do not overlap
	if (std::max({ v0.z, v1.z, v2.z }) < -e2 || std::min({ v0.z, v1.z, v2.z }) > e2) return 0;

	// Test separating axis corresponding to triangle face normal (category 2)
	Plane p;
	p.n = glm::cross(f0, f1);
	p.d = glm::dot(p.n, v0);
	return TestAABBPlane(b, p);
}

int PhysicsManager::TestAABBPlane(cAABB* b, Plane p)
{
	// These two lines not necessary with a (center, extents) AABB representation
	glm::vec3 c = (b->maxXYZ + b->minXYZ) * 0.5f; // Compute AABB center
	glm::vec3 e = b->maxXYZ - c; // Compute positive extents

	// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
	float r = e[0] * std::abs(p.n[0]) + e[1] * std::abs(p.n[1]) + e[2] * std::abs(p.n[2]);
	// Compute distance of box center from plane
	float s = glm::dot(p.n, c) - p.d;
	// Intersection occurs when distance s falls within [-r,+r] interval
	return std::abs(s) <= r;
}