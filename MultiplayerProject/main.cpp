
#include "Engine.h"
#include "Random.h"
#include <iostream>
#include <map>
#include "UdpServer.h"
#include "UdpClient.h"
#include "../Extern/proto/multiplayer.pb.h"
#include "Buffer.h"
#include <conio.h>
#include "PlayerClient.h"
#include "GameServer.h"

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


    std::cout << "Client? (y/n) " << std::endl;
    char input;
    std::cin >> input;

    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 dir = glm::vec3(0.0f, 0.0f, -1.0f);
    // PlayerClient* playerClient = new PlayerClient();

    if (input == 'y')
    {
        UdpClient client;

        if (!client.Initialize())
        {
            return 1;
        }
        //   playerClient->SetClient(client);
        Buffer m_buffer;
        Player player;
        player.set_id(-1);
        std::string serializedPlayer = player.SerializeAsString();
        m_buffer.WriteUInt32LE(serializedPlayer.length());
        m_buffer.WriteString(serializedPlayer);

        client.SetBuffer(m_buffer);
        Engine engine;
        if (!engine.Initialize())
        {
			return 1;
		}
            bool loaded = engine.meshManager->LoadCubeMap("space",
                                                        "CubeMaps/TropicalSunnyDayLeft2048.bmp",
                                                        "CubeMaps/TropicalSunnyDayRight2048.bmp",
                                                        "CubeMaps/TropicalSunnyDayUp2048.bmp",
                                                        "CubeMaps/TropicalSunnyDayDown2048.bmp",
                                                        "CubeMaps/TropicalSunnyDayFront2048.bmp",
                                                        "CubeMaps/TropicalSunnyDayBack2048.bmp",
                                                        true);

        cMesh* skyBoxMesh = engine.LoadMesh("Sphere_1_unit_Radius_UV.ply", "skybox");
        skyBoxMesh->isSkyBox = true;
        skyBoxMesh->setUniformDrawScale(5000.0f);
        std::vector<cMesh*> players;
        for (size_t i = 0; i < 4; i++)
        {
            cMesh* player = engine.LoadMesh("Sphere_1_unit_Radius_UV.ply", std::to_string(i));
            player->bUseDebugColours = true;
            player->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            players.push_back(player);
        }

        float currTime = 0;
        while (!glfwWindowShouldClose(engine.window))
        {
            engine.Update();

            currTime += engine.deltaTime;
            client.SendDataToServer();

            if (currTime > 0.2f)
            {
                currTime = 0;
				std::cout << "Sending data to server" << std::endl;
            }


            Buffer recv = client.GetRecvBuffer();
            uint32_t len = recv.ReadUInt32LE();
            if (len > 0)
            {
                GameScene gameScene;
                std::string str = recv.ReadString(len);
                bool success = gameScene.ParseFromString(str);
                if (success)
                {
                    for (size_t i = 0; i < gameScene.players_size(); i++)
                    {
                        Player player = gameScene.players(i);

                        players[i]->drawPosition = glm::vec3(player.position().x(),
                            player.position().y(),
                            player.position().z());
                    }
                }
            }

            if (glfwGetKey(engine.window, GLFW_KEY_W) == GLFW_PRESS)
            {
                pos += glm::vec3(0,0,1) * 0.01f;
            }
            if (glfwGetKey(engine.window, GLFW_KEY_A) == GLFW_PRESS)
            {
                pos += glm::vec3(-1, 0, 0) * 0.01f;
            }
            if (glfwGetKey(engine.window, GLFW_KEY_S) == GLFW_PRESS)
            {
                pos += glm::vec3(0, 0, -1) * 0.01f;
            }
            if (glfwGetKey(engine.window, GLFW_KEY_D) == GLFW_PRESS)
            {
                pos += glm::vec3(1, 0, 0) * 0.01f;
            }

            Vector* position = new Vector();
            position->set_x(pos.x);
            position->set_y(pos.y);
            position->set_z(pos.z);
            player.set_allocated_position(position);

            std::string serializedPlayer = player.SerializeAsString();

            Buffer updateBuffer;
            updateBuffer.WriteUInt32LE(serializedPlayer.length());
            updateBuffer.WriteString(serializedPlayer);

            client.SetBuffer(updateBuffer);

        }

        engine.ShutDown();
    }
    else
    {
        std::vector<Player*> players;
        players.resize(4);

        Buffer recvBuf(1024);
        UdpServer server;
        //   GameServer gameServer;
        if (!server.Initialize())
        {
            return 1;
        }

        //    gameServer.SetServer(server);
        GameScene gameScene;

        for (size_t i = 0; i < 4; i++)
        {
            Player* player = gameScene.add_players();
            player->set_id(i);
        }

        gameScene.set_id(1);
        while (true)
        {
            server.Listen();
            //  gameServer.Update();

            if (server.clientIndex < 0) continue;

            Buffer recvBuf = server.GetRecvBuffer();
            uint32_t len = recvBuf.ReadUInt32LE();

            if (len > 0)
            {
                Player player;
                std::string str = recvBuf.ReadString(len);
                bool success = player.ParseFromString(str);
                if (success)
                {
                    Player* updatePlayer = gameScene.mutable_players(server.clientIndex);
                    Vector* positionVector = new Vector;
                    positionVector->set_x(player.position().x());
                    positionVector->set_y(player.position().y());
                    positionVector->set_z(player.position().z());

                    updatePlayer->set_allocated_position(positionVector);
                    updatePlayer->set_id(server.clientIndex);

                    printf("Player %d position %.2f\n",
                        updatePlayer->id(), updatePlayer->position().y());

                    std::string gameSerialized = gameScene.SerializeAsString();
                    Buffer sendBuf;
                    sendBuf.WriteUInt32LE(gameSerialized.length());
                    sendBuf.WriteString(gameSerialized);
                    server.SetSendBuffer(sendBuf);
                }
            }
            //  printf("Connected players in scene %d\n", gameScene.players_size());

            Sleep(1);
        }

    }
}


//Engine engine;
//
//if (!engine.Initialize())
//{
//    return 1;
//}
//
//bool loaded = engine.meshManager->LoadCubeMap("space",
//    "CubeMaps/TropicalSunnyDayLeft2048.bmp",
//    "CubeMaps/TropicalSunnyDayRight2048.bmp",
//    "CubeMaps/TropicalSunnyDayUp2048.bmp",
//    "CubeMaps/TropicalSunnyDayDown2048.bmp",
//    "CubeMaps/TropicalSunnyDayFront2048.bmp",
//    "CubeMaps/TropicalSunnyDayBack2048.bmp",
//    true);
//
//cMesh* skyBoxMesh = engine.LoadMesh("Sphere_1_unit_Radius_UV.ply", "skybox");
//skyBoxMesh->isSkyBox = true;
//skyBoxMesh->setUniformDrawScale(5000.0f);
//
//for (size_t i = 0; i < 4; i++)
//{
//    cMesh* player = engine.LoadMesh("Sphere_1_unit_Radius_UV.ply", std::to_string(i));
//    player->bUseDebugColours = true;
//    player->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
//}
//
//float currTime = 0;
//camera->camControl = false;
//while (!glfwWindowShouldClose(engine.window))
//{
//    currTime += engine.deltaTime;
//    if (currTime > 0.2f)
//    {
//        currTime = 0;
//        std::cout << "Sending data to server" << std::endl;
//        client.SendDataToServer();
//    }
//
//    Buffer recv = client.GetRecvBuffer();
//    uint32_t len = recv.ReadUInt32LE();
//
//    if (len > 0)
//    {
//        GameScene gameScene;
//        std::string str = recv.ReadString(len);
//        bool success = gameScene.ParseFromString(str);
//        if (success)
//        {
//            for (size_t i = 0; i < gameScene.players_size(); i++)
//            {
//                Player player = gameScene.players(i);
//                printf("Player %d position %.2f\n",
//                    player.id(), player.position().y());
//
//                cMesh* playerMesh = engine.meshManager->FindMeshByFriendlyName(std::to_string(player.id()));
//                if (playerMesh)
//                {
//                    playerMesh->drawPosition = glm::vec3(player.position().x(),
//                        player.position().y(),
//                        player.position().z());
//
//                }
//            }
//        }
//    }
//
//    engine.Update();
//
//    if (glfwGetKey(engine.window, GLFW_KEY_W) == GLFW_PRESS)
//    {
//        pos += dir * 0.3f;
//        // printf("Player %.2f\n",pos.y);
//
//        Vector* position = new Vector();
//        position->set_x(pos.x);
//        position->set_x(30000);
//        position->set_x(pos.z);
//        player.set_allocated_position(position);
//
//        std::string serializedPlayer = player.SerializeAsString();
//
//        Buffer updateBuffer;
//        updateBuffer.WriteUInt32LE(serializedPlayer.length());
//        updateBuffer.WriteString(serializedPlayer);
//
//        client.SetBuffer(updateBuffer);
//    }
//
//
//}