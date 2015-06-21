/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Flare.cpp
* Description : Implementation for the functions of the Flare class
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Includes
#include "Flare.h"

// Constructors / Destructors
CFlare::CFlare(IRenderer* _pRenderer)
{
	m_iFlareLightID = _pRenderer->CreateFlareLight();
	m_bActive = false;
}

CFlare::~CFlare()
{
}

// Setters
void CFlare::SetActive(bool _bActive)
{ 
	m_bActive = _bActive; 
	m_pRenderer->LightEnable(m_iFlareLightID, _bActive);
}

// Prototypes
void CFlare::ReactivateFlare(v3float _v3Pos, float _fRange, UINT _iID)
{
	SetActive(true);
	m_iID = _iID;
	m_v3Position = _v3Pos;
	m_fRange = _fRange;
	m_pRenderer->UpdateFlareLight(m_iFlareLightID, m_v3Position, m_fRange);
}

void CFlare::UpdateFlareLight(v3float _v3Pos, float _fRange, float _fDT)
{
	SetActive(true);
	m_v3Position = _v3Pos;
	m_fRange = _fRange;
	m_pRenderer->UpdateFlareLight(m_iFlareLightID, m_v3Position, m_fRange);
}