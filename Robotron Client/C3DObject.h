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

#ifndef __C3DOBJECT_H__
#define __C3DOBJECT_H__

// Library Includes
#include <d3dx9.h>

// Local Includes
#include "Mesh.h"
#include "../Common Files/Clock.h"
#include "IRenderer.h"

class C3DObject
{
public:
	// Constructors / Destructors
	C3DObject();
	~C3DObject();
	
	// Getters
	float GetYawRevolution();
	float GetPitchRevolution();
	float GetRollRevolution();
	D3DXMATRIX GetWorldMatrix();
	D3DXVECTOR3* GetTarget();
	D3DXVECTOR3* GetPosition();
	D3DXVECTOR3* GetLook();
	D3DXVECTOR3* GetUp();
	
	// Setters
	void SetMesh(CMesh* _pMesh);
	void SetYaw(float _f);
	void SetPitch(float _f);
	void SetRoll(float _f);
	void SetYawRevolution(float _f);
	void SetPitchRevolution(float _f);
	void SetRollRevolution(float _f);
	void SetX(float _f);
	void SetY(float _f);
	void SetZ(float _f);
	void SetPosition(float _fX, float _fY, float _fZ);
	void SetSpeed(float _fSpeed);
	void SetMaterial(IRenderer* _pRenderer, MaterialComponents _materialComponents);

	// Prototypes
	void Initialise(CMesh* _pMesh, float _fX = 0, float _fY = 0, float _fZ = 0);
	void Draw(IRenderer* _pRenderer);
	void Process(float _fDT);
	void MoveForward(float _fDT);
	void Turn(float _fDT);
	void Pitch(float _fDT);
	
protected:
	void CalcWorldMatrix(IRenderer* _pRenderer);

protected:
	// Member Variables
	CMesh* m_pMesh;

	float m_fRotationYaw;
	float m_fRotationPitch;
	float m_fRotationRoll;

	float m_fYawRevolution;
	float m_fPitchRevolution;
	float m_fRollRevolution;

	float m_fX;
	float m_fY;
	float m_fZ;

	float m_fDT;
	float m_fSpeed;
	float m_fTurningSpeed;

	float m_fMaxPitch;
	float m_fPitch;

	D3DXVECTOR3 m_v3Look;
	D3DXVECTOR3 m_v3Position;
	D3DXVECTOR3 m_v3Up;
	D3DXVECTOR3 m_v3Velocity;
	D3DXVECTOR3 m_v3Target;
	D3DXVECTOR3 m_v3Right;

	D3DXMATRIX m_matWorld;
	D3DXMATRIX m_matView;

	int m_iMaterialID;
};

#endif // __C3DOBJECT_H__ 