/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Generic_Defines.h
* Description : Declaration of generic defines useful for all applications
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __GENERICDEFINES_H__
#define __GENERICDEFINES_H__

// Library Includes
#pragma comment(lib, "Winmm.lib")
#include <string>

// Defines
#define VALIDATE(a) if (!a) return (false)

// Enums

// Enumerators
enum eScreenState
{
	STATE_MAIN_MENU,

	// Main Menu
	STATE_SINGLEPLAYER_MENU,
	STATE_MULTIPLAYER_MENU,
	STATE_INSTRUCTIONS_MENU,
	STATE_OPTIONS_MENU,

	// Play Menu
	STATE_HOST_GAME,

	// Creating User and server names
	STATE_CREATEUSER,
	STATE_CREATE_SERVERNAME,

	STATE_SELECT_SERVER,
	STATE_GAME_LOBBY,
	STATE_GAMELOADING,

	STATE_TERMINATED_SERVER,

	// During Gameplay
	STATE_GAME_PLAY
};

enum eFontType
{
	TITLE_FONT,
	SUBTITLE_FONT,
	MENU_FONT,
	SCREEN_FONT
};

enum eAllignmentH
{
	H_LEFT,
	H_CENTER,
	H_RIGHT
};


// Structs

struct v3float
{
	float x;
	float y;
	float z;
};

struct Controls
{
	bool bAction;
	bool bUp;
	bool bDown;
	bool bLeft;
	bool bRight;
	bool bEsc;
};

#endif //__GENERICDEFINES_H__
