/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Flare.h
* Description : Declarations for the functions of the Flare class
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __FLARE_H__
#define __FLARE_H__

// Local Includes
#include "C3DObject.h"
#include "../ParticleSystem.h"

class CFlare :
	public C3DObject
{
public:
	
	/***********************
	* CFlare: Default Contructor for Flare class
	* @author: Callan 
	* @parameter: The Renderer for this application
	********************/
	CFlare(IRenderer* _pRenderer);

	/***********************
	* ~CFlare: Default Destructor for Flare class
	* @author: Callan Moore
	********************/
	~CFlare();

	// Getters

	/***********************
	* GetActive: Retrieve the active state of the flare object
	* @author: Callan Moore
	* @return: bool: The active state
	********************/
	bool GetActive() { return m_bActive; };

	// Setters

	/***********************
	* SetActive: Set the active state of the flare object
	* @author: Callan Moore
	* @parameter _bActive: Boolean to state whether the flare is active or not
	* @return: void
	********************/
	void SetActive(bool _bActive);

	/***********************
	* SetRange: Set the current range of the flare objects light
	* @author: Callan Moore
	* @parameter _fRange: The new Range of the flares light
	* @return: void
	********************/
	void SetRange(float _fRange) { m_fRange = _fRange; };

	//Prototypes

	/***********************
	* ReactivateFlare: Reactivates the flare Light with a new flare
	* @author: Callan Moore
	* @parameter _v3Pos: The new Position of the flares light
	* @parameter _fRange: The new Range of the flares light
	* @parameter _iID: The new ID of the Flare
	* @return: void
	********************/
	void ReactivateFlare(v3float _v3Pos, float _fRange, UINT _iID);

	/***********************
	* UpdateFlareLight: Updates the flare Light 
	* @author: Callan Moore
	* @parameter _v3Pos: The new Position of the flares light
	* @parameter _fRange: The new Range of the flares light
	* @parameter _fDT: The current Delta Tick
	* @return: void
	********************/
	void UpdateFlareLight(v3float _v3Pos, float _fRange, float _fDT);


private:
	// Member Variables
	int m_iFlareLightID;
	bool m_bActive;
	float m_fRange;

	// Particle System
	CParticleSystem* m_pParticleSystem;
};

#endif // __FLARE_H__

