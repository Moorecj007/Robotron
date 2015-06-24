/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Mechanics_Server.h
* Description : Handles all the Mechanics on the server
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __STEER_LIB_H__
#define __STEER_LIB_H__

// Local Includes
#include "../../Common Files/Networking/Network_Defines.h"

/***********************
* Seek: Seek steering for AI
* @author: Callan Moore
* @parameter: _steeringInfo: Steering Info structure of the Object
* @parameter: v3Pos: Enemy Info structure of the Object
* @parameter: v3Dir: Enemy Info structure of the Object
* @parameter: _fDT: The current Delta Tick
* @return: void
********************/
inline void Seek(SteeringVariables* _pSteeringInfo, v3float* _pv3Pos, v3float* _pv3Dir, float _fDT)
{
	if (_fDT != 0.0f)
	{
		// Calculate the Desired Velocity
		v3float v3Desired = _pSteeringInfo->v3TargetPos - *_pv3Pos;
		v3Desired.Normalise();

		// Calculate the steering force towards the desired velocity
		v3float v3Steer = v3Desired - _pSteeringInfo->v3Vel.Normalise();
		v3Steer = v3Steer.Limit(_pSteeringInfo->fMaxForce * _fDT);

		// Add the Steering force to the velocity and limit the velocity by the max speed of the object
		_pSteeringInfo->v3Vel += v3Steer;
		_pSteeringInfo->v3Vel = _pSteeringInfo->v3Vel.Limit(_pSteeringInfo->fMaxSpeed * _fDT);

		// Update the new position and direction
		*_pv3Pos += _pSteeringInfo->v3Vel;
		*_pv3Dir = _pSteeringInfo->v3Vel;
		(*_pv3Dir).Normalise();
	}
}

/***********************
* Wander: Wander steering for AI
* @author: Callan Moore
* @parameter: _steeringInfo: Steering Info structure of the Object
* @parameter: v3Pos: Enemy Info structure of the Object
* @parameter: v3Dir: Enemy Info structure of the Object
* @parameter: _fDT: The current Delta Tick
* @return: void
********************/
inline void Wander(SteeringVariables* _pSteeringInfo, v3float* _pv3Pos, v3float* _pv3Dir, float _fDT)
{
	if (_fDT != 0.0f)
	{
		// Calculate a circle in front of the Object
		v3float v3CirclePos = *_pv3Pos + ((*_pv3Dir).Normalise() * (_pSteeringInfo->fSize * 2.0f));
		float fCircleRadius = _pSteeringInfo->fSize;

		// Transform the axis allignment into object relative space
		float fRotation = -(atan2(_pv3Dir->z, _pv3Dir->x) - atan2(1.0f, 0.0f));

		// Generate a random angle to change the wander angle by Up to 30 degrees
		int iRandChange = rand() % 30;
		int iRand = rand() % 2;

		// Change the random angle into radians
		float fAngleChange = DegreesToRadians((float)iRandChange);

		// Randomly decide whether to increase or decrease the angle
		(iRand == 1) ? _pSteeringInfo->fWanderAngle += fAngleChange : _pSteeringInfo->fWanderAngle -= fAngleChange;
		
		// Add the new wander angle to the object relative rotation
		fRotation += _pSteeringInfo->fWanderAngle;

		// calculate the displacement vector to the point on the circle
		v3float v3Displacment;
		v3Displacment.x = (fCircleRadius * (float)sin(_pSteeringInfo->fWanderAngle));
		v3Displacment.y = 0;
		v3Displacment.z = (fCircleRadius * (float)cos(_pSteeringInfo->fWanderAngle));

		// translate the vector to originate at the circle center
		v3Displacment += v3CirclePos;

		// Set the target to be the point on the circle
		_pSteeringInfo->v3TargetPos = v3Displacment;

		// Seek the Point
		Seek(_pSteeringInfo, _pv3Pos, _pv3Dir, _fDT);
	}
}

/***********************
* Contain: Containment steering for AI
* @author: Callan Moore
* @parameter: _steeringInfo: Steering Info structure of the Object
* @parameter: v3Pos: Enemy Info structure of the Object
* @parameter: _v3ContainmentField: Containment field of the object
* @return: bool: True if the object is contained, false if it is not
********************/
inline bool Contain(SteeringVariables* _pSteeringInfo, v3float* _pv3Pos, v2float _v2ContainmentField)
{
	// Calculate a smaller containment field than the terrain requires
	float fMinX = -(_v2ContainmentField.u / 2.0f) + 5;
	float fMinZ = -(_v2ContainmentField.v / 2.0f) + 5;
	float fMaxX = (_v2ContainmentField.u / 2.0f)  - 5;
	float fMaxZ = (_v2ContainmentField.v / 2.0f)  - 5;

	// Check if the Object is within the borders of the containment field
	if (	_pv3Pos->x < fMinX
		||  _pv3Pos->x > fMaxX
		||  _pv3Pos->z < fMinZ
		||  _pv3Pos->z > fMaxZ)
	{
		// Set the target to the center of the terrain so the object starts steering back into the containment area
		_pSteeringInfo->v3TargetPos = { 0.0f, 0.0f, 0.0f };
		// TO DO - check if this needs to be updated dependant on the size of the enemy
		return false;
	}
	
	// Object was in the containment field
	return true;
}

/***********************
* Flee: Flee steering for AI
* @author: Callan Moore
* @parameter: _steeringInfo: Steering Info structure of the Object
* @parameter: v3Pos: Enemy Info structure of the Object
* @parameter: v3Dir: Enemy Info structure of the Object
* @parameter: _fDT: The current Delta Tick
* @return: void
********************/
inline void Flee(SteeringVariables* _pSteeringInfo, v3float* _pv3Pos, v3float* _pv3Dir, float _fDT)
{
	if (_fDT != 0.0f)
	{
		// Calculate the Desired Velocity
		v3float v3Desired = (_pSteeringInfo->v3TargetPos - *_pv3Pos) * -1.0f;
		v3Desired.Normalise();

		// Calculate the steering force towards the desired velocity
		v3float v3Steer = v3Desired - _pSteeringInfo->v3Vel.Normalise();
		v3Steer = v3Steer.Limit(_pSteeringInfo->fMaxForce * _fDT);

		// Add the Steering force to the velocity and limit the velocity by the max speed of the object
		_pSteeringInfo->v3Vel += v3Steer;
		_pSteeringInfo->v3Vel = _pSteeringInfo->v3Vel.Limit(_pSteeringInfo->fMaxSpeed * _fDT);

		// Update the new position and direction
		*_pv3Pos += _pSteeringInfo->v3Vel;
		*_pv3Dir = _pSteeringInfo->v3Vel;
		(*_pv3Dir).Normalise();
	}
}

/***********************
* Evade: Evade steering for AI
* @author: Callan Moore
* @parameter: _steeringInfo: Steering Info structure of the Object
* @parameter: v3Pos: Enemy Info structure of the Object
* @parameter: v3Dir: Enemy Info structure of the Object
* @parameter: _fDT: The current Delta Tick
* @return: void
********************/
inline void Evade(SteeringVariables* _pSteeringInfo, v3float* _pv3Pos, v3float* _pv3Dir, float _fDT)
{

	float fDistance = (_pSteeringInfo->v3TargetPos - *_pv3Pos).Magnitude();
	float fFuture = fDistance * _pSteeringInfo->fTargetSpeed * _fDT;
	v3float v3TargetFuturePos = _pSteeringInfo->v3TargetPos + (_pSteeringInfo->v3TargetVel.Normalise() * fFuture);

	_pSteeringInfo->v3TargetPos = v3TargetFuturePos;
	Flee(_pSteeringInfo, _pv3Pos, _pv3Dir, _fDT);
}



#endif // __STEER_LIB_H__