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
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <WS2tcpip.h>

// Local Includes
#include "../Generic_Defines.h"

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
	unsigned const  MAX_ENEMY_SPAWNED		= 30;	// TO DO - calculate properly using % increase per player
	unsigned const  MAX_PROJECTILES			= 100;
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

	START_GAME,

	CREATE_ENEMY,
	DELETE_ENEMY,

	CREATE_POWERUP,
	DELETE_POWERUP,

	CREATE_PROJECTILE,
	DELETE_PROJECTILE
};

// Structs

struct AvailableServer
{
	char cServerName[network::MAX_SERVERNAME_LENGTH];
	char cHostName[network::MAX_USERNAME_LENGTH];
	int iCurrentClients;
	sockaddr_in ServerAddr;
};

struct AvatarInfo
{
	UINT iID;
	char cUserName[network::MAX_USERNAME_LENGTH];
	bool bAlive;
	v3float v3Pos;
	v3float v3Dir;
	v3float v3Vel;
	float fMaxSpeed;
	UINT iDamage;

	BoundingBox BBox;
};

struct EnemyInfo
{
	UINT iID;
	eEnemyType eType;
	v3float v3Pos;
	v3float v3Dir;
	v3float v3Vel;
	v3float v3Acceleration;
	v3float v3Target;
	float fMaxSpeed;
	float fMaxForce;

	BoundingBox BBox;
};

struct PowerUpInfo
{
	UINT iID;
	ePowerUpType eType;
	v3float v3Pos;
	v3float v3Dir;
	v3float v3Vel;
	float fMaxSpeed;

	BoundingBox BBox;
};

struct ProjectileInfo
{
	UINT iID;
	char cUserName[network::MAX_USERNAME_LENGTH];
	v3float v3Pos;
	v3float v3Dir;
	v3float v3Vel;
	UINT iDamage;
	float fMaxSpeed;

	BoundingBox BBox;
};

struct FlareInfo
{
	UINT iID;
	bool bActive;
	v3float v3Pos;
	float fRange;
	float fMaxRange;
	float fMaxSpeed;
	float fTimeLeft;
};

struct ClientToServer
{
	sockaddr_in ClientAddr;
	bool bCommand;
	eNetworkCommand eCommand;
	char cUserName[network::MAX_USERNAME_LENGTH];
	char cAdditionalMessage[network::MAX_CHAR_LENGTH];

	// Gameplay Information
	Controls activatedControls;
};

struct ServerToClient
{
	sockaddr_in ServerAddr;
	bool bCommand;
	eNetworkCommand eCommand;
	char cServerName[network::MAX_SERVERNAME_LENGTH];
	char cUserName[network::MAX_USERNAME_LENGTH];
	EnemyInfo enemyInfo;
	PowerUpInfo powerInfo;
	ProjectileInfo projectileInfo;

	// Gameplay Information
	int iCurrentUserCount;
	AvatarInfo Avatars[network::MAX_CLIENTS];

	int iCurrentEnemyCount;
	EnemyInfo Enemies[network::MAX_ENEMY_SPAWNED];

	int iCurrentProjectileCount;
	ProjectileInfo Projectiles[network::MAX_PROJECTILES];

	FlareInfo Flare;
	
};

#endif //__NETWORKDEFINES_H__
