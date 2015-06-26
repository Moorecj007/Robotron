/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : CDebugCamera.h
* Description : Debugging camera with free movement
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __DEBUGCAMERA_H__
#define __DEBUGCAMERA_H__

// Library Includes
#include <d3d9.h>
#include <d3dx9.h>

// Local Includes
#include "IRenderer.h"

class CDebugCamera
{
public:
	// Contructors / Destructors

	/***********************
	* CDebugCamera: Default Constructor of the Debug Camera
	* @author: Callan Moore
	* @return:
	********************/
	CDebugCamera();

	/***********************
	* CDebugCamera: Default Destructor of the Debug Camera
	* @author: Callan Moore
	* @return:
	********************/
	~CDebugCamera();

	/***********************
	* Initialise: Initialise the Debug Camera for Use
	* @author: Callan Moore
	* @return: bool: Successful Initialisation (or not)
	********************/
	bool Initialise(IRenderer* _pRenderer);

	/***********************
	* Process: Process the Camera
	* @author: Callan Moore
	* @return: void
	********************/
	void Process();

	/***********************
	* GetViewMatrix: Retrieves the view matrix of the camera
	* @author: Callan Moore
	* @return: D3DXMATRIX*: Cameras view matrix
	********************/
	D3DXMATRIX* GetViewMatrix(){ return &m_matView; };

	/***********************
	* GetPosition: Retrieves the Position of the Camera
	* @author: Callan Moore
	* @return: D3DXVECTOR3*: Cameras Position vector
	********************/
	D3DXVECTOR3* GetPosition(){	return &m_vecPosition;};
	
	/***********************
	* GetTarget: Retrieves the Target vector of the camera
	* @author: Callan Moore
	* @return: D3DXVECTOR3*: Cameras Target Vector
	********************/
	D3DXVECTOR3* GetTarget(){ return &m_vecTarget;	};

	/***********************
	* GetUp: Retrieves the Up vector of the camera
	* @author: Callan Moore
	* @return: D3DXVECTOR3*: Up Vector of the Camera
	********************/
	D3DXVECTOR3* GetUp(){ return &m_vecUp; };

	/***********************
	* GetLook: Retrieves the Look at vector
	* @author: Callan Moore
	* @return: D3DXVECTOR3*: Cameras Look At Vector
	********************/
	D3DXVECTOR3* GetLook() { return &m_vecLookAt; };

	/***********************
	* GetMaxPitch: Retrieves the Max Velocity
	* @author: Callan Moore
	* @return: float: The Max Velocity
	********************/
	float GetMaxVelocity(){	return m_fMaxVelocity;	};

	/***********************
	* GetMaxPitch: Retrieves the Max Pitch rotation
	* @author: Callan Moore
	* @return: float: The Pitch Rotation
	********************/
	float GetMaxPitch() { return m_fMaxPitch;};
	
	/***********************
	* GetPitch: Retrieves the Pitch rotation 
	* @author: Callan Moore
	* @return: float: The Pitch Rotation
	********************/
	float GetPitch() {return m_fPitch;};
	
	/***********************
	* GetYaw: Retrieves the Yaw rotation
	* @author: Callan Moore
	* @return: float: The Yaw Rotation
	********************/
	float GetYaw() { return m_fYaw; };

	/***********************
	* SetView: Set the view of the camera using Position, Lookat and Up vectors
	* @author: Callan Moore
	* @parameter: _vecPos: Position vector
	* @parameter: _vecLookAt: Look at vector
	* @parameter: _vecUp: Up vector
	* @return: void
	********************/
	void SetView(D3DXVECTOR3& _vecPos, D3DXVECTOR3& _vecLookAt, D3DXVECTOR3& _vecUp);
	
	/***********************
	* SetPostionVec: Set the Position vector
	* @author: Callan Moore
	* @parameter: _vecPos: The new Position vector
	* @return: void
	********************/
	void SetPostionVec(D3DXVECTOR3& _vecPos){ m_vecPosition = _vecPos;};
	
	/***********************
	* SetLookAtVec: Set the Look At vector
	* @author: Callan Moore
	* @parameter: _D3DVecLookAt: The Look At vector to set the camera to
	* @return: void
	********************/
	void SetTargetVector(D3DXVECTOR3& _vecLookAt){ m_vecTarget = _vecLookAt;};
	
	/***********************
	* SetUpVector: Set the Up vector
	* @author: Callan Moore
	* @parameter: _vecUp: The new Up vector for the camera
	* @return: void
	********************/
	void SetUpVector(D3DXVECTOR3& _vecUp) { m_vecUp = _vecUp; };
	
	
	/***********************
	* SetMaxVelocity: Set the Max Velocity for the Camera
	* @author: Callan Moore
	* @Parameter: _fMaxVelocity: The new value for Max Velocity
	* @return: void
	********************/
	void SetMaxVelocity(float _fMaxVelocity) { m_fMaxVelocity = _fMaxVelocity; }
	
	/***********************
	* SetMaxPitch: Set the Maximum pitch for the Camera
	* @author: Callan Moore
	* @Parameter: _fMaxPitch: The new value for Max Pitch
	* @return: void
	********************/
	void SetMaxPitch(float _fMaxPitch) { m_fMaxPitch = _fMaxPitch; }
	
	/***********************
	* SetSpeed: Set the Speed for the Camera
	* @author: Callan Moore
	* @Parameter: _fSpeed: The new value of speed
	* @return: void
	********************/
	void SetSpeed(float _fSpeed) { m_fSpeed = _fSpeed; }

	/***********************
	* Pitch: Rotate on the X axis
	* @author: Callan Moore
	* @parameter: _fDeg: Degrees to rotate
	* @return: void
	********************/
	void Pitch(float _fDeg);

	/***********************
	* Yaw: Rotate on the Y axis
	* @author: Callan Moore
	* @parameter: _fDeg: Degrees to rotate
	* @return: void
	********************/
	void Yaw(float _fDeg);
	
	/***********************
	* Move: Move the Camera along the Z axis
	* @author: Callan Moore
	* @parameter: _fDistance: Distance to move
	* @return: void
	********************/
	void Move(float _fDistance);
	
private:
	// Member variables
	IRenderer* m_pRenderer;
	D3DXMATRIX  m_matView;
		
	// Vectors
	D3DXVECTOR3 m_vecRight;	
	D3DXVECTOR3 m_vecUp;		
	D3DXVECTOR3 m_vecLookAt;	
	D3DXVECTOR3 m_vecPosition;
	D3DXVECTOR3 m_vecTarget;		
	D3DXVECTOR3 m_vecVelocity;	
	
	// floats
	float m_fYaw;
	float m_fPitch;
	float m_fMaxPitch;
	float m_fMaxVelocity;
	float m_fFOV;
	float m_fAspectRatio;
	float m_fNear;
	float m_fFar;
	float m_fSpeed;

	// Bools
	bool  m_bEnableYMovement;	
};

#endif // __DEBUGCAMERA_H__

