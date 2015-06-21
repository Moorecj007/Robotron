/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : ParticleSystem.cpp
* Description : Handles all the Particles for the Client
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Include
#include "ParticleSystem.h"

CParticleSystem::CParticleSystem(IRenderer* _pRenderer)
{
	m_pRenderer = _pRenderer;
}

CParticleSystem::~CParticleSystem()
{
}

bool CParticleSystem::Initialise(UINT _iNumParticles, float _fLifeSpan)
{
	return true;
}

