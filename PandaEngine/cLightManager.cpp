#pragma once
#include "SceneSaver.h"
#include "cLightManager.h"
#include <sstream> //"string stream"
#include <imgui.h>

cLight::cLight()
{
	this->position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	this->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);		// White light
	this->specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);		// White light

	// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	this->atten = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);	
	// Spot, directional lights
	// (Default is stright down)
	this->direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);	
	// x = lightType, y = inner angle, z = outer angle, w = TBD
	// type = 0 => point light
	this->param1 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
					// 0 = pointlight
					// 1 = spot light
					// 2 = directional light
	// x = 0 for off, 1 for on
	this->param2 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);


	this->position_UL = -1;
	this->diffuse_UL = -1;
	this->specular_UL = -1;
	this->atten_UL = -1;
	this->direction_UL = -1;
	this->param1_UL = -1;
	this->param2_UL = -1;

}

	// And the uniforms:
void cLight::TurnOn(void)
{
	// x = 0 for off, 1 for on
	this->param2.x = 1.0f;		// Turn on 
	return;
}

void cLight::TurnOff(void)
{
	// x = 0 for off, 1 for on
	this->param2.x = 0.0f;		// Turn off 
	return;
}


void cLightManager::SetUniformLocations(GLuint shaderID)
{

	for (size_t i = 0; i < NUMBER_OF_LIGHTS_IM_USING; i++)
	{
		std::string lightPositon = "theLights[" + std::to_string(i) + "]" + ".position";
		std::string lightdiffuse = "theLights[" + std::to_string(i) + "]" + ".diffuse";
		std::string lightspecular = "theLights[" + std::to_string(i) + "]" + ".specular";
		std::string lightatten = "theLights[" + std::to_string(i) + "]" + ".atten";
		std::string lightdirection = "theLights[" + std::to_string(i) + "]" + ".direction";
		std::string lightparam1 = "theLights[" + std::to_string(i) + "]" + ".param1";
		std::string lightparam2 = "theLights[" + std::to_string(i) + "]" + ".param2";
		//vec4 position
		this->theLights[i]->position_UL = glGetUniformLocation(shaderID, lightPositon.c_str());
		//        vec4 diffuse;	// Colour of the light (used for diffuse)
		this->theLights[i]->diffuse_UL = glGetUniformLocation(shaderID, lightdiffuse.c_str());
		//        vec4 specular;	// rgb = highlight colour, w = power
		this->theLights[i]->specular_UL = glGetUniformLocation(shaderID, lightspecular.c_str());
		//        vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
		this->theLights[i]->atten_UL = glGetUniformLocation(shaderID, lightatten.c_str());
		//        vec4 direction;	// Spot, directional lights
		this->theLights[i]->direction_UL = glGetUniformLocation(shaderID, lightdirection.c_str());
		//        vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
		this->theLights[i]->param1_UL = glGetUniformLocation(shaderID, lightparam1.c_str());
		//        vec4 param2;	// x = 0 for off, 1 for on
		this->theLights[i]->param2_UL = glGetUniformLocation(shaderID, lightparam2.c_str());
	}

	return;
}

// This is called every frame
void cLightManager::UpdateUniformValues(GLuint shaderID)
{
	for ( unsigned int index = 0; index != cLightManager::NUMBER_OF_LIGHTS_IM_USING; index++ )
	{
		cLight* pCurrentLight = theLights[index];
		std::string label = "Light [" + std::to_string(index) + "]";
		ImGui::Begin("Lights");
		if (ImGui::Button(label.c_str(), ImVec2(100, 25)))
		{
			selectedLight = pCurrentLight;
		}
		ImGui::End();
		glUniform4f(pCurrentLight->position_UL,
					pCurrentLight->position.x,
					pCurrentLight->position.y,
					pCurrentLight->position.z,
					pCurrentLight->position.w);

		glUniform4f(theLights[index]->diffuse_UL,
					theLights[index]->diffuse.x,
					theLights[index]->diffuse.y,
					theLights[index]->diffuse.z,
					theLights[index]->diffuse.w);

		glUniform4f(theLights[index]->specular_UL,
					theLights[index]->specular.x,
					theLights[index]->specular.y,
					theLights[index]->specular.z,
					theLights[index]->specular.w);

		glUniform4f(theLights[index]->atten_UL,
					theLights[index]->atten.x,
					theLights[index]->atten.y,
					theLights[index]->atten.z,
					theLights[index]->atten.w);

		glUniform4f(theLights[index]->direction_UL,
					theLights[index]->direction.x,
					theLights[index]->direction.y,
					theLights[index]->direction.z,
					theLights[index]->direction.w);

		glUniform4f(theLights[index]->param1_UL,
					theLights[index]->param1.x,
					theLights[index]->param1.y,
					theLights[index]->param1.z,
					theLights[index]->param1.w);

		glUniform4f(theLights[index]->param2_UL,
					theLights[index]->param2.x,
					theLights[index]->param2.y,
					theLights[index]->param2.z,
					theLights[index]->param2.w);
	}// for ( unsigned int index...

	DrawBox();
	return;
}

void cLightManager::DrawBox()
{
	if (selectedLight == nullptr) return;
//	std::string boxName = "Transform " + selectedMesh->friendlyName;
	ImGui::Begin("Light Value");

	ImGui::Text("On?"); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("On", &selectedLight->param2.x);

	ImGui::Text("Type"); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("Type", &selectedLight->param1.x); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("inner angle", &selectedLight->param1.y); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("outer angle", &selectedLight->param1.z);

	ImGui::Text("Position"); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("xL", &selectedLight->position.x); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("yL", &selectedLight->position.y); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("zL", &selectedLight->position.z);
	selectedLight->position.w = 1;

	ImGui::Text("Diffuse"); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("xD", &selectedLight->diffuse.x); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("yD", &selectedLight->diffuse.y); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("zD", &selectedLight->diffuse.z);
	selectedLight->diffuse.w = 1;

	ImGui::Text("Specular"); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("xS", &selectedLight->specular.x); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("yS", &selectedLight->specular.y); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("zS", &selectedLight->specular.z);
	selectedLight->specular.w = 1;

	// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	ImGui::Text("Attenuation"); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("constant", &selectedLight->atten.x); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("linear", &selectedLight->atten.y); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("quadratic", &selectedLight->atten.z); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("DistanceCutOff", &selectedLight->atten.w); 

	ImGui::Text("Direction"); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("xDi", &selectedLight->direction.x); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("yDi", &selectedLight->direction.y); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("zDi", &selectedLight->direction.z);
	//ImGui::InputFloat("xDi", &selectedLight->atten.z); ImGui::SameLine(); ImGui::SetNextItemWidth(40);

	if (ImGui::Button("Save"))
	{
		SceneSaver saver;
		saver.SaveLights(theLights, NUMBER_OF_LIGHTS_IM_USING);
	}

	ImGui::End();
}

void cLightManager::LoadLights()
{
	/*SceneSaver saver;
	std::vector<cLight> loadedLights = saver.LoadLights();
	for (size_t i = 0; i < loadedLights.size(); i++)
	{
		theLights[i] = &loadedLights[i];
	}*/
}


cLightManager::cLightManager()
{
	for (size_t i = 0; i < NUMBER_OF_LIGHTS_IM_USING; i++)
	{
		theLights[i] = new cLight();
	}
}