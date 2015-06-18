/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Enemy.h
* Description : Declarations for the functions of the Enemy Object
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __ENEMY_H__
#define __ENEMY_H__

// Local Includes
#include "Intelligent.h"

class CEnemy :
	public CIntelligent
{
public:

	// Constructors / Destructors

	/***********************
	* CEnemy: Contructor for Enemy class
	* @author: Callan Moore
	********************/
	CEnemy(eEnemyType _eType);

	/***********************
	* ~CEnemy: Default Destructor for Enemy class
	* @author: Callan Moore
	********************/
	~CEnemy();

	// Getters

	/***********************
	* GetEnemyType: Retrieves this enemies type
	* @author: Callan Moore
	* @return: eEnemyType: The type of enemy 
	********************/
	eEnemyType GetEnemyType() { return m_eType; };

	/***********************
	* GetHealth: Retrieves this enemies health
	* @author: Callan Moore
	* @return: UINT: The Health of this enemy
	********************/
	UINT GetHealth() { return m_iHealth; };

	/***********************
	* GetMaxHealth: Retrieves this enemies maximum health
	* @author: Callan Moore
	* @return: UINT: The maximum health of this enemy
	********************/
	UINT GetMaxHealth() { return m_iMaxHealth; };

	// Setters

	/***********************
	* SetHealth: Set the health of the enemy
	* @author: Callan Moore
	* @parameter: _iHealth: The Health of the enemy
	* @return: void
	********************/
	void SetHealth(UINT _iHealth) { m_iHealth = _iHealth; };

	/***********************
	* SetMaxHealth: Set the Maximum health of the enemy
	* @author: Callan Moore
	* @parameter: _iHealth: The Maximum Health of the enemy
	* @return: void
	********************/
	void SetMaxHealth(UINT _iMaxHealth) { m_iMaxHealth = m_iMaxHealth; };

	// Prototypes

private:
	// Member Variables
	eEnemyType m_eType;
	UINT m_iHealth;
	UINT m_iMaxHealth;
};
#endif // __ENEMY_H__

