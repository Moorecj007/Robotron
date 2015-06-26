/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : CDebugCamera.cpp
* Description : Debugging camera with free movement
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Includes
#include "DebugCamera.h"

CDebugCamera::CDebugCamera()
{
}

CDebugCamera::~CDebugCamera()
{
}

bool CDebugCamera::Initialise(IRenderer* _pRenderer)
{
	m_pRenderer = _pRenderer;

	m_fMaxPitch = (89.0f);	// Prevent Gimbal Lock
	m_bEnableYMovement = true;
	m_fMaxVelocity = 100.0f;
	m_fSpeed = 200.0f;

	m_vecPosition = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_vecTarget = D3DXVECTOR3(0.0f, 0.0f, 150.0f);
	
	m_vecUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXVec3Normalize(&m_vecUp, &m_vecUp);
	
	m_vecVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vecLookAt = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	D3DXVec3Normalize(&m_vecLookAt, &m_vecLookAt);
	
	Process();
		
	return true;
}

void CDebugCamera::SetView(D3DXVECTOR3& _vecPos, D3DXVECTOR3& _vecLookAt, D3DXVECTOR3& _vecUp)
{
	m_vecPosition = _vecPos;
	m_vecTarget = _vecLookAt;
	m_vecUp = _vecUp;
}

void CDebugCamera::Process()
{
	// Limit the Velocity of the camera by Max velocity
	if (D3DXVec3Length(&m_vecVelocity) > m_fMaxVelocity)
	{
		m_vecVelocity = *(D3DXVec3Normalize(&m_vecVelocity, &m_vecVelocity)) * m_fMaxVelocity;
	}

	m_vecPosition += m_vecVelocity ;
	
	// Reset Velocity to zero to stop continuous movement
	m_vecVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vecTarget = m_vecPosition + m_vecLookAt;

	// Calculate the View Matrix
	D3DXVECTOR3 vecUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&m_matView, &m_vecPosition, &m_vecTarget, &vecUp);

	// Set the Camera vectors from the View Matrix
	m_vecRight.x = m_matView._11;
	m_vecRight.y = m_matView._21;
	m_vecRight.z = m_matView._31;
	m_vecUp.x = m_matView._12;
	m_vecUp.y = m_matView._22;
	m_vecUp.z = m_matView._32;
	m_vecLookAt.x = m_matView._13;
	m_vecLookAt.y = m_matView._23;
	m_vecLookAt.z = m_matView._33;

	// Calculate the Pitch
	float fLookLengthOnXZ = sqrtf(pow( m_vecLookAt.z, 2) + pow(m_vecLookAt.x, 2));
	m_fPitch = D3DXToDegree(atan2f(m_vecLookAt.y, fLookLengthOnXZ));
	
	// Set the View Matrix on the Renderer
	m_pRenderer->CreateViewMatrix(m_vecPosition, m_vecTarget, vecUp);
}

void CDebugCamera::Pitch(float _fDeg)
{
	float fRotation = _fDeg * m_fSpeed;
	if (fRotation == 0.0f)
	{
		return;
	}
			
	m_fPitch -= fRotation;
	
	// Prevent Gimbal Lock
	if (m_fPitch > m_fMaxPitch)
	{
		fRotation += m_fPitch - m_fMaxPitch;
	}
	else if (m_fPitch < -m_fMaxPitch)
	{
		fRotation += m_fPitch + m_fMaxPitch;
	}

	D3DXMATRIX matRotation;
	D3DXMatrixRotationAxis(&matRotation, &m_vecRight, D3DXToRadian(fRotation));
	D3DXVec3TransformNormal(&m_vecUp, &m_vecUp, &matRotation);
	D3DXVec3TransformNormal(&m_vecLookAt, &m_vecLookAt, &matRotation);
}

void CDebugCamera::Yaw(float _fDeg)
{
	float fRotation = _fDeg;
	fRotation = fRotation * m_fSpeed;
	
	if (fRotation == 0.0f)
	{
		return;
	}

	D3DXMATRIX matRotation;
	D3DXMatrixRotationAxis(&matRotation, &m_vecUp, D3DXToRadian(fRotation));
	D3DXVec3TransformNormal(&m_vecRight, &m_vecRight, &matRotation);
	D3DXVec3TransformNormal(&m_vecLookAt, &m_vecLookAt, &matRotation);
}

void CDebugCamera::Move(float _fDistance)
{
	if (m_bEnableYMovement)
	{
		_fDistance = _fDistance * m_fSpeed;
		m_vecVelocity += m_vecLookAt * _fDistance;
	}
	else
	{
		D3DXVECTOR3 vecMove(m_vecLookAt.x, 0.0f, m_vecLookAt.z);
		D3DXVec3Normalize(&vecMove, &vecMove);
		vecMove *= _fDistance;
		m_vecVelocity += vecMove;
	}
}

