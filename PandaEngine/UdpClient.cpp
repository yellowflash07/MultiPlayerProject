#include "UdpClient.h"

UdpClient::UdpClient()
{
}

UdpClient::~UdpClient()
{
}

bool UdpClient::Initialize()
{
	// Initialize WinSock
	WSADATA wsaData;
	int result;

	// Set version 2.2 with MAKEWORD(2,2)
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		printf("WSAStartup failed with error %d\n", result);
		return false;
	}
	printf("WSAStartup successfully!\n");

	// Socket
	m_serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_serverSocket == INVALID_SOCKET)
	{
		printf("socket failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return false;
	}
	printf("socket created successfully!\n");

	return true;

}

void UdpClient::SendDataToServer()
{
	const int bufLen = 32;
	char buffer[bufLen];
	std::string buf = "Hello";
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8412);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	int addrLen = sizeof(addr);

	int result = sendto(m_serverSocket, buf.c_str(), buf.length(), 0, (SOCKADDR*)&addr, addrLen);
	if (result == SOCKET_ERROR) 
	{
		printf("send failed with error %d\n", WSAGetLastError());
		closesocket(m_serverSocket);
		WSACleanup();
		return;
	}

	result = recvfrom(m_serverSocket, buffer, bufLen, 0, (SOCKADDR*)&addr, &addrLen);
	if (result == SOCKET_ERROR) 
	{
		printf("recvfrom failed with error %d\n", WSAGetLastError());
		closesocket(m_serverSocket);
		WSACleanup();
		return;
	}

	printf("From: %s:%d: %s\n", inet_ntoa(addr.sin_addr), addr.sin_port, buffer);
}


