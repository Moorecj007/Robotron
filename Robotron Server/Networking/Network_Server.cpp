/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Network_Server.cpp
* Description : Server side of the network - responsible for sending and recieving messages to the Client
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Includes
#include "Network_Server.h"

CNetwork_Server::CNetwork_Server()
{
}

CNetwork_Server::~CNetwork_Server()
{
	// Free the char arrays on the heap memory
	delete m_cReceiveData;
	m_cReceiveData = 0;
}

bool CNetwork_Server::Initialise()
{
	// Clear out the ClientAddr memory for use
	ZeroMemory(&m_ServerAddr, sizeof(m_ServerAddr));

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return false;
	}

	// Loop through a range of potential ports until one has successfully bound
	for (unsigned int iPort = network::DEFAULT_SERVER_PORT;
		iPort <= network::MAX_SERVER_PORT;
		iPort++)
	{
		// Create a socket for the client
		m_ServerSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (m_ServerSocket == INVALID_SOCKET)
		{
			return false;
		}

		// Bind the socket to the address and port.
		m_ServerAddr.sin_family = AF_INET;
		m_ServerAddr.sin_port = htons(iPort);
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

			// Socket was correctly bound. No need to check another port number
			break;
		}
		
		// Binding failed. Close socket
		closesocket(m_ServerSocket);

		if (iPort == network::MAX_SERVER_PORT)
		{
			// No ports within designated range worked. Unable to initialise
			return false;
		}	
	}

	// Create char array for storing received data
	m_cReceiveData = new char[sizeof(ClientToServer) + 1];

	// Binding was successful
	return true;
}

bool CNetwork_Server::SendPacket(sockaddr_in _clientAddr, ServerToClient* _pSendPacket)
{
	ServerToClient PacketToSend = *_pSendPacket;
	int iPacketSize = sizeof(PacketToSend) + 1;

	// Reinterpret the Data Packet into a char* for sending
	m_cSendData = reinterpret_cast<char*>(&PacketToSend);

	// Send the Data
	int iNumBytes = sendto(m_ServerSocket,
		m_cSendData,
		iPacketSize,
		0,
		reinterpret_cast<sockaddr*>(&_clientAddr),
		sizeof(_clientAddr));

	// Check to ensure the right number of bytes was sent
	if (iNumBytes != iPacketSize)
	{
		// Bytes did not match therefore an error occured
		return false;
	}

	// Data Packet sending was successful
	return true;
}

bool CNetwork_Server::ReceivePacket(ClientToServer* _pReceivePacket)
{
	// Create some local variables
	int iSizeOfAddr = sizeof(m_ClientAddr);
	int iBytesReceived = sizeof(ClientToServer) + 1;

	// Time out Value
	struct timeval timeValue;
	timeValue.tv_sec = 1;
	timeValue.tv_usec = 0;
	setsockopt(m_ServerSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeValue, sizeof(timeValue));

	// Receive the Data
	int iNumBytesReceived = recvfrom(m_ServerSocket,
		m_cReceiveData,
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
	*_pReceivePacket = *(reinterpret_cast<ClientToServer*>(m_cReceiveData));

	_pReceivePacket->ClientAddr = m_ClientAddr;

	return true;
}