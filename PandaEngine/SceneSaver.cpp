#include "SceneSaver.h"
#include <fstream>
#include <sstream>

SceneSaver::SceneSaver()
{
}

SceneSaver::~SceneSaver()
{
}

void SceneSaver::SaveMeshes(std::vector<cMesh*> meshes)
{
    std::ofstream file("mesh_values.txt");

    if (file.is_open()) 
    {
        for (cMesh* mesh : meshes) 
        {
            file << "Mesh Name:" << mesh->meshName << "\n";
            file << "Friendly Name:" << mesh->friendlyName << "\n";
            file << "Position:" << mesh->drawPosition.x << " " << mesh->drawPosition.y << " " << mesh->drawPosition.z << "\n";
            file << "Rotation:" << mesh->eulerRotation.x << " " << mesh->eulerRotation.y << " " << mesh->eulerRotation.z << "\n";
            file << "Scale:" << mesh->drawScale.x << " " << mesh->drawScale.y << " " << mesh->drawScale.z << "\n";
            file << "Color:" << mesh->color.x << " " << mesh->color.y << " " << mesh->color.z << " " << mesh->color.w << "\n";
            file << "Transparency:" << mesh->transperancy << "\n";
            // Separate meshes with an empty line
            file << "\n";
        }

        file.close();
    }
}

std::vector<cMesh*> SceneSaver::LoadMeshes()
{
    std::vector<cMesh*> loadedMeshes;
    std::ifstream file("mesh_values.txt");

    // Function to load values from a text file
    if (file.is_open()) {
        std::string line;
        bool readMeshData = false;
        cMesh currentMesh;

        while (std::getline(file, line)) 
        {
            std::istringstream iss(line);
            std::string token;

            if (std::getline(iss, token, ':')) 
            {
                if (token == "Mesh Name") 
                {
                    // If we've already read mesh data for a previous mesh, add it to the vector
                    if (readMeshData) 
                    {
                        cMesh* newMesh = new cMesh;
                        newMesh->meshName = currentMesh.meshName;
                        newMesh->friendlyName = currentMesh.friendlyName;
                        newMesh->drawPosition = currentMesh.drawPosition;
                        newMesh->eulerRotation = currentMesh.eulerRotation;
                        newMesh->drawScale = currentMesh.drawScale;
                        newMesh->color = currentMesh.color;
                        loadedMeshes.push_back(newMesh);
                    }

                    std::getline(iss, currentMesh.meshName);
                }
                else if (token == "Friendly Name") 
                {
                    std::getline(iss, currentMesh.friendlyName);
                }
                else if (token == "Position")
                {
                    iss >> currentMesh.drawPosition.x >> currentMesh.drawPosition.y >> currentMesh.drawPosition.z;
                }
                else if (token == "Rotation") 
                {
                    iss >> currentMesh.eulerRotation.x >> currentMesh.eulerRotation.y >> currentMesh.eulerRotation.z;
                }
                else if (token == "Scale") 
                {
                    iss >> currentMesh.drawScale.x >> currentMesh.drawScale.y >> currentMesh.drawScale.z;
                }
                else if (token == "Color")
                {
					iss >> currentMesh.color.x >> currentMesh.color.y >> currentMesh.color.z >> currentMesh.color.w;
				}
                else if (token == "Transparency")
                {
                    iss >> currentMesh.transperancy;
                }
            }
            else if (line.empty()) 
            {
                // An empty line indicates the end of a mesh's data
                readMeshData = true;
            }
        }

        // Add the last mesh to the vector
        if (readMeshData) 
        {
            cMesh* newMesh = new cMesh;
            newMesh->meshName = currentMesh.meshName;
            newMesh->friendlyName = currentMesh.friendlyName;
            newMesh->drawPosition = currentMesh.drawPosition;
            newMesh->eulerRotation = currentMesh.eulerRotation;
            newMesh->drawScale = currentMesh.drawScale;
            newMesh->color = currentMesh.color;
            loadedMeshes.push_back(newMesh);
        }

        file.close();
    }
    
	return loadedMeshes;
}

void SceneSaver::SaveLights(cLight* theLights[], int size)
{
    std::ofstream file("light_values.txt");

    if (file.is_open()) 
    {
        for (int i = 0; i < size; ++i) 
        {
            file << "Position: " << theLights[i]->position.x << " " << theLights[i]->position.y << " " << theLights[i]->position.z << " " << theLights[i]->position.w << "\n";
            file << "Diffuse: " << theLights[i]->diffuse.x << " " << theLights[i]->diffuse.y << " " << theLights[i]->diffuse.z << " " << theLights[i]->diffuse.w << "\n";
            file << "Specular: " << theLights[i]->specular.x << " " << theLights[i]->specular.y << " " << theLights[i]->specular.z << " " << theLights[i]->specular.w << "\n";
            file << "Atten: " << theLights[i]->atten.x << " " << theLights[i]->atten.y << " " << theLights[i]->atten.z << " " << theLights[i]->atten.w << "\n";
            file << "Direction: " << theLights[i]->direction.x << " " << theLights[i]->direction.y << " " << theLights[i]->direction.z << " " << theLights[i]->direction.w << "\n";
            file << "Param1: " << theLights[i]->param1.x << " " << theLights[i]->param1.y << " " << theLights[i]->param1.z << " " << theLights[i]->param1.w << "\n";
            file << "Param2: " << theLights[i]->param2.x << " " << theLights[i]->param2.y << " " << theLights[i]->param2.z << " " << theLights[i]->param2.w << "\n";
            // Separate lights with an empty line
            file << "\n";
        }

        file.close();
    }
}

std::vector<cLight> SceneSaver::LoadLights()
{
    std::vector<cLight> loadedLights;
    std::ifstream file("light_values.txt");

    if (file.is_open()) 
    {
        cLight currentLight; // Temporary storage for the current light
        std::string line;

        while (std::getline(file, line)) 
        {
            std::istringstream iss(line);
            std::string token;

            if (std::getline(iss, token, ':')) 
            {
                if (token == "Position") 
                {
                    iss >> currentLight.position.x >> currentLight.position.y >> currentLight.position.z >> currentLight.position.w;
                }
                else if (token == "Diffuse") 
                {
                    iss >> currentLight.diffuse.x >> currentLight.diffuse.y >> currentLight.diffuse.z >> currentLight.diffuse.w;
                }
                else if (token == "Specular")
                {
                    iss >> currentLight.specular.x >> currentLight.specular.y >> currentLight.specular.z >> currentLight.specular.w;
                }
                else if (token == "Atten") 
                {
                    iss >> currentLight.atten.x >> currentLight.atten.y >> currentLight.atten.z >> currentLight.atten.w;
                }
                else if (token == "Direction")
                {
                    iss >> currentLight.direction.x >> currentLight.direction.y >> currentLight.direction.z >> currentLight.direction.w;
                }
                else if (token == "Param1") 
                {
                    iss >> currentLight.param1.x >> currentLight.param1.y >> currentLight.param1.z >> currentLight.param1.w;
                }
                else if (token == "Param2") 
                {
                    iss >> currentLight.param2.x >> currentLight.param2.y >> currentLight.param2.z >> currentLight.param2.w;
                }
            }
            else if (line.empty())
            {
                // An empty line indicates the end of a light's data, so add the current light to the vector
                loadedLights.push_back(currentLight);
            }
        }

        file.close();
    }

    return loadedLights;
}





