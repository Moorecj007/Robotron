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

	// Send a message to all connected clients that the server is terminating
	CreateCommandPacket(TERMINATE_SERVER, m_strHostUser);
	m_pServerNetwork->SendPacket(m_pServerToClient);

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

	// Delete the Mechanics
	delete m_pMechanics;
	m_pMechanics = 0;
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
	m_pCurrentUsers = new std::map < std::string, AvatarInfo>;
	InsertUser(m_strHostUser);

	// Set the Initial ServerState
	m_eServerState = STATE_LOBBY;

	// Initialise Gameplay variables
	m_pMechanics = new CServerMechanics();
	VALIDATE(m_pMechanics->Initialise());

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
			std::map<std::string, AvatarInfo>::iterator iterCurrentUser = m_pCurrentUsers->begin();
			bool bAllReady = true;

			// Check if all users are ready/ alive
			while (iterCurrentUser != m_pCurrentUsers->end())
			{
				if (iterCurrentUser->second.bAlive == false)
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
	else if ( m_eServerState == STATE_GAMEPLAY)
	{
		m_pMechanics->Process();

		CreateDataPacket();
		m_pServerNetwork->SendPacket(m_pServerToClient);
		Sleep(16);	// TO DO - remove sleep add limiter without a sleep function more than 1 ms
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
				// Turn the network offline.
				// This stop the receivethread and allow it to join and then terminate the server application
				m_bNetworkOnline = false;
			}
			return;
		}
		// Host is looking for the Server they just created
		else if (eProcessCommand == QUERY_HOST)
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
						CreateCommandPacket(HOST_SERVER, m_strHostUser);
						m_pServerNetwork->SendPacket(m_pPacketToProcess->ClientAddr, m_pServerToClient);
						m_pServerNetwork->AddClientAddr(strCheckHost, m_pPacketToProcess->ClientAddr);
						m_bRepliedToHost = true;
					}
				}
				// Else the query is for another server
				else
				{
					CreateCommandPacket(NOT_HOST);
					m_pServerNetwork->SendPacket(m_pPacketToProcess->ClientAddr, m_pServerToClient);
				}
			}
			return;
		}
		// Broadcast message to determine what servers if any are online
		else if (eProcessCommand == QUERY_CLIENT_CONNECTION)
		{
			// Do not process this command if in gameplay
			if (m_eServerState != STATE_GAMEPLAY)
			{
				// return message saying this server is available
				CreateCommandPacket(SERVER_CONNECTION_AVAILABLE, m_strHostUser);
				m_pServerNetwork->SendPacket(m_pPacketToProcess->ClientAddr, m_pServerToClient);
			}
			return;
		}
		// Tries to Create a new User on the Server
		else if (eProcessCommand == CREATEUSER)
		{
			// Do not process this command if in gameplay
			if ( m_eServerState != STATE_GAMEPLAY)
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
						CreateCommandPacket(CREATEUSER_ACCEPTED, (std::string)(m_pPacketToProcess->cUserName));
						m_pServerNetwork->SendPacket(m_pPacketToProcess->ClientAddr, m_pServerToClient);
						m_pServerNetwork->AddClientAddr(strUser, m_pPacketToProcess->ClientAddr);

						// Send to all current users that a user has joined with their details
						CreateCommandPacket(USER_JOINED, (std::string)m_pPacketToProcess->cUserName);
						m_pServerNetwork->SendPacket(m_pServerToClient);

						// Send the new user the name of the Host user
						CreateCommandPacket(YOUR_HOST, m_strHostUser);
						m_pServerNetwork->SendPacket(m_pPacketToProcess->ClientAddr, m_pServerToClient);
					}
					else
					{
						// If the Insert failed then the username was already in use
						CreateCommandPacket(CREATEUSER_NAMEINUSE);
						m_pServerNetwork->SendPacket(m_pPacketToProcess->ClientAddr, m_pServerToClient);
					}
				}
				else
				{
					// reply to the user that the server is currently full
					CreateCommandPacket(CREATEUSER_SERVERFULL);
					m_pServerNetwork->SendPacket(m_pPacketToProcess->ClientAddr, m_pServerToClient);
				}
			}	

			return;
		}
		// User is Leaving the Server
		else if (eProcessCommand == LEAVE_SERVER)
		{
			m_pCurrentUsers->erase(m_pPacketToProcess->cUserName);

			CreateCommandPacket(USER_LEFT, m_pPacketToProcess->cUserName);
			m_pServerNetwork->RemoveClientAddr(m_pPacketToProcess->cUserName);
			m_pServerNetwork->SendPacket(m_pServerToClient);

			return;
		}
		// Set the ready state or alive state of a user
		else if (eProcessCommand == ALIVE_SET)
		{
			// Determine which user sent this message
			std::map<std::string, AvatarInfo>::iterator User = m_pCurrentUsers->find(m_pPacketToProcess->cUserName);

			// Update the state based on the additional message data
			bool bAliveness = ((std::string)m_pPacketToProcess->cAdditionalMessage == "true") ? true : false;
			User->second.bAlive = bAliveness;

			// Send to all users the new State for that user
			CreateCommandPacket(ALIVE_SET, m_pPacketToProcess->cUserName);
			m_pServerNetwork->SendPacket(m_pServerToClient);

			return;
		}
	}
	else
	{
		// Determine which user sent this message
		std::map<std::string, AvatarInfo>::iterator User = m_pCurrentUsers->find(m_pPacketToProcess->cUserName);

		// TO DO - Move to another location
		v3float v3Movement = { 0, 0, 0 };
		if (m_pPacketToProcess->activatedControls.bUp == true)
		{
			v3Movement.z = 1;
		}
		if (m_pPacketToProcess->activatedControls.bDown == true)
		{
			v3Movement.z = -1;
		}
		if (m_pPacketToProcess->activatedControls.bRight == true)
		{
			v3Movement.x = 1;
		}
		if (m_pPacketToProcess->activatedControls.bLeft == true)
		{
			v3Movement.x = -1;
		}
		v3float normV3 = NormaliseV3Float(&v3Movement);
		User->second.v3Pos.x += normV3.x;
		User->second.v3Pos.y += normV3.y;
		User->second.v3Pos.z += normV3.z;
	}
}

bool CGameServer::CreateDataPacket()
{
	// Erase old data
	ZeroMemory(*(&m_pServerToClient), sizeof(*m_pServerToClient));

	// Fill basic information of the packet
	m_pServerToClient->bCommand = false;
	m_pServerToClient->eCommand = ERROR_COMMAND;
	m_pServerToClient->CurrentUserCount = (int)(m_pCurrentUsers->size());

	// Add the Server as the username to the Packet structure
	if (!(StringToStruct(m_strServerName.c_str(), m_pServerToClient->cServerName, network::MAX_SERVERNAME_LENGTH)))
	{
		return false;	// Data invalid - Data Packet failed to create
	}

	// Add all the Users to the Packet with their information status
	std::map<std::string, AvatarInfo>::iterator iterCurrent = m_pCurrentUsers->begin();
	int iIndex = 0;
	while (iterCurrent != m_pCurrentUsers->end())
	{
		m_pServerToClient->Avatars[iIndex] = iterCurrent->second;

		iIndex++;
		iterCurrent++;
	}

	return true;
}

bool CGameServer::CreateCommandPacket(eNetworkCommand _eCommand)
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

	// Add all the Users to the Packet with their information status
	std::map<std::string, AvatarInfo>::iterator iterCurrent = m_pCurrentUsers->begin();
	int iIndex = 0;
	while (iterCurrent != m_pCurrentUsers->end())
	{
		m_pServerToClient->Avatars[iIndex] = iterCurrent->second;

		iIndex++;
		iterCurrent++;
	}

	//Add the Current user count to the packet
	m_pServerToClient->CurrentUserCount = (int)m_pCurrentUsers->size();

	return true;
}

bool CGameServer::CreateCommandPacket(eNetworkCommand _eCommand, std::string _strUserName)
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

bool CGameServer::StringToStruct(const char* _pcData, char* _pcStruct, unsigned int _iMaxLength)
{
	// Ensure no buffer overrun will occur when copying directly to memory
	if ((strlen(_pcData) + 1) <= (_iMaxLength))
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
	// Retrieve the number of the current users
	int iNumber = (int)(m_pCurrentUsers->size());

	// Create a new UserInfo with default information for the new user
	AvatarInfo tempAvatarInfo;
	tempAvatarInfo.bAlive = false;
	StringToStruct(_strUser.c_str(), tempAvatarInfo.cUserName, network::MAX_USERNAME_LENGTH);

	// Create the starting position based on the current number of users
	tempAvatarInfo.v3Pos = { (float)iNumber * 5, 20, 5 };
	tempAvatarInfo.fSpeed = 10;

	std::pair<std::string, AvatarInfo> pairUser(_strUser, tempAvatarInfo);

	std::pair<std::map<std::string, AvatarInfo>::iterator, bool> pairReturn;
	pairReturn = m_pCurrentUsers->insert(pairUser);

	return pairReturn.second;
}

v3float CGameServer::NormaliseV3Float(v3float* _v3)
{
	v3float normV3Float = { 0, 0, 0 };
	float fMagnitude = sqrt(pow(_v3->x, 2) + pow(_v3->y, 2) + pow(_v3->z, 2));

	if (fMagnitude > 0)
	{
		normV3Float.x = (_v3->x / fMagnitude);
		normV3Float.y = (_v3->y / fMagnitude);
		normV3Float.z = (_v3->z / fMagnitude);
	}

	return normV3Float;
}