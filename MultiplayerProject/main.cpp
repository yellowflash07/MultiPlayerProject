
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
    glm::vec3 rot = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 bulletDir = glm::vec3(0.0f, 0.0f, 0.0f);
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
            cMesh* player = engine.LoadMesh("Cube_1x1x1_xyz_n_rgba.ply", std::to_string(i));
            player->bUseDebugColours = true;
            player->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            players.push_back(player);
        }

        cMesh* bulletMesh = engine.LoadMesh("Cube_1x1x1_xyz_n_rgba.ply", "Bullet");
        bulletMesh->bUseDebugColours = true;
        bulletMesh->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

        float currTime = 0;
        camera->camControl = false;
        bool hasShot = false;
        while (!glfwWindowShouldClose(engine.window))
        {
            engine.Update();
           camera->cameraEye = glm::vec3(0.0f, 0.0f, -15.0f);

            currTime += engine.deltaTime;
            client.SendDataToServer();

            if (currTime > 0.2f)
            {
                currTime = 0;
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
                     
                        players[i]->setRotationFromEuler(glm::vec3(player.orientation().x(),
                                                            player.orientation().y(),
                                                            player.orientation().z()));
                        Bullet bullet = player.bullet();
                        if (bullet.playerindex() == i)
                        {
                            if (bullet.hasbullet())
                            {
                                glm::vec3 bulletDirection = (glm::vec3(bullet.direction().x(),
                                    bullet.direction().y(),
                                    bullet.direction().z()));

                                bulletMesh->drawPosition = glm::vec3(player.position().x(),
                                                                    player.position().y(),
                                                                    player.position().z());

                                bulletMesh->drawPosition += bulletDirection * 0.01f;
                               
                            }
                            else
                            {
                                bulletMesh->drawPosition = glm::vec3(0.0f, 1000.0f, 0.0f);
                            }
                        }
                        
                    }

                   
                }
            }

            if (glfwGetKey(engine.window, GLFW_KEY_W) == GLFW_PRESS)
            {
                dir = glm::vec3(0, 0, 1);
                pos += dir * 0.01f;
            }
            if (glfwGetKey(engine.window, GLFW_KEY_A) == GLFW_PRESS)
            {
                dir = glm::vec3(-1, 0, 0);
                pos += dir * 0.01f;
            }
            if (glfwGetKey(engine.window, GLFW_KEY_S) == GLFW_PRESS)
            {
                dir = glm::vec3(0, 0, -1);
                pos += dir * 0.01f;
            }
            if (glfwGetKey(engine.window, GLFW_KEY_D) == GLFW_PRESS)
            {
                dir = glm::vec3(1, 0, 0);
                pos += dir * 0.01f;
            }
            if (glfwGetKey(engine.window, GLFW_KEY_SPACE) == GLFW_PRESS)
            {
                //shoot
                if (!hasShot)
                {
					bulletDir = dir;
                    hasShot = true;
				}
            }

            Vector* position = new Vector();
            position->set_x(pos.x);
            position->set_y(pos.y);
            position->set_z(pos.z);

            Vector* direction = new Vector();
            direction->set_x(dir.x);
            direction->set_y(dir.y);
            direction->set_z(dir.z);
           
            Bullet* bullet = new Bullet();
            bullet->set_hasbullet(!hasShot);
            bullet->set_playerindex(-1);
            bullet->set_allocated_direction(direction);

            player.set_allocated_position(position);
            player.set_allocated_orientation(direction);
            player.set_allocated_bullet(bullet);
            player.release_bullet();

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
            Bullet bullet = player->bullet();
            bullet.set_hasbullet(false);
            bullet.set_playerindex(i);
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

                    Bullet bullet = player.bullet();
                    if (bullet.hasbullet())
                    {
						Bullet* updateBullet = updatePlayer->mutable_bullet();
						updateBullet->set_hasbullet(true);
						updateBullet->set_playerindex(server.clientIndex);
						Vector* directionVector = new Vector;
						directionVector->set_x(bullet.direction().x());
						directionVector->set_y(bullet.direction().y());
						directionVector->set_z(bullet.direction().z());
						updateBullet->set_allocated_direction(directionVector);
					}
                    else
                    {
						Bullet* updateBullet = updatePlayer->mutable_bullet();
						updateBullet->set_hasbullet(false);
						updateBullet->set_playerindex(server.clientIndex);
                        Vector* directionVector = new Vector;
                        directionVector->set_x(0);
                        directionVector->set_y(0);
                        directionVector->set_z(0);
                        updateBullet->set_allocated_direction(directionVector);
					}

                 //   updatePlayer->set_allocated_bullet(updatePlayer->mutable_bullet());

                  /*  Bullet* bullet = gameScene.mutable_bullets(server.clientIndex);
                    bullet->set_hasbullet(true);
                    bullet->set_playerindex(server.clientIndex);*/


                    printf("Player %d shot %d\n",
                        updatePlayer->id(), updatePlayer->has_bullet());

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

