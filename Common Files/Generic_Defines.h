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

// Defines
#define _USE_MATH_DEFINES

// Library Includes
#pragma comment(lib, "Winmm.lib")
#include <string>
#include <cassert>
#include <Windows.h>
#include <math.h>

// Defines
#define VALIDATE(a) if (!a) return (false)

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

enum ePowerUpType
{
	PT_HEALTH,		
	PT_FLARE,	
	PT_UNKNOWN	// TO DO - figure out another powerup	
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
		return v3float{ x += _v3.x, y += _v3.y, z += _v3.z };
	}

	/***********************
	* operator-: Subtraction Operator for the v3float struct
	* @author: Callan Moore
	* @parameter: _v3: Right hand side of the subtraction
	* @return: v3float: The result of the subtraction
	********************/
	v3float operator- (const v3float _v3) const
	{
		return v3float{ x - _v3.x ,y - _v3.y, z - _v3.z};
	}

	/***********************
	* operator*: Multiplication Operator for a v3float times a v3float
	* @author: Callan Moore
	* @parameter: _v3: Right hand side of the multiplication
	* @return: v3float: The result of the multiplication
	********************/
	v3float operator* ( const v3float _v3) const
	{
		return v3float{ _v3.x * x, _v3.y * y, _v3.z * z };
	}

	/***********************
	* operator*: Multiplication Operator for a v3float times a float
	* @author: Callan Moore
	* @parameter: _v3: Right hand side of the multiplication
	* @return: v3float: The result of the multiplication
	********************/
	v3float operator* (const float _f) const
	{
		return v3float{ _f * x, _f * y, _f * z };
	}

	/***********************
	* Magnitude: Calculate the magnitude of this v3float
	* @author: Callan Moore
	* @return: float: The magnitude of the v3float
	********************/
	float Magnitude()
	{
		return ( sqrt( pow(x, 2) + pow(y, 2) + pow(z, 2)));
	}

	/***********************
	* NormaliseV3Float: Normalise a vector of 3 floats
	* @author: Callan Moore
	* @parameter: _v3: vector to normalise
	* @return: v3float: the normalised vector
	********************/
	v3float Normalise()
	{
		float fMagnitude = this->Magnitude();

		if (fMagnitude != 0)
		{
			this->x = (this->x / fMagnitude);
			this->y = (this->y / fMagnitude);
			this->z = (this->z / fMagnitude);
		}
		return (*this);
	}

	/***********************
	* Limit: Limit the magnitude of the vector if it is greater than the input float
	* @author: Callan Moore
	* @parameter: _f: vector to normalise
	* @return: v3float: the normalised vector
	********************/
	v3float Limit(const float _f)
	{
		float fMagnitude = this->Magnitude();

		if (fMagnitude > _f)
		{
			(*this) = (this->Normalise()) * _f;
		}
		return (*this);
	}
};

struct Controls
{
	bool bAction;
	bool bFlare;
	bool bUp;
	bool bDown;
	bool bLeft;
	bool bRight;
	bool bEsc;
	POINT ptMouse;
};

// Functions

/***********************
* StringToStruct: Copies characters from a char array into a struct property
* @author: Callan Moore
* @parameter: _pcData: Char array to input into the struct
* @parameter: _pcStruct: Struct property to receive char array
* @parameter: _iMaxLength: Maximum allowed length of the data to copy
* @return: bool: Successful copy (or Not)
********************/
inline bool StringToStruct(const char* _pcData, char* _pcStruct, unsigned int _iMaxLength)
{
	// Ensure no buffer overrun will occur when copying directly to memory
	if ((strlen(_pcData) + 1) <= (_iMaxLength))
	{
		// Copy the characters into the struct
		strcpy_s(_pcStruct, (strlen(_pcData) + 1), _pcData);
	}
	else
	{
		// char* is too long, buffer overrun would occur so failed operation
		return false;
	}
	return true;
}

/***********************
* NormaliseV3Float: Normalise a vector of 3 floats
* @author: Callan Moore
* @parameter: _v3: vector to normalise
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

/***********************
* DegreesToRadians: Converts degrees to radians
* @author: Callan Moore
* @parameter: _fDegrees: Angle in Degrees to convert
* @return: float: Angle in Radians
********************/
inline float DegreesToRadians(float _fDegrees)
{
	return ((_fDegrees * (float)M_PI) / 180.0f);
}


#endif //__GENERICDEFINES_H__
