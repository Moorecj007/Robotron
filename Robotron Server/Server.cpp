/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Server.cpp
* Description : Server side of the network - responsible for sending and recieving messages to the Client
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Includes
#include "Server.h"

CServer::CServer()
{
}


CServer::~CServer()
{
}

bool CServer::Initialise()
{
	// Clear out the ClientAddr memory for use
	ZeroMemory(&m_ServerAddr, sizeof(m_ServerAddr));

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return false;
	}

	// Create a socket for the client
	m_ServerSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_ServerSocket == INVALID_SOCKET)
	{
		return false;
	}

	// Bind the socket to the address and port.
	m_ServerAddr.sin_family = AF_INET;
	m_ServerAddr.sin_port = htons(network::DEFAULT_SERVER_PORT);
	m_ServerAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	// Check if the current port number is available for binding
	if (bind(m_ServerSocket, reinterpret_cast<sockaddr*>(&m_ServerAddr), sizeof(m_ServerAddr)) == 0)
	{
		bool bOptionValue = true;
		int iOptionLength = sizeof(bOptionValue);
		int iResult = setsockopt(m_ServerSocket, SOL_SOCKET, SO_BROADCAST, (char*)&bOptionValue, iOptionLength);
		if (iResult == SOCKET_ERROR)
		{
			// The socket successfully bound but it was unable to broadcast
			closesocket(m_ServerSocket);
			return false;
		}
	}
	else
	{
		// Binding failed. Close socket and exit
		closesocket(m_ServerSocket);
		return false;
	}

	// Binding was successful
	return true;
}

bool CServer::SendPacket(ServerToClient* _pSendPacket)
{
	ServerToClient SendPacket = *_pSendPacket;
	int iPacketSize = sizeof(SendPacket) + 1;

	// Reinterpret the Data Packet into a char* for sending
	char* cSendData = new char[iPacketSize * sizeof(char)];
	cSendData = reinterpret_cast<char*>(&SendPacket);

	// Send the Data
	int iNumBytes = sendto(m_ServerSocket,
		cSendData,
		iPacketSize,
		0,
		reinterpret_cast<sockaddr*>(&m_ClientAddr),
		sizeof(m_ClientAddr));
	// Check to ensure the right number of bytes was sent
	if (iNumBytes != iPacketSize)
	{
		// Bytes did not match therefore an error occured
		return false;
	}

	// Data Packet sending was successful
	return true;
}

bool CServer::ReceivePacket(ClientToServer* _pReceivePacket)
{
	// Create some local variables
	int iSizeOfAddr = sizeof(m_ClientAddr);
	char* cReceiveData = new char[sizeof(ClientToServer) + 1];
	int iBytesReceived = sizeof(ClientToServer) + 1;

	// Time out Value
	struct timeval timeValue;
	timeValue.tv_sec = 1;
	timeValue.tv_usec = 0;
	setsockopt(m_ServerSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeValue, sizeof(timeValue));

	// Receive the Data
	int iNumBytesReceived = recvfrom(m_ServerSocket,
		cReceiveData,
		iBytesReceived,
		0,
		reinterpret_cast<sockaddr*>(&m_ClientAddr),
		&iSizeOfAddr);
	if (iNumBytesReceived < 0)
	{
		// Received invalid packet
		return false;
	}

	// Convert char* back into data Packet struct
	*_pReceivePacket = *(reinterpret_cast<ClientToServer*>(cReceiveData));
	return true;
}