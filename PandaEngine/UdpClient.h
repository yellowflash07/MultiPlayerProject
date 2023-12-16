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

#include <string>

// Need to link Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

// First, make it work (messy), then organize

#define DEFAULT_PORT 8412

class UdpClient
{
public:
	UdpClient();
	~UdpClient();

	bool Initialize();
	void SendDataToServer();

private:
	SOCKET m_serverSocket;
};
