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

	// Setters

	// Prototypes

private:
	// Member Variables
	eEnemyType m_eType;
};
#endif // __ENEMY_H__

