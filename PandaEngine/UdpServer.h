#pragma once

// WinSock2 Windows Sockets
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <conio.h>
#include <vector>
#include <string>
#include "Buffer.h"

// Need to link Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

// First, make it work (messy), then organize

#define DEFAULT_PORT 8412

struct ClientInfo
{
	sockaddr_in addr;
	int addrLen;
};

class UdpServer
{
public:
	UdpServer();
	~UdpServer();

	bool Initialize();
	void Listen();

	Buffer GetRecvBuffer() { return recvBuffer; }
	int NumClients() { return m_ConnectedClients.size(); }
	void SetSendBuffer(Buffer buffer) 
	{ 
		m_buffer.Clear();
		m_buffer = buffer; 
	}
	void SendDataToClient();
	int clientIndex = 0;
private:
	bool m_initialized;
	SOCKET m_listenSocket;
	std::vector<ClientInfo> m_ConnectedClients;
	Buffer recvBuffer;
	void RecvDataFromClient();
	Buffer m_buffer;
	int bufSize = 10240;
};

