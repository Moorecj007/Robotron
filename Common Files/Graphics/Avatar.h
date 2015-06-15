/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Avatar.h
* Description : Declarations for the functions of the Avatar class
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __AVATAR_H__
#define __AVATAR_H__

// Local Includes
#include "C3DObject.h"

class CAvatar :
	public C3DObject	// Derived from
{
public:
	// Constructors / Destructors

	/***********************
	* CAvatar: Default Contructor for Avatar class
	* @author: Callan Moore
	********************/
	CAvatar();

	/***********************
	* ~CAvatar: Default Contructor for Avatar class
	* @author: Callan Moore
	********************/
	~CAvatar();

	// Getters

	/***********************
	* GetUpVector: Retrieve the avatars Up vector
	* @author: Callan Moore
	* @return: v3float: Up vector as 3 floats
	********************/
	v3float GetUpVector() { return m_v3Up; };

	/***********************
	* GetAliveStatus: Retrieve the avatars current alive status
	* @author: Callan Moore
	* @return: bool: Status of the avatar being alive
	********************/
	bool GetAliveStatus() { return m_bAlive;};

	/***********************
	* GetMaxHealth: Retrieve the Maximum health for this avatar
	* @author: Callan Moore
	* @return: int: Max Health value
	********************/
	int GetMaxHealth() { return m_iMaxHealth;};

	/***********************
	* GetHealth: Retrieve the avatars current Health
	* @author: Callan Moore
	* @return: int: Current health value
	********************/
	int GetHealth() { return m_iCurrentHealth;};

	/***********************
	* GetScore: Retrieve the avatars current score
	* @author: Callan Moore
	* @return: int: The avatars score
	********************/
	int GetScore() { return m_iScore;};
	
	// Setters

	/***********************
	* SetAliveStatus: Change the avatars alive status
	* @author: Callan Moore
	* @parameter: _bAlive: The update to the alive status of the avatar
	* @return: void
	********************/
	void SetAliveStatus(bool _bAlive);

	/***********************
	* SetMaxHealth: Change the avatars maximum health
	* @author: Callan Moore
	* @parameter: _iMaxHealth: The update to the maximum health allowed to the avatar
	* @return: void
	********************/
	void SetMaxHealth(int _iMaxHealth);

	/***********************
	* SetHealth: Change the avatars current health
	* @author: Callan Moore
	* @parameter: _iHealth: The update to the current health of the avatar
	* @return: void
	********************/
	void SetHealth(int _iHealth);

	/***********************
	* SetScore: Change the avatars current score
	* @author: Callan Moore
	* @parameter: _bAlive: The update to the avatars current score
	* @return: void
	********************/
	void SetScore(int _iScore);

	// Prototypes

private:
	// Member Variables
	v3float m_v3Up;

	bool m_bAlive;

	int m_iMaxHealth;
	int m_iCurrentHealth;
	int m_iScore;

	// LIGHT - TO DO

};

#endif // __AVATAR_H__

