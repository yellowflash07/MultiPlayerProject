#include "UdpServer.h"

UdpServer::UdpServer()
{
}

UdpServer::~UdpServer()
{
}

bool UdpServer::Initialize()
{
	// Initialize WinSock
	WSADATA wsaData;
	int result;

	// Set version 2.2 with MAKEWORD(2,2)
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) 
	{
		printf("WSAStartup failed with error %d\n", result);
		return false;
	}
	printf("WSAStartup successfully!\n");



	// Socket
	m_listenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_listenSocket == INVALID_SOCKET)
	{
		printf("socket failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return false;
	}
	printf("socket created successfully!\n");

	// using sockaddr_in
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8412);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// Bind 
	result = bind(m_listenSocket, (SOCKADDR*)&addr, sizeof(addr));
	if (result == SOCKET_ERROR) {
		printf("bind failed with error %d\n", WSAGetLastError());
		closesocket(m_listenSocket);
		WSACleanup();
		return false;
	}

	printf("bind was successful!\n");
	return true;
}

void UdpServer::Listen()
{
	// Read
	const int bufLen = 32;
	char buffer[bufLen];
	int result = recvfrom(m_listenSocket, buffer, bufLen, 0, (SOCKADDR*)&clientInfo, &clientInfoLength);
	if (result == SOCKET_ERROR) {
		printf("recvfrom failed with error %d\n", WSAGetLastError());
		closesocket(m_listenSocket);
		WSACleanup();
		return;
	}

	printf("From: %s:%d: %s\n", inet_ntoa(clientInfo.sin_addr), clientInfo.sin_port, buffer);

	// Write
	result = sendto(m_listenSocket, buffer, result, 0, (SOCKADDR*)&clientInfo, clientInfoLength);
	if (result == SOCKET_ERROR) {
		printf("send failed with error %d\n", WSAGetLastError());
		closesocket(m_listenSocket);
		WSACleanup();
		return;
	}
}

