/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Enemy.cpp
* Description : Implementation for the functions of the Enemy Object
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Includes
#include "Enemy.h"


CEnemy::CEnemy(eEnemyType _eType)
{
	m_eType = _eType;
	switch (_eType)
	{
	case ET_DEMON:
	{
		m_fSize = 0.5f;
		m_iMaxHealth = 100;
		m_iHealth = m_iMaxHealth;
		m_iDamage = 20;
		m_iPointScore = 10;

		// Set a mesh
		// Set a material
		// Set a texture

		// Randomise the starting positions
		// Position
		// Direction
		// Velocity

		// Max Force
		// Acceleration
		// Desired Velocity

		break;
	}
	case ET_SENTINEL:
	{
		break;
	}
	case ET_DEATHSQUAD:
	{
		break;
	}
	default: break;
	}
}

CEnemy::~CEnemy()
{
}
