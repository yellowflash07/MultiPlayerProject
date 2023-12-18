#include "UdpClient.h"
#include "../Extern/proto/multiplayer.pb.h"

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


	unsigned long nonblock = 1;
	result = ioctlsocket(m_serverSocket, FIONBIO, &nonblock);
	if (result == SOCKET_ERROR)
	{
		printf("set nonblocking failed with error %d\n", result);
		return false;
	}
	printf("set nonblocking successfully!\n");

	addr.sin_family = AF_INET;
	addr.sin_port = htons(8412);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	return true;

}

void UdpClient::SendDataToServer()
{
	if (hasData)
	{
		int result = sendto(m_serverSocket, (const char*)&m_buffer.bufferData[0], bufSize, 0, (SOCKADDR*)&addr, addrLen);
		if (result == SOCKET_ERROR)
		{
			printf("send failed with error %d\n", WSAGetLastError());
			closesocket(m_serverSocket);
			WSACleanup();
			return;
		}

		hasData = false;
	}

	m_recvBuffer.bufferData.resize(bufSize);
	int result = recvfrom(m_serverSocket, (char*)&m_recvBuffer.bufferData[0], bufSize, 0, (SOCKADDR*)&addr, &addrLen);
	if (result == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			// Not a real error, we expect this.--------
			// -1 is an error, 0 is disconnected, >0 is a message
			// WSA uses this as a flag to check if it is a real error
			return;
		}
		printf("recvfrom failed with error %d\n", WSAGetLastError());
		closesocket(m_serverSocket);
		WSACleanup();
		return;
	}
}




