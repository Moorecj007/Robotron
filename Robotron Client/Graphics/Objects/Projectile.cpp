/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Projectile.cpp
* Description : Implementation for the functions of the Projectile class
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Includes
#include "Projectile.h"


CProjectile::CProjectile(std::string _strAvatarCreator, v3float _v3Dir, UINT _iDamage)
{
	m_strAvatarCreator = _strAvatarCreator;
	m_v3Direction = _v3Dir;
}

CProjectile::~CProjectile()
{
}
