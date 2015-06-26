/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Camera.h
* Description : Declaration file for Camera class
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/
#pragma once

#ifndef __CAMERA_H__
#define __CAMERA_H__

// Local Includes
#include "IRenderer.h"
#include "Graphics_Defines.h"

class CCamera
{
public:
	// Constructor / Destructor
	CCamera();
	~CCamera();

	// Setters
	void SetPosition(D3DXVECTOR3 _v3Position);
	void SetCamera(D3DXVECTOR3 _v3Target, D3DXVECTOR3 _v3Position, D3DXVECTOR3 _v3Up);
	void SetMode(bool _bFirstPerson);

	// Getters
	bool GetCameraMode();
	D3DXVECTOR3* GetPosition();
	D3DXVECTOR3* GetTarget();

	// Prototypes
	bool Initialise(D3DXVECTOR3 _v3Position, D3DXVECTOR3 _v3LookAt, bool m_bFirstPerson);
	void Process(IRenderer* _pRenderer);
	void ToggleMode();

private:
	// Member Variables
	D3DXVECTOR3 m_v3Position;
	D3DXVECTOR3 m_v3Target;
	D3DXVECTOR3 m_v3Up;

	float m_fDistanceAboveAvatar;

	bool m_bFirstPerson;
};
#endif __CAMERA_H__