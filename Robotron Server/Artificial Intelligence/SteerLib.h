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

// Library Includes
#include <map>

// Local Includes
#include "../../Common Files/Networking/Network_Defines.h"

inline void ApplyForce(SteeringVariables* _pSteeringInfo, v3float* _pv3Pos, v3float* _pv3Dir, v3float _v3Force, float _fDT)
{
	// Update the new position and direction
	_v3Force = _v3Force.Limit(_pSteeringInfo->fMaxForce * _fDT);
	_pSteeringInfo->v3Vel += _v3Force;
	_pSteeringInfo->v3Vel = _pSteeringInfo->v3Vel.Limit(_pSteeringInfo->fMaxSpeed * _fDT);

	*_pv3Pos += _pSteeringInfo->v3Vel;
	*_pv3Dir = _pSteeringInfo->v3Vel;
	(*_pv3Dir).Normalise();
}

/***********************
* Seek: Seek steering for AI
* @author: Callan Moore
* @parameter: _steeringInfo: Steering Info structure of the Object
* @parameter: v3Pos: Enemy Info structure of the Object
* @parameter: v3Dir: Enemy Info structure of the Object
* @parameter: _fDT: The current Delta Tick
* @return: v3float: The steering force
********************/
inline v3float Seek(SteeringVariables* _pSteeringInfo, v3float* _pv3Pos, v3float* _pv3Dir, float _fDT)
{
	if (_fDT != 0.0f)
	{
		// Calculate the Desired Velocity
		v3float v3Desired = _pSteeringInfo->v3TargetPos - *_pv3Pos;
		v3Desired.Normalise();
		v3Desired = v3Desired * _pSteeringInfo->fMaxSpeed;

		// Calculate the steering force towards the desired velocity
		v3float v3Steer = v3Desired - _pSteeringInfo->v3Vel.Normalise();
		v3Steer = v3Steer.Limit(_pSteeringInfo->fMaxForce );

		return v3Steer;
	}
	return{ 0.0f, 0.0f, 0.0f };
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
inline v3float Wander(SteeringVariables* _pSteeringInfo, v3float* _pv3Pos, v3float* _pv3Dir, float _fDT)
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
		return Seek(_pSteeringInfo, _pv3Pos, _pv3Dir, _fDT);
	}
	return { 0.0f, 0.0f, 0.0f };
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
	// Calculate the targets future position
	float fDistance = (_pSteeringInfo->v3TargetPos - *_pv3Pos).Magnitude();
	float fFuture = fDistance * _pSteeringInfo->fTargetSpeed * _fDT;
	v3float v3TargetFuturePos = _pSteeringInfo->v3TargetPos + (_pSteeringInfo->v3TargetVel.Normalise() * fFuture);

	// Set the Future Position as the target and flee from it
	_pSteeringInfo->v3TargetPos = v3TargetFuturePos;
	Flee(_pSteeringInfo, _pv3Pos, _pv3Dir, _fDT);
}

/***********************
* Separate: Separate the flock individuals from the rest of the flock
* @author: Callan Moore
* @parameter: _steeringInfo: Steering Info structure of the Object
* @parameter: v3Pos: Current Objects Position
* @parameter: v3Dir: Current Objects Direction
* @parameter: _pEnemies: The map of all enemies currently in play
* @parameter: _fDT: The current Delta Tick
* @return: void
********************/
inline v3float Separate(SteeringVariables* _pSteeringInfo, v3float* _pv3Pos, v3float* _pv3Dir, std::map<UINT, EnemyInfo>* _pEnemies, float _fDT)
{
	// Calculate the Desired Separation value based on the size of the Object
	float fDesiredSeparation = (_pSteeringInfo->fSize * 2.5f) * 1.4f;

	// Initial Variable values
	v3float v3SumOfSurroundings = { 0.0f, 0.0f, 0.0f };
	int iObjectCount = 0;

	// Cycle through the map of all enemies
	std::map<UINT, EnemyInfo>::iterator iterEnemy = _pEnemies->begin();
	while (iterEnemy != _pEnemies->end())
	{
		// check only for demon type enemies
		if (iterEnemy->second.eType == ET_DEMON)
		{
			// Calculate the distance between the current enemy and the calling object
			v3float v3PosDifference = ((*_pv3Pos) - iterEnemy->second.v3Pos);
			float fDistance = v3PosDifference.Magnitude();

			// If the distance is less than the desired separation
			if ((fDistance > 0) && (fDistance < fDesiredSeparation))
			{
				// Apply Force to separate individuals
				v3float v3Difference = v3PosDifference;
				v3Difference.Normalise();
				v3Difference = v3Difference / fDistance;

				// Sum all individual forces that are within the desired separtio circle
				v3SumOfSurroundings += v3Difference;
				iObjectCount++;
			}
		}
		iterEnemy++;
	}
	// Calculate only if other objects were in separation circle
	if (iObjectCount > 0)
	{
		// Calculate the average of all forces
		v3SumOfSurroundings = v3SumOfSurroundings / (float)iObjectCount;
		v3SumOfSurroundings.Normalise();
		v3SumOfSurroundings = v3SumOfSurroundings * (_pSteeringInfo->fMaxSpeed);;

		// Calculate the steering vector
		v3float v3Steer = v3SumOfSurroundings - _pSteeringInfo->v3Vel.Normalise();
		v3Steer.Limit(_pSteeringInfo->fMaxForce);

		return v3Steer;
	}
	else
	{
		return{ 0, 0, 0 };
	}
}

/***********************
* Allignment: Allign the flock individuals with the rest of the flock
* @author: Callan Moore
* @parameter: _steeringInfo: Steering Info structure of the Object
* @parameter: v3Pos: Current Objects Position
* @parameter: v3Dir: Current Objects Direction
* @parameter: _pEnemies: The map of all enemies currently in play
* @parameter: _fDT: The current Delta Tick
* @return: void
********************/
inline v3float Allignment(SteeringVariables* _pSteeringInfo, v3float* _pv3Pos, v3float* _pv3Dir, std::map<UINT, EnemyInfo>* _pEnemies, float _fDT)
{
	// Calculate the Desired Allignment value based on the size of the Object
	float fAllignmentRadius = (_pSteeringInfo->fSize * 4.0f) * 1.4f; 

	// Initial Variable values
	v3float v3SumOfSurroundings = { 0.0f, 0.0f, 0.0f };
	int iObjectCount = 0;

	// Cycle through the map of all enemies
	std::map<UINT, EnemyInfo>::iterator iterEnemy = _pEnemies->begin();
	while (iterEnemy != _pEnemies->end())
	{
		// check only for demon type enemies
		if (iterEnemy->second.eType == ET_DEMON)
		{
			//// Calculate the distance between the current enemy and the calling object
			v3float v3PosDifference = ((*_pv3Pos) - iterEnemy->second.v3Pos);
			float fDistance = v3PosDifference.Magnitude();
			
			// If the distance is less than the allignment radius
			if ((fDistance > 0) && (fDistance < fAllignmentRadius))
			{
				v3SumOfSurroundings += iterEnemy->second.steeringInfo.v3Vel;
				iObjectCount++;
			}
		}
		iterEnemy++;
	}
	// Calculate only if other objects were in Allignment circle
	if (iObjectCount > 0)
	{
		// Calculate the average of all forces
		v3SumOfSurroundings = v3SumOfSurroundings / (float)iObjectCount;
		v3SumOfSurroundings.Normalise();
		v3SumOfSurroundings = v3SumOfSurroundings * (_pSteeringInfo->fMaxSpeed);

		// Calculate the steering vector
		v3float v3Steer = v3SumOfSurroundings - _pSteeringInfo->v3Vel.Normalise();
		v3Steer.Limit(_pSteeringInfo->fMaxForce);

		return v3Steer;
	}
	else
	{
		return{ 0, 0, 0 };
	}
}

/***********************
* Cohesion: Have the flock individuals gravitate towards the center of the flock
* @author: Callan Moore
* @parameter: _steeringInfo: Steering Info structure of the Object
* @parameter: v3Pos: Current Objects Position
* @parameter: v3Dir: Current Objects Direction
* @parameter: _pEnemies: The map of all enemies currently in play
* @parameter: _fDT: The current Delta Tick
* @return: void
********************/
inline v3float Cohesion(SteeringVariables* _pSteeringInfo, v3float* _pv3Pos, v3float* _pv3Dir, std::map<UINT, EnemyInfo>* _pEnemies, float _fDT)
{
	// Calculate the Desired Cohesion value based on the size of the Object
	float fCohesionRadius = (_pSteeringInfo->fSize * 7.0f) * 1.4f; 

	// Initial Variable values
	v3float v3SumOfSurroundings = { 0.0f, 0.0f, 0.0f };
	int iObjectCount = 0;

	// Cycle through the map of all enemies
	std::map<UINT, EnemyInfo>::iterator iterEnemy = _pEnemies->begin();
	while (iterEnemy != _pEnemies->end())
	{
		// Check only for demon type enemies
		if (iterEnemy->second.eType == ET_DEMON)
		{
			//// Calculate the distance between the current enemy and the calling object
			v3float v3PosDifference = ((*_pv3Pos) - iterEnemy->second.v3Pos);
			float fDistance = v3PosDifference.Magnitude();

			// If the distance is less than the Cohesion radius
			if ((fDistance > 0 )&& (fDistance < fCohesionRadius))
			{
				v3SumOfSurroundings += iterEnemy->second.v3Pos;
				iObjectCount++;
			}
		}
		iterEnemy++;
	}
	// Calculate only if other objects were in Cohesion circle
	if (iObjectCount > 0)
	{
		// Calculate the average of all forces
		v3SumOfSurroundings = v3SumOfSurroundings / (float)iObjectCount;
		v3SumOfSurroundings.Normalise();

		_pSteeringInfo->v3TargetPos = v3SumOfSurroundings;
		v3float v3Steer = Seek(_pSteeringInfo, _pv3Pos, _pv3Dir, _fDT);
		return v3Steer;
	}
	else
	{
		return{ 0, 0, 0 };
	}
}

/***********************
* Cohesion: Group Objects into a flocking formations
* @author: Callan Moore
* @parameter: _steeringInfo: Steering Info structure of the Object
* @parameter: v3Pos: Current Objects Position
* @parameter: v3Dir: Current Objects Direction
* @parameter: _pEnemies: The map of all enemies currently in play
* @parameter: _fDT: The current Delta Tick
* @return: void
********************/
inline void Flock(SteeringVariables* _pSteeringInfo, v3float* _pv3Pos, v3float* _pv3Dir, std::map<UINT, EnemyInfo>* _pEnemies, float _fDT)
{
	if (_fDT != 0.0f)
	{
		// Save the Target Position for future use
		v3float v3AvatarTarget = _pSteeringInfo->v3TargetPos;
		v3float v3TotalForce = { 0.0f, 0.0f, 0.0f };

		// Call each of the 3 rules of flocking
		v3TotalForce += Separate(_pSteeringInfo, _pv3Pos, _pv3Dir, _pEnemies, _fDT) * 1.5f;
		v3TotalForce += Allignment(_pSteeringInfo, _pv3Pos, _pv3Dir, _pEnemies, _fDT) * 1.0f;
		v3TotalForce += Cohesion(_pSteeringInfo, _pv3Pos, _pv3Dir, _pEnemies, _fDT) * 1.0f;

		// Reassign the target for the seek function
		if (v3AvatarTarget == v3float{ -1000.0f, -1000.0f, -1000.0f })
		{
			v3TotalForce = Wander(_pSteeringInfo, _pv3Pos, _pv3Dir, _fDT) * 0.1f;
		}
		else
		{
			_pSteeringInfo->v3TargetPos = v3AvatarTarget;
			v3TotalForce += Seek(_pSteeringInfo, _pv3Pos, _pv3Dir, _fDT) * 1.25f;
		}

		// Apply the total force to the Object
		ApplyForce(_pSteeringInfo, _pv3Pos, _pv3Dir, v3TotalForce, _fDT);	
	}
}





#endif // __STEER_LIB_H__