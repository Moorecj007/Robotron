/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : 3DObject.h
* Description : Declarations for the functions of the 3D Object class
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

// Local Includes
#include "C3DObject.h"

class CProjectile :
	public C3DObject
{
public:
	// Constructors / Destructors

	/***********************
	* CProjectile: Contructor for Projectile class
	* @author: Callan Moore
	* @parameter: _strAvatarCreator: The Username of the Avatar that created this projectile
	* @parameter: _v3Dir: Direction the projectile is fired in
	* @parameter: _fDamage: Damage of the Projectile based on the player
	********************/
	CProjectile(std::string _strAvatar, v3float _v3Dir, UINT _fDamage);

	/***********************
	* ~CProjectile: Default Contructor for Projectile class
	* @author: Callan Moore
	********************/
	~CProjectile();

	// Getters

	/***********************
	* GetAvatarCreator: Retrieve the usersname of the avatar that fired this projectile
	* @author: Callan Moore
	* @return: std::string: The username of the avatar that created this projectile
	********************/
	std::string GetAvatarCreator() { return m_strAvatarCreator; };
	
	// Setters

	// Prototypes

private:
	// Member Variables
	std::string m_strAvatarCreator;
};

#endif // __PROJECTILE_H__

