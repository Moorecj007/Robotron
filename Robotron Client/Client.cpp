/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Client.cpp
* Description : Client side of the network - responsible for sending and recieving messages to the server
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Includes
#include "Client.h"


CClient::CClient()
{
}


CClient::~CClient()
{
}

bool CClient::Initialise()
{
	// Clear out the ClientAddr memory for use
	ZeroMemory(&m_ClientAddr, sizeof(m_ClientAddr));

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return false;
	}

	// Loop through a range of potential ports until one has successfully bound
	for	(	unsigned int iPort = network::DEFAULT_CLIENT_PORT;
			iPort <= network::MAX_CLIENT_PORT;
			iPort++)
	{
		// Create a socket for the client
		m_ClientSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (m_ClientSocket == INVALID_SOCKET)
		{
			return false;
		}

		// Bind the socket to the address and port.
		m_ClientAddr.sin_family = AF_INET;
		m_ClientAddr.sin_port = htons(iPort);
		m_ClientAddr.sin_addr.S_un.S_addr = INADDR_ANY;

		// Check if the current port number is available for binding
		if (bind(m_ClientSocket, reinterpret_cast<sockaddr*>(&m_ClientAddr), sizeof(m_ClientAddr)) == 0)
		{
			bool bOptionValue = true;
			int iOptionLength = sizeof(bOptionValue);
			int iResult = setsockopt(m_ClientSocket, SOL_SOCKET, SO_BROADCAST, (char*)&bOptionValue, iOptionLength);
			if (iResult == SOCKET_ERROR)
			{
				// The socket successfully bound but it was unable to broadcast
				closesocket(m_ClientSocket);
				continue;
			}

			// Socket was correctly bound. No need to check another port number
			break;
		}

		// Binding failed. close socket and try again
		closesocket(m_ClientSocket);

		if (iPort == network::MAX_CLIENT_PORT)
		{
			// No ports within designated range worked. Unable to initialise
			return false;
		}
	}

	// Clear out the ServerAddr memory for use
	ZeroMemory(&m_ServerAddr, sizeof(m_ServerAddr));

	m_ServerAddr.sin_family = AF_INET;
	m_ServerAddr.sin_port = htons(network::DEFAULT_SERVER_PORT);
	inet_pton(AF_INET, network::cUDPAddr, &m_ServerAddr.sin_addr);

	// Binding was successful
	return true;
}

bool CClient::SendPacket(ClientToServer* _pSendPacket)
{
	ClientToServer SendPacket = *_pSendPacket;
	int iPacketSize = sizeof(SendPacket) + 1;

	// Reinterpret the Data Packet into a char* for sending
	char* cSendData = new char[iPacketSize * sizeof(char)];
	cSendData = reinterpret_cast<char*>(&SendPacket);

	// Send the Data
	int iNumBytes = sendto(	m_ClientSocket,
							cSendData,
							iPacketSize,
							0,
							reinterpret_cast<sockaddr*>(&m_ServerAddr),
							sizeof(m_ServerAddr) );
	// Check to ensure the right number of bytes was sent
	if (iNumBytes != iPacketSize)
	{
		// Bytes did not match therefore an error occured
		return false;
	}

	// Data Packet sending was successful
	return true;
}

bool CClient::ReceivePacket(ServerToClient* _pReceivePacket)
{
	// Create some local variables
	int iSizeOfAddr = sizeof(m_ServerAddr);
	char* cReceiveData = new char[sizeof(ServerToClient) + 1];
	int iBytesReceived = sizeof(ServerToClient) + 1;

	// Receive the Data
	int iNumBytesReceived = recvfrom(m_ClientSocket,
		cReceiveData,
		iBytesReceived,
		0,
		reinterpret_cast<sockaddr*>(&m_ServerAddr),
		&iSizeOfAddr);
	if (iNumBytesReceived < 0)
	{
		// Received invalid packet
		return false;
	}

	// Convert char* back into data Packet struct
	*_pReceivePacket = *(reinterpret_cast<ServerToClient*>(cReceiveData));
	return true;
}