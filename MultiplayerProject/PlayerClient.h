#pragma once
#include <glm/vec3.hpp>
#include "../Extern/bin/multiplayer.pb.h"
#include <UdpClient.h>

class PlayerClient
{
public:
	PlayerClient();
	~PlayerClient();

	void SetPosition(glm::vec3 pos);
	void SetDirection(glm::vec3 dir);
	void SendDataToServer();
	void SetClient(UdpClient &client);
private:
	glm::vec3 m_position;
	float speed = 0.1f;
	Player* m_player;
	UdpClient* m_client;
	Buffer m_buffer;
};


