/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Avatar.h
* Description : Declarations for the functions of the Avatar Object
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
	* CAvatar: Contructor for Avatar class
	* @author: Callan Moore
	* @parameter: _pRenderer: The Renderer for this application
	********************/
	CAvatar(IRenderer* _pRenderer);

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
	* @return: UINT: Max Health value
	********************/
	UINT GetMaxHealth() { return m_iMaxHealth; };

	/***********************
	* GetHealth: Retrieve the avatars current Health
	* @author: Callan Moore
	* @return: int: Current health value
	********************/
	int GetHealth() { return m_iCurrentHealth; };

	/***********************
	* GetScore: Retrieve the avatars current score
	* @author: Callan Moore
	* @return: UINT: The avatars score
	********************/
	UINT GetScore() { return m_iScore; };

	/***********************
	* GetTorchID: Retrieve the avatars Torch ID
	* @author: Callan Moore
	* @return: int: The avatars Torch ID
	********************/
	int GetTorchID() { return m_iTorchID; };

	/***********************
	* GetFlareCount: Retrieve the avatars flare count
	* @author: Callan Moore
	* @return: UINT: The avatars current flare count
	********************/
	UINT GetFlareCount() { return m_iFlareCount; };

	/***********************
	* GetLives: Retrieve the avatars Life count
	* @author: Callan Moore
	* @return: int: The avatars current Life count
	********************/
	int GetLives() { return m_iLives; };
	
	// Setters

	/***********************
	* SetAliveStatus: Change the avatars alive status
	* @author: Callan Moore
	* @parameter: _bAlive: The update to the alive status of the avatar
	* @return: void
	********************/
	void SetAliveStatus(bool _bAlive) { m_bAlive = _bAlive; };

	/***********************
	* SetMaxHealth: Change the avatars maximum health
	* @author: Callan Moore
	* @parameter: _iMaxHealth: The update to the maximum health allowed to the avatar
	* @return: void
	********************/
	void SetMaxHealth(UINT _iMaxHealth) { m_iMaxHealth = _iMaxHealth; };

	/***********************
	* SetHealth: Change the avatars current health
	* @author: Callan Moore
	* @parameter: _iHealth: The update to the current health of the avatar
	* @return: void
	********************/
	void SetHealth(int _iHealth) { m_iCurrentHealth = _iHealth; };

	/***********************
	* SetScore: Change the avatars current score
	* @author: Callan Moore
	* @parameter: _iScore: The update to the avatars current score
	* @return: void
	********************/
	void SetScore(UINT _iScore) { m_iScore = _iScore; };

	/***********************
	* SetScore: Change the avatars current score
	* @author: Callan Moore
	* @parameter: _iFlareCount: The update to the avatars Flare count
	* @return: void
	********************/
	void SetFlareCount(UINT _iFlareCount) { m_iFlareCount = _iFlareCount; };

	/***********************
	* SetLives: Set the current life count for the Avatar
	* @author: Callan Moore
	* @parameter: _iLives: The update to the avatars Life Count
	* @return: void
	********************/
	void SetLives(UINT _iLives) { m_iLives = _iLives; };

	// Prototypes

private:
	// Member Variables
	v3float m_v3Up;

	bool m_bAlive;
	int m_iLives;

	UINT m_iMaxHealth;
	int m_iCurrentHealth;
	UINT m_iScore;
	UINT m_iFlareCount;

	int m_iTorchID;
};

#endif // __AVATAR_H__

