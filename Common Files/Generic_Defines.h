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
#include <cassert>

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

enum eEnemyType
{
	ET_DEMON,		// Classic grunt
	ET_SENTINEL,	// Big Passive wanderer. Attacks only when hit first
	ET_DEATHSQUAD	// Leader Following
};

// Structs

struct v2float
{
	float u;
	float v;

	/***********************
	* v2float: Constructor for the v2float struct
	* @author: Callan Moore
	* @parameter: u: U coordinate
	* @parameter: v: V coordinate
	********************/
	v2float(float _u = 0, float _v = 0)
	{
		u = _u;
		v = _v;
	}
};

struct v3float
{
	float x;
	float y;
	float z;

	/***********************
	* v3float: Constructor for the v3float struct
	* @author: Callan Moore
	* @parameter: x: X coordinate
	* @parameter: y: Y coordinate
	* @parameter: z: Z coordinate
	********************/
	v3float(float _x = 0, float _y = 0, float _z = 0)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	/***********************
	* operator=: Assignment Operator for the v3float struct
	* @author: Callan Moore
	* @parameter: _v3: v3float to be copied
	* @return: v3float: the result of the assignment 
	********************/
	v3float& operator=(const v3float _v3)
	{
		x = _v3.x;
		y = _v3.y;
		z = _v3.z;
		return *this;
	}

	/***********************
	* operator+: Addition Operator for the v3float struct
	* @author: Callan Moore
	* @parameter: _v3: Right hand side of the addition
	* @return: v3float: The result of the addition
	********************/
	v3float operator+ (const v3float _v3) const
	{
		return v3float{ _v3.x + x, _v3.y + y, _v3.z + z };
	}

	/***********************
	* operator+=: Addition Assignment Operator for the v3float struct
	* @author: Callan Moore
	* @parameter: _v3: Right hand side of the addition
	* @return: v3float: The result of the addition assignment
	********************/
	v3float operator+= (const v3float _v3)
	{
		return v3float{ x += _v3.x, y += _v3.x, z += _v3.x };
	}

	/***********************
	* operator-: Subtraction Operator for the v3float struct
	* @author: Callan Moore
	* @parameter: _v3: Right hand side of the subtraction
	* @return: v3float: The result of the subtraction
	********************/
	v3float operator- (const v3float _v3) const
	{
		return v3float{ _v3.x - x, _v3.y - y, _v3.z - z };
	}

	/***********************
	* operator-: Multiplication Operator for the v3float struct
	* @author: Callan Moore
	* @parameter: _v3: Right hand side of the multiplication
	* @return: v3float: The result of the multiplication
	********************/
	v3float operator* ( const v3float _v3) const
	{
		return v3float{ _v3.x * x, _v3.y * y, _v3.z * z };
	}

	float Magnitude()
	{
		return ( sqrt( pow(x, 2) + pow(x, 2) + pow(x, 2)));
	}
};

struct Controls
{
	bool bAction;
	bool bUp;
	bool bDown;
	bool bLeft;
	bool bRight;
	bool bEsc;
	// TO DO - add Cursor pos as a point
};

// Functions

/***********************
* NormaliseV3Float: Normalise a vector of 3 floats
* @author: Callan Moore
* @parameter: _v3: vecctor to normalise
* @return: v3float: the normalised vector
********************/
inline bool NormaliseV3Float(v3float* _v3)
{
	float fMagnitude = _v3->Magnitude();

	if (fMagnitude != 0)
	{
		_v3->x = (_v3->x / fMagnitude);
		_v3->y = (_v3->y / fMagnitude);
		_v3->z = (_v3->z / fMagnitude);
		return true;
	}
	return false;
}

#endif //__GENERICDEFINES_H__
