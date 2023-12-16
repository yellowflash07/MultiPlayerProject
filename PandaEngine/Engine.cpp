#include "Engine.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp> 
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "GLFWCallbacks.h"

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

Camera* camera;

//void mouse_callback(GLFWwindow* window, double xpos, double ypos)
//{
//    camera->ProcessMouseMovement(xpos, ypos);
//}

Engine::Engine()
{
	shaderManager = new cShaderManager();
	meshManager = new MeshManager();
	lightManager = new cLightManager();
    physicsManager = new PhysicsManager(meshManager);
    audioManager = new cAudioManager(meshManager);
    camera = new Camera(glm::vec3(0.0,0.0f,0.0f),
        		        glm::vec3(0.0f, 0.0f, -1.0f),
        		        glm::vec3(0.0f, 1.0f, 0.0f), 0.1f, 10000.0f);
}

Engine::~Engine()
{
}

bool Engine::Initialize()
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(1920, 1080, "Template Scene", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return false;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    audioManager->Initialize();

    SetShaderPath("../Assets/Shaders");
    SetModelPath("../Assets/Models");
    SetAudioPath("../Assets/Audio");
    meshManager->SetTexturePath("../Assets/Textures");

    if (!LoadDefaultShaders())
    {
        return false;
    }

    LoadDefaultLights();

    return true;
}

void Engine::Update()
{
    float ratio;
    int width, height;

    glUseProgram(shaderProgramID);

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float)height;

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    // While drawing a pixel, see if the pixel that's already there is closer or not?
    glEnable(GL_DEPTH_TEST);
    // (Usually) the default - does NOT draw "back facing" triangles
    glCullFace(GL_BACK);

    //update lights
    lightManager->UpdateUniformValues(shaderProgramID);

    //update camera
    camera->Update(window, deltaTime);

    //draw meshes
    meshManager->DrawAllObjects(shaderProgramID);

    // Time per frame
    double currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    frameCount++;

    if (deltaTime >= 1.0) {
        double fps = static_cast<double>(frameCount) / deltaTime;
        frameCount = 0;
        // Display FPS
        ImGui::Begin("FPS Counter"); ImGui::SetNextItemWidth(100);
        ImGui::Text("FPS: %.2f", fps); 
        ImGui::End();
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
		glfwSetWindowShouldClose(window, true);
        exit(EXIT_SUCCESS);
	}

    //update physics
    physicsManager->Update(deltaTime);

    //update audio
    audioManager->Update();

    audioManager->SetListenerAttributes(camera->cameraEye, 
                                        glm::vec3(0,0,0), 
                                        camera->GetCameraRotation() * glm::vec3(0, 0, 1),
                                        camera->upVector);

    //render the frame
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


    glfwSwapBuffers(window);
    glfwPollEvents();

}

void Engine::SetShaderPath(std::string filePath)
{
	//shaderManager->setBasePath("../Assets/Shaders");
	shaderManager->setBasePath(filePath);
}

void Engine::SetModelPath(std::string filePath)
{
    meshManager->SetBasePath(filePath);
}

void Engine::SetAudioPath(std::string filePath)
{
    audioManager->SetBasePath(filePath);
}

cMesh* Engine::LoadMesh(std::string filePath, std::string friendlyName, bool dontDraw)
{
    cMesh* mesh = nullptr;
    if (dontDraw)
    {
		mesh = meshManager->LoadMesh(filePath, friendlyName, shaderProgramID);
	}
    else
    {
		mesh = meshManager->AddMesh(filePath, friendlyName, shaderProgramID);
	}
    //cMesh* mesh = meshManager->AddMesh(filePath, friendlyName, shaderProgramID);
    return mesh;
}

PhysicsBody* Engine::AddPhysicsBody(std::string friendlyMeshName)
{
    cMesh* mesh = meshManager->FindMeshByFriendlyName(friendlyMeshName);
    PhysicsBody* pb = new PhysicsBody();
    pb->mesh = mesh;
    physicsManager->AddMesh(pb);
    return pb;
}


bool Engine::LoadDefaultShaders()
{
    cShaderManager::cShader vertexShader;
    vertexShader.fileName = "vertexShader.glsl";

    cShaderManager::cShader fragmentShader;
    fragmentShader.fileName = "fragmentShader.glsl";

    if (!shaderManager->createProgramFromFile("shader01", vertexShader, fragmentShader))
    {
        std::cout << "Error: Couldn't compile or link:" << std::endl;
        std::cout << shaderManager->getLastError();
        return false;
    }
    shaderProgramID = shaderManager->getIDFromFriendlyName("shader01");
    std::cout << "Shader compliled! Program ID: " << shaderProgramID << std::endl;

    camera->shaderProgramID = shaderProgramID;

    return true;
}

void Engine::LoadDefaultLights()
{
    lightManager->SetUniformLocations(shaderProgramID);
    lightManager->theLights[0]->param2.x = 1; //on
    lightManager->theLights[0]->param1.x = 2; //directional
    lightManager->theLights[0]->direction = glm::vec4(-1.0,-1.0,-1.0,1); //directional
}

void Engine::LoadSave()
{
    meshManager->LoadSavedMeshes(shaderProgramID);
	//lightManager->LoadLights();
}

void Engine::ShutDown()
{
    delete shaderManager;
    delete meshManager;
    delete lightManager;
    delete physicsManager;
    delete audioManager;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    //exit(EXIT_SUCCESS);
}

//void Engine::SetCamera(Camera* camera)
//{
//  //  this->camera = camera;
//   // this->camera->shaderProgramID = shaderProgramID;
//}

