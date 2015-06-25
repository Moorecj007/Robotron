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

#ifndef __SERVER_MECHANICS_H__
#define __SERVER_MECHANICS_H__

// Library Includes
#include <time.h>
#include <map>
#include <queue>
#include <fstream>

// Local Includes
#include "../Common Files/Networking/Network_Defines.h"
#include "../Common Files/Clock.h"
#include "Artificial Intelligence\SteerLib.h"

class CMechanics_Server
{
public:
	// Constructors / Destructors

	/***********************
	* CMechanics_Server: Default Constructor for Server Mechanics class
	* @author: Callan Moore
	********************/
	CMechanics_Server();

	/***********************
	* ~CMechanics_Server: Default Destructor for Server Mechanics class
	* @author: Callan Moore
	********************/
	~CMechanics_Server();

	// Getters

	/***********************
	* GetNextDeletedProjectile: Retrieve the Info of the next Projected that has been deleted
	* @author: Callan Moore
	* @parameter: _projectileInfo: Info of the projectile to be deleted
	* @return: bool: True if another projectile to be deleted exists
	********************/
	bool GetNextDeletedProjectile(ProjectileInfo* _projectileInfo);

	/***********************
	* GetNextCreatedProjectile: Retrieve the Info of the next Projectile to be Spawned
	* @author: Callan Moore
	* @parameter: _projectileInfo: Info of the next projectile to be spawned
	* @return: bool: True if another projectile to be spawned exists
	********************/
	bool GetNextCreatedProjectile(ProjectileInfo* _projectileInfo);

	/***********************
	* GetNextDeletedEnemy: Retrieve the Info of the next Enemy that has been killed
	* @author: Callan Moore
	* @parameter: _enemyInfo: Info of the enemy to be deleted
	* @return: bool: True if another enemy to be deleted exists
	********************/
	bool GetNextDeletedEnemy(EnemyInfo* _enemyInfo);

	/***********************
	* GetNextCreatedEnemy: Retrieve the Info of the next Enemy to be Spawned
	* @author: Callan Moore
	* @parameter: _enemyInfo: Info of the next enemy to be spawned
	* @return: bool: True if another enemy to be spawned exists
	********************/
	bool GetNextCreatedEnemy(EnemyInfo* _enemyInfo);

	/***********************
	* GetNextDeletedPower: Retrieve the Info of the next PowerUp that has been absorbed
	* @author: Callan Moore
	* @parameter: _enemyInfo: Info of the PowerUp to be deleted
	* @return: bool: True if another PowerUp to be deleted exists
	********************/
	bool GetNextDeletedPower(PowerUpInfo* _powerUpInfo);

	/***********************
	* GetNextCreatedPower: Retrieve the Info of the next PowerUp to be Spawned
	* @author: Callan Moore
	* @parameter: _enemyInfo: Info of the PowerUp to be spawned
	* @return: bool: True if another PowerUp to be spawned exists
	********************/
	bool GetNextCreatedPower(PowerUpInfo* _powerUpInfo);
	
	// Setters

	/***********************
	* SetAvatarAliveState: Set the Alive state of an Avatar
	* @author: Callan Moore
	* @parameter: _strAvatar: Name of the Avatar
	* @parameter: _bAlive: Alive state to set
	* @return: void
	********************/
	void SetAvatarAliveState(std::string _strAvatar, bool _bAlive);

	// Prototypes

	/***********************
	* Initialise: Initialise the Server mechanics class for use
	* @author: Callan Moore
	* @parameter: _strServerName: Name of the Server
	* @parameter: _bSinglePlayer: Single player version of the game
	* @return: bool: Successful initialisation (or not)
	********************/
	bool Initialise(std::string _strServerName, bool _bSinglePlayer);

	/***********************
	* Process: Process the mechanics of the server
	* @author: Callan Moore
	* @parameter: void
	********************/
	void Process();

	/***********************
	* ProcessAvatar: Process an Avatars input
	* @author: Callan Moore
	* @parameter: _pClientPacket: Client Packet that holds the movement information for a client/avatar
	* @parameter: void
	********************/
	void ProcessAvatar(ClientToServer* _pClientPacket);

	/***********************
	* ProcessProjectiles: Process the Projectiles
	* @author: Callan Moore
	* @parameter: void
	********************/
	void ProcessProjectiles();

	/***********************
	* ProcessFlare: Process the Flares
	* @author: Callan Moore
	* @parameter: void
	********************/
	void ProcessFlare();

	/***********************
	* ProcessPowerUps: Process the PowerUps
	* @author: Callan Moore
	* @parameter: void
	********************/
	void ProcessPowerUps();

	/***********************
	* ProcessEnemies: Process the enemies
	* @author: Callan Moore
	* @parameter: _fDT: The current Delta Tick
	* @parameter: void
	********************/
	void ProcessEnemies(float _fDT);

	/***********************
	* ProcessDemon: Process a Demons enemies AI capabilities
	* @author: Callan Moore
	* @parameter: _enemyInfo: Enemy Info structure of the Demon enemy
	* @parameter: _fDT: The current Delta Tick
	* @parameter: void
	********************/
	void ProcessDemon(EnemyInfo* _enemyInfo, float _fDT);

	/***********************
	* ProcessSentinel: Process a Sentinel enemies AI capabilities
	* @author: Callan Moore
	* @parameter: _enemyInfo: Enemy Info structure of the Sentinel enemy
	* @parameter: _fDT: The current Delta Tick
	* @parameter: void
	********************/
	void ProcessSentinel(EnemyInfo* _enemyInfo, float _fDT);

	/***********************
	* ProcessShadow: Process a Shadow enemies AI capabilities
	* @author: Callan Moore
	* @parameter: _enemyInfo: Enemy Info structure of the Shadow enemy
	* @parameter: _fDT: The current Delta Tick
	* @parameter: void
	********************/
	void ProcessShadow(EnemyInfo* _enemyInfo, float _fDT);

	/***********************
	* SpawnNextWave: Spawn the Next wave of enemies
	* @author: Callan Moore
	* @parameter: void
	********************/
	void SpawnNextWave();

	/***********************
	* SpawnNextPowerUp: Spawn the Next PowerUp
	* @author: Callan Moore
	* @parameter: void
	********************/
	void SpawnNextPowerUp();

	/***********************
	* CreateDataPacket: Creates the DataPacket to send with all relevant information
	* @author: Callan Moore
	* @parameter: _pServerPacket: Server to client packet to create
	* @return: bool: Successful creation of Data Packet (or not)
	********************/
	bool CreateDataPacket(ServerToClient* _pServerPacket);

	/***********************
	* AddAvatar: Add an Avatar to the Container
	* @author: Callan Moore
	* @parameter: _pClientPacket: Packet that contains the data needed to create the new Avatar
	* @return: void
	********************/
	void AddAvatar(ClientToServer* _pClientPacket);

	/***********************
	* RemoveAvatar: Remove an Avatar from the Container
	* @author: Callan Moore
	* @parameter: _strUserName: The username of the avatar to remove
	* @return: void
	********************/
	void RemoveAvatar(std::string _strUserName);

	/***********************
	* AddAvatarsToPacket: Adds all the Avatar Info to the input Packet
	* @author: Callan Moore
	* @parameter: _pServerPacket: Server to client packet to add Avatar Info to
	* @return: bool: Successful addition (or not
	********************/
	bool AddAvatarsToPacket(ServerToClient* _pServerPacket);

	/***********************
	* CheckAllAvatarsReady: Check if all the avatars have said they are ready
	* @author: Callan Moore
	* @return: bool: Successful addition (or not
	********************/
	bool CheckAllAvatarsReady();

	/***********************
	* CollsionCheck: Check if there is a collision between the calling object and another target object
	* @author: Callan Moore
	* @parameter: _BBoxCaller: Bounding Box of the Calling Object
	* @parameter: _BBoxTarget: Bounding Box of the Target Object
	* @return: bool: If there is a collison
	********************/
	bool CollisionCheck(BoundingBox _BBoxCaller, BoundingBox _BBoxTarget);


private:
	// Member Variables
	std::string m_strServerName;
	CClock* m_pClock;
	UINT m_iNextObjectID;

	// Game Variables
	float m_fTerrainWidth;
	float m_fTerrainDepth;
	bool m_bSinglePlayer;

	UINT m_iWaveNumber;
	UINT m_iDemonCount;
	UINT m_iSentinelCount;
	UINT m_iShadowCount;
	float m_fLastPowerUpSpawned;

	// Avatar Variables
	std::map<std::string, AvatarInfo>* m_pAvatars;

	// Projectile Variables
	std::map<UINT, ProjectileInfo>* m_pProjectiles;
	std::queue<ProjectileInfo>* m_pDeletedProjectiles;
	std::queue<ProjectileInfo>* m_pCreatedProjectiles;

	// Enemy Variables
	std::map<UINT, EnemyInfo>* m_pEnemies;
	std::queue<EnemyInfo>* m_pDeletedEnemies;
	std::queue<EnemyInfo>* m_pCreatedEnemies;

	// PowerUp Variables
	std::map<UINT, PowerUpInfo>* m_pPowerUps;
	std::queue<PowerUpInfo>* m_pDeletedPowerUps;
	std::queue<PowerUpInfo>* m_pCreatedPowerUps;

	// Flare PowerUp Variabes
	bool m_bFlareActive;
	FlareInfo m_Flare;
};

#endif // __SERVER_MECHANICS_H__