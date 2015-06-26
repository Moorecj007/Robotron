/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Hub_Client.cpp
* Description : Runs the main game functions for the Client - The game manager
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Includes
#include "Hub_Client.h"

// Static Variables
CHub_Client* CHub_Client::s_pGame = 0;
CMySemaphore* CHub_Client::m_pClientMutex = new CMySemaphore(1);

// #Constructors/Destructors
CHub_Client::CHub_Client()
{
}

CHub_Client::~CHub_Client()
{
	if (m_bHost == true)
	{
		// Send a Terminate command to the Server
		CreateCommandPacket(TERMINATE_SERVER);
		m_pNetworkClient->SendPacket(m_pClientToServer);
	}

	// Send a left user packet - Invalid send if the user is not connected
	CreateCommandPacket(LEAVE_SERVER);
	m_pNetworkClient->SendPacket(m_pClientToServer);

	// Turn the Network off so the Receive thread can exit
	m_bNetworkOnline = false;
	m_ReceiveThread.join();

	// Delete the Clock
	delete m_pClock;
	m_pClock = 0;

	// Delete Client Network
	delete m_pNetworkClient;
	m_pNetworkClient = 0;

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

	// Delete the Game Mechanics handler
	delete m_pMechanics;
	m_pMechanics = 0;

	// Destroy the DirectInput
	m_pDInput->Shutdown();
	delete m_pDInput;
	m_pDInput = 0;
}

CHub_Client& CHub_Client::GetInstance()
{
	if (s_pGame == 0)
	{
		s_pGame = new CHub_Client();
	}
	return (*s_pGame);
}

void CHub_Client::DestroyInstance()
{
	delete s_pGame;
	s_pGame = 0;
}

// #Setters
void CHub_Client::SetMousePos(int _iX, int _iY)
{
	m_ptMousePos.x = _iX;
	m_ptMousePos.y = _iY;
}

bool CHub_Client::Initialise(HINSTANCE _hInstance, HWND _hWnd, int _iScreenWidth, int _iScreenHeight)
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
	m_pNetworkClient = new CNetwork_Client;
	VALIDATE(m_pNetworkClient->Initialise());
	m_bNetworkOnline = true;

	// Create a thread to receive data from the Server
	m_ReceiveThread = std::thread(&CHub_Client::ReceiveDataFromNetwork,(this), m_pServerToClient);

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

	// Set the Menu Variables
	m_strMenuSelection = "";
	m_bMenuSelected = false;
	m_strMenuTempSelection = "";
	m_iServerIndex = -1;
	m_bPauseKey = false;
	m_bPrevPauseKey = false;
	m_bDebugMode = false;
	m_bDebugKeyPrev = false;
	m_bFirstPerson = false;
	m_bFirstPersonKeyPrev = false;
	m_bWireFrame = false;

	// Game Variables
	m_bSinglePlayer = false;
	m_bHost = false;
	m_bFoundServer = false;
	m_bGameLoading = false;
	m_bLostDevice = false;

	// Set User Information
	m_pCurrentUsers = new std::map<std::string, AvatarInfo>;
	m_bAlive = false;

	// Initialise the DirectInput
	m_pDInput = new CDInput();
	VALIDATE(m_pDInput->Initialise(_hInstance, _hWnd, _iScreenWidth, _iScreenHeight));

	return true;
}

bool CHub_Client::RenderOneFrame()
{
	// Shut down the application if the network goes offline
	if (m_bNetworkOnline == false)
	{
		return false;
	}

	//if (CheckRenderer() == true)
	//{
		m_bLostDevice = false;
		// Snapshot Time at the beginning of the frame
		m_iFrameTimeStart = (int)timeGetTime();
		Sleep(1);

		Process();
		Draw();

		// Snapshot Time at the end of the frame
		m_iFrameTimeEnd = (int)timeGetTime();
		FrameLimiter();

	//}
	return true;

}

// #Processes
void CHub_Client::Process()
{
	// Process the Clock and get the new Delta Tick
	m_pClock->Process();
	float fDT = m_pClock->GetDeltaTick();

	// Process and retrieve the Input
	m_pDInput->Process();
	m_activatedControls = m_pDInput->GetControls();
	m_ptMousePos = m_pDInput->GetMousePos();

	// check if the pause screen has been activated
	if (m_bPrevPauseKey == true && m_activatedControls.bEsc == false)
	{
		m_bPauseKey = !m_bPauseKey;
	}
	m_bPrevPauseKey = m_activatedControls.bEsc;

	if (m_bSinglePlayer == true)
	{
		// Check if the Debug Camera Mode has been toggle on/off
		if (m_bDebugKeyPrev == true && m_activatedControls.bDebugToggle == false)
		{
			m_bDebugMode = !m_bDebugMode;
			m_pMechanics->ToggleDebugMode();
		}
		m_bDebugKeyPrev = m_activatedControls.bDebugToggle;

		if (m_bDebugMode == true)
		{
			DebugCameraInput(&m_activatedControls);
		}
	}

	if (m_bFirstPersonKeyPrev == true && m_activatedControls.bCameraViewToggle == false)
	{
		m_bFirstPerson = !m_bFirstPerson;
		m_pMechanics->ToggleViewMode();
	}
	m_bFirstPersonKeyPrev = m_activatedControls.bCameraViewToggle;

	// Determine if a Menu has been selected
	if (m_strMenuTempSelection != "")
	{
		if (m_activatedControls.bAction == false)
		{
			m_strMenuSelection = m_strMenuTempSelection;
			m_strMenuTempSelection = "";
		}
	}

	// Check if the the user in is the game or not
	if (m_eScreenState == STATE_GAME_PLAY)
	{
		if (m_pMechanics->CheckAvatarsAliveStatus() == false)
		{
			ProcessGameOverScreen();
		}
		else if (m_bPauseKey == true)
		{
			ProcessPausedScreen();
		}
		else
		{
			// Determine the correct process dependent on the current screen state
			ProcessScreenState(fDT);
		}
	}
	else
	{
		// Determine the correct process dependent on the current screen state
		ProcessScreenState(fDT);
	}

	// Reset the Menu Selection
	m_strMenuSelection = "";

	// Process all Packets in the Work Queue
	while (m_pWorkQueue->empty() == false)
	{
		m_pClientMutex->Wait();
		*m_pPacketToProcess = m_pWorkQueue->front();
		m_pWorkQueue->pop();
		m_pClientMutex->Signal();

		// Process the Pulled Packet
		ProcessPacket(fDT);
	}
}

void CHub_Client::ProcessTextInput(WPARAM _wKeyPress)
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

void CHub_Client::ProcessServerNameInput(WPARAM _wKeyPress)
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

void CHub_Client::ProcessUserNameInput(WPARAM _wKeyPress)
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
			if (m_bSinglePlayer == true)
			{
				CreateServer();
			}
			else if (m_bHost == true)
			{
				m_eScreenState = STATE_CREATE_SERVERNAME;
			}
			else
			{
				CreateCommandPacket(CREATEUSER);
				m_pNetworkClient->SendPacket(m_pClientToServer);
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

void CHub_Client::ProcessPacket(float _fDT)
{
	if (m_pPacketToProcess->bCommand == true)
	{
		//Process a command code
		eNetworkCommand eProcessCommand = m_pPacketToProcess->eCommand;

		// Reply from the server when you have successfully gained control as the host
		switch (eProcessCommand)
		{
			case HOST_SERVER:
			{
				if (m_bHost == true && m_bFoundServer == false)
				{
					std::string strCheckHost = (std::string)(m_pPacketToProcess->cUserName);
					std::string strCheckServer = (std::string)(m_pPacketToProcess->cServerName);

					if (strCheckHost == m_strServerHost
						&&	strCheckServer == m_strServerName)
					{
						m_bFoundServer = true;

						m_pNetworkClient->SelectServer(m_pPacketToProcess->ServerAddr);

						CreateCommandPacket(QUERY_HOST, m_strServerName);
						m_pNetworkClient->SendPacket(m_pClientToServer);
					}

					//InsertUser(m_strUserName, m_pPacketToProcess->Avatars[0]);
					//m_pMechanics->AddAvatar(m_pPacketToProcess);
				}
				break;
			}
			// Reply when a server is saying they are available for connection
			case SERVER_CONNECTION_AVAILABLE:
			{
				// Create Server structure and add in the details from the received packet
				AvailableServer tempServer;
				strcpy_s(tempServer.cServerName, m_pPacketToProcess->cServerName);
				strcpy_s(tempServer.cHostName, m_pPacketToProcess->cUserName);
				tempServer.iCurrentClients = m_pPacketToProcess->iCurrentUserCount;
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
				break;
			}
			// Reply when accepted into a server
			case CREATEUSER_ACCEPTED:
			{
				// TO DO - look if this statement needs to be here
				//InsertUser(m_strUserName);
				m_eUserNameFailed = eProcessCommand;

				for (int i = 0; i < m_pPacketToProcess->iCurrentUserCount; i++)
				{
					InsertUser(m_pPacketToProcess->Avatars[i].cUserName, m_pPacketToProcess->Avatars[i]);
					//m_pGameMechanics->AddAvatar(m_pPacketToProcess);
				}

				m_eScreenState = STATE_GAMELOADING;
				ProcessGameLoading();
				m_pMechanics->AddAllAvatars(m_pPacketToProcess);
				break;
			}
			// Reply if the chosen username is already in use on that server
			case CREATEUSER_NAMEINUSE:
			{
				// Username was already in use. Wipe username and start again
				//m_strUserName = "";
				m_eUserNameFailed = eProcessCommand;
				m_eScreenState = STATE_CREATEUSER;
				break;
			}
			// Reply if the Server was full
			case CREATEUSER_SERVERFULL:
			{
				// Server was full. Wipe username and start again
				//m_strUserName = "";
				m_eUserNameFailed = eProcessCommand;
				m_eScreenState = STATE_CREATEUSER;
				break;
			}
			// Add a user that has joined the server
			case USER_JOINED:
			{
				//if ((std::string)(m_pPacketToProcess->cUserName) != m_strUserName)
				//{
					int iIndex;
					// Find the Index of the new user within the UserInfo List
					for (int i = 0; i < network::MAX_CLIENTS; i++)
					{
						if ((std::string)m_pPacketToProcess->cUserName == (std::string)m_pPacketToProcess->Avatars[i].cUserName)
						{
							iIndex = i;
						}
					}

					InsertUser(m_pPacketToProcess->cUserName, m_pPacketToProcess->Avatars[iIndex]);
					m_pMechanics->AddAvatar(m_pPacketToProcess);
				//}
				break;
			}
			// Delete users that have left
			case USER_LEFT:
			{
				// Delete the User from the list
				m_pCurrentUsers->erase(m_pPacketToProcess->cUserName);
				m_pMechanics->RemoveAvatar(m_pPacketToProcess->cUserName);
				break;
			}
			case YOUR_HOST:
			{
				m_strServerHost = m_pPacketToProcess->cUserName;
				break;
			}
			case ALIVE_SET:
			{
				// Get the boolean value
				bool bAliveness;
				for (int i = 0; i < m_pPacketToProcess->iCurrentUserCount; i++)
				{
					if ((std::string)m_pPacketToProcess->cUserName == (std::string)m_pPacketToProcess->Avatars[i].cUserName)
					{
						bAliveness = m_pPacketToProcess->Avatars[i].bAlive;
					}
				}

				// Check if the user in question is yourself
				if (m_pPacketToProcess->cUserName == m_strUserName)
				{
					m_bAlive = bAliveness;
				}

				// Find the user
				std::map<std::string, AvatarInfo>::iterator User = m_pCurrentUsers->find(m_pPacketToProcess->cUserName);
				User->second.bAlive = bAliveness;
				break;
			}
			// All users are ready. Start the Game
			case START_GAME:
			{
				m_eScreenState = STATE_GAME_PLAY;
				break;
			}
			// The server has been terminated
			case TERMINATE_SERVER:
			{
				if (m_eScreenState != STATE_MAIN_MENU)
				{
					m_eScreenState = STATE_TERMINATED_SERVER;
				}
				break;
			}
			case CREATE_PROJECTILE:
			{
				m_pMechanics->SpawnProjectile(m_pPacketToProcess);
				break;
			}
			case DELETE_PROJECTILE:
			{
				m_pMechanics->DeleteProjectile(m_pPacketToProcess);
				break;
			}
			case CREATE_ENEMY:
			{
				m_pMechanics->SpawnEnemy(m_pPacketToProcess);
				break;
			}
			case DELETE_ENEMY:
			{
				m_pMechanics->DeleteEnemy(m_pPacketToProcess);
				break;
			}
			case CREATE_POWERUP:
			{
				m_pMechanics->SpawnPowerUp(m_pPacketToProcess);
				break;
			}
			case DELETE_POWERUP:
			{
				m_pMechanics->DeletePowerUp(m_pPacketToProcess);
				break;
			}
		}
	}
	else
	{
		if (m_eScreenState == STATE_GAME_PLAY 
			&& (std::string)m_pPacketToProcess->cServerName == m_strServerName)
		{
			m_pMechanics->Process(_fDT, m_pPacketToProcess);
		}
	}
}

void CHub_Client::ProcessScreenState(float _fDT)
{
	m_bTextInput = false;

	// Process dependent on the current screen state
	switch (m_eScreenState)
	{
	case STATE_GAME_PLAY:
	{
		m_pMechanics->Process(_fDT, m_pPacketToProcess);
		
		if (m_bPauseKey == false)
		{
			// Create Data packet for the user input
			CreateDataPacket();
			m_pNetworkClient->SendPacket(m_pClientToServer);
		}

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
	case STATE_GAMELOADING:
	{
		ProcessGameLoading();
		break;
	}
	
	default: break;
	}	// End Switch(m_eScreenState)
}

void CHub_Client::ProcessCreateUser()
{
	m_bTextInput = true;

	if (m_strMenuSelection == "Back")
	{
		if (m_bSinglePlayer == true)
		{
			ResetGameData();
			m_eScreenState = STATE_MAIN_MENU;
		}
		else if (m_bHost == true)
		{
			ResetGameData();
			m_eScreenState = STATE_MULTIPLAYER_MENU;
		}
		else
		{
			m_eScreenState = STATE_SELECT_SERVER;
		}
	}
}

void CHub_Client::ProcessCreateServer()
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

void CHub_Client::ProcessMainMenu()
{
	m_bHost = false;

	if (m_strMenuSelection == "Single Player")
	{
		m_bHost = true;
		m_bSinglePlayer = true;
		m_eScreenState = STATE_CREATEUSER;
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

void CHub_Client::ProcessSingleMenu()
{

}

void CHub_Client::ProcessMultiMenu()
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

void CHub_Client::ProcessInstructionsMenu()
{
	if (m_strMenuSelection == "Back")
	{
		m_eScreenState = STATE_MAIN_MENU;
	}
}

void CHub_Client::ProcessOptionsMenu()
{
	if (m_strMenuSelection == "Back")
	{
		m_eScreenState = STATE_MAIN_MENU;
	}
	else if (m_strMenuSelection == "Active")
	{
		m_bWireFrame = !m_bWireFrame;
		m_pRenderer->ToggleWireFrame(m_bWireFrame);
	}
}

void CHub_Client::ProcessGameLobby()
{
	if (m_strMenuSelection == "Exit")
	{
		m_eScreenState = STATE_MAIN_MENU;

		if (m_bHost == true)
		{
			CreateCommandPacket(TERMINATE_SERVER);
			m_pNetworkClient->SendPacket(m_pClientToServer);
		}
		else
		{
			CreateCommandPacket(LEAVE_SERVER);
			m_pNetworkClient->SendPacket(m_pClientToServer);
		}

		ResetGameData();
	}
	else if (m_strMenuSelection == "I am Ready!")
	{
		CreateCommandPacket(ALIVE_SET, "true");
		m_pNetworkClient->SendPacket(m_pClientToServer);
	}
	else if (m_strMenuSelection == "Actually I'm not ready!")
	{
		CreateCommandPacket(ALIVE_SET, "false");
		m_pNetworkClient->SendPacket(m_pClientToServer);
	}
}

void CHub_Client::ProcessSelectServer()
{
	m_eUserNameFailed = ERROR_COMMAND;

	// Check if a server was selected
	if (m_iServerIndex != -1)
	{
		// Determine what server was selected
		m_strServerName = (*m_pAvailableServers)[m_iServerIndex].cServerName;
		m_pNetworkClient->SelectServer((*m_pAvailableServers)[m_iServerIndex].ServerAddr);
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
	m_pNetworkClient->BroadcastToServers(m_pClientToServer);
	// TO DO - take this off a sleep function so it can be done less often ( like once every 5 secs or so)
	// Broadcast only 10 times a second to allow server responce
	Sleep(100);
}

void CHub_Client::ProcessTerminatedServer()
{
	if (m_strMenuSelection == "Back to Main Menu")
	{
		m_eScreenState = STATE_MAIN_MENU;
		ResetGameData();
	}
}

void CHub_Client::ProcessGameLoading()
{
	// Multithreaded loading screen
	m_bGameLoading = true;
	std::thread LoadingThread = std::thread(&CHub_Client::DisplayGameLoading, this);

	// Create the GameMechanics Object for handling the mechanics of the game
	m_pMechanics = new CMechanics_Client();
	m_pMechanics->Initialise(m_pRenderer, m_strUserName, m_bSinglePlayer);

	m_bGameLoading = false;
	LoadingThread.join();

	m_eScreenState = STATE_GAME_LOBBY;
}

void CHub_Client::ProcessPausedScreen()
{
	if (m_strMenuSelection == "Resume")
	{
		m_bPauseKey = false;
	}
	else if (m_strMenuSelection == "Options")
	{

	}
	else if (m_strMenuSelection == "Exit Game")
	{
		m_eScreenState = STATE_MAIN_MENU;

		if (m_bHost == true)
		{
			CreateCommandPacket(TERMINATE_SERVER);
			m_pNetworkClient->SendPacket(m_pClientToServer);
		}
		else
		{
			CreateCommandPacket(LEAVE_SERVER);
			m_pNetworkClient->SendPacket(m_pClientToServer);
		}

		ResetGameData();
		m_bPauseKey = false;
	}
}

void CHub_Client::ProcessGameOverScreen()
{
	if (m_strMenuSelection == "Exit To Main Menu")
	{
		if (m_bHost == true)
		{
			// Send a Terminate command to the Server
			CreateCommandPacket(TERMINATE_SERVER);
			m_pNetworkClient->SendPacket(m_pClientToServer);
		}
		ResetGameData();
		m_eScreenState = STATE_MAIN_MENU;
	}
}

// #Draw
void CHub_Client::Draw()
{
	
	m_pRenderer->StartRender(true, true, false);

	switch (m_eScreenState)
	{
		case STATE_GAME_PLAY:
		{
			m_pMechanics->Draw();

			if (m_pMechanics->CheckAvatarsAliveStatus() == false)
			{
				m_pMechanics->OverlayGameOver(&m_strMenuTempSelection, m_activatedControls.ptMouse, m_activatedControls.bAction);
			}
			else if (m_bPauseKey == true)
			{
				m_pMechanics->OverlayPauseScreen(&m_strMenuTempSelection, m_activatedControls.ptMouse, m_activatedControls.bAction);
			}
			// Render all avatars scores if the player presses the correct input
			else if (m_activatedControls.bTab == true)
			{
				m_pMechanics->OverlayAvatarScores(m_iFPS);
			}
			// Check to see if the player has opted to pause the game


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
		case STATE_GAMELOADING:
		{
			m_pRenderer->RenderColor(0xff000000);
			break;
		}

		default: break;

	}	// End Switch(m_eScreenState)

	m_pRenderer->EndRender();
}

// #DisplayMenus
void CHub_Client::CreateUserName()
{
	int iYpos = 0;
	D3DXCOLOR colorRed = 0xffff0000;
	D3DXCOLOR colorWhite = 0xffffffff;
	std::string strSelection = "";

	// Render the Backbuffer to Black
	m_pRenderer->RenderColor(0xff000000);

	// Print the Title Text
	iYpos = PrintFullTitle(iYpos);
	m_pRenderer->RenderText(false, m_ptMousePos, "Create a User Name", (iYpos += 150), SUBTITLE_FONT, colorRed, H_CENTER);

	if (m_eUserNameFailed == CREATEUSER_NAMEINUSE)
	{
		m_pRenderer->RenderText(false, m_ptMousePos, "Username already in use. Try again", (iYpos += 80), SCREEN_FONT, colorRed, H_CENTER);
	}
	else if (m_eUserNameFailed == CREATEUSER_SERVERFULL)
	{
		m_pRenderer->RenderText(false, m_ptMousePos, "Server is currently full. You may try again or exit", (iYpos += 80), SCREEN_FONT, colorRed, H_CENTER);
	}
	else
	{
		iYpos += 80;
	}

	m_pRenderer->RenderText(false, m_ptMousePos, m_strUserName, (iYpos += 70), SUBTITLE_FONT, colorRed, H_CENTER);

	iYpos = m_iScreenHeight - 100;
	strSelection = m_pRenderer->RenderText(true, m_ptMousePos, "Back", (iYpos), MENU_FONT, colorWhite, H_CENTER);

	// Change the MenuSelection Variable to the menu enum
	if (m_activatedControls.bAction == true)
	{
		m_strMenuTempSelection = strSelection;
	}
	else
	{
		m_strMenuTempSelection = "";
	}
}

void CHub_Client::CreateServerName()
{
	int iYpos = 0;
	D3DXCOLOR colorRed = 0xffff0000;
	D3DXCOLOR colorWhite = 0xffffffff;
	std::string strSelection = "";

	// Render the Backbuffer to Black
	m_pRenderer->RenderColor(0xff000000);

	// Print the Title Text
	iYpos = PrintFullTitle(iYpos);
	m_pRenderer->RenderText(false, m_ptMousePos, "Name your Server", (iYpos += 150), SUBTITLE_FONT, colorRed, H_CENTER);

	m_pRenderer->RenderText(false, m_ptMousePos, m_strServerName, (iYpos += 150), SUBTITLE_FONT, colorRed, H_CENTER);

	iYpos = m_iScreenHeight - 100;
	strSelection = m_pRenderer->RenderText(true, m_ptMousePos, "Back", (iYpos), MENU_FONT, colorWhite, H_CENTER);

	// Change the MenuSelection Variable to the menu enum
	if (m_activatedControls.bAction == true)
	{
		m_strMenuTempSelection = strSelection;
	}
	else
	{
		m_strMenuTempSelection = "";
	}
}

void CHub_Client::DisplayMainMenu()
{
	int iYpos = 0;
	D3DXCOLOR colorRed = 0xffff0000;
	D3DXCOLOR colorWhite = 0xffffffff;
	std::string strSelection = "";

	// Render the Backbuffer to Black
	m_pRenderer->RenderColor(0xff000000);

	// Print the Title
	iYpos = PrintFullTitle(iYpos);
	m_pRenderer->RenderText(false, m_ptMousePos, "Main Menu", (iYpos += 150), SUBTITLE_FONT, colorRed, H_CENTER);

	// Print the Menu Options
	strSelection += m_pRenderer->RenderText(true, m_ptMousePos, "Single Player", (iYpos += 120), MENU_FONT, colorWhite, H_CENTER);
	strSelection += m_pRenderer->RenderText(true, m_ptMousePos, "Multiplayer", (iYpos += 60), MENU_FONT, colorWhite, H_CENTER);
	strSelection += m_pRenderer->RenderText(true, m_ptMousePos, "Instructions", (iYpos += 60), MENU_FONT, colorWhite, H_CENTER);
	strSelection += m_pRenderer->RenderText(true, m_ptMousePos, "Options", (iYpos += 60), MENU_FONT, colorWhite, H_CENTER);
	strSelection += m_pRenderer->RenderText(true, m_ptMousePos, "Exit Game", (iYpos += 60), MENU_FONT, colorWhite, H_CENTER);

	// Change the MenuSelection Variable to the menu enum
	if (m_activatedControls.bAction == true)
	{
		m_strMenuTempSelection = strSelection;
	}
	else
	{
		m_strMenuTempSelection = "";
	}
}

void CHub_Client::DisplayMultiplayerMenu()
{
	int iYpos = 0;
	D3DXCOLOR colorRed = 0xffff0000;
	D3DXCOLOR colorWhite = 0xffffffff;
	std::string strSelection = "";

	// Render the Backbuffer to Black
	m_pRenderer->RenderColor(0xff000000);

	// Print the Title Text
	iYpos = PrintFullTitle(iYpos);
	m_pRenderer->RenderText(false, m_ptMousePos, "Multiplayer", (iYpos += 150), SUBTITLE_FONT, colorRed, H_CENTER);

	// Print the Menu Options
	strSelection += m_pRenderer->RenderText(true, m_ptMousePos, "Host a Game", (iYpos += 120), MENU_FONT, colorWhite, H_CENTER);
	strSelection += m_pRenderer->RenderText(true, m_ptMousePos, "Join a Game", (iYpos += 60), MENU_FONT, colorWhite, H_CENTER);
	strSelection += m_pRenderer->RenderText(true, m_ptMousePos, "Back", (iYpos += 100), MENU_FONT, colorWhite, H_CENTER);

	// Change the MenuSelection Variable to the menu enum
	if (m_activatedControls.bAction == true)
	{
		m_strMenuTempSelection = strSelection;
	}
	else
	{
		m_strMenuTempSelection = "";
	}
}

void CHub_Client::DisplayInstructionsMenu()
{
	int iYpos = 0;
	D3DXCOLOR colorRed = 0xffff0000;
	D3DXCOLOR colorWhite = 0xffffffff;
	std::string strSelection = "";

	// Render the Backbuffer to Black
	m_pRenderer->RenderColor(0xff000000);

	m_pRenderer->RenderText(false, m_ptMousePos, "Instructions", (iYpos += 60), SUBTITLE_FONT, colorRed, H_CENTER);

	// Print the Menu Options
	m_pRenderer->RenderText(false, m_ptMousePos, "Survive as long as you can against the hordes of hell with endless waves.", (iYpos += 120), SCREEN_FONT, colorWhite, H_CENTER);
	m_pRenderer->RenderText(false, m_ptMousePos, "Utilise the PowerUps to your advantage and score the highest points.", (iYpos += 30), SCREEN_FONT, colorWhite, H_CENTER);

	m_pRenderer->RenderText(false, m_ptMousePos, "Controls", (iYpos += 80), MENU_FONT, colorRed, H_CENTER);
	m_pRenderer->RenderText(false, m_ptMousePos, "WASD", (iYpos += 60), MENU_FONT, colorWhite, H_LEFT);
	m_pRenderer->RenderText(false, m_ptMousePos, "Avatar Movement", (iYpos), MENU_FONT, colorWhite, H_RIGHT);
	m_pRenderer->RenderText(false, m_ptMousePos, "Left Click", (iYpos += 40), MENU_FONT, colorWhite, H_LEFT);
	m_pRenderer->RenderText(false, m_ptMousePos, "Shoot", (iYpos), MENU_FONT, colorWhite, H_RIGHT);
	m_pRenderer->RenderText(false, m_ptMousePos, "Mouse Cursor", (iYpos += 40), MENU_FONT, colorWhite, H_LEFT);
	m_pRenderer->RenderText(false, m_ptMousePos, "Targeting System", (iYpos), MENU_FONT, colorWhite, H_RIGHT);
	m_pRenderer->RenderText(false, m_ptMousePos, "E", (iYpos += 40), MENU_FONT, colorWhite, H_LEFT);
	m_pRenderer->RenderText(false, m_ptMousePos, "Deploy Flare", (iYpos), MENU_FONT, colorWhite, H_RIGHT);
	m_pRenderer->RenderText(false, m_ptMousePos, "Esc", (iYpos += 40), MENU_FONT, colorWhite, H_LEFT);
	m_pRenderer->RenderText(false, m_ptMousePos, "Pause Menu", (iYpos), MENU_FONT, colorWhite, H_RIGHT);
	m_pRenderer->RenderText(false, m_ptMousePos, "F1", (iYpos += 40), MENU_FONT, colorWhite, H_LEFT);
	m_pRenderer->RenderText(false, m_ptMousePos, "Toggle Camera Mode", (iYpos), MENU_FONT, colorWhite, H_RIGHT);
	m_pRenderer->RenderText(false, m_ptMousePos, "F4", (iYpos += 40), MENU_FONT, colorWhite, H_LEFT);
	m_pRenderer->RenderText(false, m_ptMousePos, "Toggle Free Camera", (iYpos), MENU_FONT, colorWhite, H_RIGHT);

	m_pRenderer->RenderText(false, m_ptMousePos, "PowerUps", (iYpos += 80), MENU_FONT, colorRed, H_CENTER);
	m_pRenderer->RenderText(false, m_ptMousePos, "Heart", (iYpos += 60), MENU_FONT, colorWhite, H_LEFT);
	m_pRenderer->RenderText(false, m_ptMousePos, "Replenish Your Health", (iYpos), MENU_FONT, colorWhite, H_RIGHT);
	m_pRenderer->RenderText(false, m_ptMousePos, "Flare", (iYpos += 40), MENU_FONT, colorWhite, H_LEFT);
	m_pRenderer->RenderText(false, m_ptMousePos, "Receive one Flare", (iYpos), MENU_FONT, colorWhite, H_RIGHT);
	m_pRenderer->RenderText(false, m_ptMousePos, "Gold", (iYpos += 40), MENU_FONT, colorWhite, H_LEFT);
	m_pRenderer->RenderText(false, m_ptMousePos, "1500 Points", (iYpos), MENU_FONT, colorWhite, H_RIGHT);
	
	iYpos = m_iScreenHeight;
	strSelection += m_pRenderer->RenderText(true, m_ptMousePos, "Back", (iYpos -= 100), MENU_FONT, colorWhite, H_CENTER);

	// Change the MenuSelection Variable to the menu enum
	if (m_activatedControls.bAction == true)
	{
		m_strMenuTempSelection = strSelection;
	}
	else
	{
		m_strMenuTempSelection = "";
	}
}

void CHub_Client::DisplayOptionsMenu()
{
	int iYpos = 0;
	D3DXCOLOR colorRed = 0xffff0000;
	D3DXCOLOR colorWhite = 0xffffffff;
	D3DXCOLOR colorRedUnSelected = 0xff440000;
	D3DXCOLOR WireframeColor;
	std::string strSelection = "";

	// Render the Backbuffer to Black
	m_pRenderer->RenderColor(0xff000000);

	// Print the Title Text
	iYpos = PrintFullTitle(iYpos);
	m_pRenderer->RenderText(false, m_ptMousePos, "Options", (iYpos += 150), SUBTITLE_FONT, colorRed, H_CENTER);

	// Print the Menu Options
	if (m_bWireFrame == true)
	{
		WireframeColor = colorRed;
	}
	else
	{
		WireframeColor = colorRedUnSelected;
	}
	m_pRenderer->RenderText(false, m_ptMousePos, "Wireframe Mode", (iYpos += 150), MENU_FONT, colorWhite, H_LEFT);
	strSelection += m_pRenderer->RenderText(true, m_ptMousePos, "Active", (iYpos), MENU_FONT, WireframeColor, H_RIGHT);

	iYpos = m_iScreenHeight - 100;
	strSelection += m_pRenderer->RenderText(true, m_ptMousePos, "Back", (iYpos), MENU_FONT, colorWhite, H_CENTER);

	// Change the MenuSelection Variable to the menu enum
	if (m_activatedControls.bAction == true)
	{
		m_strMenuTempSelection = strSelection;
	}
	else
	{
		m_strMenuTempSelection = "";
	}

}

void CHub_Client::DisplayGameLobby()
{
	int iYpos = 0;
	D3DXCOLOR colorRed = 0xffff0000;
	D3DXCOLOR colorWhite = 0xffffffff;
	std::string strSelection = "";

	// Render the Backbuffer to Black
	m_pRenderer->RenderColor(0xff000000);

	// Print the Title Text
	m_pRenderer->RenderText(false, m_ptMousePos, "Game Lobby", (iYpos += 50), TITLE_FONT, colorRed, H_CENTER);
	m_pRenderer->RenderText(false, m_ptMousePos, "Server Name : " + m_strServerName, (iYpos += 150), SCREEN_FONT, colorRed, H_LEFT);
	m_pRenderer->RenderText(false, m_ptMousePos, "Server Host : " + m_strServerHost, (iYpos += 30), SCREEN_FONT, colorRed, H_LEFT);

	iYpos += 40;
	m_pRenderer->RenderText(false, m_ptMousePos, "  Current Users", (iYpos += 30), SCREEN_FONT, colorRed, H_LEFT);
	m_pRenderer->RenderText(false, m_ptMousePos, "Status", (iYpos), SCREEN_FONT, colorRed, H_RIGHT);

	std::map<std::string, AvatarInfo>::iterator iterUsersCurrent = m_pCurrentUsers->begin();

	while (iterUsersCurrent != m_pCurrentUsers->end())
	{
		if (iterUsersCurrent == m_pCurrentUsers->begin())
		{
			// Don't move the text down by the full 60 the first time
			iYpos -= 30;
		}
		m_pRenderer->RenderText(false, m_ptMousePos, iterUsersCurrent->first, (iYpos += 60), MENU_FONT, colorRed, H_LEFT);
		D3DXCOLOR colorReady = (iterUsersCurrent->second.bAlive == true) ? colorRed : (0xff440000);
		m_pRenderer->RenderText(false, m_ptMousePos, "Ready", (iYpos), MENU_FONT, colorReady, H_RIGHT);

		iterUsersCurrent++;
	}

	iYpos = m_iScreenHeight - 150;
	std::string strReadyState = (m_bAlive == true) ? ("Actually I'm not ready!") : ("I am Ready!");
	strSelection += m_pRenderer->RenderText(true, m_ptMousePos, strReadyState, (iYpos), MENU_FONT, colorWhite, H_CENTER);
	strSelection += m_pRenderer->RenderText(true, m_ptMousePos, "Exit", (iYpos += 60), MENU_FONT, colorWhite, H_CENTER);

	// Change the MenuSelection Variable to the menu enum
	if (m_activatedControls.bAction == true)
	{
		m_strMenuTempSelection = strSelection;
	}
	else
	{
		m_strMenuTempSelection = "";
	}
}

void CHub_Client::DisplaySelectServer()
{
	int iYpos = 0;
	D3DXCOLOR colorRed = 0xffff0000;
	D3DXCOLOR colorWhite = 0xffffffff;
	std::string strSelection = "";
	int iServerIndex = -1;

	// Render the Backbuffer to Black
	m_pRenderer->RenderColor(0xff000000);

	// Print the Title Text
	m_pRenderer->RenderText(false, m_ptMousePos, "Available Servers", (iYpos += 50), SUBTITLE_FONT, colorRed, H_CENTER);
	iYpos += 120;

	// Print the Headings
	m_pRenderer->RenderText(false, m_ptMousePos, "Server Name", (iYpos += 40), MENU_FONT, colorRed, H_LEFT);
	m_pRenderer->RenderText(false, m_ptMousePos, "Host", (iYpos), MENU_FONT, colorRed, H_CENTER);
	m_pRenderer->RenderText(false, m_ptMousePos, "Users", (iYpos), MENU_FONT, colorRed, H_RIGHT);

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

				strSelection += m_pRenderer->RenderText(bSelectable, m_ptMousePos, (*m_pAvailableServers)[i].cServerName, (iYpos += 30), SCREEN_FONT, color, H_LEFT);
				m_pRenderer->RenderText(bSelectable, m_ptMousePos, (*m_pAvailableServers)[i].cHostName, (iYpos), SCREEN_FONT, color, H_CENTER);
				m_pRenderer->RenderText(bSelectable, m_ptMousePos, strCurrentClients, (iYpos), SCREEN_FONT, color, H_RIGHT);

				if (strSelection != "" && iServerIndex == -1)
				{
					iServerIndex = i;
				}
			}
		}
		else
		{
			m_pRenderer->RenderText(false, m_ptMousePos, "No Servers are Currently Active", (iYpos += 300), SCREEN_FONT, colorRed, H_CENTER);
		}
	}

	iYpos = m_iScreenHeight - 100;
	strSelection += m_pRenderer->RenderText(true, m_ptMousePos, "Back", (iYpos), MENU_FONT, colorWhite, H_CENTER);

	// Change the MenuSelection Variable to the menu enum
	if (m_activatedControls.bAction == true)
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

void CHub_Client::DisplayTerminatedServer()
{
	int iYpos = 0;
	D3DXCOLOR colorRed = 0xffff0000;
	D3DXCOLOR colorWhite = 0xffffffff;
	std::string strSelection = "";

	// Render the Backbuffer to Black
	m_pRenderer->RenderColor(0xff000000);

	// Print the Title Text
	iYpos = PrintFullTitle(iYpos);

	// Print the Menu Options
	m_pRenderer->RenderText(false, m_ptMousePos, "Connection has been lost.", (iYpos += 270), SCREEN_FONT, colorRed, H_CENTER);
	m_pRenderer->RenderText(false, m_ptMousePos, "Server has been terminated", (iYpos += 30), SCREEN_FONT, colorRed, H_CENTER);

	iYpos = m_iScreenHeight - 100;
	strSelection += m_pRenderer->RenderText(true, m_ptMousePos, "Back to Main Menu", (iYpos += 30), MENU_FONT, colorWhite, H_CENTER);

	// Change the MenuSelection Variable to the menu enum
	if (m_activatedControls.bAction == true)
	{
		m_strMenuTempSelection = strSelection;
	}
	else
	{
		m_strMenuTempSelection = "";
	}
}

void CHub_Client::DisplayGameLoading()
{
	D3DXCOLOR colorRed = 0xffff0000;
	D3DXCOLOR colorWhite = 0xffffffff;

	int iDotCount = 1;
	std::string strDots = ".";
	while (m_bGameLoading == true)
	{
		int iYpos = 0;

		m_pRenderer->StartRender(true, true, false);

		// Render the Backbuffer to Black
		m_pRenderer->RenderColor(0xff000000);

		// Print the Title Text
		iYpos = PrintFullTitle(iYpos);

		// Print the Menu Options
		m_pRenderer->RenderText(false, m_ptMousePos, "Game Loading", (iYpos += 270), MENU_FONT, colorRed, H_CENTER);
	
		m_pRenderer->RenderText(false, m_ptMousePos, strDots, (iYpos += 60), MENU_FONT, colorRed, H_CENTER);
		

		strDots.append(" .");
		iDotCount++;

		if (iDotCount == 6)
		{
			strDots = ".";
			iDotCount = 1;
		}

		m_pRenderer->EndRender();
		Sleep(200);
	}
}

int CHub_Client::PrintFullTitle(int _iYpos)
{
	// Print the Title Text
	m_pRenderer->RenderText(false, m_ptMousePos, "ROBOTRON", (_iYpos += 50), SUBTITLE_FONT, 0xff450000, H_CENTER);
	m_pRenderer->RenderText(false, m_ptMousePos, "HORDES", (_iYpos += 80), TITLE_FONT, 0xff650000, H_CENTER);
	m_pRenderer->RenderText(false, m_ptMousePos, "OF", (_iYpos += 100), TITLE_FONT, 0xff7c0000, H_CENTER);
	m_pRenderer->RenderText(false, m_ptMousePos, "HELL", (_iYpos += 100), TITLE_FONT, 0xffa50000, H_CENTER);

	return _iYpos;
}

// #Packets
bool CHub_Client::CreateDataPacket()
{
	// Clear out old data from the Data Packet packet
	ZeroMemory(*(&m_pClientToServer), sizeof(*m_pClientToServer));

	// Add the Username of the Client to the Packet structure
	if (!(StringToStruct(m_strUserName.c_str(), m_pClientToServer->cUserName, network::MAX_USERNAME_LENGTH)))
	{
		return false;	// Data invalid - Data Packet failed to create
	}

	m_pClientToServer->activatedControls = m_activatedControls;
	
	return true;
}

bool CHub_Client::CreateCommandPacket(eNetworkCommand _eCommand)
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

bool CHub_Client::CreateCommandPacket(eNetworkCommand _eCommand, std::string _strMessage)
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
void CHub_Client::CreateServer()
{
	std::string strFilename;
	#ifdef _DEBUG
		strFilename = "..\\Debug\\Robotron Server";
	#endif // _DEBUG
	#ifndef _DEBUG
		// TO DO - change file path when handing in final build
		strFilename = "Robotron Server";
	#endif // Not _DEBUG

	// Start the Server executable running
	if (m_bSinglePlayer == true)
	{
		m_strServerName = "Single_Player";
	}
	std::string strOpenParameters = m_strUserName + " " + m_strServerName;
	int iError = (int)(ShellExecuteA(m_hWnd, "open", strFilename.c_str(), strOpenParameters.c_str(), NULL, SW_MINIMIZE));

	m_strServerHost = m_strUserName;
	m_eScreenState = STATE_GAMELOADING;
}

void CHub_Client::ReceiveDataFromNetwork(ServerToClient* _pReceiveData)
{
	while (m_bNetworkOnline)
	{
		if (m_pNetworkClient->ReceivePacket(_pReceiveData) == true)
		{
			// Add the Received Packet to the Work Queue
			m_pClientMutex->Wait();
			m_pWorkQueue->push(*_pReceiveData);
			m_pClientMutex->Signal();
		}
	}
}

void CHub_Client::InsertUser(std::string _strUser, AvatarInfo _AvatarInfo)
{
	std::pair<std::string, AvatarInfo> pairUser(_strUser, _AvatarInfo);
	m_pCurrentUsers->insert(pairUser);
}

void CHub_Client::ResetGameData()
{
	m_bSinglePlayer = false;
	m_bHost = false;
	m_bFoundServer = false;
	m_bAlive = false;

	// remove all users from the map
	m_pCurrentUsers->clear();

	m_strServerName = "";
	m_strServerHost = "";

	m_pNetworkClient->Reset();

	if (m_pMechanics != 0)
	{
		delete m_pMechanics;
		m_pMechanics = 0;
	}
}

void CHub_Client::FrameLimiter()
{
	// TO DO - change this so that sleep isnt used

	// Calculate the Total time taken to complete frame
	m_iFrameTimeDifference = m_iFrameTimeEnd - m_iFrameTimeStart;

	// Increase the Second Counter by the Frame difference
	m_iSecondCounter += m_iFrameTimeDifference;
	m_iFrameCounter++;	// Increment the number of frames Rendered

	// Calculate the the remaining time to render frames at 60 per Second
	int iTimeToWait;

	// Sleep function works in whole milliseconds and 1000/60 = 16.6667
	if (m_iFrameCounter % 3 == 0)
	{
		// Every third frame wait 1 millisecond less to offset the 2 frames over the 16.667 mark
		iTimeToWait = (16) - (m_iFrameTimeDifference);
	}
	else
	{
		iTimeToWait = (17) - (m_iFrameTimeDifference);
	}

	// Sleep only if the Rendering of the frame took less than 1/60 seconds
	if (iTimeToWait > 0)
	{
		Sleep(iTimeToWait);
		m_iSecondCounter += iTimeToWait;
	}

	if (m_iSecondCounter >= 1000)
	{
		m_iSecondCounter -= 1000;	// Remove one second of time from counter. Prevents integer wrap around
		m_iFPS = m_iFrameCounter;	// FPS for this second is set to the number of frames rendered this second
		m_iFrameCounter = 0;		// Reset the Frame counter
	}
}

void CHub_Client::DebugCameraInput(Controls* _controls)
{
	float fDT = m_pClock->GetDeltaTick();
	CDebugCamera* pDebugCam = m_pMechanics->GetDebugCam();

	if ((_controls->bDebugLeft) 
		&&(!(_controls->bDebugRight)))
	{
		// Turn Left
		pDebugCam->Yaw(-fDT);
	}
	if ((_controls->bDebugRight)
		&& (!(_controls->bDebugLeft)))
	{
		//Turn Right
		pDebugCam->Yaw(fDT);
	}
	if ((_controls->bDebugUp)
		&& (!(_controls->bDebugDown)))
	{
		// Pitch Up
		pDebugCam->Pitch(fDT);
	}
	if ((_controls->bDebugDown)
		&& (!(_controls->bDebugUp)))
	{
		// Pitch Down
		pDebugCam->Pitch(-fDT);
	}
	if ((_controls->bDebugForward)
		&& (!(_controls->bDebugBackwards)))
	{
		// Move Forwards
		pDebugCam->Move(fDT);
	}
	if ((_controls->bDebugBackwards)
		&& (!(_controls->bDebugForward)))
	{
		// Move Backwards
		pDebugCam->Move(-fDT);
	}
}

bool CHub_Client::CheckRenderer()
{
	HRESULT hResult = m_pRenderer->CheckDevice();

	switch (hResult)
	{
	case D3D_OK:
	{
		return true;
		break;
	}
	case D3DERR_DEVICELOST:
	{
		Sleep(50);
		return false;
		break;
	}
	case D3DERR_DEVICENOTRESET:
	{
		m_pRenderer->InvalidateResources();
		m_pMechanics->InitialiseGraphicsResources(true);

		if (m_pRenderer->CheckDevice() != D3D_OK)
		{
			return false;
		}
		else
		{
			return true;
		}

		break;
	}
	default:
	{
		return false;
		break;
	}
	}
}
