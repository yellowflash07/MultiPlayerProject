#pragma once

#include <fmod/fmod.h>
#include <glm/vec3.hpp>

void CheckError(FMOD_RESULT result, const char* file, int line);
#define FMODCheckError(result) CheckError(result, __FILE__, __LINE__)
#define GLMToFMOD(in, out) GLMToFMODSt(in, out);
#define FMODToGLM(in, out) FMODToGLMst(in, out);
static void GLMToFMODSt(const glm::vec3& in, FMOD_VECTOR& out)
{
	out.x = in.x;
	out.y = in.y;
	out.z = in.z;
}
static void FMODToGLMst(const FMOD_VECTOR& in, glm::vec3& out)
{
	out.x = in.x;
	out.y = in.y;
	out.z = in.z;
}
