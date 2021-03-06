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
		break;
	}
	case ET_SENTINEL:
	{
		break;
	}
	case ET_SHADOW:
	{
		break;
	}
	default: break;
	}
}

CEnemy::~CEnemy()
{
}
