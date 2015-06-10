/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Network_Defines.h
* Description : Declaration file to holds structs, enums and various containers/variables to be shared among the Networking files
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __NETWORKDEFINES_H__
#define __NETWORKDEFINES_H__

// Library Includes
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "Ws2_32.lib")
#include <string>
#include <winsock2.h>
#include <WS2tcpip.h>

// Defines
#define VALIDATE(a) if (!a) return (false)

namespace network
{
	unsigned const	DEFAULT_SERVER_PORT		= 60000; 
	unsigned const	MAX_SERVER_PORT			= 60004;
	unsigned const	DEFAULT_CLIENT_PORT		= 60005;
	unsigned const	MAX_CLIENT_PORT			= 61000;
	unsigned const	MAX_CLIENTS				= 6;
	const char		cUDPAddr[32]			= "127.0.0.1";
	unsigned const	MAX_CHAR_LENGTH			= 30;
	unsigned const	MAX_USERNAME_LENGTH		= 13;
	unsigned const  MAX_SERVERNAME_LENGTH	= 16;
}

// Enums
enum eNetworkCommand
{
	ERROR_COMMAND,

	TERMINATE_SERVER,

	QUERY_HOST,
	HOST_SERVER,
	NOT_HOST,

	CREATEUSER,
	CREATEUSER_ACCEPTED,
	CREATEUSER_NAMEINUSE,
	CREATEUSER_SERVERFULL,

	QUERY_CLIENT_CONNECTION,
	SERVER_CONNECTION_AVAILABLE,

	USER_JOINED,
	USER_LEFT,
	YOUR_HOST,
	LEAVE_SERVER,

	ALIVE_SET,

	START_GAME
};

// Structs

struct AvailableServer
{
	char cServerName[network::MAX_SERVERNAME_LENGTH];
	char cHostName[network::MAX_USERNAME_LENGTH];
	int iCurrentClients;
	sockaddr_in ServerAddr;
};

struct UserInfo
{
	char cUserName[network::MAX_USERNAME_LENGTH];
	bool bAlive;
	float fPosX;
	float fPosY;
	float fPosZ;
};

struct EnemyInfo
{

};

struct PowerUpInfo
{

};

struct ClientToServer
{
	sockaddr_in ClientAddr;
	bool bCommand;
	eNetworkCommand eCommand;
	char cUserName[network::MAX_USERNAME_LENGTH];
	char cAdditionalMessage[network::MAX_CHAR_LENGTH];

	// Gameplay Information
	//
};

struct ServerToClient
{
	sockaddr_in ServerAddr;

	bool bCommand;
	eNetworkCommand eCommand;
	char cServerName[network::MAX_SERVERNAME_LENGTH];
	char cUserName[network::MAX_USERNAME_LENGTH];
	//char cAdditionalMessage[network::MAX_CHAR_LENGTH];

	// Gameplay Information
	int CurrentUserCount;
	UserInfo UserInfos[network::MAX_CLIENTS];
};




#endif //__NETWORKDEFINES_H__
