/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : PowerUp.cpp
* Description : Implementation for the functions of the PowerUp class
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Includes
#include "PowerUp.h"

CPowerUp::CPowerUp(ePowerUpType _eType)
{
	m_eType = _eType;
	switch (_eType)
	{
	case PT_HEALTH:
	{
		break;
	}
	default: break;
	}
}


CPowerUp::~CPowerUp()
{
}
