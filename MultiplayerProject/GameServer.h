#pragma once

#include <UdpServer.h>
#include "PlayerClient.h"
#include "../Extern/proto/multiplayer.pb.h"

class GameServer
{
public:
	GameServer();
	~GameServer();


	void Update();
	void SendDataToClients();
	void SetServer(UdpServer &server);
	void AddPlayer(PlayerClient &client);
	GameScene* m_scene;

private:
	Buffer buffer;
	UdpServer* m_server;
	Buffer m_buffer;
	std::map<int, Player*> m_players;
	int connectedPlayers = 0;
};

