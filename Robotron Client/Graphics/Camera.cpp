/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Camera.cpp
* Description : Implementation for the functions of the Camera class
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Includes
#include "Camera.h"

/***********************9
* CCamera: Default Contructor for Camera Class
* @author: Callan Moore

********************/
CCamera::CCamera()
{
	m_v3Position = { 0.0f, 0.0f, 0.0f };;
	m_v3Target = { 0.0f, 0.0f, 0.0f };
	m_v3Up = { 0.0f, 1.0f, 0.0f };

	m_bFirstPerson = true;
}

/***********************
* ~CCamera: Default Destructor for Camera Class
* @author: Callan Moore
********************/
CCamera::~CCamera()
{
}

/***********************
* Initialise: Initialise the Camera for use
* @author: Callan Moore
* @parameter: _v3Position: Position of the Camera in 3D model space
* @parameter: _v3LookAt: Direction the Camera is looking in 3D model space
* @parameter: _bFirstPerson: True if the Camera is in First Person Mode. False for Third Person Mode
* @return: bool: Successful Initialisation or not
********************/
bool CCamera::Initialise(D3DXVECTOR3 _v3Position, D3DXVECTOR3 _v3LookAt, bool _bFirstPerson)
{
	m_v3Position = _v3Position;
	m_v3Target = _v3LookAt;
	m_v3Up = { 0.0f, 1.0f, 0.0f };	// Set the Up Vector to be positive up the Y Axis
	m_bFirstPerson = _bFirstPerson;

	// Sets the Distance the Camera will be behind the Avatar in third person mode
	m_fDistanceBehindAvatar = 15.0f;

	// Sets the Distance the Camera will be Above the Avatar in third person mode
	m_fDistanceAboveAvatar = 5.0f;

	return true;
}

/***********************
* Process: Process the Camera updating the Renderers View Matrix
* @author: Callan Moore
* @parameter: _pRenderer: Pointer to the Renderer for this application
* @return: void
********************/
void CCamera::Process(IRenderer* _pRenderer)
{
	_pRenderer->CreateViewMatrix(m_v3Position, m_v3Target, m_v3Up);
}

/***********************
* SetPosition: Sets the Cameras Position
* @author: Callan Moore
* @parameter: _vecPosition: The new position for the Camera
* @return: void
********************/
void CCamera::SetPosition(D3DXVECTOR3 _vecPosition)
{
	m_v3Position = _vecPosition;
}

/***********************
* SetCamera: Set the Cameras Target and Position based on the current View Mode
* @author: Callan Moore
* @parameter: _v3Target: The Target vector of the Avatar
* @parameter: _v3Position: The Position vector of the Avatar
* @parameter: _v3Up: The Up vector of the Avatar
* @parameter: _v3Look: The Look vector of the Avatar
* @return: void
********************/
void CCamera::SetCamera(D3DXVECTOR3 _v3Target, D3DXVECTOR3 _v3Position, D3DXVECTOR3 _v3Up, D3DXVECTOR3 _v3Look)
{
	if (m_bFirstPerson)	// First Person Mode
	{
		// Set the Camera to be look at the Avatar
		m_v3Target = _v3Target;

		// Set the Camera to be at the same Position as the Avatar
		m_v3Position = _v3Position;
	}
	else	// Third Person Mode
	{
		// Set the Cameras Target to be the Postion of the Avatar
		m_v3Target = _v3Position;

		// Set the Cameras Postion to the Avatar then Move it back using teh inverted Look Vector
		m_v3Position = _v3Position;
		m_v3Position += (-_v3Look) * m_fDistanceBehindAvatar;

		// Set the Camera to be Above the Avatar by the set ammount 
		m_v3Position += _v3Up * m_fDistanceAboveAvatar;
	}

	// Set the Cameras Up vector to mimic the Avatar
	m_v3Up = _v3Up;
}

/***********************
* SetMode: Set the Camera Mode Directly
* @author: Callan Moore
* @parameter: _bFirstPerson: True if the Camera is to be set to First Person Mode. False for Third Person Mode
* @return: void
********************/
void CCamera::SetMode(bool _bFirstPerson)
{
	m_bFirstPerson = _bFirstPerson;
}

/***********************
* GetCameraMode: Retrieves the boolean for whether the Camera is in First Person Mode
* @author: Callan Moore
* @return: bool: True if Camera is First Person Mode. False if Camera is in Third Person Mode
********************/
bool CCamera::GetCameraMode()
{
	return m_bFirstPerson;
}

/***********************
* GetPosition: Retrieves the Cameras Postion
* @author: Callan Moore
* @return: D3DXVECTOR3: The Cameras Position in 3D model space
********************/
D3DXVECTOR3* CCamera::GetPosition()
{
	return &m_v3Position;
}

/***********************
* GetTarget: Retrieves the Cameras Target Vector
* @author: Callan Moore
* @return: D3DXVECTOR3: Target of the Camera in 3D model space
********************/
D3DXVECTOR3* CCamera::GetTarget()
{
	return &m_v3Target;
}

/***********************
* ToggleMode: Toggles the Cameras Mode between First Person and Third Person
* @author: Callan Moore
* @return: void
********************/
void CCamera::ToggleMode()
{
	// Set the Camera Mode to be the Opposite of the Current Mode
	m_bFirstPerson = !m_bFirstPerson;
}


