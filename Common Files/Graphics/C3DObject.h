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

class C3DObject
{
public:
	// Constructors / Destructors

	/***********************
	* C3DObject: Default Contructor for 3DObject class
	* @author: Callan Moore
	********************/
	C3DObject();

	/***********************
	* ~C3DObject: Default Destructor for 3DObject class
	* @author: Callan Moore
	********************/
	~C3DObject();
	
	// Getters

	/***********************
	* GetPosition: Retrieves the Objects Position in 3D model space
	* @author: Callan Moore
	* @return: v3float*: The Objects Position in 3D model space
	********************/
	v3float* C3DObject::GetPosition() { return &m_v3Position; };

	/***********************
	* GetDirection: Retrieves the Objects Direction
	* @author: Callan Moore
	* @return: v3float*: The Objects Direction as a vector of 3 floats
	********************/
	v3float* C3DObject::GetDirection() { return &m_v3Direction; };

	/***********************
	* GetVelocity: Retrieves the Objects Velocity
	* @author: Callan Moore
	* @return: v3float*: The Objects Velocity as a vector of 3 floats
	********************/
	v3float* C3DObject::GetVelocity() { return &m_v3Velocity; };

	/***********************
	* GetMaxSpeed: Retrieves the Objects Maximum speed
	* @author: Callan Moore
	* @return: float: The maximum speed
	********************/
	float GetMaxSpeed() { return m_fMaxSpeed;};

	/***********************
	* GetDamage: Retrieves this objects Damage capabilities
	* @author: Callan Moore
	* @return: UINT: The Damage this objects can inflict
	********************/
	UINT GetDamage() { return m_iDamage; };

	// Setters

	/***********************
	* SetPosition: Sets the Position of the Object in 3D model space
	* @author: Callan Moore
	* @parameter: _v3Pos: Position as a vector of 3 floats
	* @return: void
	********************/
	void SetPosition(v3float _v3Pos) { m_v3Position = _v3Pos;};

	/***********************
	* SetDirection: Sets the Direction of the Object
	* @author: Callan Moore
	* @parameter: _v3Dir: Direction as a vector of 3 floats
	* @return: void
	********************/
	void SetDirection(v3float _v3Dir) { m_v3Direction = _v3Dir; };

	/***********************
	* SetVelocity: Sets the Velocity of the Object
	* @author: Callan Moore
	* @parameter: _v3Vel: Velocity as a vector of 3 floats
	* @return: void
	********************/
	void SetVelocity(v3float _v3Vel) { m_v3Velocity = _v3Vel; };

	/***********************
	* SetRotation: Sets the Rotation around the Yaw axis of the object
	* @author: Callan Moore
	* @parameter: _fRotation: Rotation around the Yaw in Radians
	* @return: void
	********************/
	void SetRotation(float _fRotation) { m_fRotation = _fRotation;};

	/***********************
	* SetMaxSpeed: Sets the Maximum Speed for this Object when moving
	* @author: Callan Moore
	* @parameter: _fSpeed: The new maximum Speed for this Object
	* @return: void
	********************/
	void C3DObject::SetMaxSpeed(float _fSpeed) { m_fMaxSpeed = _fSpeed;};

	/***********************
	* SetDamage: Sets the damage of the object
	* @author: Callan Moore
	* @parameter: _iDamage: The damage of the object
	* @return: void
	********************/
	void SetDamage(UINT _iDamage) { m_iDamage = _iDamage; };

	// Prototypes

	/***********************
	* Initialise: Initialise the 3D Object for use
	* @author: Callan Moore
	* @parameter: _pRenderer: Pointer to the Renderer for this application
	* @parameter: _pMesh: Mesh for this 3D Object
	* @parameter: _MatComponents: The composition for the material the object will have
	* @parameter: _v3Pos: Starting position for the object
	* @return: void
	********************/
	void Initialise(IRenderer* _pRenderer, CMesh* _pMesh, MaterialComposition _MatComp, v3float _v3Pos = { 0, 0, 0 });

	/***********************
	* Draw: Draws the 3D Object
	* @author: Callan Moore
	* @return: void
	********************/
	void Draw();

	/***********************
	* Process: Processes the 3D Object for the Delta tick
	* @author: Callan Moore
	* @parameter: _fDT: The Delta Tick
	* @return: void
	********************/
	void Process(float _fDT);

	/***********************
	* CreateTexture: Create a texture for the Object
	* @author: Callan Moore
	* @parameter: strFilePath: File path for the file to create the texture from
	* @return: bool: Successful creation (or not)
	********************/
	bool CreateTexture(std::string strFilePath);

	
protected:
	/***********************
	* CalcWorldMatrix: Calculates the World Matrix for this Object
	* @author: Callan Moore
	* @return: void
	********************/
	void CalcWorldMatrix();

protected:
	// Member Variables
	IRenderer* m_pRenderer;
	CMesh* m_pMesh;
	int m_iMaterialID;

	v3float m_v3Position;
	v3float m_v3Direction;
	v3float m_v3Velocity;

	// Texture
	// struct Collision Box

	float m_fSize;	// Might not be needed, used only in Collision box
	float m_fRotation; // Yaw in radians
	float m_fMaxSpeed;
	UINT m_iDamage;
};

#endif // __C3DOBJECT_H__ 