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
	if (m_bHost == true)
	{
		// Send a Terminate command to the Server
		CreateCommandPacket(TERMINATE_SERVER);
		m_pClientNetwork->SendPacket(m_pClientToServer);
	}

	// Send a left user packet - Invalid send if the user is not connected
	CreateCommandPacket(LEAVE_SERVER);
	m_pClientNetwork->SendPacket(m_pClientToServer);

	// Turn the Network off so the Receive thread can exit
	m_bNetworkOnline = false;
	m_ReceiveThread.join();

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

	// Delete the Available server vector
	if (m_pAvailableServers != 0)
	{
		delete m_pAvailableServers;
		m_pAvailableServers = 0;
	}

	// Delete the Users List
	delete m_pCurrentUsers;
	m_pCurrentUsers = 0;

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
	// TO DO - remove
	LPRECT clientRect = new RECT;
	LPRECT windowRect = new RECT;
	GetClientRect(_hWnd, clientRect);
	GetWindowRect(_hWnd, windowRect);


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

	// Set the Menu Variables
	m_strMenuSelection = "";
	m_bMenuSelected = false;
	m_strMenuTempSelection = "";
	m_iServerIndex = -1;

	// Game Variables
	m_bHost = false;

	// Set User Information
	m_pCurrentUsers = new std::map<std::string, UserInfo>;
	m_bAlive = false;


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

	// Determine if a Menu has been selected
	if (m_strMenuTempSelection != "")
	{
		if (m_bLeftMouseClick == false)
		{
			m_strMenuSelection = m_strMenuTempSelection;
			m_strMenuTempSelection = "";
		}
	}

	// Determine the correct process dependent on the current screen state
	ProcessScreenState();

	// Process all Packets in the Work Queue
	while (m_pWorkQueue->empty() == false)
	{
		m_pClientMutex->Wait();
		*m_pPacketToProcess = m_pWorkQueue->front();
		m_pWorkQueue->pop();
		m_pClientMutex->Signal();

		// Process the Pulled Packet
		ProcessPacket();
	}

	// Reset the Menu Selection
	m_strMenuSelection = "";
}

void CGameClient::ProcessTextInput(WPARAM _wKeyPress)
{
	switch (m_eScreenState)
	{
		case STATE_CREATEUSER:
		{
			ProcessUserNameInput(_wKeyPress);
			break;
		}
		case STATE_CREATE_SERVERNAME:
		{
			ProcessServerNameInput(_wKeyPress);
			break;
		}

	}	// End Switch (m_eScreenState)
}

void CGameClient::ProcessServerNameInput(WPARAM _wKeyPress)
{
	bool bAddLetter = false;
	char cInput = (char)(_wKeyPress);
	if (cInput >= 65 && cInput <= 90)		// Allows The alphabet
	{
		cInput += 32;	// Default to Lower case
		if (m_pbKeyDown[16] == true)	// Shift key is down
		{
			cInput -= 32;	// Change lowercase into uppercase
		}

		if (m_strServerName.length() < network::MAX_SERVERNAME_LENGTH - 1)
		{
			// Add letter if Server name isnt at length limit
			bAddLetter = true;
		}
	}
	else if ((cInput >= 48 && cInput <= 57)	// Allows Number
		|| (cInput == 32))					// Allows Spaces
	{
		if (cInput == ' ')
		{
			cInput = '_';
		}

		if (m_strServerName.length() < network::MAX_SERVERNAME_LENGTH - 1)
		{
			// Add letter if Server name isnt at length limit
			bAddLetter = true;
		}
	}
	else if (cInput == 8)	// backspace
	{
		if (m_strServerName.length() > 0)
		{
			m_strServerName.erase(m_strServerName.length() - 1);	// Remove last character
		}
	}
	else if (cInput == '\n'	// New line command
		|| cInput == '\r')	// Enter key
	{
		if (m_strServerName.length() != 0)
		{
			CreateServer();
		}
	}

	// Add the letter to the required string
	if (bAddLetter == true)
	{
		m_strServerName += cInput;
	}
}

void CGameClient::ProcessUserNameInput(WPARAM _wKeyPress)
{
	bool bAddLetter = false;
	char cInput = (char)(_wKeyPress);

	if (cInput >= 65 && cInput <= 90)		// Allows the Alphabet	
	{
		cInput += 32;	// Default to Lower case
		if (m_pbKeyDown[16] == true)	// Shift key is down
		{
			cInput -= 32;	// Change lowercase into uppercase
		}
		// Determine which name to edit
		if (m_strUserName.length() < network::MAX_USERNAME_LENGTH - 1)
		{
			// Add letter if Username isnt at length limit
			bAddLetter = true;
		}
	}
	else if ((cInput >= 48 && cInput <= 57)	// Allows Number
		|| (cInput == 32))					// Allows Spaces
	{
		if (cInput == ' ')
		{
			cInput = '_';
		}

		// Add letter if Username isnt at length limit
		if (m_strUserName.length() < network::MAX_USERNAME_LENGTH - 1)
		{
			
			bAddLetter = true;
		}
	}
	else if (cInput == 8)	// backspace
	{
		if (m_strUserName.length() > 0)
		{
			m_strUserName.erase(m_strUserName.length() - 1);	// Remove last character
		}
	}
	else if (cInput == '\n'	// New line command
		|| cInput == '\r')	// Enter key
	{
		if (m_strUserName.length() != 0)
		{
			if (m_bHost == true)
			{
				m_eScreenState = STATE_CREATE_SERVERNAME;
			}
			else
			{
				CreateCommandPacket(CREATEUSER);
				m_pClientNetwork->SendPacket(m_pClientToServer);
				Sleep(50);
			}
			return;
		}
	}

	// Add the letter to the required string
	if (bAddLetter == true)
	{
		m_strUserName += cInput;
	}
}

void CGameClient::ProcessPacket()
{
	if (m_pPacketToProcess->bCommand == true)
	{
		//Process a command code
		eNetworkCommand eProcessCommand = m_pPacketToProcess->eCommand;

		// Reply from the server when you have successfully gained control as the host
		if (eProcessCommand == HOST_SERVER)
		{
			InsertUser(m_strUserName, m_pPacketToProcess->UserInfos[0]);
		}
		// Reply when a server is saying they are available for connection
		else if (eProcessCommand == SERVER_CONNECTION_AVAILABLE)
		{
			// Create Server structure and add in the details from the received packet
			AvailableServer tempServer;
			strcpy_s(tempServer.cServerName, m_pPacketToProcess->cServerName);
			strcpy_s(tempServer.cHostName, m_pPacketToProcess->cUserName);
			tempServer.iCurrentClients = m_pPacketToProcess->CurrentUserCount;
			tempServer.ServerAddr = m_pPacketToProcess->ServerAddr;

			bool bInserted = false;

			// Insertion Sort the available servers
			for (unsigned int i = 0; i < m_pAvailableServers->size(); i++)
			{
				int iCompare = strcmp(tempServer.cServerName, (*m_pAvailableServers)[i].cServerName);
				if (iCompare <= 0)
				{
					// Add the server if the alphabetical order of the servername is less or equal
					m_pAvailableServers->insert(m_pAvailableServers->begin() + i, tempServer);
					bInserted = true;
					break;
				}
			}
			if (bInserted == false)
			{
				// Add the Server to the available servers vector
				m_pAvailableServers->push_back(tempServer);
			}
			return;
		}
		// Reply when accepted into a server
		else if (eProcessCommand == CREATEUSER_ACCEPTED)
		{
			// TO DO - look if this statement needs to be here
			//InsertUser(m_strUserName);
			m_eUserNameFailed = eProcessCommand;

			for (int i = 0; i < m_pPacketToProcess->CurrentUserCount; i++)
			{
				InsertUser(m_pPacketToProcess->UserInfos[i].cUserName, m_pPacketToProcess->UserInfos[i]);
			}

			m_eScreenState = STATE_GAME_LOBBY;	
		}
		// Reply if the chosen username is already in use on that server
		else if (eProcessCommand == CREATEUSER_NAMEINUSE)
		{
			// Username was already in use. Wipe username and start again
			//m_strUserName = "";
			m_eUserNameFailed = eProcessCommand;
			m_eScreenState = STATE_CREATEUSER;
		}
		// Reply if the Server was full
		else if (eProcessCommand == CREATEUSER_SERVERFULL)
		{
			// Server was full. Wipe username and start again
			//m_strUserName = "";
			m_eUserNameFailed = eProcessCommand;
			m_eScreenState = STATE_CREATEUSER;
		}
		// Add a user that has joined the server
		else if (eProcessCommand == USER_JOINED)
		{
				int iIndex;
				// Find the Index of the new user within the UserInfo List
				for (int i = 0; i < network::MAX_CLIENTS; i++)
				{
					if ((std::string)m_pPacketToProcess->cUserName == (std::string)m_pPacketToProcess->UserInfos[i].cUserName)
					{
						iIndex = i;
					}
				}

				InsertUser(m_pPacketToProcess->cUserName, m_pPacketToProcess->UserInfos[iIndex]);
		}
		// Delete users that have left
		else if (eProcessCommand == USER_LEFT)
		{
			// Delete the User from the list
			m_pCurrentUsers->erase(m_pPacketToProcess->cUserName);

		}
		else if (eProcessCommand == YOUR_HOST)
		{
			m_strServerHost = m_pPacketToProcess->cUserName;
		}
		else if (eProcessCommand == ALIVE_SET)
		{
			// Get the boolean value
			bool bAliveness;
			for (int i = 0; i < m_pPacketToProcess->CurrentUserCount; i++)
			{
				if ((std::string)m_pPacketToProcess->cUserName == (std::string)m_pPacketToProcess->UserInfos[i].cUserName)
				{
					bAliveness = m_pPacketToProcess->UserInfos[i].bAlive;
				}
			}

			// Check if the user in question is yourself
			if (m_pPacketToProcess->cUserName == m_strUserName)
			{
				m_bAlive = bAliveness;
			}

			// Find the user
			std::map<std::string, UserInfo>::iterator User = m_pCurrentUsers->find(m_pPacketToProcess->cUserName);
			User->second.bAlive = bAliveness;
		}
		// All users are ready. Start the Game
		else if (eProcessCommand == START_GAME)
		{
			m_eScreenState = STATE_GAME_PLAY;
		}
		// The server has been terminated
		else if (eProcessCommand == TERMINATE_SERVER)
		{
			if (m_eScreenState != STATE_MAIN_MENU)
			{
				m_eScreenState = STATE_TERMINATED_SERVER;
			}
		}
	}
	else
	{
		// Process as normal client input
	}
}

void CGameClient::ProcessScreenState()
{
	m_bTextInput = false;

	// Process dependent on the current screen state
	switch (m_eScreenState)
	{
	case STATE_GAME_PLAY:
	{

		break;
	}
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
		ProcessCreateUser();
		break;
	}
	case STATE_CREATE_SERVERNAME:
	{
		ProcessCreateServer();
		break;
	}
	case STATE_SELECT_SERVER:
	{
		ProcessSelectServer();
		break;
	}
	case STATE_GAME_LOBBY:
	{
		ProcessGameLobby();
		break;
	}
	case STATE_TERMINATED_SERVER:
	{
		ProcessTerminatedServer();
		break;
	}
	
	default: break;
	}	// End Switch(m_eScreenState)
}

void CGameClient::ProcessCreateUser()
{
	m_bTextInput = true;

	if (m_strMenuSelection == "Back")
	{
		if (m_bHost == true)
		{
			m_eScreenState = STATE_MULTIPLAYER_MENU;
		}
		else
		{
			m_eScreenState = STATE_SELECT_SERVER;
		}
	}
}

void CGameClient::ProcessCreateServer()
{
	m_bTextInput = true;

	if (m_strMenuSelection == "Back")
	{
		if (m_bHost == true)
		{
			m_eScreenState = STATE_CREATEUSER;
		}
		else
		{
			m_eScreenState = STATE_MAIN_MENU;
		}
	}
}

void CGameClient::ProcessMainMenu()
{
	m_bHost = false;

	if (m_strMenuSelection == "Single Player")
	{
		m_eScreenState = STATE_SINGLEPLAYER_MENU;
	}
	else if (m_strMenuSelection == "Multiplayer")
	{
		m_eScreenState = STATE_MULTIPLAYER_MENU;
	}
	else if (m_strMenuSelection == "Instructions")
	{
		m_eScreenState = STATE_INSTRUCTIONS_MENU;
	}
	else if (m_strMenuSelection == "Options")
	{
		m_eScreenState = STATE_OPTIONS_MENU;
	}
	else if (m_strMenuSelection == "Exit Game")
	{
		m_bNetworkOnline = false;
	}
}

void CGameClient::ProcessSingleMenu()
{

}

void CGameClient::ProcessMultiMenu()
{
	m_eUserNameFailed = ERROR_COMMAND;

	if (m_strMenuSelection == "Host a Game")
	{
		m_bHost = true;
		m_eScreenState = STATE_CREATEUSER;
		return;
	}
	else if (m_strMenuSelection == "Join a Game")
	{
		m_eScreenState = STATE_SELECT_SERVER;
	}
	else if (m_strMenuSelection == "Back")
	{
		ResetGameData();
		m_eScreenState = STATE_MAIN_MENU;
	}
}

void CGameClient::ProcessInstructionsMenu()
{
	if (m_strMenuSelection == "Back")
	{
		m_eScreenState = STATE_MAIN_MENU;
	}
}

void CGameClient::ProcessOptionsMenu()
{
	if (m_strMenuSelection == "Back")
	{
		m_eScreenState = STATE_MAIN_MENU;
	}
}

void CGameClient::ProcessGameLobby()
{
	if (m_strMenuSelection == "Exit")
	{
		m_eScreenState = STATE_MAIN_MENU;

		if (m_bHost == true)
		{
			CreateCommandPacket(TERMINATE_SERVER);
			m_pClientNetwork->SendPacket(m_pClientToServer);
		}
		else
		{
			CreateCommandPacket(LEAVE_SERVER);
			m_pClientNetwork->SendPacket(m_pClientToServer);
		}

		ResetGameData();
	}
	else if (m_strMenuSelection == "I am Ready!")
	{
		CreateCommandPacket(ALIVE_SET, "true");
		m_pClientNetwork->SendPacket(m_pClientToServer);
	}
	else if (m_strMenuSelection == "Actually I'm not ready!")
	{
		CreateCommandPacket(ALIVE_SET, "false");
		m_pClientNetwork->SendPacket(m_pClientToServer);
	}
}

void CGameClient::ProcessSelectServer()
{
	m_eUserNameFailed = ERROR_COMMAND;

	// Check if a server was selected
	if (m_iServerIndex != -1)
	{
		// Determine what server was selected
		m_strServerName = (*m_pAvailableServers)[m_iServerIndex].cServerName;
		m_pClientNetwork->SelectServer((*m_pAvailableServers)[m_iServerIndex].ServerAddr);
		m_eScreenState = STATE_CREATEUSER;

		m_iServerIndex = -1;
	}
	else if (m_strMenuSelection == "Back")
	{
		m_eScreenState = STATE_MULTIPLAYER_MENU;
	}
	
	// delete the vector for available servers
	if (m_pAvailableServers != 0)
	{
		delete m_pAvailableServers;
		m_pAvailableServers = 0;
	}
	
	m_pAvailableServers = new std::vector<AvailableServer>;
	
	// Broadcast to check which servers are available
	CreateCommandPacket(QUERY_CLIENT_CONNECTION);
	m_pClientNetwork->BroadcastToServers(m_pClientToServer);
	// TO DO - take this off a sleep function so it can be done less often ( like once every 5 secs or so)
	// Broadcast only 10 times a second to allow server responce
	Sleep(100);

	
}

void CGameClient::ProcessTerminatedServer()
{
	if (m_strMenuSelection == "Back to Main Menu")
	{
		m_eScreenState = STATE_MAIN_MENU;
		ResetGameData();
	}
}




void CGameClient::Draw()
{
	m_pRenderer->StartRender(true, true, false);

	switch (m_eScreenState)
	{
		case STATE_GAME_PLAY:
		{
		
			break;
		}
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
		case STATE_CREATEUSER:
		{
			CreateUserName();
			break;
		}
		case STATE_CREATE_SERVERNAME:
		{
			CreateServerName();
			break;
		}
		case STATE_SELECT_SERVER:
		{
			DisplaySelectServer();
			break;
		}
		case STATE_GAME_LOBBY:
		{
			DisplayGameLobby();
			break;
		}
		case STATE_TERMINATED_SERVER:
		{
			DisplayTerminatedServer();
			break;
		}
		
		default: break;
		
	}	// End Switch(m_eScreenState)

	m_pRenderer->EndRender();
	int i = 0;
}

// #DisplayMenus
void CGameClient::CreateUserName()
{
	int iYpos = 0;
	D3DXCOLOR colorRed = 0xffff0000;
	D3DXCOLOR colorWhite = 0xffffffff;
	std::string strSelection = "";

	// Render the Backbuffer to Black
	m_pRenderer->RenderColor(0xff000000);

	// Print the Title Text
	iYpos = PrintFullTitle(iYpos);
	m_pRenderer->RenderText(false, m_iMouseY, "Create a User Name", (iYpos += 150), SUBTITLE_FONT, colorRed, H_CENTER);

	if (m_eUserNameFailed == CREATEUSER_NAMEINUSE)
	{
		m_pRenderer->RenderText(false, m_iMouseY, "Username already in use. Try again", (iYpos += 80), SCREEN_FONT, colorRed, H_CENTER);
	}
	else if (m_eUserNameFailed == CREATEUSER_SERVERFULL)
	{
		m_pRenderer->RenderText(false, m_iMouseY, "Server is currently full. You may try again or exit", (iYpos += 80), SCREEN_FONT, colorRed, H_CENTER);
	}
	else
	{
		iYpos += 80;
	}

	m_pRenderer->RenderText(false, m_iMouseY, m_strUserName, (iYpos += 70), SUBTITLE_FONT, colorRed, H_CENTER);

	iYpos = m_iScreenHeight - 100;
	strSelection = m_pRenderer->RenderText(true, m_iMouseY, "Back", (iYpos), MENU_FONT, colorWhite, H_CENTER);

	// Change the MenuSelection Variable to the menu enum
	if (m_bLeftMouseClick == true)
	{
		m_strMenuTempSelection = strSelection;
	}
	else
	{
		m_strMenuTempSelection = "";
	}
}

void CGameClient::CreateServerName()
{
	int iYpos = 0;
	D3DXCOLOR colorRed = 0xffff0000;
	D3DXCOLOR colorWhite = 0xffffffff;
	std::string strSelection = "";

	// Render the Backbuffer to Black
	m_pRenderer->RenderColor(0xff000000);

	// Print the Title Text
	iYpos = PrintFullTitle(iYpos);
	m_pRenderer->RenderText(false, m_iMouseY, "Name your Server", (iYpos += 150), SUBTITLE_FONT, colorRed, H_CENTER);

	m_pRenderer->RenderText(false, m_iMouseY, m_strServerName, (iYpos += 150), SUBTITLE_FONT, colorRed, H_CENTER);

	iYpos = m_iScreenHeight - 100;
	strSelection = m_pRenderer->RenderText(true, m_iMouseY, "Back", (iYpos), MENU_FONT, colorWhite, H_CENTER);

	// Change the MenuSelection Variable to the menu enum
	if (m_bLeftMouseClick == true)
	{
		m_strMenuTempSelection = strSelection;
	}
	else
	{
		m_strMenuTempSelection = "";
	}
}

void CGameClient::DisplayMainMenu()
{
	int iYpos = 0;
	D3DXCOLOR colorRed = 0xffff0000;
	D3DXCOLOR colorWhite = 0xffffffff;
	std::string strSelection = "";

	// Render the Backbuffer to Black
	m_pRenderer->RenderColor(0xff000000);

	// Print the Title
	iYpos = PrintFullTitle(iYpos);
	m_pRenderer->RenderText(false, m_iMouseY, "Main Menu", (iYpos += 150), SUBTITLE_FONT, colorRed, H_CENTER);

	// Print the Menu Options
	strSelection += m_pRenderer->RenderText(true, m_iMouseY, "Single Player", (iYpos += 120), MENU_FONT, colorWhite, H_CENTER);
	strSelection += m_pRenderer->RenderText(true, m_iMouseY, "Multiplayer", (iYpos += 60), MENU_FONT, colorWhite, H_CENTER);
	strSelection += m_pRenderer->RenderText(true, m_iMouseY, "Instructions", (iYpos += 60), MENU_FONT, colorWhite, H_CENTER);
	strSelection += m_pRenderer->RenderText(true, m_iMouseY, "Options", (iYpos += 60), MENU_FONT, colorWhite, H_CENTER);
	strSelection += m_pRenderer->RenderText(true, m_iMouseY, "Exit Game", (iYpos += 60), MENU_FONT, colorWhite, H_CENTER);

	// Change the MenuSelection Variable to the menu enum
	if (m_bLeftMouseClick == true)
	{
		m_strMenuTempSelection = strSelection;
	}
	else
	{
		m_strMenuTempSelection = "";
	}
}

void CGameClient::DisplayMultiplayerMenu()
{
	int iYpos = 0;
	D3DXCOLOR colorRed = 0xffff0000;
	D3DXCOLOR colorWhite = 0xffffffff;
	std::string strSelection = "";

	// Render the Backbuffer to Black
	m_pRenderer->RenderColor(0xff000000);

	// Print the Title Text
	iYpos = PrintFullTitle(iYpos);
	m_pRenderer->RenderText(false, m_iMouseY, "Multiplayer", (iYpos += 150), SUBTITLE_FONT, colorRed, H_CENTER);

	// Print the Menu Options
	strSelection += m_pRenderer->RenderText(true, m_iMouseY, "Host a Game", (iYpos += 120), MENU_FONT, colorWhite, H_CENTER);
	strSelection += m_pRenderer->RenderText(true, m_iMouseY, "Join a Game", (iYpos += 60), MENU_FONT, colorWhite, H_CENTER);
	strSelection += m_pRenderer->RenderText(true, m_iMouseY, "Back", (iYpos += 100), MENU_FONT, colorWhite, H_CENTER);

	// Change the MenuSelection Variable to the menu enum
	if (m_bLeftMouseClick == true)
	{
		m_strMenuTempSelection = strSelection;
	}
	else
	{
		m_strMenuTempSelection = "";
	}
}

void CGameClient::DisplayInstructionsMenu()
{
	int iYpos = 0;
	D3DXCOLOR colorRed = 0xffff0000;
	D3DXCOLOR colorWhite = 0xffffffff;
	std::string strSelection = "";

	// Render the Backbuffer to Black
	m_pRenderer->RenderColor(0xff000000);

	// Print the Title Text
	iYpos = PrintFullTitle(iYpos);
	m_pRenderer->RenderText(false, m_iMouseY, "Instructions", (iYpos += 150), SUBTITLE_FONT, colorRed, H_CENTER);

	// Print the Menu Options
	m_pRenderer->RenderText(false, m_iMouseY, "Input Instructions Here", (iYpos += 120), MENU_FONT, colorWhite, H_CENTER);
	strSelection += m_pRenderer->RenderText(true, m_iMouseY, "Back", (iYpos += 100), MENU_FONT, colorWhite, H_CENTER);

	// Change the MenuSelection Variable to the menu enum
	if (m_bLeftMouseClick == true)
	{
		m_strMenuTempSelection = strSelection;
	}
	else
	{
		m_strMenuTempSelection = "";
	}
}

void CGameClient::DisplayOptionsMenu()
{
	int iYpos = 0;
	D3DXCOLOR colorRed = 0xffff0000;
	D3DXCOLOR colorWhite = 0xffffffff;
	std::string strSelection = "";

	// Render the Backbuffer to Black
	m_pRenderer->RenderColor(0xff000000);

	// Print the Title Text
	iYpos = PrintFullTitle(iYpos);
	m_pRenderer->RenderText(false, m_iMouseY, "Options", (iYpos += 150), SUBTITLE_FONT, colorRed, H_CENTER);

	// Print the Menu Options
	m_pRenderer->RenderText(false, m_iMouseY, "Input Options Here", (iYpos += 120), MENU_FONT, colorWhite, H_CENTER);
	strSelection += m_pRenderer->RenderText(true, m_iMouseY, "Back", (iYpos += 100), MENU_FONT, colorWhite, H_CENTER);

	// Change the MenuSelection Variable to the menu enum
	if (m_bLeftMouseClick == true)
	{
		m_strMenuTempSelection = strSelection;
	}
	else
	{
		m_strMenuTempSelection = "";
	}
}

void CGameClient::DisplayGameLobby()
{
	int iYpos = 0;
	D3DXCOLOR colorRed = 0xffff0000;
	D3DXCOLOR colorWhite = 0xffffffff;
	std::string strSelection = "";

	// Render the Backbuffer to Black
	m_pRenderer->RenderColor(0xff000000);

	// Print the Title Text
	m_pRenderer->RenderText(false, m_iMouseY, "Game Lobby", (iYpos += 50), TITLE_FONT, colorRed, H_CENTER);
	m_pRenderer->RenderText(false, m_iMouseY, "Server Name : " + m_strServerName, (iYpos += 150), SCREEN_FONT, colorRed, H_LEFT);
	m_pRenderer->RenderText(false, m_iMouseY, "Server Host : " + m_strServerHost, (iYpos += 30), SCREEN_FONT, colorRed, H_LEFT);

	iYpos += 40;
	m_pRenderer->RenderText(false, m_iMouseY, "  Current Users", (iYpos += 30), SCREEN_FONT, colorRed, H_LEFT);
	m_pRenderer->RenderText(false, m_iMouseY, "Status", (iYpos), SCREEN_FONT, colorRed, H_RIGHT);

	std::map<std::string, UserInfo>::iterator iterUsersCurrent = m_pCurrentUsers->begin();

	while (iterUsersCurrent != m_pCurrentUsers->end())
	{
		if (iterUsersCurrent == m_pCurrentUsers->begin())
		{
			// Don't move the text down by the full 60 the first time
			iYpos -= 30;
		}
		m_pRenderer->RenderText(false, m_iMouseY, iterUsersCurrent->first, (iYpos += 60), MENU_FONT, colorRed, H_LEFT);
		D3DXCOLOR colorReady = (iterUsersCurrent->second.bAlive == true) ? colorRed : (0xff440000);
		m_pRenderer->RenderText(false, m_iMouseY, "Ready", (iYpos), MENU_FONT, colorReady, H_RIGHT);

		iterUsersCurrent++;
	}

	iYpos = m_iScreenHeight - 150;
	std::string strReadyState = (m_bAlive == true) ? ("Actually I'm not ready!") : ("I am Ready!");
	strSelection += m_pRenderer->RenderText(true, m_iMouseY, strReadyState, (iYpos), MENU_FONT, colorWhite, H_CENTER);
	strSelection += m_pRenderer->RenderText(true, m_iMouseY, "Exit", (iYpos += 60), MENU_FONT, colorWhite, H_CENTER);

	// Change the MenuSelection Variable to the menu enum
	if (m_bLeftMouseClick == true)
	{
		m_strMenuTempSelection = strSelection;
	}
	else
	{
		m_strMenuTempSelection = "";
	}
}

void CGameClient::DisplaySelectServer()
{
	int iYpos = 0;
	D3DXCOLOR colorRed = 0xffff0000;
	D3DXCOLOR colorWhite = 0xffffffff;
	std::string strSelection = "";
	int iServerIndex = -1;

	// Render the Backbuffer to Black
	m_pRenderer->RenderColor(0xff000000);

	// Print the Title Text
	m_pRenderer->RenderText(false, m_iMouseY, "Available Servers", (iYpos += 50), SUBTITLE_FONT, colorRed, H_CENTER);
	iYpos += 120;

	// Print the Headings
	m_pRenderer->RenderText(false, m_iMouseY, "Server Name", (iYpos += 40), MENU_FONT, colorRed, H_LEFT);
	m_pRenderer->RenderText(false, m_iMouseY, "Host", (iYpos), MENU_FONT, colorRed, H_CENTER);
	m_pRenderer->RenderText(false, m_iMouseY, "Users", (iYpos), MENU_FONT, colorRed, H_RIGHT);

	iYpos += 50;

	if (m_pAvailableServers != 0)
	{
		if (m_pAvailableServers->size() > 0)
		{
			for (unsigned int i = 0; i < m_pAvailableServers->size(); i++)
			{
				int iCurrentClients = (*m_pAvailableServers)[i].iCurrentClients;
				std::string strCurrentClients;
				bool bSelectable;
				D3DCOLOR color;

				if (iCurrentClients >= network::MAX_CLIENTS)
				{
					strCurrentClients = "FULL";
					bSelectable = false;
					color = colorRed;
				}
				else
				{
					strCurrentClients = std::to_string((*m_pAvailableServers)[i].iCurrentClients) + " / " + std::to_string(network::MAX_CLIENTS);
					bSelectable = true;
					color = colorWhite;
				}

				strSelection += m_pRenderer->RenderText(bSelectable, m_iMouseY, (*m_pAvailableServers)[i].cServerName, (iYpos += 30), SCREEN_FONT, color, H_LEFT);
				m_pRenderer->RenderText(bSelectable, m_iMouseY, (*m_pAvailableServers)[i].cHostName, (iYpos), SCREEN_FONT, color, H_CENTER);
				m_pRenderer->RenderText(bSelectable, m_iMouseY, strCurrentClients, (iYpos), SCREEN_FONT, color, H_RIGHT);

				if (strSelection != "" && iServerIndex == -1)
				{
					iServerIndex = i;
				}
			}
		}
		else
		{
			m_pRenderer->RenderText(false, m_iMouseY, "No Servers are Currently Active", (iYpos += 300), SCREEN_FONT, colorRed, H_CENTER);
		}
	}

	iYpos = m_iScreenHeight - 100;
	strSelection += m_pRenderer->RenderText(true, m_iMouseY, "Back", (iYpos), MENU_FONT, colorWhite, H_CENTER);

	// Change the MenuSelection Variable to the menu enum
	if (m_bLeftMouseClick == true)
	{
		m_strMenuTempSelection = strSelection;
		m_iServerIndex = iServerIndex;
	}
	else
	{
		m_strMenuTempSelection = "";
		m_iServerIndex = -1;
	}
	
}

void CGameClient::DisplayTerminatedServer()
{
	int iYpos = 0;
	D3DXCOLOR colorRed = 0xffff0000;
	D3DXCOLOR colorWhite = 0xffffffff;
	std::string strSelection = "";

	// Render the Backbuffer to Black
	m_pRenderer->RenderColor(0xff000000);

	// Print the Title Text
	PrintFullTitle(iYpos);

	// Print the Menu Options
	m_pRenderer->RenderText(false, m_iMouseY, "Connection has been lost.", (iYpos += 270), SCREEN_FONT, colorRed, H_CENTER);
	m_pRenderer->RenderText(false, m_iMouseY, "Server has been terminated", (iYpos += 30), SCREEN_FONT, colorRed, H_CENTER);

	iYpos = m_iScreenHeight - 100;
	strSelection += m_pRenderer->RenderText(true, m_iMouseY, "Back to Main Menu", (iYpos += 30), MENU_FONT, colorWhite, H_CENTER);

	// Change the MenuSelection Variable to the menu enum
	if (m_bLeftMouseClick == true)
	{
		m_strMenuTempSelection = strSelection;
	}
	else
	{
		m_strMenuTempSelection = "";
	}
}

int CGameClient::PrintFullTitle(int _iYpos)
{
	// Print the Title Text
	m_pRenderer->RenderText(false, m_iMouseY, "ROBOTRON", (_iYpos += 50), SUBTITLE_FONT, 0xff450000, H_CENTER);
	m_pRenderer->RenderText(false, m_iMouseY, "HORDES", (_iYpos += 80), TITLE_FONT, 0xff650000, H_CENTER);
	m_pRenderer->RenderText(false, m_iMouseY, "OF", (_iYpos += 100), TITLE_FONT, 0xff7c0000, H_CENTER);
	m_pRenderer->RenderText(false, m_iMouseY, "HELL", (_iYpos += 100), TITLE_FONT, 0xffa50000, H_CENTER);

	return _iYpos;
}

// #Packets
bool CGameClient::CreateDataPacket()
{
	// TO DO - create actual data
	return true;
}

bool CGameClient::CreateCommandPacket(eNetworkCommand _eCommand)
{
	// Clear out old data from the Data Packet packet
	ZeroMemory(*(&m_pServerToClient), sizeof(*m_pServerToClient));

	// Change the Packet Data to contain a Command
	m_pClientToServer->bCommand = true;

	// Add the Command to The Packet structure
	m_pClientToServer->eCommand = _eCommand;

	// Add the Username of the Client to the Packet structure
	if (!(StringToStruct(m_strUserName.c_str(), m_pClientToServer->cUserName, network::MAX_USERNAME_LENGTH)))
	{
		return false;	// Data invalid - Data Packet failed to create
	}

	return true;
}

bool CGameClient::CreateCommandPacket(eNetworkCommand _eCommand, std::string _strMessage)
{
	// Create a general command packet
	CreateCommandPacket(_eCommand);

	// Add the additional meesage
	if (!(StringToStruct(_strMessage.c_str(), m_pClientToServer->cAdditionalMessage, network::MAX_CHAR_LENGTH)))
	{
		return false;	// Data invalid - Data Packet failed to create
	}

	return true;
}

// #Extras
void CGameClient::CreateServer()
{
	std::string strFilename;
	#ifdef _DEBUG
		strFilename = "..\\Debug\\Robotron Server";
	#endif // _DEBUG
	#ifndef _DEBUG
		// TO DO - change file path when handing in final build
		strFilename = "..\\Release\\Robotron Server";
	#endif // Not _DEBUG

	// Start the Server executable running
	std::string strOpenParameters = m_strUserName + " " + m_strServerName;
	int iError = (int)(ShellExecuteA(m_hWnd, "open", strFilename.c_str(), strOpenParameters.c_str(), NULL, SW_MINIMIZE));

	// Sleep to give the server time to start up
	Sleep(200);
	CreateCommandPacket(QUERY_HOST, m_strServerName);
	m_pClientNetwork->BroadcastToServers(m_pClientToServer);

	m_strServerHost = m_strUserName;
	m_eScreenState = STATE_GAME_LOBBY;
}

bool CGameClient::StringToStruct(const char* _pcData, char* _pcStruct, unsigned int _iMaxLength)
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
		}
	}
}

void CGameClient::ChangeMenuSelection()
{
	//m_strMenuSelection = m_eMenuTempSelection;
}

void CGameClient::InsertUser(std::string _strUser, UserInfo _UserInfo)
{
	std::pair<std::string, UserInfo> pairUser(_strUser, _UserInfo);
	m_pCurrentUsers->insert(pairUser);
}

void CGameClient::ResetGameData()
{
	m_bHost = false;
	m_bAlive = false;

	// remove all users from the map
	m_pCurrentUsers->clear();

	m_strServerName = "";
	m_strServerHost = "";

	m_pClientNetwork->Reset();
}