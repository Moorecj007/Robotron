/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : ParticleSystem.h
* Description : Handles all the Particles for the Client
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __PARTICLESYSTEM_H__
#define __PARTICLESYSTEM_H__

// Library Includes
#include <vector>

// Local Includes
#include "Particle.h"
#include "IRenderer.h"

class CParticleSystem
{
public:

	/***********************
	* CParticleSystem: Default Constructor for ParticleSystem class
	* @author: Callan Moore
	* @parameter: _pRenderer: The Renderer for this application
	********************/
	CParticleSystem(IRenderer* _pRenderer);

	/***********************
	* ~CParticleSystem: Default Constructor for ParticleSystem class
	* @author: Callan Moore
	********************/
	~CParticleSystem();

	/***********************
	* Initialise: Initialise the Particle System for use
	* @author: Callan Moore
	* @parameter: _iNumParticles: Number of particles in the system
	* @parameter: _fLifeSpan: Life span of each particle in the system
	* @return: bool: Successful Initialisation (or not);
	********************/
	bool Initialise(UINT _iNumParticles, float _fLifeSpan);

private:
	// Member Variables
	IRenderer* m_pRenderer;
	bool m_bActive;
	std::vector<CParticle*>* m_pVecParticles;
	float m_fParticleLifeSpan;
	v3float m_v3Position;
};

#endif // __PARTICLESYSTEM_H__

