/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : CStaticCamera.h
* Description : Declaration file for StaticCamera class
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/
#pragma once

#ifndef __CAMERA_H__
#define __CAMERA_H__

// Local Includes
#include "IRenderer.h"
#include "Graphics_Defines.h"

class CStaticCamera
{
public:
	// Constructor / Destructor
	CStaticCamera();
	~CStaticCamera();

	// Setters
	void SetPosition(D3DXVECTOR3 _v3Position);
	void SetCamera(D3DXVECTOR3 _v3Target, D3DXVECTOR3 _v3Position, D3DXVECTOR3 _v3Up, D3DXVECTOR3 _v3Look);
	void SetMode(bool _bFirstPerson);

	// Getters
	bool GetCameraMode();
	D3DXVECTOR3* GetPosition();
	D3DXVECTOR3* GetTarget();

	// Prototypes
	bool Initialise(D3DXVECTOR3 _v3Position, D3DXVECTOR3 _v3LookAt, bool _bFirstPerson);
	void Process(IRenderer* _pRenderer);
	void ToggleMode();

private:
	// Member Variables
	D3DXVECTOR3 m_v3Position;
	D3DXVECTOR3 m_v3Target;
	D3DXVECTOR3 m_v3Up;

	float m_fDistanceBehindAvatar;
	float m_fDistanceAboveAvatar;

	bool m_bFirstPerson;
};
#endif __CAMERA_H__