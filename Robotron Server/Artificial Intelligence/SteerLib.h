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
* @parameter: _enemyInfo: Enemy Info structure of the enemy
* @parameter: _fDT: The current Delta Tick
* @parameter: void
********************/
inline void Seek(EnemyInfo* _enemyInfo, float _fDT)
{
	if (_fDT != 0.0f)
	{
		// Calculate the Desired Velocity
		v3float v3Desired = _enemyInfo->v3Target - _enemyInfo->v3Pos;
		v3Desired = v3Desired.Normalise();

		v3float v3Steer = v3Desired - _enemyInfo->v3Vel.Normalise();
		v3Steer = v3Steer.Limit(_enemyInfo->fMaxForce * _fDT);

		_enemyInfo->v3Vel += v3Steer;
		_enemyInfo->v3Vel = _enemyInfo->v3Vel.Limit(_enemyInfo->fMaxSpeed * _fDT);

		_enemyInfo->v3Pos += _enemyInfo->v3Vel;
		_enemyInfo->v3Dir = _enemyInfo->v3Vel;
		_enemyInfo->v3Dir.Normalise();
	}
}

/***********************
* Wander: Wander steering for Enemies
* @author: Callan Moore
* @parameter: _enemyInfo: Enemy Info structure of the enemy
* @parameter: _fDT: The current Delta Tick
* @parameter: void
********************/
inline void Wander(EnemyInfo* _enemyInfo, float _fDT)
{
	if (_fDT != 0.0f)
	{
		v3float v3CirclePos = _enemyInfo->v3Pos + (_enemyInfo->v3Dir.Normalise() * (_enemyInfo->fSize * 2.0f));
		float fCircleRadius = _enemyInfo->fSize;

		float fRotation = -(atan2(_enemyInfo->v3Dir.z, _enemyInfo->v3Dir.x) - atan2(1.0f, 0.0f));

		int iRandChange = rand() % 30;
		int iRand = rand() % 2;

		float fAngleChange = DegreesToRadians((float)iRandChange);
		(iRand == 1) ? _enemyInfo->fWanderAngle += fAngleChange : _enemyInfo->fWanderAngle -= fAngleChange;

		fRotation += _enemyInfo->fWanderAngle;

		v3float v3Displacment;
		v3Displacment.x = (fCircleRadius * (float)sin(_enemyInfo->fWanderAngle));
		v3Displacment.y = 0;
		v3Displacment.z = (fCircleRadius * (float)cos(_enemyInfo->fWanderAngle));

		v3Displacment += v3CirclePos;

		_enemyInfo->v3Target = v3Displacment;
		Seek(_enemyInfo, _fDT);
	}
}

/***********************
* Wander: Wander steering for PowerUps
* @author: Callan Moore
* @parameter: _powerUpInfo: PowerUp Info structure of the enemy
* @parameter: _fDT: The current Delta Tick
* @parameter: void
********************/
inline void Wander(PowerUpInfo* _powerUpInfo, float _fDT)
{
	if (_fDT != 0.0f)
	{
		v3float v3CirclePos = _powerUpInfo->v3Pos + (_powerUpInfo->v3Dir.Normalise() * 3.0f);
		float fCircleRadius = 0.5f;
	
		int iRandChange = rand() % 5;
		int iRand = rand() % 2;
	
		float fAngleChange = DegreesToRadians((float)iRandChange) * _fDT;
	
		(iRand == 1) ? _powerUpInfo->fWanderAngle += fAngleChange : _powerUpInfo->fWanderAngle -= fAngleChange;
	
		if (_powerUpInfo->fWanderAngle > (2 * M_PI))
		{
			_powerUpInfo->fWanderAngle -= (float)(2 * M_PI);
		}
	
		v3float v3Displacment;
		v3Displacment.x = (_powerUpInfo->v3Dir.x * (float)cos(_powerUpInfo->fWanderAngle) + (_powerUpInfo->v3Dir.z * (float)sin(_powerUpInfo->fWanderAngle)));
		v3Displacment.y = _powerUpInfo->v3Dir.y;
		v3Displacment.z = (_powerUpInfo->v3Dir.z * (float)cos(_powerUpInfo->fWanderAngle) - (_powerUpInfo->v3Dir.x * (float)sin(_powerUpInfo->fWanderAngle)));
	
		v3float v3Steer = _powerUpInfo->v3Dir + v3Displacment;
		v3Steer = v3Steer.Limit(_powerUpInfo->fMaxForce * _fDT);
	
		_powerUpInfo->v3Acceleration += v3Steer;
	
		_powerUpInfo->v3Vel += _powerUpInfo->v3Acceleration;
		_powerUpInfo->v3Vel = _powerUpInfo->v3Vel.Limit(_powerUpInfo->fMaxSpeed * _fDT);
	
		_powerUpInfo->v3Pos += _powerUpInfo->v3Vel;
		_powerUpInfo->v3Dir = _powerUpInfo->v3Vel;
		_powerUpInfo->v3Dir.Normalise();
		_powerUpInfo->v3Acceleration = _powerUpInfo->v3Acceleration * 0.0f;
	}
}



#endif // __STEER_LIB_H__