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
	// Delete the Map of Users
	delete m_pServerUsers;
	m_pServerUsers = 0;

	// Free the char arrays on the heap memory
	delete m_cReceiveData;
	m_cReceiveData = 0;
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

	// Create a map to hold users
	m_pServerUsers = new std::map < std::string, sockaddr_in>;

	// Create char array for storing received data
	m_cReceiveData = new char[sizeof(ClientToServer) + 1];

	// Binding was successful
	return true;
}

bool CServer::SendPacket(ServerToClient* _pSendPacket)
{
	std::map < std::string, sockaddr_in>::iterator iterCurrentUser = m_pServerUsers->begin();
	std::map < std::string, sockaddr_in>::iterator iterUsersEnd = m_pServerUsers->end();

	// Send to All current clients
	while (iterCurrentUser != iterUsersEnd)
	{
		ServerToClient SendPacket = *_pSendPacket;
		int iPacketSize = sizeof(SendPacket) + 1;

		// Reinterpret the Data Packet into a char* for sending
		m_cSendData = reinterpret_cast<char*>(&SendPacket);

		// Send the Data
		int iNumBytes = sendto(m_ServerSocket,
			m_cSendData,
			iPacketSize,
			0,
			reinterpret_cast<sockaddr*>(&iterCurrentUser->second),
			sizeof(iterCurrentUser->second));

		// Check to ensure the right number of bytes was sent
		if (iNumBytes != iPacketSize)
		{
			// Bytes did not match therefore an error occured
			return false;
		}

		iterCurrentUser++;
	}

	// Data Packet sending was successful
	return true;
}

bool CServer::SendPacket(std::string _strUserName, ServerToClient* _pSendPacket)
{
	// Find the Addr of the Client to send Packet to
	sockaddr_in clientAddr;
	if (_pSendPacket->eCommand == CREATEUSER_NAMEINUSE)
	{
		// Packet is for the Failed to join client due to username in use
		clientAddr = m_FailedClientAddr;
	}
	else if (_pSendPacket->eCommand == SERVER_CONNECTION_AVAILABLE)
	{
		clientAddr = m_ClientRequest;
	}
	else
	{
		// Packet it for a connected Client
		std::map < std::string, sockaddr_in>::iterator clientRecipient = m_pServerUsers->find(_strUserName);
		clientAddr = clientRecipient->second;	
	}

	// Delete users from the Network list if they were rejected for any reason
	if ((_pSendPacket->eCommand == CREATEUSER_SERVERFULL)
		|| (_pSendPacket->eCommand == NOT_HOST)
		|| (_pSendPacket->eCommand == SERVER_CONNECTION_AVAILABLE)
		|| (_pSendPacket->eCommand == USER_LEFT))
	{
		if ((std::string)(_pSendPacket->cUserName) == "Cal")
		{
			int i = 0; // TO DO
		}
		m_pServerUsers->erase(_strUserName);
	}

	ServerToClient PacketToSend = *_pSendPacket;
	int iPacketSize = sizeof(PacketToSend) + 1;

	// Reinterpret the Data Packet into a char* for sending
	m_cSendData = reinterpret_cast<char*>(&PacketToSend);

	// Send the Data
	int iNumBytes = sendto(m_ServerSocket,
		m_cSendData,
		iPacketSize,
		0,
		reinterpret_cast<sockaddr*>(&clientAddr),
		sizeof(clientAddr));

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

	// Packet contains a command
	if (_pReceivePacket->bCommand == true)
	{
		if (	(_pReceivePacket->eCommand == CREATEUSER)
			||	(_pReceivePacket->eCommand == QUERY_HOST))
		{
			std::pair<std::string, sockaddr_in> insertUser = { (std::string)(_pReceivePacket->cUserName), m_ClientAddr };
			std::pair<std::map<std::string, sockaddr_in>::iterator, bool> addedUser;
			addedUser = m_pServerUsers->insert(insertUser);

			// Keep a record of the client address for failed to insert clients
			if (addedUser.second == false)
			{
				m_FailedClientAddr = insertUser.second;
			}
		}
		else if (_pReceivePacket->eCommand == QUERY_CLIENT_CONNECTION)
		{
			m_ClientRequest = m_ClientAddr;
		}
	}
	else	// Packet was not a command
	{
		// Check if the packet arrived from a connected user
		std::string strUser = (std::string)(_pReceivePacket->cUserName);
		if (m_pServerUsers->find(strUser) == m_pServerUsers->end())
		{
			// Ignore packet - it did not come from a connected client
			return false;
		}
	}

	return true;
}