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
#include "Graphics_Defines.h"

// Structs

struct VertexParticle
{
	D3DXVECTOR3 v3Position;
	D3DCOLOR color;
};

struct Particle
{
	D3DXVECTOR3 v3Position;			// Position in world space
	D3DXVECTOR3 v3Velocity;			// Units per second
	D3DXVECTOR3 v3Acceleration;		// Units per second...
	float fLifeSpan;				// Lifespan
	float fAge;						// Current age
	D3DXCOLOR color;				// Colour
	D3DXCOLOR colorFade;			// Change over time
	bool bAlive;					// True = Alive
};

class CParticleSystem
{
public:

	/***********************
	* CParticleSystem: Default Constructor for ParticleSystem class
	* @author: Callan Moore
	********************/
	CParticleSystem();

	/***********************
	* ~CParticleSystem: Default Constructor for ParticleSystem class
	* @author: Callan Moore
	********************/
	~CParticleSystem();

	/***********************
	* Initialise: Initialise the Particle System for use
	* @author: Callan Moore
	* @parameter: _pDevice: The Device of the application
	* @return: bool: Successful Initialisation (or not)
	********************/
	virtual bool Initialise(IDirect3DDevice9* _pDevice);

	/***********************
	* Reset: Resets the Particle System
	* @author: Callan Moore
	* @return: void
	********************/
	virtual void Reset();

	/***********************
	* Reset: Resets a Particle in the System
	* @author: Callan Moore
	* @parameter: _pParticle: The Particle to reset
	* @return: void
	********************/
	virtual void ResetParticle(Particle* _pParticle);

	/***********************
	* AddParticle: Adds a new Particle to the System
	* @author: Callan Moore
	* @return: void
	********************/
	virtual void AddParticle();

	virtual void Process(float _fDT) = 0;

	virtual void PreDraw();

	virtual void Draw();

	virtual void PostDraw();

	bool IsEmpty();

	bool IsDead();

protected:
	virtual void RemoveDeadParticles();

protected:
	// Member Variables
	IDirect3DDevice9* m_pDevice;
	IDirect3DVertexBuffer9* m_pVertexBuffer;

	D3DXVECTOR3 m_vec3Origin;
	float m_fEmitRate;
	float m_fSize;
	int m_iMaxParticles;
	unsigned int m_uiVertexBufferSize;

	std::vector<Particle> m_vecParticles; 
};

#endif // __PARTICLESYSTEM_H__

