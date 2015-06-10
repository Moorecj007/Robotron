/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : 3DObject.cpp
* Description : Implementation for the functions of the 3DObject class
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Includes
#include "C3DObject.h"

/***********************
* C3DObject: Default Contructor for 3DObject class
* @author: Callan Moore
********************/
C3DObject::C3DObject()
{
	m_pMesh = 0;

	m_fX = 0;
	m_fY = 0;
	m_fZ = 0;
	m_fRotationYaw = 0;
	m_fRotationPitch = 0;
	m_fRotationRoll = 0;
	m_fYawRevolution = 0;
	m_fPitchRevolution = 0;
	m_fRollRevolution = 0;

	m_v3Position.x = 0;
	m_v3Position.y = 0;
	m_v3Position.z = 0;

	m_fDT = 0;
	m_fSpeed = 0;
	m_fTurningSpeed = 0;
}

/***********************
* ~C3DObject: Default Destructor for 3DObject class
* @author: Callan Moore
********************/
C3DObject::~C3DObject()
{
}

/***********************
* GetYawRevolution: Retrieves the Yaw Revolution for the 3D Object
* @author: Callan Moore
* @return: float: The Yaw Revolution
********************/
float C3DObject::GetYawRevolution()
{
	return m_fYawRevolution;
}

/***********************
* GetPitchRevolution: Retrieves the Pitch Revolution for the 3D Object
* @author: Callan Moore
* @return: float: The Pitch Revolution
********************/
float C3DObject::GetPitchRevolution()
{
	return m_fPitchRevolution;
}

/***********************
* GetRollRevolution: Retrieves the Roll Revolution for the 3D Object
* @author: Callan Moore
* @return: float: The Roll Revolution 
********************/
float C3DObject::GetRollRevolution()
{
	return m_fRollRevolution;
}

/***********************
* GetWorldMatrix: Retrieves the Objects World Matrix
* @author: Callan Moore
* @return: D3DXMATRIX: The World Matrix of the Object
********************/
D3DXMATRIX C3DObject::GetWorldMatrix()
{
	return m_matWorld;
}

/***********************
* SetMesh: Sets a new Mesh for the 3D Object
* @author: Callan Moore
* @parameter: _pMesh: Pointer to a new mesh for the 3D Object
* @return: void
********************/
void C3DObject::SetMesh(CMesh* _pMesh)
{
	m_pMesh = _pMesh;
}

/***********************
* SetYaw: Sets the Yaw component for the 3D Object
* @author: Callan Moore
* @parameter: _fYaw: New Yaw componenent (Y Rotation) in degrees per second
* @return: void
********************/
void C3DObject::SetYaw(float _fYaw)
{
	m_fRotationYaw = _fYaw;
}

/***********************
* SetPitch: Sets the Pitch component for the 3D Object
* @author: Callan Moore
* @parameter: _fPitch: New Pitch componenent (X Rotation) in degrees per second
* @return: void
********************/
void C3DObject::SetPitch(float _fPitch)
{
	m_fRotationPitch = _fPitch;
}

/***********************
* SetRoll: Sets the Roll component for the 3D Object
* @author: Callan Moore
* @parameter: _fRoll: New Roll componenent (Z Rotation) in degrees per second
* @return: void
********************/
void C3DObject::SetRoll(float _fRoll)
{
	m_fRotationRoll = _fRoll;
}

/***********************
* SetYawRevolution: Sets the Yaw Revolution for the 3D Object
* @author: Callan Moore
* @parameter: _fYaw: New Yaw Revolution (Y Rotation)
* @return: void
********************/
void C3DObject::SetYawRevolution(float _fYaw)
{
	m_fYawRevolution = _fYaw;
}

/***********************
* SetPitchRevolution: Sets the Pitch Revolution for the 3D Object
* @author: Callan Moore
* @parameter: _fPitch: New Pitch Revolution (X Rotation)
* @return: void
********************/
void C3DObject::SetPitchRevolution(float _fPitch)
{
	m_fPitchRevolution = _fPitch;
}

/***********************
* SetRollRevolution: Sets the Roll Revolution for the 3D Object
* @author: Callan Moore
* @parameter: _fRoll: New Roll Revolution (Z Rotation)
* @return: void
********************/
void C3DObject::SetRollRevolution(float _fRoll)
{
	m_fRollRevolution = _fRoll;
}

/***********************
* SetX: Sets the X component for the 3D Object
* @author: Callan Moore
* @parameter: _fX: New X componenent
* @return: void
********************/
void C3DObject::SetX(float _fX)
{
	m_fX = _fX;
}

/***********************
* SetY: Sets the Y component for the 3D Object
* @author: Callan Moore
* @parameter: _fY: New Y componenent
* @return: void
********************/
void C3DObject::SetY(float _fY)
{
	m_fY = _fY;
}

/***********************
* SetZ: Sets the Z component for the 3D Object
* @author: Callan Moore
* @parameter: _fZ: New Z componenent
* @return: void
********************/
void C3DObject::SetZ(float _fZ)
{
	m_fZ = _fZ;
}

/***********************
* C3DObject: Contructor for 3DObject class
* @author: Callan Moore
* @parameter: _pMesh: Mesh for this 3D Object
* @parameter: _fX: Starting X coordinate. Default to 0 if not specified
* @parameter: _fY: Starting Y coordinate. Default to 0 if not specified
* @parameter: _fZ: Starting Z coordinate. Default to 0 if not specified
* @return: void
********************/
void C3DObject::Initialise(CMesh* _pMesh, float _fX, float _fY, float _fZ)
{
	m_pMesh = _pMesh;
	m_fX = _fX;
	m_fY = _fY;
	m_fZ = _fZ;

	m_v3Position.x = _fX;
	m_v3Position.y = _fY;
	m_v3Position.z = _fZ;

	m_fSpeed = 250.0f;					// Movement Speed
	m_fTurningSpeed = 0.5f * m_fSpeed;	// Turning Speed Based on the Movement Speed

	m_v3Look = { 0.0f, 0.0f, 1.0f };
	m_v3Velocity = { 0.0f, 0.0f, 0.0f };
	m_v3Up = { 0.0f, 1.0f, 0.0f };

	m_fMaxPitch = 89.0f;
}

/***********************
* Draw: Draws the 3D Object
* @author: Callan Moore
* @parameter: _pRenderer: Renderer for this application
* @return: void
********************/
void C3DObject::Draw(IRenderer* _pRenderer)
{
	// Calculates the World Matrix for this 3D Object
	CalcWorldMatrix(_pRenderer);
	_pRenderer->SetMaterial(m_iMaterialID);
	m_pMesh->Draw();
}

/***********************
* Process: Processes the 3D Object for the Delta tick
* @author: Callan Moore
* @parameter: _fDT: The Delta Tick
* @return: void
********************/
void C3DObject::Process(float _fDT)
{
	// Set the new Delta tick on the Object
	m_fDT = _fDT;

	// Update the Objects Position
	m_v3Position += m_v3Velocity;

	// Reset the Velocity to stop continous movement
	m_v3Velocity = { 0.0f, 0.0f, 0.0f };

	// Set the Target to be 10 units down the Look vector
	m_v3Target =  m_v3Position + (10.0f * m_v3Look);

	// Reset the Up Vector
	m_v3Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	// Update the View Matrix of the Object
	D3DXMatrixLookAtLH(&m_matView, &m_v3Position, &m_v3Target, &m_v3Up);

	// Update the Objects Look Vector
	m_v3Look.x = m_matView._13;
	m_v3Look.y = m_matView._23;
	m_v3Look.z = m_matView._33;

	// Update the Objects Right Vector
	m_v3Right.x = m_matView._11;
	m_v3Right.y = m_matView._21;
	m_v3Right.z = m_matView._31;

	// Calculate the Pitch of the Object
	float fLookMagnitudePlaneXZ = sqrtf( pow(m_v3Look.x, 2) + pow(m_v3Look.z, 2));
	m_fPitch = D3DXToDegree(atan2f(m_v3Look.y, fLookMagnitudePlaneXZ));
}

/***********************
* CalcWorldMatrix: Calculates the World Matrix for this Object
* @author: Callan Moore
* @parameter: _pRenderer: Renderer for this application
* @return: void
********************/
void C3DObject::CalcWorldMatrix(IRenderer* _pRenderer)
{
	// Matrices to make up the World Matrix
	D3DXMATRIX matRotatePitch;
	D3DXMATRIX matRotateYaw;
	D3DXMATRIX matTranslation;

	D3DXMatrixIdentity(&matRotatePitch);
	D3DXMatrixIdentity(&matRotateYaw);
	D3DXMatrixIdentity(&matTranslation);

	// Reset the Objects World Matrix for new Calculation
	D3DXMatrixIdentity(&m_matWorld);

	// Increases the rotation by the Objects Revolution components
	m_fRotationPitch += m_fPitchRevolution;
	m_fRotationYaw += m_fYawRevolution;
	m_fRotationRoll += m_fRollRevolution;

	// Calculate a Rotation Matrix around the Pitch and Yaw
	D3DXMatrixRotationX(&matRotatePitch, m_fRotationPitch);
	D3DXMatrixRotationY(&matRotateYaw, m_fRotationYaw);

	// Calculate the Full Rotation Matrix and store in the World Matrix
	m_matWorld = matRotatePitch * matRotateYaw;

	// Calculate a translation Matrix based on the Objects current position in world space
	D3DXMatrixTranslation(&matTranslation, m_v3Position.x, m_v3Position.y, m_v3Position.z);

	// Calculate the Objects World Matrix
	m_matWorld *= matTranslation;

	// Change the Renderers World Matrix to the Objects World Matrix
	_pRenderer->SetWorldMatrix(m_matWorld);

	// Reset the Revolution components stopping continuous rotation
	m_fPitchRevolution = 0;
	m_fYawRevolution = 0;
	m_fRollRevolution = 0;
}


/***********************
* SetPosition: Sets the Position of the Object in 3D model space
* @author: Callan Moore
* @parameter: _fX: The X component
* @parameter: _fY: The Y component
* @parameter: _fZ: The Z component
* @return: void
********************/
void C3DObject::SetPosition(float _fX, float _fY, float _fZ)
{
	m_v3Position.x = _fX;
	m_v3Position.y = _fY;
	m_v3Position.z = _fZ;
}

/***********************
* SetSpeed: Sets The Speed for this Object when moving
* @author: Callan Moore
* @parameter: _fSpeed: The new Speed for this Object
* @return: void
********************/
void C3DObject::SetSpeed(float _fSpeed)
{
	m_fSpeed = _fSpeed;
}

/***********************
* SetMaterial: Sets The Material for the Object
* @author: Callan Moore
* @parameter: _materialComponents: Structure containing all needed material info
* @parameter: _pRenderer: Pointer to the Renderer for this application
* @return: void
********************/
void C3DObject::SetMaterial(IRenderer* _pRenderer, MaterialComponents _materialComponents)
{
	m_iMaterialID = _pRenderer->CreateMaterial(_materialComponents);
}

/***********************
* MoveForward: Move the Object Forward 
* @author: Callan Moore
* @parameter: _fDT: The current Delta Tick
* @return: void
********************/
void C3DObject::MoveForward(float _fDT)
{
	// Calculate how far to move in the current delta tick
	float fMovement = _fDT * m_fSpeed;

	// Calculate the Velocity based on the Movement and the Look direction
	D3DXVECTOR3 v3NormalLook;
	D3DXVec3Normalize(&v3NormalLook, &m_v3Look);
	m_v3Velocity += v3NormalLook * fMovement;
}

/***********************
* Turn: Turns the Object around the Yaw (Y-Axis)
* @author: Callan Moore
* @parameter: _fDT: The Current Delta Tick
* @return: void
********************/
void C3DObject::Turn(float _fDT)
{
	// Don't Calculate if Delta tick is too small (Potentially truncated) , No change to Yaw
	if (_fDT == 0.0f)
	{
		return;
	}

	// Calculate The turning arc
	_fDT *= m_fTurningSpeed;

	// Change the Yaw Revolution to Radians
	m_fYawRevolution = D3DXToRadian(_fDT);

	// Recalculate the Rotation Matrix
	D3DXMATRIX D3DXRotation;
	D3DXMatrixRotationAxis(&D3DXRotation, &m_v3Up, D3DXToRadian(_fDT));

	// Rotate the Look Vector by the Rotation matrix
	D3DXVec3TransformCoord(&m_v3Look, &m_v3Look, &D3DXRotation);

	// Rotate the Right Vector by the Rotation matrix
	D3DXVec3TransformCoord(&m_v3Right, &m_v3Right, &D3DXRotation);
}

/***********************
* Pitch: Rotates the Object around the Pitch (X-Axis)
* @author: Callan Moore
* @parameter: _fDT: The Current Delta Tick
* @return: void
********************/
void C3DObject::Pitch(float _fDT)
{
	// Don't Calculate if Delta tick is too small (Potentially truncated) , No change to Yaw
	if (_fDT == 0.0f)
	{
		return;
	}

	// Calculate The turning arc
	_fDT *= m_fTurningSpeed;

	// Calculate the Pitch minus the Delta Tick
	m_fPitch -= _fDT;

	// Bool variable to decide if pitch rotation is ok
	bool bRotate = true;

	// Check if the pitch is not greater or smaller than the max/min Pitch. Prevents Gimbal Lock
	if (m_fPitch > m_fMaxPitch)
	{
		bRotate = false;
	}
	else if (m_fPitch < -m_fMaxPitch)
	{
		bRotate = false;
	}

	// Rotate if Pitch is acceptable
	if (bRotate == true)
	{
		// Convert Delta tick to radians and set the Pitch Rotation
		m_fPitchRevolution = D3DXToRadian(_fDT);

		// Calculate the Rotation matrix
		D3DXMATRIX D3DXRotation;
		D3DXMatrixRotationAxis(&D3DXRotation, &m_v3Right, D3DXToRadian(_fDT));

		// Rotate the Other Axis and Normalise the vectors
		D3DXVec3TransformNormal(&m_v3Up, &m_v3Up, &D3DXRotation);
		D3DXVec3TransformNormal(&m_v3Look, &m_v3Look, &D3DXRotation);
	}
}

/***********************
* GetTarget: Retrieves the Objects Target vector in 3D model space
* @author: Callan Moore
* @return: D3DXVECTOR3*: The Objects Target vector in 3D model space
********************/
D3DXVECTOR3* C3DObject::GetTarget()
{
	return &m_v3Target;
}

/***********************
* GetPosition: Retrieves the Objects Position in 3D model space
* @author: Callan Moore
* @return: D3DXVECTOR3*: The Objects Position in 3D model space
********************/
D3DXVECTOR3* C3DObject::GetPosition()
{
	return &m_v3Position;
}

/***********************
* GetLook: Retrieves the Objects Look vector in 3D model space
* @author: Callan Moore
* @return: D3DXVECTOR3*: The Objects Look vector in 3D model space
********************/
D3DXVECTOR3* C3DObject::GetLook()
{
	return &m_v3Look;
}

/***********************
* GetUp: Retrieves the Objects Up vector in 3D model space
* @author: Callan Moore
* @return: D3DXVECTOR3*: The Objects Up vector in 3D model space
********************/
D3DXVECTOR3* C3DObject::GetUp()
{
	return &m_v3Up;
}