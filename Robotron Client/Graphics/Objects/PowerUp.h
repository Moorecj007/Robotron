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

#pragma once


// Local Includes
#include "C3DObject.h"
#pragma once

class CPowerUp :
	public C3DObject
{
public:
	// Constructors / Destructors
	CPowerUp(ePowerUpType _eType);
	~CPowerUp();

private:
	// member Variables
	ePowerUpType m_eType;
};

