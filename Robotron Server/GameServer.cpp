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
	delete m_pVecUsers;
	m_pVecUsers = 0;
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
	m_pVecUsers = new std::vector<std::string>;
	m_pVecUsers->push_back(m_strHostUser);

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
			// Accept Terminate COmmand only if the Host client sent it
			if ((std::string)(m_pPacketToProcess->cUserName) == m_strHostUser)
			{
				m_bNetworkOnline = false;
			}
			return;
		}
		else if (eProcessCommand == QUERY_HOST)
		{
			std::string strCheckHost = (std::string)(m_pPacketToProcess->cUserName);
			if (strCheckHost == m_strHostUser)
			{
				if (m_bRepliedToHost == false)
				{
					CreateCommandPacket(HOST_SERVER);
					m_pServerNetwork->SendPacket(strCheckHost, m_pServerToClient);
					m_bRepliedToHost = true;
				}
			}
			else
			{
				CreateCommandPacket(NOT_HOST);
				m_pServerNetwork->SendPacket(strCheckHost, m_pServerToClient);
			}
			return;
		}
		else if (eProcessCommand == QUERY_CLIENT_CONNECTION)
		{
			CreateCommandPacket(SERVER_CONNECTION_AVAILABLE);
			m_pServerNetwork->SendPacket(m_pPacketToProcess->cUserName, m_pServerToClient);

			return;
		}
		// Tries to Create a new User on the Server
		else if (eProcessCommand == CREATEUSER)
		{
			if (m_pVecUsers->size() < network::MAX_CLIENTS)
			{
				bool bCanAdd = true;
				std::string strUser = (std::string)(m_pPacketToProcess->cUserName);
				for (unsigned int i = 0; i < m_pVecUsers->size(); i++)
				{
					if (strUser == (*m_pVecUsers)[i])
					{
						bCanAdd = false;
					}
				}
				if (bCanAdd == true)
				{
					CreateCommandPacket(CREATEUSER_ACCEPTED);
					m_pServerNetwork->SendPacket(strUser, m_pServerToClient);

					CreateCommandPacket(USER_JOINED, (std::string)m_pPacketToProcess->cUserName);
					m_pServerNetwork->SendPacket(m_pServerToClient);

					// Send the list of users to the new User
					for (unsigned int i = 0; i < m_pVecUsers->size(); i++)
					{
						CreateCommandPacket(USER_JOINED, (*m_pVecUsers)[i]);
						m_pServerNetwork->SendPacket(m_pPacketToProcess->cUserName, m_pServerToClient);
					}

					CreateCommandPacket(YOUR_HOST, m_strHostUser);
					m_pServerNetwork->SendPacket(m_pPacketToProcess->cUserName, m_pServerToClient);

					m_pVecUsers->push_back(strUser);
				}
				else
				{
					CreateCommandPacket(CREATEUSER_NAMEINUSE);
					m_pServerNetwork->SendPacket(strUser, m_pServerToClient);
				}
			}
			else
			{
				CreateCommandPacket(CREATEUSER_SERVERFULL);
				m_pServerNetwork->SendPacket(m_strHostUser, m_pServerToClient);
			}

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
	// Change the Packet Data to contain a Command
	m_pServerToClient->bCommand = true;

	// Add the Command to The Packet structure
	m_pServerToClient->eCommand = _eCommand;

	// Add the Server as the username to the Packet structure
	if (!(StringToStruct(m_strServerName.c_str(), m_pServerToClient->cUserName)))
	{
		return false;	// Data invalid - Data Packet failed to create
	}

	return true;
}

bool CGameServer::CreateCommandPacket(eNetworkCommand _eCommand, std::string _strMessage)
{
	// Create a general command packet
	CreateCommandPacket(_eCommand);

	// Add the additional meesage
	if (!(StringToStruct(_strMessage.c_str(), m_pServerToClient->cAdditionalMessage)))
	{
		return false;	// Data invalid - Data Packet failed to create
	}

	return true;
}

bool CGameServer::StringToStruct(const char* _pcData, char* _pcStruct)
{
	int iMaxLength;
	// Determine the maximum amount of characters that can be copied
	// based on the memory address that it is going to be writing to
	if (*(&m_pServerToClient->cUserName) == *(&_pcStruct))
	{
		iMaxLength = network::MAX_USERNAME_LENGTH - 1;
	}
	else if (*(&m_pServerToClient->cAdditionalMessage) == *(&_pcStruct))
	{
		iMaxLength = network::MAX_CHAR_LENGTH - 1;
	}

	// Ensure no buffer overrun will occur when copying directly to memory
	if ((strlen(_pcData) + 1) < network::MAX_CHAR_LENGTH - 1)
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

			// TO DO - Remove
			int i = 0;
		}
	}
}

std::string CGameServer::WideStringToString(wchar_t* _wstr)
{
	// Convert the WideString to a standard string
	size_t lengthWstr = (wcslen(_wstr) + 1);
	size_t convertedCount = 0;
	char* cHostName = new char[lengthWstr * 2];
	wcstombs_s(&convertedCount, cHostName, lengthWstr, _wstr, _TRUNCATE);

	std::string strReturn = (std::string)(cHostName);

	// Delete the created memory
	delete cHostName;
	cHostName = 0;

	// Store the hosts username
	return strReturn;
}
