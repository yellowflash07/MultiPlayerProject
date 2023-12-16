#include "Camera.h"
#include <imgui.h>
#include <iostream>

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 upVector, float near, float far)
{
    this->cameraEye = position;
    this->cameraTarget = target;
    this->upVector = upVector;
    this->near = near;
    this->far = far;

    this->yaw = -90.0f;
    this->pitch = 0.0f;
    this->speed = 35.0f;
    this->stopUpdates = false;
    matView = glm::mat4(1.0f);
}

Camera::~Camera()
{
}


void Camera::Update(GLFWwindow* window, double deltaTime)
{

    ImGui::Begin("Camera Controls");
    ImGui::Text("Camera Position: (%f, %f, %f)", cameraEye.x, cameraEye.y, cameraEye.z);
    ImGui::Text("Camera Target: (%f, %f, %f)", cameraTarget.x, cameraTarget.y, cameraTarget.z);
    ImGui::Text("Camera Up Vector: (%f, %f, %f)", upVector.x, upVector.y, upVector.z);
    ImGui::End();

    float ratio;
    int width, height;

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float)height;

    /*if (isFollowing)
    {
		cameraEye = followPos;
		cameraTarget = followTarget;
	}*/

    GLint eyeLocation_UL = glGetUniformLocation(shaderProgramID, "eyeLocation");
    glUniform4f(eyeLocation_UL,
        cameraEye.x, cameraEye.y, cameraEye.z, 1.0f);

    glm::mat4 matProjection = glm::perspective(0.6f,
                                                ratio,
                                                near,
                                                far);

    if (camControl)
    {
        matView = glm::lookAt(cameraEye,
            cameraEye + cameraTarget,
            upVector);
    }
    else
    {
		matView = glm::lookAt(cameraEye,
            			cameraTarget,
            			upVector);
	}
   


    GLint matProjection_UL = glGetUniformLocation(shaderProgramID, "matProjection");
    glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, glm::value_ptr(matProjection));

    GLint matView_UL = glGetUniformLocation(shaderProgramID, "matView");
    glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(matView));

    ProcessKeyboardInput(window, deltaTime);

}

void Camera::ProcessMouseMovement(double xpos, double ypos)
{
    if (!camControl)
    {
		return;
	}
    if (stopUpdates)
    {
		return;
	}
    static bool firstMouse = true;
    static float lastX = 0.0f;
    static float lastY = 0.0f;

    if (firstMouse)
    {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }


    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos; 

    lastX = (float)xpos;
    lastY = (float)ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    this->yaw += xoffset;
    this->pitch += yoffset;

    cameraTarget.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    cameraTarget.y = sin(glm::radians(this->pitch));
    cameraTarget.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    cameraTarget = glm::normalize(cameraTarget);
}

void Camera::ProcessKeyboardInput(GLFWwindow* window, double deltaTime)
{
    if (!camControl)
    {
        return;
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
        stopUpdates = !stopUpdates;
    }
    if (stopUpdates)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        return;
    }
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraEye += speed * (float)deltaTime * cameraTarget;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraEye -= speed * (float)deltaTime * cameraTarget;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraEye -= speed * (float)deltaTime * glm::normalize(glm::cross(cameraTarget, upVector));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraEye += speed * (float)deltaTime * glm::normalize(glm::cross(cameraTarget, upVector));
    }
}

void Camera::Follow(glm::vec3 followPos, glm::vec3 followTarget)
{
    this->followPos = followPos;
    this->followTarget = followTarget;
    isFollowing = true;
}

glm::vec3 Camera::GetCameraRotation()
{
    // Extract pitch, yaw, and roll from the rotation matrix
    float pitch = 0.0f;
    float yaw = 0.0f;
    float roll = 0.0f;;
    glm::mat4 transposedMatrix = glm::transpose(matView);
    // Calculate pitch (around x-axis)
    pitch = (asinf(transposedMatrix[1][2]));

    // Calculate yaw (around y-axis)
    yaw = (atan2f(-transposedMatrix[0][2], transposedMatrix[2][2]));

    // Calculate roll (around z-axis)
    roll = (atan2f(-transposedMatrix[1][0], transposedMatrix[1][1]));

    return glm::vec3(pitch, yaw, roll);

}

