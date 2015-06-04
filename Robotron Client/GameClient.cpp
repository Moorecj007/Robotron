/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : GameClient.cpp
* Description : Runs the main game functions for the Client - The game manager
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Library Includes


// Local Includes
#include "GameClient.h"

// Static Variables
CGameClient* CGameClient::s_pGame = 0;

CGameClient::CGameClient()
{
}

CGameClient::~CGameClient()
{
	m_bNetworkOnline = false;

	// Delete Client Network
	delete m_pClientNetwork;
	m_pClientNetwork = 0;

	// Delete the Networking Packets
	delete m_pClientToServer;
	m_pClientToServer = 0;
	delete m_pServerToClient;
	m_pServerToClient = 0;
}

CGameClient& CGameClient::GetInstance()
{
	if (s_pGame == 0)
	{
		s_pGame = new CGameClient();
	}
	return (*s_pGame);
}

void CGameClient::DestroyInstance()
{
	delete s_pGame;
	s_pGame = 0;
}

bool CGameClient::Initialise(HWND _hWnd, int _iScreenWidth, int _iScreenHeight)
{
	// Populate window variables
	m_hWnd = _hWnd;
	m_iScreenWidth = _iScreenWidth;
	m_iScreenHeight = _iScreenHeight;

	// Create Data Packets
	m_pClientToServer = new ClientToServer();
	m_pServerToClient = new ServerToClient();

	std::string strFilename;
	#ifdef _DEBUG
		strFilename = "..\\Debug\\Robotron Server";
	#endif // _DEBUG
	#ifndef _DEBUG
		// TO DO - change file path when handing in final build
		strFilename = "..\\Release\\Robotron Server";
	#endif // Not _DEBUG

	//int iError = (int)(ShellExecuteA(m_hWnd, "open", strFilename.c_str(), NULL, NULL, SW_NORMAL));

	// Create and Initialise the Client-side Network
	m_pClientNetwork = new CClient;
	VALIDATE(m_pClientNetwork->Initialise());
	m_bNetworkOnline = true;

	// Create a thread to receive data from the Server
	m_ReceiveThread = std::thread(&CGameClient::ReceiveDataFromNetwork,(this), m_pServerToClient);

	// TO DO - remove Temporary check
	CreateDataPacket();
	m_pClientNetwork->SendPacket(m_pClientToServer);
	// End TO DO

	return true;
}

void CGameClient::RenderOneFrame()
{

}

void CGameClient::Process()
{

}

void CGameClient::Draw()
{

}

void CGameClient::MainMenu()
{

}

bool CGameClient::CreateDataPacket()
{
	// TO DO - Create actual Data
	m_pClientToServer->bCommand = true;
	char* strCommand = "Testing - Client to Server with TRUE command boolean";

	// Check that the structure was able to accept the data
	if (!(StringToStruct(strCommand, m_pClientToServer->cCommand)))
	{
		// Data invalid - Data Packet failed to create
		return false;
	}

	return true;
}

bool CGameClient::StringToStruct(const char* _pcData, char* _pcStruct)
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

void CGameClient::ReceiveDataFromNetwork(ServerToClient* _pReceiveData)
{
	while (m_bNetworkOnline)
	{
		if (m_pClientNetwork->ReceivePacket(_pReceiveData))
		{
			int i = 0;
		}
	}
}