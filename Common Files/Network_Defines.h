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
	unsigned const	DEFAULT_CLIENT_PORT		= 60001;
	unsigned const	MAX_CLIENT_PORT			= 61000;
	unsigned const	MAX_CLIENTS				= 16;
	const char		cUDPAddr[128]			= "127.0.0.1";
	unsigned const	MAX_CHAR_LENGTH			= 100;
}

// Structs

struct ClientToServer
{
	bool bCommand;
	char cCommand[network::MAX_CHAR_LENGTH];
};

struct ServerToClient
{
	bool bCommand;
	char cCommand[network::MAX_CHAR_LENGTH];
};

#endif //__NETWORKDEFINES_H__
