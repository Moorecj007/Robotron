/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Hub_Server.h
* Description : Declaration file for the main game functions of the server
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __Hub_Server_H__
#define __Hub_Server_H__

// Library Includes
#include <thread>
#include <queue>

// Local Includes
#include "Networking/Network_Server.h"
#include "Mechanics_Server.h"
#include "../Common Files/Clock.h"
#include "../Common Files/Networking/MySemaphore.h"
#include "../Common Files/Graphics/Graphics_Defines.h"


class CHub_Server
{
public:

	enum eServerState
	{
		STATE_LOBBY,
		STATE_GAMEPLAY
	};

	// Constructors / Destructors

	/***********************
	* ~Game: Default Destructor for Game class
	* @author: Callan Moore
	********************/
	~CHub_Server();

	// Singleton Methods

	/***********************
	* GetInstance: Returns the singleton instance of the game, if it doesnt exist creates it.
	* @author: Callan Moore
	* @return: CGame&: The current instance of the game
	********************/
	static CHub_Server& GetInstance();

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
	* Initialize: Initialise the Game Object for Use
	* @author: Callan Moore
	* @parameters: _hWnd: Handle to The creating window
	* @parameters: _iScreenWidth: The Height of the Screen
	* @parameters: _iScreenHeight: The Width of the Screen
	* @parameters: _wstrArgs: Command Arguments for the executable. Contains the host name and the server name
	* @return: bool: Successful Initialisation or not
	********************/
	bool Initialise(HWND _hWnd, int _iScreenWidth, int _iScreenHeight, LPWSTR* _wstrArgs);
	
	/***********************
	* ExecuteOneFrame: Executes one frame
	* @author: Callan Moore
	* @return: bool: False if the network is offline
	********************/
	bool ExecuteOneFrame();

	/***********************
	* Process: Processes the Game for the Delta tick
	* @author: Callan Moore
	* @return: void
	********************/
	void Process();

	/***********************
	* ProcessPacket: Process a packet from the work queue
	* @author: Callan Moore
	* @return: void
	********************/
	void ProcessPacket();

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
	* CreateCommandPacket: Create a Packet that sends a command
	* @author: Callan Moore
	* @parameter: _command: The command to send
	* @return: bool: Successful creation of Data Packet (or not)
	********************/
	bool CreateCommandPacket(eNetworkCommand _command);

	/***********************
	* CreateCommandPacket: Create a Packet that sends a command with an Client Username attached 
	* @author: Callan Moore
	* @parameter: _command: The command to send
	* @parameter: _strUserName: A user name of a client
	* @return: bool: Successful creation of Data Packet (or not)
	********************/
	bool CreateCommandPacket(eNetworkCommand _eCommand, std::string _strUserName);

	/***********************
	* StringToStruct: Copies characters from a char array into a struct property
	* @author: Callan Moore
	* @parameter: _pcData: Char array to input into the struct
	* @parameter: _pcStruct: Struct property to receive char array
	* @parameter: _iMaxLength: Maximum allowed length of the data to copy
	* @return: bool: Successful copy (or Not)
	********************/
	bool StringToStruct(const char* _pcData, char* _pcStruct, unsigned int _iMaxLength);

	/***********************
	* ReceiveDataFromNetwork: Thread to receive data from the network
	* @author: Callan Moore
	* @parameter: _pReceiveData: Struct property to receive Data from the Network
	* @return: void
	********************/
	void ReceiveDataFromNetwork(ClientToServer* _pReceiveData);

	/***********************
	* WideStringToString: Converts a wide char string into a standard string
	* @author: Callan Moore
	* @parameter: _wstr: Wide character string to convert
	* @return: std::string: The input as a standard string
	********************/
	std::string WideStringToString(wchar_t* _wstr);

	/***********************
	* InsertUser: Insert a new user into the Map
	* @author: Callan Moore
	* @parameter: _strUser: The username of the user to add
	* @return: bool: True if the user can be added, false if the user cannot
	********************/
	bool InsertUser(std::string _strUser);

private:
	//Disallowing copies and extra constructions
	CHub_Server();
	CHub_Server(const CHub_Server& _kr);
	CHub_Server& operator= (const CHub_Server& _kr);

public:
	// Static Variables
	static CMySemaphore* m_pMutexServer;

private:
	// Singleton Instance
	static CHub_Server* s_pGame;

	// Window Variables
	HWND m_hWnd;
	int m_iScreenWidth;
	int m_iScreenHeight;

	// Server Network Variables
	bool m_bNetworkOnline;
	eServerState m_eServerState;
	CNetwork_Server* m_pNetworkServer;
	ClientToServer* m_pClientToServer;
	ClientToServer* m_pPacketToProcess;
	ServerToClient* m_pServerToClient;
	std::thread m_ReceiveThread;
	std::queue<ClientToServer>* m_pWorkQueue;
	std::string m_strServerName;

	// Host Variable
	std::string m_strHostUser;
	bool m_bRepliedToHost;

	// Server Users
	std::map<std::string, AvatarInfo>* m_pCurrentUsers;

	// Gameplay
	CMechanics_Server* m_pMechanics;
};

#endif //__Hub_Server_H__