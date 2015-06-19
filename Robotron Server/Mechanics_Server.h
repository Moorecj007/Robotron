/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Mechanics_Server.cpp
* Description : Handles all the Mechanics on the server
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __SERVER_MECHANICS_H__
#define __SERVER_MECHANICS_H__

// Library Includes
#include <map>

// Local Includes
#include "../Common Files/Networking/Network_Defines.h"
#include "../Common Files/Clock.h"


class CMechanics_Server
{
public:
	// Constructors / Destructors

	/***********************
	* CMechanics_Server: Default Constructor for Server Mechanics class
	* @author: Callan Moore
	********************/
	CMechanics_Server();

	/***********************
	* ~CMechanics_Server: Default Destructor for Server Mechanics class
	* @author: Callan Moore
	********************/
	~CMechanics_Server();

	// Getters

	// Setters

	/***********************
	* SetAvatarAliveState: Set the Alive state of an Avatar
	* @author: Callan Moore
	* @parameter: _strAvatar: Name of the Avatar
	* @parameter: _bAlive: Alive state to set
	* @return: void
	********************/
	void SetAvatarAliveState(std::string _strAvatar, bool _bAlive);

	// Prototypes

	/***********************
	* Initialise: Initialise the Server mechanics class for use
	* @author: Callan Moore
	* @parameter: _strServerName: Name of the Server
	* @return: bool: Successful initialisation (or not)
	********************/
	bool Initialise(std::string _strServerName);

	/***********************
	* Process: Process the mechanics of the server
	* @author: Callan Moore
	* @parameter: _pServerPacket: Server to Client Packet to create from the mechanics info
	* @parameter: bool: Successful initialisation (or not)
	********************/
	void Process(ServerToClient* _pServerPacket);

	/***********************
	* ProcessAvatarMovement: Process an Avatars movement input
	* @author: Callan Moore
	* @parameter: _pClientPacket: Client Packet that holds the movement information for a client/avatar
	* @parameter: bool: Successful initialisation (or not)
	********************/
	void ProcessAvatarMovement(ClientToServer* _pClientPacket);

	/***********************
	* CreateDataPacket: Creates the DataPacket to send with all relevant information
	* @author: Callan Moore
	* @parameter: _pServerPacket: Server to client packet to create
	* @return: bool: Successful creation of Data Packet (or not)
	********************/
	bool CreateDataPacket(ServerToClient* _pServerPacket);

	/***********************
	* AddAvatar: Add an Avatar to the Container
	* @author: Callan Moore
	* @parameter: _pClientPacket: Packet that contains the data needed to create the new Avatar
	* @return: void
	********************/
	void AddAvatar(ClientToServer* _pClientPacket);

	/***********************
	* RemoveAvatar: Remove an Avatar from the Container
	* @author: Callan Moore
	* @parameter: _strUserName: The username of the avatar to remove
	* @return: void
	********************/
	void RemoveAvatar(std::string _strUserName);

	/***********************
	* AddAvatarsToPacket: Adds all the Avatar Info to the input Packet
	* @author: Callan Moore
	* @parameter: _pServerPacket: Server to client packet to add Avatar Info to
	* @return: bool: Successful addition (or not
	********************/
	bool AddAvatarsToPacket(ServerToClient* _pServerPacket);

	/***********************
	* CheckAllAvatarsReady: Check if all the avatars have said they are ready
	* @author: Callan Moore
	* @return: bool: Successful addition (or not
	********************/
	bool CheckAllAvatarsReady();


private:
	// Member Variables
	std::string m_strServerName;
	CClock* m_pClock;

	std::map<std::string, AvatarInfo>* m_pAvatars;
	std::map<UINT, EnemyInfo>* m_pEnemies;
};

#endif // __SERVER_MECHANICS_H__