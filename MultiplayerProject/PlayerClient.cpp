#include "PlayerClient.h"

PlayerClient::PlayerClient()
{
	m_player = new Player();
}

PlayerClient::~PlayerClient()
{
}

void PlayerClient::SetPosition(glm::vec3 pos)
{
	Vector* vec = new Vector();
	vec->set_x(pos.x);
	vec->set_y(pos.y);
	vec->set_z(pos.z);

	m_player->clear_position();
	m_player->set_allocated_position(vec);
}

void PlayerClient::SetDirection(glm::vec3 dir)
{
	Vector* vec = new Vector();
	vec->set_x(dir.x);
	vec->set_y(dir.y);
	vec->set_z(dir.z);

	m_player->clear_direction();
	m_player->set_allocated_direction(vec);
}

void PlayerClient::SendDataToServer()
{
	m_buffer.Clear();
	std::string serializedPlayer = m_player->SerializeAsString();
	m_buffer.WriteUInt32LE(serializedPlayer.length());
	m_buffer.WriteString(serializedPlayer);

	m_client->SetBuffer(m_buffer);	

	//m_client.SendDataToServer();
}

void PlayerClient::SetClient(UdpClient &client)
{
	client.SetBuffer(m_buffer);
	m_client = &client;
}
