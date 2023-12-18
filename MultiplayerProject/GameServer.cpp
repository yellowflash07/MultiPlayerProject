#include "GameServer.h"

GameServer::GameServer()
{
	m_scene = new GameScene();	
	m_scene->set_id(1);
}

GameServer::~GameServer()
{
}

void GameServer::Update()
{
	if (m_server->NumClients() > connectedPlayers)
	{
		connectedPlayers++;
	//	Player* player = m_scene->add_players();
	 //   m_players.insert(std::pair<int, Player*>(connectedPlayers, player));
	}

	Buffer recvBuf = m_server->GetRecvBuffer();
	uint32_t len = recvBuf.ReadUInt32LE();

	if (len > 0)
	{
		std::string str = recvBuf.ReadString(len);
		Player player;
		bool success = player.ParseFromString(str);
		if (success)
		{
			if (player.id() < 0)
			{
				//new player
				Player newPlayer;
				newPlayer.set_id(connectedPlayers);
				std::string serialiezedString = newPlayer.SerializeAsString();

				Buffer newbuf;
				newbuf.WriteUInt32LE(serialiezedString.length());
				newbuf.WriteString(serialiezedString);

				m_server->SetSendBuffer(newbuf);
			}
			//printf("Player id %d\n", player.id());
		}

	}

	for (int i = 0; i < m_players.size(); i++)
	{
		buffer.Clear();
		std::string serializedGameScene = m_scene->SerializeAsString();
		buffer.WriteUInt32LE(serializedGameScene.length());
		buffer.WriteString(serializedGameScene);

	 //   m_server->SetSendBuffer(buffer);
	}
}

void GameServer::SendDataToClients()
{

}

void GameServer::SetServer(UdpServer& server)
{
	m_server = &server;
}

void GameServer::AddPlayer(PlayerClient& client)
{
}
