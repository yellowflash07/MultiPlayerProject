#include "UdpServer.h"

UdpServer::UdpServer()
{
	m_buffer.bufferData.resize(bufSize);
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

	unsigned long nonblock = 1;
	result = ioctlsocket(m_listenSocket, FIONBIO, &nonblock);
	if (result == SOCKET_ERROR) 
	{
		printf("set socket to nonblocking failed with error %d\n", WSAGetLastError());
		closesocket(m_listenSocket);
		WSACleanup();
		return false;
	}

	printf("set socket to nonblocking was successful!\n");
	return true;
}

void UdpServer::Listen()
{
	// Read
	RecvDataFromClient();

	// Write
	SendDataToClient();
}

void UdpServer::SendDataToClient()
{
	if (m_ConnectedClients.size() == 0)
	{
		return;
	}
	if (m_buffer.bufferData.size() > 0)
	{
		for (ClientInfo info: m_ConnectedClients)
		{
			int result = sendto(m_listenSocket, (char*)&m_buffer.bufferData[0], bufSize, 
								0, 
								(SOCKADDR*)&info.addr,
								info.addrLen);
			if (result == SOCKET_ERROR)
			{
				printf("send failed with error %d\n", WSAGetLastError());
				closesocket(m_listenSocket);
				WSACleanup();
				return;
			}
		}
		
	}
}

void UdpServer::RecvDataFromClient()
{
	sockaddr_in addr;
	int addrLen = sizeof(addr);
//	Buffer m_recv;
	recvBuffer.Clear();
	recvBuffer.bufferData.resize(bufSize);
	int result = recvfrom(m_listenSocket, (char*)&recvBuffer.bufferData[0], bufSize, 0,
						(SOCKADDR*)&addr, &addrLen);
	if (result == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			// Not a real error, we expect this.
			// -1 is an error, 0 is disconnected, >0 is a message
			// WSA uses this as a flag to check if it is a real error
			return;
		}

		//client disconnected
		if (WSAGetLastError() == WSAECONNRESET)
		{
			printf("Client disconnected\n");

			// Remove the client
			for (int i = 0; i < m_ConnectedClients.size(); i++)
			{
				ClientInfo& client = m_ConnectedClients[i];
				if (client.addr.sin_addr.s_addr == addr.sin_addr.s_addr
					&& client.addr.sin_port == addr.sin_port)
				{
					m_ConnectedClients.erase(m_ConnectedClients.begin() + i);
					break;
				}
			}

			return;
		}

		printf("recvfrom failed with error %d\n", WSAGetLastError());
		closesocket(m_listenSocket);
		WSACleanup();
		return;
	}

	int clientId = -1;
	for (int i = 0; i < m_ConnectedClients.size(); i++)
	{
		ClientInfo& client = m_ConnectedClients[i];
		if (client.addr.sin_addr.s_addr == addr.sin_addr.s_addr
			&& client.addr.sin_port == addr.sin_port)
		{
			clientId = i;
			break;
		}
	}

	if (clientId == -1)
	{
		// Add the client
		ClientInfo newClient;
		newClient.addr = addr;
		newClient.addrLen = sizeof(addr);
		m_ConnectedClients.push_back(newClient);
		clientId = m_ConnectedClients.size() - 1;

		printf("New client connected: %s:%d\n", inet_ntoa(addr.sin_addr), addr.sin_port);
	}

	clientIndex = clientId;

}

