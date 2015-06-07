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
	// Free the char arrays on the heap memory
	delete m_cReceiveData;
	m_cReceiveData = 0;

	// Delete the available servers list
	delete m_pAvailableServers;
	m_pAvailableServers = 0;
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
	ZeroMemory(&m_ReceivedAddr, sizeof(m_ReceivedAddr));

	// Create char array for storing received data
	m_cReceiveData = new char[sizeof(ServerToClient) + 1];

	m_pAvailableServers = new std::map<std::string, sockaddr_in>;

	// Binding was successful
	return true;
}

bool CClient::SendPacket(ClientToServer* _pSendPacket)
{
	ClientToServer SendPacket = *_pSendPacket;
	int iPacketSize = sizeof(SendPacket) + 1;

	// Reinterpret the Data Packet into a char* for sending
	m_cSendData = reinterpret_cast<char*>(&SendPacket);

	// Send the Data
	int iNumBytes = sendto(	m_ClientSocket,
							m_cSendData,
							iPacketSize,
							0,
							reinterpret_cast<sockaddr*>(&m_ServerAddr),
							sizeof(m_ServerAddr));


	// Check to ensure the right number of bytes was sent
	if (iNumBytes != iPacketSize)
	{
		// Bytes did not match therefore an error occured
		return false;
	}

	// Data Packet sending was successful
	return true;
}

bool CClient::BroadcastToServers(ClientToServer* _pSendPacket)
{
	ClientToServer SendPacket = *_pSendPacket;
	int iPacketSize = sizeof(SendPacket) + 1;

	// Structure to hold the potential server addresses
	sockaddr_in ServerAddr;
	ServerAddr.sin_family = AF_INET;
	inet_pton(AF_INET, network::cUDPAddr, &ServerAddr.sin_addr);

	// Reinterpret the Data Packet into a char* for sending
	m_cSendData = reinterpret_cast<char*>(&SendPacket);

	// Send to all potential server ports
	for (int iServerPort = network::DEFAULT_SERVER_PORT;
		iServerPort <= network::MAX_SERVER_PORT;
		iServerPort++)
	{
		ServerAddr.sin_port = htons(iServerPort);

		// Send the Data
		int iNumBytes = sendto(m_ClientSocket,
			m_cSendData,
			iPacketSize,
			0,
			reinterpret_cast<sockaddr*>(&ServerAddr),
			sizeof(ServerAddr));

		// Check to ensure the right number of bytes was sent
		if (iNumBytes != iPacketSize)
		{
			// Bytes did not match therefore an error occured
			return false;
		}
	}
	// Data Packet sending was successful
	return true;
}

bool CClient::ReceivePacket(ServerToClient* _pReceivePacket)
{
	// Create some local variables
	sockaddr_in receivedSockAddr;
	int iSizeOfAddr = sizeof(receivedSockAddr);
	int iBytesReceived = sizeof(ServerToClient) + 1;
	

	// Time out Value
	struct timeval timeValue;
	timeValue.tv_sec = 1;
	timeValue.tv_usec = 0;
	setsockopt(m_ClientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeValue, sizeof(timeValue));

	// Receive the Data
	int iNumBytesReceived = recvfrom(m_ClientSocket,
		m_cReceiveData,
		iBytesReceived,
		0,
		reinterpret_cast<sockaddr*>(&receivedSockAddr),
		&iSizeOfAddr);
	if (iNumBytesReceived < 0)
	{
		// Received invalid packet
		return false;
	}

	// Convert char* back into data Packet struct
	*_pReceivePacket = *(reinterpret_cast<ServerToClient*>(m_cReceiveData));

	if (_pReceivePacket->bCommand == true)
	{
		eNetworkCommand eCommand = _pReceivePacket->eCommand;
		if (eCommand == HOST_SERVER)
		{
			m_ServerAddr = receivedSockAddr;
		}
		else if (eCommand == SERVER_CONNECTION_AVAILABLE)
		{
			std::pair<std::string, sockaddr_in> pairServer(_pReceivePacket->cUserName, receivedSockAddr);
			m_pAvailableServers->insert(pairServer);
		}
		
	}
	return true;
}

void CClient::SelectServer(std::string _strServerName)
{
	std::map<std::string, sockaddr_in>::iterator iterSelectedServer;
	iterSelectedServer = m_pAvailableServers->find(_strServerName);

	if (iterSelectedServer != m_pAvailableServers->end())
	{
		m_ServerAddr = iterSelectedServer->second;
	}
}