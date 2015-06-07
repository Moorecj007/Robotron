/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : GameClient.h
* Description : Declaration file for the main game functions of the client
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __GAMECLIENT_H__
#define __GAMECLIENT_H__

// Library Includes
#include <thread>
#include <queue>
#include <algorithm>

// Local Includes
#include "Client.h"
#include "IRenderer.h"
#include "D3D9Renderer.h"
#include "../Common Files/Clock.h"
#include "../Common Files/MySemaphore.h"

class CGameClient
{
public:
	// Constructors / Destructors

	/***********************
	* ~Game: Default Destructor for Game class
	* @author: Callan Moore
	********************/
	~CGameClient();

	// Singleton Methods

	/***********************
	* GetInstance: Returns the singleton instance of the game, if it doesnt exist creates it.
	* @author: Callan Moore
	* @return: CGame&: The current instance of the game
	********************/
	static CGameClient& GetInstance();

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

	/***********************
	* SetMouseDown: Sets the Mouse Down variable
	* @author: Callan Moore
	* @parameter: _b: Boolean state for the Mouse down variable
	* @return: void
	********************/
	void SetLeftMouseClick(bool _b) { m_bLeftMouseClick = _b; }

	// Prototypes

	/***********************
	* Initialise: Initialise the Game Object for Use
	* @author: Callan Moore
	* @parameters: _hWnd: Handle to The creating window
	* @parameters: _iScreenWidth: The Height of the Screen
	* @parameters: _iScreenHeight: The Width of the Screen
	* @return: bool: Successful initialisation or not
	********************/
	bool Initialise(HWND _hWnd, int _iScreenWidth, int _iScreenHeight);

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
	* ProcessPacket: Process a packet from the work queue
	* @author: Callan Moore
	* @return: void
	********************/
	void ProcessPacket();

	/***********************
	* ProcessScreenState: Determine which process to run dependent on the current screen state
	* @author: Callan Moore
	* @return: void
	********************/
	void ProcessScreenState();

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
	void CGameClient::CreateUserName();

	/***********************
	* CreateUserName: Allows the User to Create a Server Name
	* @author: Callan Moore
	* @return: void
	********************/
	void CGameClient::CreateServerName();

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
	* StringToStruct: Copies characters from a char array into a struct property
	* @author: Callan Moore
	* @parameter: _pcData: Char array to input into the struct
	* @parameter: _pcStruct: Struct property to receive char array
	* @return: bool: Successful copy (or Not)
	********************/
	bool StringToStruct(const char* _pcData, char* _pcStruct);

	/***********************
	* ReceiveDataFromNetwork: Thread to receive data from the network
	* @author: Callan Moore
	* @parameter: _pReceiveData: Struct property to receive Data from the Network
	* @return: void
	********************/
	void ReceiveDataFromNetwork(ServerToClient* _pReceiveData);

	/***********************
	* ChangeMenuSelection: Change the Menu Selection to the current Temp Menu Selection
	* @author: Callan Moore
	* @return: void
	********************/
	void ChangeMenuSelection();

private:
	//Disallowing copies and extra constructions
	CGameClient();
	CGameClient(const CGameClient& _kr);
	CGameClient& operator= (const CGameClient& _kr);

public:
	// Static Variables
	static CMySemaphore* m_pClientMutex;
	bool* m_pbKeyDown;

private:
	// Singleton Instance
	static CGameClient* s_pGame;

	// Game Variables
	CClock* m_pClock;
	eScreenState m_eScreenState;
	eMenuSelection m_eMenuSelection;
	eMenuSelection m_eMenuTempSelection;
	bool m_bHost;

	// Window Variables
	HWND m_hWnd;
	int m_iScreenWidth;
	int m_iScreenHeight;
	int m_iMouseX;
	int m_iMouseY;
	bool m_bLeftMouseClick;
	bool m_bTextInput;

	// Client Network Variables
	bool m_bNetworkOnline;
	CClient* m_pClientNetwork;
	ClientToServer* m_pClientToServer;
	ServerToClient* m_pPacketToProcess;
	ServerToClient* m_pServerToClient;
	std::thread m_ReceiveThread;
	std::queue<ServerToClient>* m_pWorkQueue;

	// Users
	std::string m_strUserName;
	std::string m_strServerHost;
	eNetworkCommand m_eUserNameFailed;
	std::vector<std::string>* m_pCurrentUsers;
		
	// Server Information
	std::string m_strServerName;
	std::vector<std::string>* m_pAvailableServers;
	

	// Graphics Variables
	IRenderer* m_pRenderer;
	

};

#endif //__GAMECLIENT_H__