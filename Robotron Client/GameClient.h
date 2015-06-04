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

// Local Includes
#include "Client.h"
#include "../Common Files/Clock.h"

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

	// Setters

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
	* @return: void
	********************/
	void RenderOneFrame();

	/***********************
	* Process: Processes the Game for the Delta tick
	* @author: Callan Moore
	* @return: void
	********************/
	void Process();

	/***********************
	* Draw: Draws all the game world
	* @author: Callan Moore
	* @return: void
	********************/
	void Draw();

	/***********************
	* MainMenu: Displays the Games Main Menu
	* @author: Callan Moore
	* @return: void
	********************/
	void MainMenu();

	/***********************
	* CreateDataPacket: Creates the DataPacket to send with all relevant information
	* @author: Callan Moore
	* @return: bool: Successful creation of Data Packet (or not)
	********************/
	bool CreateDataPacket();

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

private:
	//Disallowing copies and extra constructions
	CGameClient();
	CGameClient(const CGameClient& _kr);
	CGameClient& operator= (const CGameClient& _kr);

private:
	// Singleton Instance
	static CGameClient* s_pGame;

	// Window Variables
	HWND m_hWnd;
	int m_iScreenWidth;
	int m_iScreenHeight;

	// Client Network Variables
	bool m_bNetworkOnline;
	CClient* m_pClientNetwork;
	ClientToServer* m_pClientToServer;
	ServerToClient* m_pServerToClient;
	std::thread m_ReceiveThread;


};

#endif //__GAMECLIENT_H__