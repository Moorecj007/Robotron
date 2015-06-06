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

// #Constructors/Destructors
CGameClient::CGameClient()
{
}

CGameClient::~CGameClient()
{
	// Turn the Network off so the Receive thread can exit
	m_bNetworkOnline = false;
	m_ReceiveThread.join();

	if (m_bHost == true)
	{
		// Send a Terminate command to the Server
		CreateCommandPacket(TERMINATE_SERVER);
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
	delete m_pPacketToProcess;
	m_pPacketToProcess = 0;

	// Delete the ClientMutex
	delete m_pClientMutex;
	m_pClientMutex = 0;

	// Delete the WorkQueue
	delete m_pWorkQueue;
	m_pWorkQueue = 0;

	// Delete the Renderer
	delete m_pRenderer;
	m_pRenderer = 0;

	// Delete the Input Array
	delete m_pbKeyDown;
	m_pbKeyDown = 0;

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

// #Setters
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
	m_pPacketToProcess = new ServerToClient();

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


	return true;
}

bool CGameClient::RenderOneFrame()
{
	if (m_bNetworkOnline == false)
	{
		return false;
	}

	Process();
	Draw();

	return true;
}

// #Processes
void CGameClient::Process()
{
	// Process the Clock and get the new Delta Tick
	m_pClock->Process();
	float fDT = m_pClock->GetDeltaTick();

	// Process all Packets in the Work Queue
	// TO DO - Potentially limit to 1 - 3 packets per process loop
	while (m_pWorkQueue->empty() == false)
	{
		m_pClientMutex->Wait();
		*m_pPacketToProcess = m_pWorkQueue->front();
		m_pWorkQueue->pop();
		m_pClientMutex->Signal();

		// Process the Pulled Packet
		ProcessPacket();
	}

	// Determine the correct process dependent on the current screen state
	ProcessScreenState();

	// Reset the Menu Selection
	m_eMenuSelection = NO_SELECTION;
}

void CGameClient::ProcessPacket()
{
	if (m_pPacketToProcess->bCommand == true)
	{
		//Process a command code

		// TO DO - remove
		int i = 0;
		

	}
	else
	{
		// Process as normal client input

		
	}
}

void CGameClient::ProcessScreenState()
{
	// Process dependent on the current screen state
	switch (m_eScreenState)
	{
	case STATE_MAIN_MENU:
	{
		ProcessMainMenu();
		break;
	}
	case STATE_SINGLEPLAYER_MENU:
	{
		ProcessSingleMenu();
		break;
	}
	case STATE_MULTIPLAYER_MENU:
	{
		ProcessMultiMenu();
		break;
	}
	case STATE_INSTRUCTIONS_MENU:
	{
		ProcessInstructionsMenu();
		break;
	}
	case STATE_OPTIONS_MENU:
	{
		ProcessOptionsMenu();
		break;
	}
	case STATE_CREATEUSER:
	{
		CreateUserName(m_bCreatingUserforHost);
		break;
	}
	case STATE_GAME_PLAY:
	{

		break;
	}
	default: break;
	}	// End Switch(m_eScreenState)
}

void CGameClient::ProcessMainMenu()
{
	if (m_eMenuSelection == SINGLE_PLAYER)
	{
		m_eScreenState = STATE_SINGLEPLAYER_MENU;
	}
	else if (m_eMenuSelection == MULTIPLAYER)
	{
		m_eScreenState = STATE_MULTIPLAYER_MENU;
	}
	else if (m_eMenuSelection == INSTRUCTIONS)
	{
		m_eScreenState = STATE_INSTRUCTIONS_MENU;
	}
	else if (m_eMenuSelection == OPTIONS)
	{
		m_eScreenState = STATE_OPTIONS_MENU;
	}
	else if (m_eMenuSelection == EXIT_GAME)
	{
		m_bNetworkOnline = false;
	}
}

void CGameClient::ProcessSingleMenu()
{

}

void CGameClient::ProcessMultiMenu()
{
	if (m_eMenuSelection == HOST_GAME)
	{
		//m_bCreatingUserforHost = true;
		//m_eScreenState = STATE_CREATEUSER;
		//return;

		std::string strFilename;
		#ifdef _DEBUG
			strFilename = "..\\Debug\\Robotron Server";
		#endif // _DEBUG
		#ifndef _DEBUG
			// TO DO - change file path when handing in final build
			strFilename = "..\\Release\\Robotron Server";
		#endif // Not _DEBUG

		// Start the Server executable running
			// TO DO - Create Username before hand
		m_strUserName = "KortazaHost";
		int iError = (int)(ShellExecuteA(m_hWnd, "open", strFilename.c_str(), m_strUserName.c_str(), NULL, SW_NORMAL));
		m_bHost = true;

		// TO DO - remove sleep and add input user name
		Sleep(50);
		CreateCommandPacket(QUERY_HOST);
		m_pClientNetwork->BroadcastToServers(m_pClientToServer);
	}
	else if (m_eMenuSelection == JOIN_GAME)
	{

	}
	else if (m_eMenuSelection == MULTIPLAYER_BACK)
	{
		m_eScreenState = STATE_MAIN_MENU;
	}
}

void CGameClient::ProcessInstructionsMenu()
{
	if (m_eMenuSelection == INSTRUCTIONS_BACK)
	{
		m_eScreenState = STATE_MAIN_MENU;
	}
}

void CGameClient::ProcessOptionsMenu()
{
	if (m_eMenuSelection == OPTIONS_BACK)
	{
		m_eScreenState = STATE_MAIN_MENU;
	}
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
		case STATE_SINGLEPLAYER_MENU:
		{
			//DisplaySingleplayerMenu();
			break;
		}
		case STATE_MULTIPLAYER_MENU:
		{
			DisplayMultiplayerMenu();
			break;
		}
		case STATE_INSTRUCTIONS_MENU:
		{
			DisplayInstructionsMenu();
			break;
		}
		case STATE_OPTIONS_MENU:
		{
			DisplayOptionsMenu();
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

// #DisplayMenus
void CGameClient::CreateUserName(bool _bHost)
{
	int iYpos = 0;
	D3DXCOLOR defaultColor = 0xffff0000;

	// Render the Backbuffer to Black
	m_pRenderer->RenderColor(0xff000000);

	// Print the Title Text
	m_pRenderer->RenderText(NO_SELECTION, m_iMouseY, "ROBOTRON", (iYpos += 50), SUBTITLE_FONT, defaultColor);
	m_pRenderer->RenderText(NO_SELECTION, m_iMouseY, "HORDES", (iYpos += 80), TITLE_FONT, 0xff550000);
	m_pRenderer->RenderText(NO_SELECTION, m_iMouseY, "OF", (iYpos += 100), TITLE_FONT, 0xff550000);
	m_pRenderer->RenderText(NO_SELECTION, m_iMouseY, "HELL", (iYpos += 100), TITLE_FONT, 0xff550000);
	m_pRenderer->RenderText(NO_SELECTION, m_iMouseY, "Create a User Name", (iYpos += 150), SUBTITLE_FONT, defaultColor);

	m_pRenderer->RenderText(NO_SELECTION, m_iMouseY, m_strUserName, (iYpos += 150), SUBTITLE_FONT, defaultColor);
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
	iSelection += m_pRenderer->RenderText(SINGLE_PLAYER, m_iMouseY, "Single Player", (iYpos += 120), MENU_FONT, defaultColor);
	iSelection += m_pRenderer->RenderText(MULTIPLAYER, m_iMouseY, "Multiplayer", (iYpos += 60), MENU_FONT, defaultColor);
	iSelection += m_pRenderer->RenderText(INSTRUCTIONS, m_iMouseY, "Instructions", (iYpos += 60), MENU_FONT, defaultColor);
	iSelection += m_pRenderer->RenderText(OPTIONS, m_iMouseY, "Options", (iYpos += 60), MENU_FONT, defaultColor);
	iSelection += m_pRenderer->RenderText(EXIT_GAME, m_iMouseY, "Exit Game", (iYpos += 60), MENU_FONT, defaultColor);

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

void CGameClient::DisplayMultiplayerMenu()
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
	iSelection += m_pRenderer->RenderText(NO_SELECTION, m_iMouseY, "Multiplayer", (iYpos += 150), SUBTITLE_FONT, defaultColor);

	// Print the Menu Options
	iSelection += m_pRenderer->RenderText(HOST_GAME, m_iMouseY, "Host a Game", (iYpos += 120), MENU_FONT, defaultColor);
	iSelection += m_pRenderer->RenderText(JOIN_GAME, m_iMouseY, "Join a Game", (iYpos += 60), MENU_FONT, defaultColor);
	iSelection += m_pRenderer->RenderText(MULTIPLAYER_BACK, m_iMouseY, "Back", (iYpos += 100), MENU_FONT, defaultColor);

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

void CGameClient::DisplayInstructionsMenu()
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
	iSelection += m_pRenderer->RenderText(NO_SELECTION, m_iMouseY, "Instructions", (iYpos += 150), SUBTITLE_FONT, defaultColor);

	// Print the Menu Options
	iSelection += m_pRenderer->RenderText(INSTRUCTIONS_BACK, m_iMouseY, "Input Instructions Here", (iYpos += 120), MENU_FONT, defaultColor);
	//iSelection += m_pRenderer->RenderText(JOIN_GAME, m_iMouseY, "Join a Game", (iYpos += 60), MENU_FONT, defaultColor);
	iSelection += m_pRenderer->RenderText(INSTRUCTIONS_BACK, m_iMouseY, "Back", (iYpos += 100), MENU_FONT, defaultColor);

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

void CGameClient::DisplayOptionsMenu()
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
	iSelection += m_pRenderer->RenderText(NO_SELECTION, m_iMouseY, "Options", (iYpos += 150), SUBTITLE_FONT, defaultColor);

	// Print the Menu Options
	iSelection += m_pRenderer->RenderText(OPTIONS_BACK, m_iMouseY, "Input Options Here", (iYpos += 120), MENU_FONT, defaultColor);
	//iSelection += m_pRenderer->RenderText(JOIN_GAME, m_iMouseY, "Join a Game", (iYpos += 60), MENU_FONT, defaultColor);
	iSelection += m_pRenderer->RenderText(OPTIONS_BACK, m_iMouseY, "Back", (iYpos += 100), MENU_FONT, defaultColor);

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

// #Packets
bool CGameClient::CreateDataPacket()
{
	// TO DO - create actual data
	return true;
}

bool CGameClient::CreateCommandPacket(eNetworkCommand _eCommand)
{
	// Change the Packet Data to contain a Command
	m_pClientToServer->bCommand = true;

	// Add the Command to The Packet structure
	m_pClientToServer->eCommand = _eCommand;

	// Add the Username of the Client to the Packet structure
	if (!(StringToStruct(m_strUserName.c_str(), m_pClientToServer->cUserName)))
	{
		return false;	// Data invalid - Data Packet failed to create
	}

	return true;
}

// #Extras
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
		if (m_pClientNetwork->ReceivePacket(_pReceiveData) == true)
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

