#include "cVAOManager.h"

#include "../PandaEngine/GraphicsCommon.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>

#include <sstream>
#include <fstream>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <iostream>

void cVAOManager::setBasePath(std::string basePathWithoutSlash)
{
    this->m_basePathWithoutSlash = basePathWithoutSlash;
    return;
}

bool cVAOManager::LoadModelIntoVAOAI(
    std::string fileName,
    sModelDrawInfo& drawInfo,
    unsigned int shaderProgramID,
    bool bIsDynamicBuffer /*=false*/)

{
    // Load the model from file
        // Create an instance of the Importer class
 
    if (!this->m_LoadTheFile(fileName, drawInfo))
    {
        return false;
    };

    // Create a VAO (Vertex Array Object), which will 
    //	keep track of all the 'state' needed to draw 
    //	from this buffer...

    // Ask OpenGL for a new buffer ID...
    glGenVertexArrays(1, &(drawInfo.VAO_ID));
    // "Bind" this buffer:
    // - aka "make this the 'current' VAO buffer
    glBindVertexArray(drawInfo.VAO_ID);

    // Now ANY state that is related to vertex or index buffer
    //	and vertex attribute layout, is stored in the 'state' 
    //	of the VAO... 

    glGenBuffers(1, &(drawInfo.VertexBufferID));

    glBindBuffer(GL_ARRAY_BUFFER, drawInfo.VertexBufferID);

    glBufferData(GL_ARRAY_BUFFER,
        sizeof(sVertex) * drawInfo.numberOfVertices,
        (GLvoid*)drawInfo.pVertices,
        (bIsDynamicBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));


    // Copy the index buffer into the video card, too
    // Create an index buffer.
    glGenBuffers(1, &(drawInfo.IndexBufferID));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.IndexBufferID);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,			// Type: Index element array
        sizeof(unsigned int) * drawInfo.numberOfIndices,
        (GLvoid*)drawInfo.pIndices,
        GL_STATIC_DRAW);

    // Set the vertex attributes.

    GLint vpos_location = glGetAttribLocation(shaderProgramID, "vPos");	// program
    GLint vcol_location = glGetAttribLocation(shaderProgramID, "vCol");	// program;
    GLint vNormal_location = glGetAttribLocation(shaderProgramID, "vNormal");	// program;
    GLint vTexureCoord_location = glGetAttribLocation(shaderProgramID, "vTexCoord");	// program;

    // Set the vertex attributes for this shader
    glEnableVertexAttribArray(vpos_location);	    // vPos
    glVertexAttribPointer(vpos_location, 4,		// vPos
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, x));

    glEnableVertexAttribArray(vcol_location);	    // vCol
    glVertexAttribPointer(vcol_location, 4,		// vCol
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, r));

    glEnableVertexAttribArray(vNormal_location);	// vNormal
    glVertexAttribPointer(vNormal_location, 4,		// vNormal
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, nx));

    glEnableVertexAttribArray(vTexureCoord_location);	// vTexCoord
    glVertexAttribPointer(vTexureCoord_location, 2,		// vTexCoord
        		GL_FLOAT, GL_FALSE,
        		sizeof(sVertex),
        		(void*)offsetof(sVertex, u));

    // Now that all the parts are set up, set the VAO to zero
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(vpos_location);
    glDisableVertexAttribArray(vcol_location);
    glDisableVertexAttribArray(vNormal_location);

    // Store the draw information into the map
    this->m_map_ModelName_to_VAOID[drawInfo.meshName] = drawInfo;
    return true;
}


bool cVAOManager::FindDrawInfoByModelName(
		std::string filename,
		sModelDrawInfo &drawInfo) 
{
	std::map< std::string /*model name*/,
			sModelDrawInfo /* info needed to draw*/ >::iterator 
		itDrawInfo = this->m_map_ModelName_to_VAOID.find( filename );

	// Find it? 
	if ( itDrawInfo == this->m_map_ModelName_to_VAOID.end() )
	{
		// Nope
		return false;
	}

	// Else we found the thing to draw
	// ...so 'return' that information
	drawInfo = itDrawInfo->second;
	return true;
}



bool cVAOManager::m_LoadTheFile(std::string fileName, sModelDrawInfo& drawInfo)
{

    Assimp::Importer importer;
    std::string filePath = this->m_basePathWithoutSlash + "/" + fileName;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate);

    if (!scene)
    {
        // Error loading the model
        // Handle the error as needed
        return false;
    }

    const aiMesh* mesh = scene->mMeshes[0]; // Assuming there's only one mesh in the scene

    drawInfo.meshName = fileName; // You can change this as needed

    // Allocate memory for vertices and indices
    drawInfo.numberOfVertices = mesh->mNumVertices;
    drawInfo.pVertices = new sVertex[drawInfo.numberOfVertices];

    for (unsigned int i = 0; i < drawInfo.numberOfVertices; ++i)
    {
        const aiVector3D& vertex = mesh->mVertices[i];
        const aiVector3D& normal = mesh->mNormals[i];
        const aiColor4D& color = mesh->mColors[0][i]; // Assuming there's only one color

        drawInfo.pVertices[i].x = vertex.x;
        drawInfo.pVertices[i].y = vertex.y;
        drawInfo.pVertices[i].z = vertex.z;

        if (mesh->HasVertexColors(0))
        {
            drawInfo.pVertices[i].r = color.r;
            drawInfo.pVertices[i].g = color.g;
            drawInfo.pVertices[i].b = color.b;
            drawInfo.pVertices[i].a = color.a;
        }
      
        if (mesh->HasNormals())
        {
            drawInfo.pVertices[i].nx = normal.x;
            drawInfo.pVertices[i].ny = normal.y;
            drawInfo.pVertices[i].nz = normal.z;
        }

        if (mesh->HasTextureCoords(0))
        {
			drawInfo.pVertices[i].u = mesh->mTextureCoords[0][i].x;
			drawInfo.pVertices[i].v = mesh->mTextureCoords[0][i].y;
		}
       
    }

    drawInfo.numberOfIndices = mesh->mNumFaces * 3; // Triangles assumed
    drawInfo.pIndices = new unsigned int[drawInfo.numberOfIndices];
    drawInfo.numberOfTriangles = mesh->mNumFaces;
    drawInfo.pTriangles = new sTriangle[drawInfo.numberOfTriangles];

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
        const aiFace& face = mesh->mFaces[i];
        drawInfo.pIndices[i * 3] = face.mIndices[0];
        drawInfo.pIndices[i * 3 + 1] = face.mIndices[1];
        drawInfo.pIndices[i * 3 + 2] = face.mIndices[2];

        drawInfo.pTriangles[i].v1.x = mesh->mVertices[face.mIndices[0]].x;
        drawInfo.pTriangles[i].v1.y = mesh->mVertices[face.mIndices[0]].y;
        drawInfo.pTriangles[i].v1.z = mesh->mVertices[face.mIndices[0]].z;

        drawInfo.pTriangles[i].v2.x = mesh->mVertices[face.mIndices[1]].x;
        drawInfo.pTriangles[i].v2.y = mesh->mVertices[face.mIndices[1]].y;
        drawInfo.pTriangles[i].v2.z = mesh->mVertices[face.mIndices[1]].z;

        drawInfo.pTriangles[i].v3.x = mesh->mVertices[face.mIndices[2]].x;
        drawInfo.pTriangles[i].v3.y = mesh->mVertices[face.mIndices[2]].y;
        drawInfo.pTriangles[i].v3.z = mesh->mVertices[face.mIndices[2]].z;


    }

    return true;
}

bool cVAOManager::UpdateVAOBuffers(std::string fileName,
                      sModelDrawInfo& updatedDrawInfo,
                      unsigned int shaderProgramID)
{
    // This exists? 
    sModelDrawInfo updatedDrawInfo_TEMP;
    if ( ! this->FindDrawInfoByModelName(fileName,  updatedDrawInfo_TEMP) )
    {
        // Didn't find this buffer
        return false;
    }


    glBindBuffer(GL_ARRAY_BUFFER, updatedDrawInfo.VertexBufferID);

// Original call to create and copy the initial data:
//     
//    glBufferData(GL_ARRAY_BUFFER,
//                 sizeof(sVertex) * updatedDrawInfo.numberOfVertices,	
//                 (GLvoid*)updatedDrawInfo.pVertices,					
//                 GL_DYNAMIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 
                    0,  // Offset
                    sizeof(sVertex) * updatedDrawInfo.numberOfVertices,	
                    (GLvoid*)updatedDrawInfo.pVertices);

    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, updatedDrawInfo.IndexBufferID);

    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,			
                    0,  
                    sizeof(unsigned int) * updatedDrawInfo.numberOfIndices,
                    (GLvoid*)updatedDrawInfo.pIndices);

    glBindBuffer(GL_ARRAY_BUFFER, 0);


    return true;
}
