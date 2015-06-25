/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Avatar.cpp
* Description : Avatar class to hold all information for the playable characters/avatars
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Includes
#include "Avatar.h"

CAvatar::CAvatar(IRenderer* _pRenderer)
{
	// Set the starting values for the vectors
	m_v3Position	= { 0.0f, 0.0f, 0.0f };
	m_v3Direction	= { 0.0f, 0.0f, 1.0f };
	m_v3Up			= { 0.0f, 0.0f, 0.0f };

	// All avatars start out as not ready in the game lobby
	m_bAlive = false;

	// Avatar starts out at max health
	m_iMaxHealth = 100;
	m_iCurrentHealth = m_iMaxHealth;

	// Avatars start with no score
	m_iScore = 0;

	// Create the Avatars Torch Light
	m_iTorchID = _pRenderer->CreateTorchLight();
}

CAvatar::~CAvatar()
{
}
