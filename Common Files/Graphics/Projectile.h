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
	* @parameter: _strAvatar: The Username of the Avatar that created this projectile
	* @parameter: _v3Pos: The starting positionof the projectile
	* @parameter: _v3Dir: Direction the projectile is fired in
	********************/
	CProjectile(std::string _strAvatar, v3float _v3Pos, v3float _v3Dir);

	/***********************
	* ~CProjectile: Default Contructor for Projectile class
	* @author: Callan Moore
	********************/
	~CProjectile();

	// Getters
	
	/***********************
	* GetDamage: Retrieves this projectile Damage 
	* @author: Callan Moore
	* @return: UINT: The Damage this enemy can inflict
	********************/
	UINT GetDamage() { return m_iDamage; };


	
	// Setters

	/***********************
	* SetDamage: Sets the damage of the projectile
	* @author: Callan Moore
	* @parameter: _iDamage: The damage of the projectile
	* @return: void
	********************/
	void SetDamage(UINT _iDamage) { m_iDamage = _iDamage; };

	// Prototypes

private:
	// Member Variables
	UINT m_iDamage;
	std::string m_strAvatar;
	
};

#endif // __PROJECTILE_H__

