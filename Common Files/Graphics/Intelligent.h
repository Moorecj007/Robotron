/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Intelligent.h
* Description : Declarations for the functions of the Intelligent Object
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __INTELLIGENT_H__
#define __INTELLIGENT_H__

// Local Includes
#include "C3DObject.h"

class CIntelligent :
	public C3DObject
{
public:
	// Constructors / Destructors

	/***********************
	* CIntelligent: Default Contructor for Intelligent class
	* @author: Callan Moore
	********************/
	CIntelligent();

	/***********************
	* ~CIntelligent: Default Destructor for Intelligent class
	* @author: Callan Moore
	********************/
	~CIntelligent();

	// Getters

	/***********************
	* GetPointScore: Retrieves the points to be awarded to an avatar
	* @author: Callan Moore
	* @return: UINT: The Intelligents point score
	********************/
	UINT GetPointScore() { return m_iPointScore; };

	/***********************
	* GetMaxForce: Retrieves the Maximum force of the Intelligent
	* @author: Callan Moore
	* @return: float: The Maximum force of the Intelligent
	********************/
	float GetMaxForce()	{ return m_fMaxForce; };

	/***********************
	* GetAcceleration: Retrieves the acceleration vector of the Intelligent
	* @author: Callan Moore
	* @return: v3float: The acceleration of the Intelligent as a vector
	********************/
	v3float GetAcceleration() { return m_v3Acceleration; };

	/***********************
	* GetDesiredVelocity: Retrieves the Desired Velocity vector of the Intelligent
	* @author: Callan Moore
	* @return: v3float: The Desired Velocity of the Intelligent as a vector
	********************/
	v3float GetDesiredVelocity() { return m_v3DesiredVelocity; };
	
	// Setters

	/***********************
	* SetPoints: Sets a new Point score for the Intelligent
	* @author: Callan Moore
	* @parameter: _iPoints: The new score to be awarded to an avatar for killing/acquiring this Intelligent
	* @return: void
	********************/
	void SetPoints(UINT _iPoints) { m_iPointScore = _iPoints; };

	/***********************
	* SetMaxForce: Sets a new maximum force for this Intelligent
	* @author: Callan Moore
	* @parameter: _fMaxForce: The maximum force for this Intelligent
	* @return: void
	********************/
	void SetMaxForce(float _fMaxForce) { m_fMaxForce = _fMaxForce; };

	/***********************
	* SetAcceleration: Sets the Acceleration vector of this Intelligent
	* @author: Callan Moore
	* @parameter: _v3Acc: The acceleration vector
	* @return: void
	********************/
	void SetAcceleration(v3float _v3Acc) { m_v3Acceleration = _v3Acc; };

	/***********************
	* SetDesiredVelocity: Sets the Desired Velocity Vector for this Intelligent
	* @author: Callan Moore
	* @parameter: _v3DesVel: The Desired Velocity vector
	* @return: void
	********************/
	void SetDesiredVelocity(v3float _v3DesVel) { m_v3DesiredVelocity = _v3DesVel; };

	// Prototypes

protected:
	// Member Variables
	UINT m_iPointScore;
	float m_fMaxForce;
	v3float m_v3Acceleration;
	v3float m_v3DesiredVelocity;
};
#endif // __INTELLIGENT_H__