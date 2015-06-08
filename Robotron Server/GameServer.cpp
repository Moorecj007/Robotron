/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : GameServer.cpp
* Description : Runs the main game functions for the Server - The game manager
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Includes
#include "GameServer.h"

// Static Variables
CGameServer* CGameServer::s_pGame = 0;
CMySemaphore* CGameServer::m_pServerMutex = new CMySemaphore(1);

CGameServer::CGameServer()
{
}

CGameServer::~CGameServer()
{
	m_bNetworkOnline = false;
	m_ReceiveThread.join();

	// Delete Client Network
	delete m_pServerNetwork;
	m_pServerNetwork = 0;

	// Delete the Networking Packets
	delete m_pClientToServer;
	m_pClientToServer = 0;
	delete m_pServerToClient;
	m_pServerToClient = 0;
	delete m_pPacketToProcess;
	m_pPacketToProcess = 0;

	// Delete the ServerMutex
	delete m_pServerMutex;
	m_pServerMutex = 0;

	// Delete the WorkQueue
	delete m_pWorkQueue;
	m_pWorkQueue = 0;

	// Delete the container of Users from the Heap Memory
	delete m_pCurrentUsers;
	m_pCurrentUsers = 0;
}

CGameServer& CGameServer::GetInstance()
{
	if (s_pGame == 0)
	{
		s_pGame = new CGameServer();
	}
	return (*s_pGame);
}

void CGameServer::DestroyInstance()
{
	delete s_pGame;
	s_pGame = 0;
}

bool CGameServer::Initialise(HWND _hWnd, int _iScreenWidth, int _iScreenHeight, LPWSTR* _wstrArgs)
{
	// Populate window variables
	m_hWnd = _hWnd;
	m_iScreenWidth = _iScreenWidth;
	m_iScreenHeight = _iScreenHeight;

	
	// Get the Host and server name from the command arguments
	wchar_t* wstrHostName = _wstrArgs[1];
	wchar_t* wstrServerName = _wstrArgs[2];

	if (wstrHostName == 0 || wstrServerName == 0)
	{
		// Invalid server creation
		return false;
	}

	// Save the Host Name
	m_strHostUser = WideStringToString(wstrHostName);
	m_bRepliedToHost = false;

	// Save the Server Name
	m_strServerName = WideStringToString(wstrServerName);
	m_bRepliedToHost = false;

	// Create Data Packets
	m_pClientToServer = new ClientToServer();
	m_pPacketToProcess = new ClientToServer();
	m_pServerToClient = new ServerToClient();

	// Create and Initialise the Server-side Network
	m_pServerNetwork = new CServer();
	VALIDATE(m_pServerNetwork->Initialise());
	m_bNetworkOnline = true;

	// Create a thread to receive data from the Server
	m_ReceiveThread = std::thread(&CGameServer::ReceiveDataFromNetwork, (this), m_pClientToServer);

	// Create the WorkQueue
	m_pWorkQueue = new std::queue<ClientToServer>;

	// Create a vector to store current Users
	m_pCurrentUsers = new std::map < std::string, bool>;
	InsertUser(m_strHostUser);

	// Set the Initial ServerState
	m_eServerState = STATE_LOBBY;

	return true;
}

bool CGameServer::ExecuteOneFrame()
{
	if (m_bNetworkOnline == false)
	{
		return false;
	}

	Process();

	return true;

}

void CGameServer::Process()
{
	while (m_pWorkQueue->empty() == false)
	{
		m_pServerMutex->Wait();
		*m_pPacketToProcess = m_pWorkQueue->front();
		m_pWorkQueue->pop();
		m_pServerMutex->Signal();

		// Process the Pulled Packet
		ProcessPacket();
	}

	// Only process if in the Lobby Screen
	if (m_eServerState == STATE_LOBBY)
	{
		if (m_pCurrentUsers->size() > 0)
		{
			std::map<std::string, bool>::iterator iterCurrentUser = m_pCurrentUsers->begin();
			bool bAllReady = true;

			// Check if all users are ready/ alive
			while (iterCurrentUser != m_pCurrentUsers->end())
			{
				if (iterCurrentUser->second == false)
				{
					// One or more is not ready therefore do not start game
					bAllReady = false;
					break;
				}

				iterCurrentUser++;
			}

			if (bAllReady == true)
			{
				// All Participants are ready to go -> start the game
				m_eServerState = STATE_GAMEPLAY;

				CreateCommandPacket(START_GAME);
				m_pServerNetwork->SendPacket(m_pServerToClient);
			}
		}
	}
}

void CGameServer::ProcessPacket()
{
	if (m_pPacketToProcess->bCommand == true)
	{
		//Process a command code

		eNetworkCommand eProcessCommand = m_pPacketToProcess->eCommand;

		// Terminate Server
		if (eProcessCommand == TERMINATE_SERVER)
		{
			// Accept Terminate Command only if the Host client sent it
			if ((std::string)(m_pPacketToProcess->cUserName) == m_strHostUser)
			{
				// Send a message to all connected users that the server is being terminated except the host
				std::map<std::string, bool>::iterator iterUsersCurrent = m_pCurrentUsers->begin();
				while (iterUsersCurrent != m_pCurrentUsers->end())
				{
					// Don't sent the terminated message back to the host
					if (iterUsersCurrent->first != m_strHostUser)
					{
						CreateCommandPacket(TERMINATE_SERVER);
						m_pServerNetwork->SendPacket(m_pServerToClient);
					}

					iterUsersCurrent++;
				}
				
				// Turn the network offline.
				// This stop the receivethread and allow it to join and then terminate the server application
				m_bNetworkOnline = false;
			}
			return;
		}
		// Host is looking for the Server they just created
		else if (eProcessCommand == QUERY_HOST)
		{
			std::string strCheckHost = (std::string)(m_pPacketToProcess->cUserName);
			std::string strCheckServer = (std::string)(m_pPacketToProcess->cAdditionalMessage);

			// Ensure the sender of this command is the correct host for only they would know the
			// host and server names created via the command line
			if (	strCheckHost == m_strHostUser 
				&&	strCheckServer == m_strServerName)
			{
				// If for any reason another Host creates a server with the same names
				// This server will only reply to very first message with this command
				if (m_bRepliedToHost == false)
				{
					CreateCommandPacket(HOST_SERVER);
					m_pServerNetwork->SendPacket(strCheckHost, m_pServerToClient);
					m_bRepliedToHost = true;
				}
			}
			// Else the query is for another server
			else
			{
				CreateCommandPacket(NOT_HOST);
				m_pServerNetwork->SendPacket(strCheckHost, m_pServerToClient);
			}
			return;
		}
		// Broadcast message to determine what servers if any are online
		else if (eProcessCommand == QUERY_CLIENT_CONNECTION)
		{
			// return message saying this server is available
			CreateCommandPacket(SERVER_CONNECTION_AVAILABLE);
			if ((std::string)(m_pPacketToProcess->cUserName) == "Cal")
			{
				int i = 0; // TO DO
			}
			m_pServerNetwork->SendPacket(m_pPacketToProcess->cUserName, m_pServerToClient);

			return;
		}
		// Tries to Create a new User on the Server
		else if (eProcessCommand == CREATEUSER)
		{
			// Check if the server already has the maximum allowed users
			if (m_pCurrentUsers->size() < network::MAX_CLIENTS)
			{
				// Try to insert the new user into the map of users for this server
				std::string strUser = (std::string)(m_pPacketToProcess->cUserName);
				bool bAdded = InsertUser(strUser);

				if (bAdded == true)
				{
					// If the User was successfully added send back that they were accepted
					CreateCommandPacket(CREATEUSER_ACCEPTED);
					m_pServerNetwork->SendPacket(strUser, m_pServerToClient);

					// Send to all current users that a user has joined with their details
					CreateCommandPacket(USER_JOINED, (std::string)m_pPacketToProcess->cUserName);
					m_pServerNetwork->SendPacket(m_pServerToClient);

					// Send the list of other current users to the new user
					std::map<std::string, bool>::iterator iterUsersCurrent = m_pCurrentUsers->begin();
					while (iterUsersCurrent != m_pCurrentUsers->end())
					{
						// don't send their own username back to them
						if (iterUsersCurrent->first != strUser)
						{
							CreateCommandPacket(USER_JOINED, iterUsersCurrent->first);
							m_pServerNetwork->SendPacket(m_pPacketToProcess->cUserName, m_pServerToClient);
						}

						iterUsersCurrent++;
					}

					// Send the new user the name of the Host user
					CreateCommandPacket(YOUR_HOST, m_strHostUser);
					m_pServerNetwork->SendPacket(m_pPacketToProcess->cUserName, m_pServerToClient);
				}
				else
				{
					// If the Insert failed then the username was already in use
					CreateCommandPacket(CREATEUSER_NAMEINUSE);
					m_pServerNetwork->SendPacket(strUser, m_pServerToClient);
				}
			}
			else
			{
				// reply to the user that the server is currently full
				CreateCommandPacket(CREATEUSER_SERVERFULL);
				m_pServerNetwork->SendPacket(m_strHostUser, m_pServerToClient);
			}

			return;
		}
		// User is Leaving the Server
		else if (eProcessCommand == LEAVE_SERVER)
		{
			m_pCurrentUsers->erase(m_pPacketToProcess->cUserName);

			CreateCommandPacket(USER_LEFT, m_pPacketToProcess->cUserName);
			m_pServerNetwork->SendPacket(m_pServerToClient);

			return;
		}
		// Set the ready state or alive state of a user
		else if (eProcessCommand == ALIVE_SET)
		{
			// Determine which user sent this message
			std::map<std::string, bool>::iterator User = m_pCurrentUsers->find(m_pPacketToProcess->cUserName);

			// Update the state based on the additional message data
			bool bAliveness = ((std::string)m_pPacketToProcess->cAdditionalMessage == "true") ? true : false;
			User->second = bAliveness;

			// Send to all users the new State for that user
			CreateCommandPacket(ALIVE_SET, m_pPacketToProcess->cUserName, m_pPacketToProcess->cAdditionalMessage);
			m_pServerNetwork->SendPacket(m_pServerToClient);

			return;
		}
	}
	else
	{
		// Process as normal client input
	}
}

bool CGameServer::CreateDataPacket()
{
	// TO DO - Create actual Data

	return true;
}

bool CGameServer::CreateCommandPacket(eNetworkCommand _eCommand)
{
	// Clear out the memory of the Data Packet to stop old data being sent
	//m_pServerToClient = {};
	//ZeroMemory(&m_pServerToClient, sizeof(m_pServerToClient));

	// Change the Packet Data to contain a Command
	m_pServerToClient->bCommand = true;

	// Add the Command to The Packet structure
	m_pServerToClient->eCommand = _eCommand;

	// Add the Server as the username to the Packet structure
	if (!(StringToStruct(m_strServerName.c_str(), m_pServerToClient->cServerName)))
	{
		return false;	// Data invalid - Data Packet failed to create
	}

	return true;
}

bool CGameServer::CreateCommandPacket(eNetworkCommand _eCommand, std::string _strUserName)
{
	// Create a general command packet
	CreateCommandPacket(_eCommand);

	// Add the additional meesage
	if (!(StringToStruct(_strUserName.c_str(), m_pServerToClient->cUserName)))
	{
		return false;	// Data invalid - Data Packet failed to create
	}

	return true;
}

bool CGameServer::CreateCommandPacket(eNetworkCommand _eCommand, std::string _strUserName, std::string _strMessage)
{
	// Create a general command packet
	CreateCommandPacket(_eCommand, _strUserName);

	// Add the additional meesage
	if (!(StringToStruct(_strMessage.c_str(), m_pServerToClient->cAdditionalMessage)))
	{
		return false;	// Data invalid - Data Packet failed to create
	}

	return true;
}

bool CGameServer::StringToStruct(const char* _pcData, char* _pcStruct)
{
	unsigned int iMaxLength;
	// Determine the maximum amount of characters that can be copied
	// based on the memory address that it is going to be writing to
	if (*(&m_pServerToClient->cServerName) == *(&_pcStruct))
	{
		iMaxLength = network::MAX_SERVERNAME_LENGTH;
	}
	else if (*(&m_pServerToClient->cUserName) == *(&_pcStruct))
	{
		iMaxLength = network::MAX_USERNAME_LENGTH;
	}
	else if (*(&m_pServerToClient->cAdditionalMessage) == *(&_pcStruct))
	{
		iMaxLength = network::MAX_CHAR_LENGTH;
	}

	// Ensure no buffer overrun will occur when copying directly to memory
	if ((strlen(_pcData) + 1) <= (iMaxLength))
	{
		// Copy the characters into the struct
		strcpy_s(_pcStruct, (strlen(_pcData) + 1), _pcData);
	}
	else
	{
		// char* is too long, buffer overrun would occur so failed operation
		return false;
	}
	return true;
}

void CGameServer::ReceiveDataFromNetwork(ClientToServer* _pReceiveData)
{
	while (m_bNetworkOnline)
	{
		if (m_pServerNetwork->ReceivePacket(_pReceiveData))
		{
			// Add the Received Packet to the Work Queue
			m_pServerMutex->Wait();
			m_pWorkQueue->push(*_pReceiveData);
			m_pServerMutex->Signal();
		}
	}
}

std::string CGameServer::WideStringToString(wchar_t* _wstr)
{
	// Convert the Wide String to a standard string
	size_t lengthWstr = (wcslen(_wstr) + 1);
	size_t convertedCount = 0;
	char* cConversion = new char[lengthWstr * 2];
	wcstombs_s(&convertedCount, cConversion, lengthWstr, _wstr, _TRUNCATE);

	std::string strConverted = (std::string)(cConversion);

	// Delete the created memory
	delete cConversion;
	cConversion = 0;

	// Return the converted string
	return strConverted;
}

bool CGameServer::InsertUser(std::string _strUser)
{
	std::pair<std::string, bool> pairUser(_strUser, false);

	std::pair<std::map<std::string, bool>::iterator, bool> pairReturn;
	pairReturn = m_pCurrentUsers->insert(pairUser);

	return pairReturn.second;
}
