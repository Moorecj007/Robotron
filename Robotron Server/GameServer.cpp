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

	// Delete the ServerMutex
	delete m_pServerMutex;
	m_pServerMutex = 0;

	// Delete the WorkQueue
	delete m_pWorkQueue;
	m_pWorkQueue = 0;
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

bool CGameServer::Initialise(HWND _hWnd, int _iScreenWidth, int _iScreenHeight)
{
	// Populate window variables
	m_hWnd = _hWnd;
	m_iScreenWidth = _iScreenWidth;
	m_iScreenHeight = _iScreenHeight;

	// Create Data Packets
	m_pClientToServer = new ClientToServer();
	m_pServerToClient = new ServerToClient();

	// Create and Initialise the Server-side Network
	m_pServerNetwork = new CServer();
	VALIDATE(m_pServerNetwork->Initialise());
	m_bNetworkOnline = true;

	// Create a thread to receive data from the Server
	m_ReceiveThread = std::thread(&CGameServer::ReceiveDataFromNetwork, (this), m_pClientToServer);

	// Create the WorkQueue
	m_pWorkQueue = new std::queue<ClientToServer>;

	return true;
}

void CGameServer::RenderOneFrame()
{
	
}

void CGameServer::Process()
{

}

bool CGameServer::CreateDataPacket()
{
	// TO DO - Create actual Data
	m_pClientToServer->bCommand = false;
	char* strCommand = "Testing - Server to Client with FALSE command boolean";

	// Check that the structure was able to accept the data
	if (!(StringToStruct(strCommand, m_pServerToClient->cCommand)))
	{
		// Data invalid - Data Packet failed to create
		return false;
	}

	return true;
}

bool CGameServer::StringToStruct(const char* _pcData, char* _pcStruct)
{
	// Ensure no buffer overrun will occur when copying directly to memory
	if ((strlen(_pcData) + 1) < network::MAX_CHAR_LENGTH)
	{
		// Copy the characters into the struct
		strcpy_s(_pcStruct, (strlen(_pcData) + 1), _pcData);
		return true;
	}
	else
	{
		// char* is too long, buffer overrun would occur so failed operation
		return false;
	}
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
			CreateDataPacket();
			m_pServerNetwork->SendPacket(m_pServerToClient);
			int i = 0;
		}
	}
}