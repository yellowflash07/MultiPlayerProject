
#include "Engine.h"
#include "Random.h"
#include <iostream>
#include <map>
#include "UdpServer.h"
#include "UdpClient.h"
#include "../Extern/bin/multiplayer.pb.h"
#include "Buffer.h"
#include <conio.h>
#include "PlayerClient.h"

extern Camera* camera;
int keyHit = 0;
int action = 0;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        //   std::cout << "Hit" << key << std::endl;
        keyHit = key;
        action = action;
    }
}

float pitch = 0.0f;
float yaw = 0.0f;
float roll = 0.0f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    camera->ProcessMouseMovement(xpos, ypos);
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

    yaw += xoffset;
    pitch += yoffset;
}

int main(void)
{
    //Engine engine;
    //if (!engine.Initialize())
    //{
    //    return 1;
    //}

    //engine.meshManager->LoadTexture("PaletteV1.bmp");
    //bool loaded = engine.meshManager->LoadCubeMap("space",
    //                                "CubeMaps/TropicalSunnyDayLeft2048.bmp",
    //                                "CubeMaps/TropicalSunnyDayRight2048.bmp",
    //                                "CubeMaps/TropicalSunnyDayUp2048.bmp",
    //                                "CubeMaps/TropicalSunnyDayDown2048.bmp",
    //                                "CubeMaps/TropicalSunnyDayFront2048.bmp",
    //                                "CubeMaps/TropicalSunnyDayBack2048.bmp",
    //                                true);
    //
    //cMesh* skyBoxMesh = engine.LoadMesh("Sphere_1_unit_Radius_UV.ply", "skybox");
    //skyBoxMesh->isSkyBox = true;
    //skyBoxMesh->setUniformDrawScale(5000.0f);

    std::cout << "Client? (y/n) " << std::endl;
    char input;
    std::cin >> input;

    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 dir = glm::vec3(0.0f, 1.0f, 0.0f);
    PlayerClient* playerClient = new PlayerClient();
   // std::string str = "Hello";
    Buffer buf(1024);
    if (input == 'y')
    {
        UdpClient client;
        if (!client.Initialize())
        {
			return 1;
		}
        playerClient->SetClient(client);
      //  buf.WriteUInt32LE(str.length());
      //  buf.WriteString(str);
       // client.SetBuffer(buf);
        while (true)
        {
            client.SendDataToServer();

            if (_kbhit())
            {
				keyHit = _getch();
              //  printf("Hit: %d\n", keyHit);
                if (keyHit == 119)
                {

                    pos += dir * 1.0f;
                    playerClient->SetPosition(pos);
                    playerClient->SendDataToServer();

                    printf("Sending data to server\n");
                    //Vector* playerPos = new Vector();
                    //playerPos->set_x(1);
                    //playerPos->set_y(2);
                    //playerPos->set_z(3);

                    //Player* player = new Player();
                    //player->set_allocated_position(playerPos);

                    //std::string serializedPlayer = player->SerializeAsString();
                    //buf.WriteUInt32LE(serializedPlayer.length());
                    //buf.WriteString(serializedPlayer);
                    //client.SetBuffer(buf);
                    
                }
			}
            Sleep(1000);
        }
    }
    else
    {
        Buffer recvBuf(1024);
        UdpServer server;
        if (!server.Initialize())
        {
            return 1;
        }
        while (true)
        {
            server.Listen();
            recvBuf = server.GetRecvBuffer();
            if (recvBuf.bufferData.size() > 0)
            {
                uint32_t len = recvBuf.ReadUInt32LE();
                std::string str = recvBuf.ReadString(len);
                Player player;
                bool success = player.ParseFromString(str);
                if (success)
                {
                    std::cout << "Received X: " << player.position().x() << std::endl;
                    std::cout << "Received Y: " << player.position().y() << std::endl;
                    std::cout << "Received Z: " << player.position().z() << std::endl;
                }
			}

            Sleep(1000);
        }

    }



    //float currTime = 0;
    //float myTime = 0;
    //while (!glfwWindowShouldClose(engine.window))
    //{
    //    engine.Update();
    //}

    //engine.ShutDown();
}
