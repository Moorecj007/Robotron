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

inline void Seek(EnemyInfo* _enemyInfo)
{
	_enemyInfo->v3Dir = _enemyInfo->v3Target - _enemyInfo->v3Pos;
	_enemyInfo->v3Dir = _enemyInfo->v3Dir.Normalise();
	_enemyInfo->v3Vel = _enemyInfo->v3Dir * _enemyInfo->fMaxSpeed;
	_enemyInfo->v3Pos += _enemyInfo->v3Vel;

	//v3float v3Desired = (_enemyInfo->v3Target - _enemyInfo->v3Pos);
	//v3Desired = v3Desired.Normalise();
	//v3Desired = v3Desired * _enemyInfo->fMaxSpeed;
	//
	//v3float v3Steer = v3Desired - _enemyInfo->v3Vel;
	//v3Steer = v3Steer.Limit(_enemyInfo->fMaxForce);
	//
	//_enemyInfo->v3Acceleration += v3Steer;	// TO DO - delta tick
	//
	//_enemyInfo->v3Vel += _enemyInfo->v3Acceleration;
	//_enemyInfo->v3Vel.Limit(_enemyInfo->fMaxSpeed);
	//_enemyInfo->v3Pos += _enemyInfo->v3Vel;
	//_enemyInfo->v3Acceleration = { 0.0f, 0.0f, 0.0f };
}



#endif // __STEER_LIB_H__