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

// Local Includes
#include "GameClient.h"

// Static Variables
CGameClient* CGameClient::s_pGame = 0;
CMySemaphore* CGameClient::m_pClientMutex = new CMySemaphore(1);

CGameClient::CGameClient()
{
}

CGameClient::~CGameClient()
{
	m_bNetworkOnline = false;
	m_ReceiveThread.join();

	if (m_bHost == true)
	{
		// TO DO - create command to terminate server
		CreateCommandPacket("Terminate");
		m_pClientNetwork->SendPacket(m_pClientToServer);
	}

	// Delete the Clock
	delete m_pClock;
	m_pClock = 0;

	// Delete Client Network
	delete m_pClientNetwork;
	m_pClientNetwork = 0;

	// Delete the Networking Packets
	delete m_pClientToServer;
	m_pClientToServer = 0;
	delete m_pServerToClient;
	m_pServerToClient = 0;

	// Delete the ServerMutex
	delete m_pClientMutex;
	m_pClientMutex = 0;

	// Delete the WorkQueue
	delete m_pWorkQueue;
	m_pWorkQueue = 0;

	// Delete the Renderer
	delete m_pRenderer;
	m_pRenderer = 0;

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

void CGameClient::SetMousePos(int _iX, int _iY)
{
	m_iMouseX = _iX;
	m_iMouseY = _iY;
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

	// Create the Clock
	m_pClock = new CClock();

	// Create and Initialise the Client-side Network
	m_pClientNetwork = new CClient;
	VALIDATE(m_pClientNetwork->Initialise());
	m_bNetworkOnline = true;

	// Create a thread to receive data from the Server
	m_ReceiveThread = std::thread(&CGameClient::ReceiveDataFromNetwork,(this), m_pServerToClient);

	// Create the WorkQueue
	m_pWorkQueue = new std::queue<ServerToClient>;

	// Create and Initialise the Renderer
	m_pRenderer = new CD3D9Renderer();
	m_pRenderer->Initialise(m_iScreenWidth, m_iScreenHeight, m_hWnd, false);

	// Start the Game in Main Menu screen state
	m_eScreenState = STATE_MAIN_MENU;

	// Initialise boolean array for KeyDown to false
	m_pbKeyDown = new bool[255];
	memset(m_pbKeyDown, false, 255);
	m_bLeftMouseClick = false;		// False beginning value

	// Set the Game Variables
	m_eMenuSelection = NO_SELECTION;
	m_eMenuTempSelection = NO_SELECTION;
	m_bHost = false;



	// TO DO - remove Temporary check
	//CreateDataPacket();
	//m_pClientNetwork->SendPacket(m_pClientToServer);
	// End TO DO

	return true;
}

void CGameClient::RenderOneFrame()
{

	Process();
	Draw();
}

void CGameClient::Process()
{
	// Process the Clock and get the new Delta Tick
	m_pClock->Process();
	float fDT = m_pClock->GetDeltaTick();

	switch (m_eScreenState)
	{
	case STATE_MAIN_MENU:
	{
		if (m_eMenuSelection == PLAY_GAME)
		{
			// Reset the Menu Selection
			m_eMenuSelection = NO_SELECTION;
			m_eScreenState = STATE_PLAY_MENU;
		}
		break;
	}
	case STATE_PLAY_MENU:
	{
		if (m_eMenuSelection == HOST_GAME)
		{
			// Reset the Menu Selection
			m_eMenuSelection = NO_SELECTION;

			std::string strFilename;
			#ifdef _DEBUG
				strFilename = "..\\Debug\\Robotron Server";
			#endif // _DEBUG
			#ifndef _DEBUG
				// TO DO - change file path when handing in final build
				strFilename = "..\\Release\\Robotron Server";
			#endif // Not _DEBUG

			// Start the Server executable running
			int iError = (int)(ShellExecuteA(m_hWnd, "open", strFilename.c_str(), NULL, NULL, SW_NORMAL));
			m_bHost = true;

			Sleep(100);
			CreateDataPacket();
			m_pClientNetwork->SendPacket(m_pClientToServer);
		
		}
		break;
	}
	case STATE_GAME_PLAY:
	{

		break;
	}
	default: break;

	}	// End Switch(m_eScreenState)

}

void CGameClient::Draw()
{
	m_pRenderer->StartRender(true, true, false);

	switch (m_eScreenState)
	{
		case STATE_MAIN_MENU:
		{
			DisplayMainMenu();
			break;
		}
		case STATE_PLAY_MENU:
		{
			DisplayPlayMenu();
			break;
		}
		case STATE_GAME_PLAY:
		{
			CreateDataPacket();
			m_pClientNetwork->SendPacket(m_pClientToServer);

			break;
		}
		default: break;
		
	}	// End Switch(m_eScreenState)

	m_pRenderer->EndRender();
	int i = 0;
}

void CGameClient::DisplayMainMenu()
{
	int iYpos = 0;
	D3DXCOLOR defaultColor = 0xffff0000;
	int iSelection = 0;

	// Render the Backbuffer to Black
	m_pRenderer->RenderColor(0xff000000);

	// Print the Title Text
	iSelection += m_pRenderer->RenderText(NO_SELECTION, m_iMouseY, "ROBOTRON", (iYpos += 50), SUBTITLE_FONT, defaultColor);
	iSelection += m_pRenderer->RenderText(NO_SELECTION, m_iMouseY, "HORDES", (iYpos += 80), TITLE_FONT, 0xff550000);
	iSelection += m_pRenderer->RenderText(NO_SELECTION, m_iMouseY, "OF", (iYpos += 100), TITLE_FONT, 0xff550000);
	iSelection += m_pRenderer->RenderText(NO_SELECTION, m_iMouseY, "HELL", (iYpos += 100), TITLE_FONT, 0xff550000);
	iSelection += m_pRenderer->RenderText(NO_SELECTION, m_iMouseY, "Main Menu", (iYpos += 150), SUBTITLE_FONT, defaultColor);

	// Print the Menu Options
	iSelection += m_pRenderer->RenderText(PLAY_GAME, m_iMouseY, "Play Game", (iYpos += 150), MENU_FONT, defaultColor);
	iSelection += m_pRenderer->RenderText(OPTIONS, m_iMouseY, "Options", (iYpos += 80), MENU_FONT, defaultColor);
	iSelection += m_pRenderer->RenderText(EXIT_GAME, m_iMouseY, "Exit Game", (iYpos += 80), MENU_FONT, defaultColor);

	// Change the MenuSelection Variable to the menu enum
	if (m_bLeftMouseClick == true)
	{
		m_eMenuTempSelection = (eMenuSelection)(iSelection);
	}
	else
	{
		m_eMenuTempSelection = NO_SELECTION;
	}
}

void CGameClient::DisplayPlayMenu()
{
	int iYpos = 0;
	D3DXCOLOR defaultColor = 0xffff0000;
	int iSelection = 0;

	// Render the Backbuffer to Black
	m_pRenderer->RenderColor(0xff000000);

	// Print the Title Text
	iSelection += m_pRenderer->RenderText(NO_SELECTION, m_iMouseY, "ROBOTRON", (iYpos += 50), SUBTITLE_FONT, defaultColor);
	iSelection += m_pRenderer->RenderText(NO_SELECTION, m_iMouseY, "HORDES", (iYpos += 80), TITLE_FONT, 0xff550000);
	iSelection += m_pRenderer->RenderText(NO_SELECTION, m_iMouseY, "OF", (iYpos += 100), TITLE_FONT, 0xff550000);
	iSelection += m_pRenderer->RenderText(NO_SELECTION, m_iMouseY, "HELL", (iYpos += 100), TITLE_FONT, 0xff550000);
	iSelection += m_pRenderer->RenderText(NO_SELECTION, m_iMouseY, "Play Game", (iYpos += 150), SUBTITLE_FONT, defaultColor);

	// Print the Menu Options
	iSelection += m_pRenderer->RenderText(HOST_GAME, m_iMouseY, "Host a Game", (iYpos += 150), MENU_FONT, defaultColor);
	iSelection += m_pRenderer->RenderText(JOIN_GAME, m_iMouseY, "Join a Game", (iYpos += 80), MENU_FONT, defaultColor);

	// Change the MenuSelection Variable to the menu enum
	if (m_bLeftMouseClick == true)
	{
		m_eMenuTempSelection = (eMenuSelection)(iSelection);
	}
	else
	{
		m_eMenuTempSelection = NO_SELECTION;
	}

}

bool CGameClient::CreateDataPacket()
{
	// TO DO - Create actual Data
	m_pClientToServer->bCommand = false;
	char* strCommand = "Testing - Client to Server with FALSE command boolean";

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
			// Add the Received Packet to the Work Queue
			m_pClientMutex->Wait();
			m_pWorkQueue->push(*_pReceiveData);
			m_pClientMutex->Signal();

			// TO DO - Remove
			int i = 0;
		}
	}
}

void CGameClient::ChangeMenuSelection()
{
	m_eMenuSelection = m_eMenuTempSelection;
}

bool CGameClient::CreateCommandPacket(std::string _strCommand)
{
	m_pClientToServer->bCommand = true;

	// Check that the structure was able to accept the data
	if (!(StringToStruct(_strCommand.c_str(), m_pClientToServer->cCommand)))
	{
		// Data invalid - Data Packet failed to create
		return false;
	}

	return true;
}