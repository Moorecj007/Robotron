/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Hub_Client.h
* Description : Declaration file for the main game functions of the client
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __HUB_CLIENT_H__
#define __HUB_CLIENT_H__

// Library Includes
#include <thread>
#include <queue>

// Local Includes
#include "Mechanics_Client.h"
#include "Graphics/D3D9Renderer.h"
#include "Graphics\DInput.h"
#include "../Common Files/Clock.h"
#include "../Common Files/Networking/MySemaphore.h"

class CHub_Client
{
public:
	// Constructors / Destructors

	/***********************
	* ~Game: Default Destructor for Game class
	* @author: Callan Moore
	********************/
	~CHub_Client();

	// Singleton Methods

	/***********************
	* GetInstance: Returns the singleton instance of the game, if it doesnt exist creates it.
	* @author: Callan Moore
	* @return: CGame&: The current instance of the game
	********************/
	static CHub_Client& GetInstance();

	/***********************
	* DestroyInstance: Deletes the instance of the game.
	* @author: Callan Moore
	* @return: void
	********************/
	static void DestroyInstance();

	// Getters

	/***********************
	* GetTextInput: Returns if the input is text or controls
	* @author: Callan Moore
	* @return: bool: True if the input is intended for text
	********************/
	bool GetTextInput() { return m_bTextInput; };

	// Setters

	/***********************
	* SetMousePos: Sets the Mouse coordinates
	* @author: Callan Moore
	* @parameter: _iX: New Mouse position on the X axis
	* @parameter: _iY: New Mouse position on the Y axis
	* @return: void
	********************/
	void SetMousePos(int _iX, int _iY);

	// Prototypes

	/***********************
	* Initialise: Initialise the Game Object for Use
	* @author: Callan Moore
	* @parameters: _hInstance: Handle to Instance of the application
	* @parameters: _hWnd: Handle to The creating window
	* @parameters: _iScreenWidth: The Height of the Screen
	* @parameters: _iScreenHeight: The Width of the Screen
	* @return: bool: Successful initialisation or not
	********************/
	bool Initialise(HINSTANCE _hInstance, HWND _hWnd, int _iScreenWidth, int _iScreenHeight);

	/***********************
	* RenderOneFrame: Renders one frame
	* @author: Callan Moore
	* @return: bool: False if the network is offline
	********************/
	bool RenderOneFrame();

	// #Processes
	/***********************
	* Process: Processes the Game for the Delta tick
	* @author: Callan Moore
	* @return: void
	********************/
	void Process();

	/***********************
	* ProcessTextInput: Processes Text input
	* @author: Callan Moore
	* @parameter: _wKeyPress: The pressed key
	* @return: void
	********************/
	void ProcessTextInput(WPARAM _wKeyPress);

	/***********************
	* ProcessServerNameInput: Processes Text input for the Server Name
	* @author: Callan Moore
	* @parameter: _wKeyPress: The pressed key
	* @return: void
	********************/
	void ProcessServerNameInput(WPARAM _wKeyPress);

	/***********************
	* ProcessUserNameInput: Processes Text input for the User Name
	* @author: Callan Moore
	* @parameter: _wKeyPress: The pressed key
	* @return: void
	********************/
	void ProcessUserNameInput(WPARAM _wKeyPress);

	/***********************
	* ProcessPacket: Process a packet from the work queue
	* @author: Callan Moore
	* @parameter: _fDT: The current Delta tick
	* @return: void
	********************/
	void ProcessPacket(float _fDT);

	/***********************
	* ProcessScreenState: Determine which process to run dependent on the current screen state
	* @author: Callan Moore
	* @parameter: _fDT: The current Delta tick
	* @return: void
	********************/
	void ProcessScreenState(float _fDT);

	/***********************
	* ProcessCreateUser: Process information from the Create User Menu
	* @author: Callan Moore
	* @return: void
	********************/
	void ProcessCreateUser();

	/***********************
	* ProcessCreateServer: Process information from the Server Name Menu
	* @author: Callan Moore
	* @return: void
	********************/
	void ProcessCreateServer();

	/***********************
	* ProcessMainMenu: Process information from the Main Menu
	* @author: Callan Moore
	* @return: void
	********************/
	void ProcessMainMenu();

	/***********************
	* ProcessSingleMenu: Process information from the Single Player Menu
	* @author: Callan Moore
	* @return: void
	********************/
	void ProcessSingleMenu();

	/***********************
	* ProcessMultiMenu: Process information from the Multi Player Menu
	* @author: Callan Moore
	* @return: void
	********************/
	void ProcessMultiMenu();

	/***********************
	* ProcessInstructionsMenu: Process information from the Instructions Menu
	* @author: Callan Moore
	* @return: void
	********************/
	void ProcessInstructionsMenu();

	/***********************
	* ProcessOptionsMenu: Process information from the Options Menu
	* @author: Callan Moore
	* @return: void
	********************/
	void ProcessOptionsMenu();

	/***********************
	* ProcessGameLobby: Process information for the Game Lobby Screen
	* @author: Callan Moore
	* @return: void
	********************/
	void ProcessGameLobby();

	/***********************
	* ProcessSelectServer: Process information for the Select server screen
	* @author: Callan Moore
	* @return: void
	********************/
	void ProcessSelectServer();

	/***********************
	* ProcessTerminatedServer: Process information for the Terminated server screen
	* @author: Callan Moore
	* @return: void
	********************/
	void ProcessTerminatedServer();

	/***********************
	* ProcessGameLoading: Process information for the Game Loading Screen
	* @author: Callan Moore
	* @return: void
	********************/
	void ProcessGameLoading();

	/***********************
	* ProcessPausedScreen: Process information for the Pause Screen
	* @author: Callan Moore
	* @return: void
	********************/
	void ProcessPausedScreen();

	/***********************
	* ProcessGameOverScreen: Process information for the game over Screen
	* @author: Callan Moore
	* @return: void
	********************/
	void ProcessGameOverScreen();



	/***********************
	* Draw: Draws all the game world
	* @author: Callan Moore
	* @return: void
	********************/
	void Draw();

	// #DisplayMenus
	/***********************
	* CreateUserName: Allows the User to Create a username
	* @author: Callan Moore
	* @return: void
	********************/
	void CHub_Client::CreateUserName();

	/***********************
	* CreateUserName: Allows the User to Create a Server Name
	* @author: Callan Moore
	* @return: void
	********************/
	void CHub_Client::CreateServerName();

	/***********************
	* DisplayMainMenu: Displays the Games Main Menu
	* @author: Callan Moore
	* @return: void
	********************/
	void DisplayMainMenu();

	/***********************
	* DisplayMultiplayerMenu: Displays the Games Multiplayer Menu
	* @author: Callan Moore
	* @return: void
	********************/
	void DisplayMultiplayerMenu();

	/***********************
	* DisplayInstructionsMenu: Displays the Games Instructions Menu
	* @author: Callan Moore
	* @return: void
	********************/
	void DisplayInstructionsMenu();

	/***********************
	* DisplayOptionsMenu: Displays the Games Options Menu
	* @author: Callan Moore
	* @return: void
	********************/
	void DisplayOptionsMenu();

	/***********************
	* DisplayGameLobby: Displays the Games Lobby
	* @author: Callan Moore
	* @return: void
	********************/
	void DisplayGameLobby();

	/***********************
	* DisplaySelectServer: Displays the Select a Server menu
	* @author: Callan Moore
	* @return: void
	********************/
	void DisplaySelectServer();

	/***********************
	* DisplayTerminatedServer: Displays the Terminated Server menu
	* @author: Callan Moore
	* @return: void
	********************/
	void DisplayTerminatedServer();

	/***********************
	* DisplayGameLoading: Displays the Game Loading Screen
	* @author: Callan Moore
	* @return: void
	********************/
	void DisplayGameLoading();

	/***********************
	* PrintFullTitle: Print the full title
	* @author: Callan Moore
	* @parameter: _iYpos: The starting Y coordinate for the Text
	* @return: int: The current yPos after printing the title
	********************/
	int PrintFullTitle(int _iYpos);

	// #Packets
	/***********************
	* CreateDataPacket: Creates the DataPacket to send with all relevant information
	* @author: Callan Moore
	* @return: bool: Successful creation of Data Packet (or not)
	********************/
	bool CreateDataPacket();

	/***********************
	* CreateCommandPacket: Create a Packet that sends a command
	* @author: Callan Moore
	* @parameter: _command: The command to Send
	* @return: bool: Successful creation of Data Packet (or not)
	********************/
	bool CreateCommandPacket(eNetworkCommand _command);

	/***********************
	* CreateCommandPacket: Create a Packet that sends a command
	* @author: Callan Moore
	* @parameter: _command: The command to Send
	* @return: bool: Successful creation of Data Packet (or not)
	********************/
	bool CreateCommandPacket(eNetworkCommand _eCommand, std::string _strMessage);

	// #Extras
	/***********************
	* CreateServer: Create server from a separate application
	* @author: Callan Moore
	* @return: void
	********************/
	void CreateServer();

	/***********************
	* ReceiveDataFromNetwork: Thread to receive data from the network
	* @author: Callan Moore
	* @parameter: _pReceiveData: Struct property to receive Data from the Network
	* @return: void
	********************/
	void ReceiveDataFromNetwork(ServerToClient* _pReceiveData);

	/***********************
	* InsertUser: Creates a pair for a new user and adds them to the clientside map
	* @author: Callan Moore
	* @parameter: _strUser: String of the new users username
	* @parameter: _UserInfo: The new users information
	* @return: void: 
	********************/
	void InsertUser(std::string _strUser, AvatarInfo _AvatarInfo);

	/***********************
	* ResetGameData: Reset the game data linked to a particular server/game
	* @author: Callan Moore
	* @return: void:
	********************/
	void ResetGameData();

	/***********************
	* FrameLimiter: Limit the ammount of frames processes to 60
	* @author: Callan Moore
	* @return: void:
	********************/
	void FrameLimiter();

private:
	//Disallowing copies and extra constructions
	CHub_Client();
	CHub_Client(const CHub_Client& _kr);
	CHub_Client& operator= (const CHub_Client& _kr);

public:
	// Static Variables
	static CMySemaphore* m_pClientMutex;
	bool* m_pbKeyDown;

private:
	// Singleton Instance
	static CHub_Client* s_pGame;

	// Menu Variables
	CClock* m_pClock;
	eScreenState m_eScreenState;
	std::string m_strMenuSelection;
	bool m_bMenuSelected;
	std::string m_strMenuTempSelection;
	int m_iServerIndex;
	bool m_bPauseKey;
	bool m_bPrevPauseKey;

	// Game Variables
	bool m_bSinglePlayer;
	bool m_bHost;
	bool m_bFoundServer;
	CDInput* m_pDInput;
	Controls m_activatedControls;
	bool m_bGameLoading;

	// Window Variables
	HWND m_hWnd;
	int m_iScreenWidth;
	int m_iScreenHeight;
	POINT m_ptMousePos;
	bool m_bTextInput;

	// Client Network Variables
	bool m_bNetworkOnline;
	CNetwork_Client* m_pNetworkClient;
	ClientToServer* m_pClientToServer;
	ServerToClient* m_pPacketToProcess;
	ServerToClient* m_pServerToClient;
	std::thread m_ReceiveThread;
	std::queue<ServerToClient>* m_pWorkQueue;

	// Users
	std::string m_strUserName;
	bool m_bAlive;
	std::string m_strServerHost;
	eNetworkCommand m_eUserNameFailed;
	std::map<std::string, AvatarInfo>* m_pCurrentUsers;
		
	// Server Information
	std::string m_strServerName;
	std::vector<AvailableServer>* m_pAvailableServers;
	
	// Graphics Variables
	IRenderer* m_pRenderer;

	// Gameplay Information
	CMechanics_Client* m_pMechanics;

	// Time keeping variables
	int m_iFrameTimeStart;
	int m_iFrameTimeEnd;
	int m_iSecondCounter;
	int m_iFPS;
	int m_iFrameCounter;
	int m_iFrameTimeDifference;

};

#endif //__HUB_CLIENT_H__