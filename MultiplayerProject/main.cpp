
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

//set camera
extern Camera* camera;
int keyHit = 0;
int action = 0;

//set key callback
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        //   std::cout << "Hit" << key << std::endl;
        keyHit = key;
        action = action;
    }
}

//set pitch, yaw, roll
float pitch = 0.0f;
float yaw = 0.0f;
float roll = 0.0f;

//set mouse callback
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


//server state for reconcilation
struct ServerState
{
    glm::vec3 pos;
    glm::vec3 dir;
    glm::vec3 bulletDir;
};

//local state for reconcilation
struct LocalState
{
    glm::vec3 pos;
    glm::vec3 dir;
    glm::vec3 bulletDir;
};

//reconcilation
void Reconcilate(LocalState localState, ServerState serverState, cMesh* player)
{
    // Assuming constant velocity for prediction
    glm::vec3 serverPos = serverState.pos;

    // Calculate the difference between the predicted position and the server position
    glm::vec3 diff = serverPos - localState.pos;

    // If the difference is significant, correct the player's position
    if (glm::length(diff) > 0.1f)
    {
        localState.pos = serverPos;
    }

    // Update the player's position
    player->drawPosition = localState.pos + localState.dir * 0.01f;
}

int main(void)
{
    //client or server
    std::cout << "Client? (y/n) " << std::endl;
    char input;
    std::cin >> input;

    if (input == 'y')
    {
#pragma region CLIENT  
        //client
        UdpClient client;
        glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 dir = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 bulletDir = glm::vec3(0.0f, 0.0f, 0.0f);

        //initialize client
        if (!client.Initialize())
        {
            return 1;
        }

        //send player data to server
        Buffer m_buffer;
        Player player;
        player.set_id(-1);

        //send the buffer to server
        std::string serializedPlayer = player.SerializeAsString();
        m_buffer.WriteUInt32LE(serializedPlayer.length());
        m_buffer.WriteString(serializedPlayer);
        client.SetBuffer(m_buffer);      
        //engine initialization
#pragma region ENGINE
        //initialize engine
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
        std::vector<PhysicsBody*> bodies;

        //initialize players
        for (size_t i = 0; i < 4; i++)
        {
            cMesh* player = engine.LoadMesh("Cube_1x1x1_xyz_n_rgba.ply", std::to_string(i));
            player->bUseDebugColours = true;
            player->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            players.push_back(player);

            PhysicsBody* body = engine.AddPhysicsBody(std::to_string(i));
            body->inverseMass = 0.0f;
            body->shapeType = PhysicsShapes::AABB;
            body->setShape(new PhysicsShapes::sAABB(glm::vec3(0), glm::vec3(0)));
            engine.physicsManager->GenerateAABBs(body, 1, 1);
            bodies.push_back(body);
        }

        //initialize bullet
        cMesh* bulletMesh = engine.LoadMesh("Cube_1x1x1_xyz_n_rgba.ply", "Bullet");
        bulletMesh->bUseDebugColours = true;
        bulletMesh->setUniformDrawScale(0.5f);  
        bulletMesh->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

        //initialize bullet physics
        PhysicsBody* body = engine.AddPhysicsBody("Bullet");
        body->inverseMass = 0.0f;
        body->shapeType = PhysicsShapes::AABB;
        body->setShape(new PhysicsShapes::sAABB(glm::vec3(0), glm::vec3(0)));
        engine.physicsManager->GenerateAABBs(body, 1, 1);

        float currTime = 0;
        camera->camControl = false;
        bool hasShot = false;
        float bulletCoolDown = 2.0f;
#pragma endregion

        //local and server state for reconcilation
        LocalState localState{};
        ServerState serverState[4]{};

        cMesh* playerMesh = new cMesh();
        while (!glfwWindowShouldClose(engine.window))
        {
            //update engine
            engine.Update();;

            //set camera
            camera->cameraEye = glm::vec3(0.0f, 0.0f, -15.0f);

            currTime += engine.deltaTime;

            //send data to server every 0.1 seconds
            if (currTime > 0.1f)
            {
                currTime = 0;
                client.SendDataToServer();
            }

            //get data from server
            Buffer recv = client.GetRecvBuffer();
            uint32_t len = recv.ReadUInt32LE();

            //update game scene
            if (len > 0)
            {
                //deserialize game scene
                GameScene gameScene;
                std::string str = recv.ReadString(len);
                bool success = gameScene.ParseFromString(str);
                if (success)
                {
                    //update players
                    for (size_t i = 0; i < gameScene.players_size(); i++)
                    {
                        //update player position
                        Player player = gameScene.players(i);                      
                        players[i]->drawPosition = glm::vec3(player.position().x(),
                                                            player.position().y(),
                                                            player.position().z());
                        Bullet bullet = player.bullet();
                        //update bullet position
                        if (bullet.playerindex() == i)
                        {
                            if (!bullet.hasbullet())
                            {
                                glm::vec3 bulletDirection = (glm::vec3(bullet.direction().x(),
                                                                    bullet.direction().y(),
                                                                    bullet.direction().z()));                             

                                bulletMesh->drawPosition += bulletDirection * 0.1f;
                                body->UpdateAABBs();
                            }
                            else
                            {
                                bulletMesh->drawPosition = players[i]->drawPosition;
                                body->UpdateAABBs();
                            }
                        }
                        //check for collisions
                        if (body->aabbPairs.size() > 0)
                        {
                            for (int j = 0; j < body->aabbPairs.size(); j++)
                            {
								cAABB* aabb = body->aabbPairs[j].first;
								cAABB* aabb2 = body->aabbPairs[j].second;
                                if (aabb->overlappingMeshName == "Bullet" && aabb2->overlappingMeshName == std::to_string(i))
                                {
									printf("Player %d hit\n", i);
								}
							}
						}
                    } 
                }
            }

            //client prediction
          //  playerMesh->drawPosition = localState.pos;

            //reconcilation
           /* for (size_t i = 0; i < 4; i++)
            {
				Reconcilate(localState, serverState[i], players[i]);
			}*/

            //set inputs
            if (glfwGetKey(engine.window, GLFW_KEY_W) == GLFW_PRESS)
            {
                dir = glm::vec3(0, 1, 0);
                pos += dir * 0.01f;
            }
            if (glfwGetKey(engine.window, GLFW_KEY_A) == GLFW_PRESS)
            {
                dir = glm::vec3(1, 0, 0);
                pos += dir * 0.01f;
            }
            if (glfwGetKey(engine.window, GLFW_KEY_S) == GLFW_PRESS)
            {
                dir = glm::vec3(0, -1, 0);
                pos += dir * 0.01f;
            }
            if (glfwGetKey(engine.window, GLFW_KEY_D) == GLFW_PRESS)
            {
                dir = glm::vec3(-1, 0, 0);
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

            localState.pos = pos;
            localState.dir = dir;

            if (hasShot)
            {
                bulletCoolDown -= engine.deltaTime;
                if (bulletCoolDown < 0.0f)
                {
					hasShot = false;
					bulletCoolDown = 2.0f;
				}
            }

            //set position vector
            Vector* position = new Vector();
            position->set_x(pos.x);
            position->set_y(pos.y);
            position->set_z(pos.z);

            //set direction vector
            Vector* direction = new Vector();
            direction->set_x(dir.x);
            direction->set_y(dir.y);
            direction->set_z(dir.z);
           
            //set bullet info
            Bullet* bullet = new Bullet();
            bullet->set_hasbullet(!hasShot);
            bullet->set_playerindex(-1);
            bullet->set_allocated_direction(direction);

            //set player info
            player.set_allocated_position(position);
            player.set_allocated_orientation(direction);
            player.set_allocated_bullet(bullet);

            //serialize player
            std::string serializedPlayer = player.SerializeAsString();

            //send player to server
            Buffer updateBuffer;
            updateBuffer.WriteUInt32LE(serializedPlayer.length());
            updateBuffer.WriteString(serializedPlayer);
           
            //set buffer
            client.SetBuffer(updateBuffer);
            
            player.release_bullet();
        }

        //shutdown engine
        engine.ShutDown();
#pragma endregion
    }
    else
    {
#pragma region SERVER
        //server
        std::vector<Player*> players;
        players.resize(4);

        //server initialization
        Buffer recvBuf(1024);
        UdpServer server;
        //   GameServer gameServer;
        if (!server.Initialize())
        {
            return 1;
        }

        //game scene initialization
        GameScene gameScene;

        //initialize players in game scene
        for (size_t i = 0; i < 4; i++)
        {
            Player* player = gameScene.add_players();
            player->set_id(i);
            Bullet bullet = player->bullet();
            bullet.set_hasbullet(false);
            bullet.set_playerindex(i);
        }

        //set game scene id
        gameScene.set_id(1);
        while (true)
        {
            //listen for clients
            server.Listen();
            //  gameServer.Update();

            if (server.clientIndex < 0) continue;

            //get data from client
            Buffer recvBuf = server.GetRecvBuffer();
            uint32_t len = recvBuf.ReadUInt32LE();

            if (len > 0)
            {
                //deserialize player
                Player player;
                std::string str = recvBuf.ReadString(len);
                bool success = player.ParseFromString(str);
                if (success)
                {
                    //update player
                    Player* updatePlayer = gameScene.mutable_players(server.clientIndex);
                    Vector* positionVector = new Vector;
                    positionVector->set_x(player.position().x());
                    positionVector->set_y(player.position().y());
                    positionVector->set_z(player.position().z());

                    updatePlayer->set_allocated_position(positionVector);
                    updatePlayer->set_id(server.clientIndex);

                    //update bullet
                    Bullet bullet = player.bullet();

                    Bullet* updateBullet = new Bullet();
                    if (!bullet.hasbullet())
                    {
						
						updateBullet->set_hasbullet(bullet.hasbullet());
						updateBullet->set_playerindex(server.clientIndex);
						Vector* directionVector = new Vector;
						directionVector->set_x(bullet.direction().x());
						directionVector->set_y(bullet.direction().y());
						directionVector->set_z(bullet.direction().z());
						updateBullet->set_allocated_direction(directionVector);
					}
                    else
                    {
						updateBullet->set_hasbullet(bullet.hasbullet());
						updateBullet->set_playerindex(server.clientIndex);
                        Vector* directionVector = new Vector;
                        directionVector->set_x(0);
                        directionVector->set_y(0);
                        directionVector->set_z(0);
                        updateBullet->set_allocated_direction(directionVector);
					}

                    //set bullet
                    updatePlayer->set_allocated_bullet(updateBullet);

                    //serialize game scene
                    std::string gameSerialized = gameScene.SerializeAsString();
                    Buffer sendBuf;
                    sendBuf.WriteUInt32LE(gameSerialized.length());
                    sendBuf.WriteString(gameSerialized);

                    //send game scene to client
                    server.SetSendBuffer(sendBuf);
                    updatePlayer->release_bullet();

                }
            }

            //send game scene to clients every 100ms
            Sleep(100);
        }
#pragma endregion
    }
}



