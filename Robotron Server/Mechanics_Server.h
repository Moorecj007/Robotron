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

	// Prototypes

	/***********************
	* Initialise: Initialise the Server mechanics class for use
	* @author: Callan Moore
	* @parameter: bool: Successful initialisation (or not)
	********************/
	bool Initialise();

	/***********************
	* Process: Process the mechanics on the server
	* @author: Callan Moore
	* @parameter: bool: Successful initialisation (or not)
	********************/
	void Process();

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


private:
	// Member Variables
	CClock* m_pClock;

	std::map<std::string, AvatarInfo>* m_pAvatars;
	std::map<UINT, EnemyInfo>* m_pEnemies;
};

#endif // __SERVER_MECHANICS_H__