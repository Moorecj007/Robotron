/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Hub_Server.cpp
* Description : Runs the main game functions for the Server - The game manager
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Includes
#include "Hub_Server.h"

// Static Variables
CHub_Server* CHub_Server::s_pGame = 0;
CMySemaphore* CHub_Server::m_pMutexServer = new CMySemaphore(1);

CHub_Server::CHub_Server()
{
}

CHub_Server::~CHub_Server()
{
	m_bNetworkOnline = false;
	m_ReceiveThread.join();

	// Send a message to all connected clients that the server is terminating
	CreateCommandPacket(TERMINATE_SERVER, m_strHostUser);
	SendPacket(m_pServerToClient);

	// Delete Client Network
	delete m_pNetworkServer;
	m_pNetworkServer = 0;

	// Delete the Networking Packets
	delete m_pClientToServer;
	m_pClientToServer = 0;
	delete m_pServerToClient;
	m_pServerToClient = 0;
	delete m_pPacketToProcess;
	m_pPacketToProcess = 0;

	// Delete the ServerMutex
	delete m_pMutexServer;
	m_pMutexServer = 0;

	// Delete the WorkQueue
	delete m_pWorkQueue;
	m_pWorkQueue = 0;

	// Delete the container of Users from the Heap Memory
	delete m_pCurrentUsers;
	m_pCurrentUsers = 0;

	// Delete the Mechanics
	delete m_pMechanics;
	m_pMechanics = 0;
}

CHub_Server& CHub_Server::GetInstance()
{
	if (s_pGame == 0)
	{
		s_pGame = new CHub_Server();
	}
	return (*s_pGame);
}

void CHub_Server::DestroyInstance()
{
	delete s_pGame;
	s_pGame = 0;
}

bool CHub_Server::Initialise(HWND _hWnd, int _iScreenWidth, int _iScreenHeight, LPWSTR* _wstrArgs)
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
	m_pNetworkServer = new CNetwork_Server();
	VALIDATE(m_pNetworkServer->Initialise());
	m_bNetworkOnline = true;

	// Create a thread to receive data from the Server
	m_ReceiveThread = std::thread(&CHub_Server::ReceiveDataFromNetwork, (this), m_pClientToServer);

	// Create the WorkQueue
	m_pWorkQueue = new std::queue<ClientToServer>;

	// Create a vector to store current Users
	m_pCurrentUsers = new std::map < std::string, sockaddr_in>;

	// Set the Initial ServerState
	m_eServerState = STATE_LOBBY;

	// Initialise Gameplay variables
	m_pMechanics = new CMechanics_Server();
	VALIDATE(m_pMechanics->Initialise(m_strServerName));

	return true;
}

bool CHub_Server::ExecuteOneFrame()
{
	if (m_bNetworkOnline == false)
	{
		return false;
	}

	Process();

	return true;

}

void CHub_Server::Process()
{
	while (m_pWorkQueue->empty() == false)
	{
		m_pMutexServer->Wait();
		*m_pPacketToProcess = m_pWorkQueue->front();
		m_pWorkQueue->pop();
		m_pMutexServer->Signal();

		// Process the Pulled Packet
		ProcessPacket();
	}

	// Only process if in the Lobby Screen
	if (m_eServerState == STATE_LOBBY)
	{
		if (m_pCurrentUsers->size() > 0)
		{
			if (m_pMechanics->CheckAllAvatarsReady() == true)
			{
				// All Participants are ready to go -> start the game
				m_eServerState = STATE_GAMEPLAY;
			
				CreateCommandPacket(START_GAME);
				SendPacket(m_pServerToClient);
			}
		}
	}
	else if ( m_eServerState == STATE_GAMEPLAY)
	{
		// Process the Mechanics
		m_pMechanics->Process();
		
		// Create Command Packets for Deleted and created Enemies
		EnemyInfo enemyInfo;
		PowerUpInfo powerInfo;
		while (m_pMechanics->GetNextDeletedEnemy(&enemyInfo) == true)
		{
			CreateCommandPacket(DELETE_ENEMY);
			m_pServerToClient->enemyInfo = enemyInfo;
			SendPacket(m_pServerToClient);
		}
		while (m_pMechanics->GetNextCreatedEnemy(&enemyInfo) == true)
		{
			CreateCommandPacket(CREATE_ENEMY);
			m_pServerToClient->enemyInfo = enemyInfo;
			SendPacket(m_pServerToClient);
		}
		while (m_pMechanics->GetNextDeletedPower(&powerInfo) == true)
		{
			CreateCommandPacket(DELETE_POWERUP);
			m_pServerToClient->powerInfo = powerInfo;
			SendPacket(m_pServerToClient);
		}
		while (m_pMechanics->GetNextCreatedPower(&powerInfo) == true)
		{
			CreateCommandPacket(CREATE_POWERUP);
			m_pServerToClient->powerInfo = powerInfo;
			SendPacket(m_pServerToClient);
		}
		


		// Send the Data packet with the current states
		m_pMechanics->CreateDataPacket(m_pServerToClient);
		SendPacket(m_pServerToClient);
		Sleep(16);	// TO DO - remove sleep add limiter without a sleep function more than 1 ms
	}
}

void CHub_Server::ProcessPacket()
{
	if (m_pPacketToProcess->bCommand == true)
	{
		//Process a command code

		eNetworkCommand eProcessCommand = m_pPacketToProcess->eCommand;

		switch (eProcessCommand)
		{
			// Terminate Server
			case TERMINATE_SERVER:
			{
				// Accept Terminate Command only if the Host client sent it
				if ((std::string)(m_pPacketToProcess->cUserName) == m_strHostUser)
				{
					// Turn the network offline.
					// This stop the receivethread and allow it to join and then terminate the server application
					m_bNetworkOnline = false;
				}
				break;
			}
			// Host is looking for the Server they just created
			case QUERY_HOST:
			{
				// Do not process this command if in gameplay
				if (m_eServerState != STATE_GAMEPLAY)
				{
					std::string strCheckHost = (std::string)(m_pPacketToProcess->cUserName);
					std::string strCheckServer = (std::string)(m_pPacketToProcess->cAdditionalMessage);

					// Ensure the sender of this command is the correct host for only they would know the
					// host and server names created via the command line
					if (strCheckHost == m_strHostUser
						&&	strCheckServer == m_strServerName)
					{
						// If for any reason another Host creates a server with the same names
						// This server will only reply to very first message with this command
						if (m_bRepliedToHost == false)
						{
							InsertUser(m_pPacketToProcess);
							CreateCommandPacket(HOST_SERVER, m_strHostUser);
							SendPacket(m_pServerToClient, m_pPacketToProcess->ClientAddr);
							m_bRepliedToHost = true;
						}
					}
					// Else the query is for another server
					else
					{
						CreateCommandPacket(NOT_HOST);
						SendPacket(m_pServerToClient, m_pPacketToProcess->ClientAddr);
					}
				}
				break;
			}
			// Broadcast message to determine what servers if any are online
			case QUERY_CLIENT_CONNECTION:
			{
				// Do not process this command if in gameplay
				if (m_eServerState != STATE_GAMEPLAY)
				{
					// return message saying this server is available
					CreateCommandPacket(SERVER_CONNECTION_AVAILABLE, m_strHostUser);
					SendPacket(m_pServerToClient, m_pPacketToProcess->ClientAddr);
				}
				break;
			}
			// Tries to Create a new User on the Server
			case CREATEUSER:
			{
				// Do not process this command if in gameplay
				if (m_eServerState != STATE_GAMEPLAY)
				{
					// Check if the server already has the maximum allowed users
					if (m_pCurrentUsers->size() < network::MAX_CLIENTS)
					{
						// Try to insert the new user into the map of users for this server
						std::string strUser = (std::string)(m_pPacketToProcess->cUserName);
						bool bAdded = InsertUser(m_pPacketToProcess);

						if (bAdded == true)
						{
							// If the User was successfully added send back that they were accepted
							CreateCommandPacket(CREATEUSER_ACCEPTED, (std::string)(m_pPacketToProcess->cUserName));
							SendPacket(m_pServerToClient, m_pPacketToProcess->ClientAddr);

							// Send to all current users that a user has joined with their details
							CreateCommandPacket(USER_JOINED, (std::string)m_pPacketToProcess->cUserName);
							SendPacket(m_pServerToClient);

							// Send the new user the name of the Host user
							CreateCommandPacket(YOUR_HOST, m_strHostUser);
							SendPacket(m_pServerToClient, m_pPacketToProcess->ClientAddr);
						}
						else
						{
							// If the Insert failed then the username was already in use
							CreateCommandPacket(CREATEUSER_NAMEINUSE);
							SendPacket(m_pServerToClient, m_pPacketToProcess->ClientAddr);
						}
					}
					else
					{
						// reply to the user that the server is currently full
						CreateCommandPacket(CREATEUSER_SERVERFULL);
						SendPacket(m_pServerToClient, m_pPacketToProcess->ClientAddr);
					}
				}

				break;
			}
			// User is Leaving the Server
			case LEAVE_SERVER:
			{
				m_pCurrentUsers->erase(m_pPacketToProcess->cUserName);
				m_pMechanics->RemoveAvatar((std::string)m_pPacketToProcess->cUserName);

				CreateCommandPacket(USER_LEFT, m_pPacketToProcess->cUserName);
				SendPacket(m_pServerToClient);

				break;
			}
			// Set the ready state or alive state of a user
			case ALIVE_SET:
			{
				// Determine which user sent this message
				std::map<std::string, sockaddr_in>::iterator User = m_pCurrentUsers->find(m_pPacketToProcess->cUserName);

				// Update the state based on the additional message data
				bool bAlive = (std::string)m_pPacketToProcess->cAdditionalMessage == "true" ? true : false;
				m_pMechanics->SetAvatarAliveState(m_pPacketToProcess->cUserName, bAlive);

				// Send to all users the new State for that user
				CreateCommandPacket(ALIVE_SET, m_pPacketToProcess->cUserName);
				SendPacket(m_pServerToClient);

				break;
			}
		}
	}
	else
	{
		// Process the Input from the User
		m_pMechanics->ProcessAvatarMovement(m_pPacketToProcess);
		
	}
}

bool CHub_Server::CreateCommandPacket(eNetworkCommand _eCommand)
{
	// Clear out old data from the Data Packet packet
	ZeroMemory(*(&m_pServerToClient), sizeof(*m_pServerToClient));

	// Change the Packet Data to contain a Command
	m_pServerToClient->bCommand = true;

	// Add the Command to The Packet structure
	m_pServerToClient->eCommand = _eCommand;

	// Add the Server as the username to the Packet structure
	if (!(StringToStruct(m_strServerName.c_str(), m_pServerToClient->cServerName, network::MAX_SERVERNAME_LENGTH)))
	{
		return false;	// Data invalid - Data Packet failed to create
	}

	// Add all the Avatar Info to the Packet
	m_pMechanics->AddAvatarsToPacket(m_pServerToClient);

	//Add the Current user count to the packet
	m_pServerToClient->CurrentUserCount = (int)m_pCurrentUsers->size();

	return true;
}

bool CHub_Server::CreateCommandPacket(eNetworkCommand _eCommand, std::string _strUserName)
{
	// Create a general command packet
	CreateCommandPacket(_eCommand);

	// Add the additional meesage
	if (!(StringToStruct(_strUserName.c_str(), m_pServerToClient->cUserName, network::MAX_USERNAME_LENGTH)))
	{
		return false;	// Data invalid - Data Packet failed to create
	}

	return true;
}

void CHub_Server::ReceiveDataFromNetwork(ClientToServer* _pReceiveData)
{
	while (m_bNetworkOnline)
	{
		if (m_pNetworkServer->ReceivePacket(_pReceiveData))
		{
			// Add the Received Packet to the Work Queue
			m_pMutexServer->Wait();
			m_pWorkQueue->push(*_pReceiveData);
			m_pMutexServer->Signal();
		}
	}
}

bool CHub_Server::SendPacket(ServerToClient* _pServerPacket, sockaddr_in _sockAddr)
{
	// Send the packet to that user only
	return m_pNetworkServer->SendPacket(_sockAddr, _pServerPacket);

	
}

bool CHub_Server::SendPacket(ServerToClient* _pServerPacket)
{
	std::map<std::string, sockaddr_in>::iterator iterUser = m_pCurrentUsers->begin();

	// Send Packet to all current Users
	while (iterUser != m_pCurrentUsers->end())
	{
		VALIDATE(m_pNetworkServer->SendPacket(iterUser->second, _pServerPacket));
		iterUser++;
	}

	return true;
}

std::string CHub_Server::WideStringToString(wchar_t* _wstr)
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

bool CHub_Server::InsertUser(ClientToServer* _pClientPacket)
{
	// Create a pair of the new User
	std::pair<std::string, sockaddr_in> newUser(_pClientPacket->cUserName, _pClientPacket->ClientAddr);
	std::pair<std::map<std::string, sockaddr_in>::iterator, bool> pairReturn;

	// Try to add the new User to the map
	pairReturn = m_pCurrentUsers->insert(newUser);

	if (pairReturn.second == true)
	{
		m_pMechanics->AddAvatar(_pClientPacket);
	}

	// Return the outcome of the insert
	return pairReturn.second;
}