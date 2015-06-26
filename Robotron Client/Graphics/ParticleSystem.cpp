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

// Constants
const DWORD kFvfVertexParticle = D3DFVF_XYZ | D3DFVF_DIFFUSE;

CParticleSystem::CParticleSystem()
{
	m_pDevice = 0;
	m_pVertexBuffer = 0;

	m_vec3Origin = { 0, 0, 0 };
	m_fEmitRate = 10;
	m_fSize = 1;
	m_iMaxParticles = 100;
	m_uiVertexBufferSize = 100;
}

CParticleSystem::~CParticleSystem()
{
	// Delete the vertex Buffer
	if (m_pVertexBuffer != 0)
	{
		m_pVertexBuffer->Release();
	}
}

bool CParticleSystem::Initialise(IDirect3DDevice9* _pDevice)
{
	// PS: Particle Emitter

	// Create a Vertex Buffer for the Particle System
	_pDevice->CreateVertexBuffer(	(m_uiVertexBufferSize * sizeof(VertexParticle)),
									D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
									kFvfVertexParticle,
									D3DPOOL_DEFAULT,
									&m_pVertexBuffer,
									0);


	return true;
}

void CParticleSystem::Reset()
{
	// Loop through all Particles
	for (UINT i = 0; i < m_vecParticles.size(); i++)
	{
		ResetParticle(&(m_vecParticles[i]));
	}
}

void CParticleSystem::ResetParticle(Particle* _pParticle)
{
	_pParticle->bAlive = false;
	_pParticle->fAge = 0;
	_pParticle->v3Acceleration = { 0, 0, 0 };
	_pParticle->v3Position = { 0, 0, 0 };
	_pParticle->v3Velocity = { 0, 0, 0 };
}

void CParticleSystem::AddParticle()
{
	// Creates and Resets new Particle
	Particle newParticle;
	ResetParticle(&newParticle);

	// Adds the Particle to the vector
	m_vecParticles.push_back(newParticle);
}

void CParticleSystem::PreDraw()
{

}

void CParticleSystem::Draw()
{

}

void CParticleSystem::PostDraw()
{

}

bool CParticleSystem::IsEmpty()
{
	return true;
}

bool CParticleSystem::IsDead()
{
	return true;
}

void CParticleSystem::RemoveDeadParticles()
{

}




