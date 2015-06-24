/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Mechanics_Server.cpp
* Description : Handles all the Mechanics on the server
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Includes
#include "Mechanics_Server.h"

CMechanics_Server::CMechanics_Server()
{
}

CMechanics_Server::~CMechanics_Server()
{
	// Delete the clock
	delete m_pClock;
	m_pClock = 0;

	// Delete the container of Avatars
	delete m_pAvatars;
	m_pAvatars = 0;

	// Delete the container of Projectiles
	delete m_pProjectiles;
	m_pProjectiles = 0;

	// Delete the container of Enemies
	delete m_pEnemies;
	m_pEnemies = 0;

	// Delete the container of PowerUps
	delete m_pPowerUps;
	m_pPowerUps = 0;

	// Delete the queues of Projectiles
	delete m_pDeletedProjectiles;
	m_pDeletedProjectiles = 0;
	delete m_pCreatedProjectiles;
	m_pCreatedProjectiles = 0;

	// Delete the queues of enemies
	delete m_pDeletedEnemies;
	m_pDeletedEnemies = 0;
	delete m_pCreatedEnemies;
	m_pCreatedEnemies = 0;

	// Delete the queues of Powerups
	delete m_pDeletedPowerUps;
	m_pDeletedPowerUps = 0;
	delete m_pCreatedPowerUps;
	m_pCreatedPowerUps = 0;
}

// Getters
bool CMechanics_Server::GetNextDeletedProjectile(ProjectileInfo* _projectileInfo)
{
	// Check if any enemies are in queue to be created
	if (m_pDeletedProjectiles->empty() == false)
	{
		// Find the info of the Deleted Projectile
		*_projectileInfo = m_pDeletedProjectiles->front();

		// Erase from the Map of Projectile
		m_pProjectiles->erase(_projectileInfo->iID);

		// remove the queued enemy
		m_pDeletedProjectiles->pop();
		return true;
	}

	return false;
}

bool CMechanics_Server::GetNextCreatedProjectile(ProjectileInfo* _projectileInfo)
{
	// Check if any Projectile are in queue to be created
	if (m_pCreatedProjectiles->empty() == false)
	{
		// Find the info of the Created Projectile
		*_projectileInfo = m_pCreatedProjectiles->front();

		// remove the queued projectile
		m_pCreatedProjectiles->pop();
		return true;
	}

	return false;
}

bool CMechanics_Server::GetNextDeletedEnemy(EnemyInfo* _enemyInfo)
{
	// Check if any enemies are in queue to be created
	if (m_pDeletedEnemies->empty() == false)
	{
		// Find the info of the Created enemy
		*_enemyInfo = m_pDeletedEnemies->front();

		// Erase from the Map of Enemies
		m_pEnemies->erase(_enemyInfo->iID);

		// Decrement the Enemy Counter
		switch (_enemyInfo->eType)
		{
			case ET_DEMON:
			{
				m_iDemonCount--;
				break;
			}
			case ET_SENTINEL:
			{
				m_iSentinelCount--;
				break;
			}
		default:
			break;
		}

		// remove the queued enemy
		m_pDeletedEnemies->pop();
		return true;
	}

	return false;
}

bool CMechanics_Server::GetNextCreatedEnemy(EnemyInfo* _enemyInfo)
{
	// Check if any enemies are in queue to be created
	if (m_pCreatedEnemies->empty() == false)
	{
		// Find the info of the Created enemy
		*_enemyInfo = m_pCreatedEnemies->front();

		// remove the queued enemy
		m_pCreatedEnemies->pop();
		return true;
	}

	return false;
}

bool CMechanics_Server::GetNextDeletedPower(PowerUpInfo* _powerInfo)
{
	// Check if any PowerUps are in queue to be created
	if (m_pDeletedPowerUps->empty() == false)
	{
		// Find the info of the Created PowerUp
		*_powerInfo = m_pDeletedPowerUps->front();

		// Erase from the Map of Projectile
		//m_pPowerUps->erase(_powerInfo->iID);

		// remove the queued PowerUp
		m_pDeletedPowerUps->pop();
		return true;
	}

	return false;
}

bool CMechanics_Server::GetNextCreatedPower(PowerUpInfo* _powerInfo)
{
	// Check if any PowerUps are in queue to be created
	if (m_pCreatedPowerUps->empty() == false)
	{
		// Find the info of the Created PowerUp
		*_powerInfo = m_pCreatedPowerUps->front();

		// remove the queued PowerUp
		m_pCreatedPowerUps->pop();
		return true;
	}

	return false;
}

// Setters
void CMechanics_Server::SetAvatarAliveState(std::string _strAvatar, bool _bAlive)
{
	std::map<std::string, AvatarInfo>::iterator iterAvatar = m_pAvatars->find(_strAvatar);
	iterAvatar->second.bAlive = _bAlive;
}

bool CMechanics_Server::Initialise(std::string _strServerName)
{
	// Create and initialise the clock
	m_pClock = new CClock();
	m_pClock->Initialise();

	// Seed random generator
	srand((unsigned int)time(NULL));

	m_strServerName = _strServerName;

	// Set up the Object ID
	m_iNextObjectID = 1;

	// Create the Container for the Avatars
	m_pAvatars = new std::map<std::string, AvatarInfo>;

	// Create the Container for the Projectiles
	m_pProjectiles = new std::map<UINT, ProjectileInfo>;

	// Create the Container for the Enemies
	m_pEnemies = new std::map<UINT, EnemyInfo>;

	// Create the Container for the PowerUps
	m_pPowerUps = new std::map<UINT, PowerUpInfo>;

	// Create the Queues for Created and Deleted Projectiles
	m_pDeletedProjectiles = new std::queue<ProjectileInfo>;
	m_pCreatedProjectiles = new std::queue<ProjectileInfo>;

	// Create the Queues for Created and Deleted Enemies
	m_pDeletedEnemies = new std::queue<EnemyInfo>;
	m_pCreatedEnemies = new std::queue<EnemyInfo>;

	// Create the Queues for Created and deleted Powerups
	m_pDeletedPowerUps = new std::queue<PowerUpInfo>;
	m_pCreatedPowerUps = new std::queue<PowerUpInfo>;

	// Game Variables
	m_iWaveNumber = 0;

	// Spawn the first wave of enemies
	SpawnNextWave();

	return true;
}

void CMechanics_Server::Process()
{
	m_pClock->Process();
	float fDT = m_pClock->GetDeltaTick();
	
	ProcessProjectiles();
	ProcessEnemies(fDT);
	ProcessFlare();
	ProcessPowerUps();

	// Check if all enemies are Dead
	if (	m_iDemonCount == 0 
		&&	m_iSentinelCount == 0)
	{
		SpawnNextWave();
	}

	// Spawn a PowerUp every 10 seconds
	m_fLastPowerUpSpawned += fDT;
	if (m_fLastPowerUpSpawned > 10.0f)
	{
		SpawnNextPowerUp();
		m_fLastPowerUpSpawned -= 10.0f;
	}
}

void CMechanics_Server::ProcessAvatar(ClientToServer* _pClientPacket)
{
	// Get the current Delta Tick
	float fDT = m_pClock->GetDeltaTick();

	Controls avatarControls = _pClientPacket->activatedControls;
	std::map<std::string, AvatarInfo>::iterator Avatar = m_pAvatars->find(_pClientPacket->cUserName);

	if (Avatar->second.fFireCountDown > 0.0f)
	{
		Avatar->second.fFireCountDown -= fDT;
	}

	// Check if the Avatar has activated a Flare
	if (avatarControls.bFlare == true)
	{
		if (m_bFlareActive == true)
		{
			// TO DO: Message on screen to state flare already active
		}
		else
		{
			// retrieve the AvatarInfo of the Avatar invoking a flare
			std::map<std::string, AvatarInfo>::iterator Avatar = m_pAvatars->find((std::string)_pClientPacket->cUserName);

			m_bFlareActive = true;
			ZeroMemory(&m_Flare, sizeof(m_Flare));

			m_Flare.iID = m_iNextObjectID++;
			m_Flare.bActive = true;
			m_Flare.v3Pos = Avatar->second.v3Pos;
			m_Flare.fRange = 10.0f;
			m_Flare.fMaxRange = 100;
			m_Flare.fMaxSpeed = 20.0f;
			m_Flare.fTimeLeft = 10.0f;
		}
	}

	if (avatarControls.bAction == true)
	{
		if (Avatar->second.fFireCountDown <= 0)
		{
			Avatar->second.fFireCountDown = Avatar->second.fRateOfFire;

			// Create a new Projectile
			ProjectileInfo tempProjectileInfo;
			tempProjectileInfo.iID = m_iNextObjectID++;
			StringToStruct(Avatar->second.cUserName, tempProjectileInfo.cUserName, network::MAX_USERNAME_LENGTH);
			tempProjectileInfo.iDamage = Avatar->second.iDamage;
			tempProjectileInfo.v3Dir = Avatar->second.v3Dir;
			tempProjectileInfo.v3Pos = Avatar->second.v3Pos;
			tempProjectileInfo.v3Vel = { 0.0f, 0.0f, 0.0f };
			tempProjectileInfo.fMaxSpeed = 30.0f;
			tempProjectileInfo.BBox.v3Max = tempProjectileInfo.v3Pos + kfProjectileSize;
			tempProjectileInfo.BBox.v3Min = tempProjectileInfo.v3Pos - kfProjectileSize;

			// Add to both the queue of created and to the servers Map of Projectiles
			m_pCreatedProjectiles->push(tempProjectileInfo);

			if (m_pProjectiles->size() >= network::MAX_PROJECTILES)
			{
				// erase the oldest projectile in the Map
				m_pDeletedProjectiles->push(m_pProjectiles->begin()->second);
				m_pProjectiles->erase(m_pProjectiles->begin(), ++m_pProjectiles->begin());
			}
			m_pProjectiles->insert(std::pair<UINT, ProjectileInfo>(tempProjectileInfo.iID, tempProjectileInfo));
		}
	}

	// Process the Avatars movement for Position
	Avatar->second.v3Vel = { 0, 0, 0 };
	if (avatarControls.bUp == true)
	{
		Avatar->second.v3Vel.z += 1;
	}
	if (avatarControls.bDown == true)
	{
		Avatar->second.v3Vel.z += -1;
	}
	if (avatarControls.bRight == true)
	{
		Avatar->second.v3Vel.x += 1;
	}
	if (avatarControls.bLeft == true)
	{
		Avatar->second.v3Vel.x += -1;
	}
	Avatar->second.v3Vel.Normalise();
	Avatar->second.v3Vel = Avatar->second.v3Vel * (Avatar->second.fMaxSpeed * fDT);
	Avatar->second.v3Pos += Avatar->second.v3Vel;

	// Calculate the Avatars Look direction
	POINT pt = _pClientPacket->activatedControls.ptMouse;
	pt.x -= 500;
	pt.y -= 500;

	v3float v3Dir = { (float)pt.x, 0.0f, (float)(-pt.y) };
	NormaliseV3Float(&v3Dir);
	Avatar->second.v3Dir = v3Dir;

	// Calculate the Avatars updated Bounding Box
	Avatar->second.BBox.v3Max = Avatar->second.v3Pos + (1.1f * kfAvatarSize);
	Avatar->second.BBox.v3Min = Avatar->second.v3Pos - (1.1f * kfAvatarSize);

	bool bCollisionDetected = false;
	// Check for Collisions with Avatars
	std::map<std::string, AvatarInfo>::iterator iterCollisionAvatar = m_pAvatars->begin();
	while (iterCollisionAvatar != m_pAvatars->end())
	{
		if (Avatar->second.iID != iterCollisionAvatar->second.iID)
		{
			if (CollisionCheck(Avatar->second.BBox, iterCollisionAvatar->second.BBox) == true)
			{
				bCollisionDetected = true;
				break;
			}
		}
		iterCollisionAvatar++;
	}

	// Check only if a collision hasn't already been detected
	if (bCollisionDetected == false)
	{
		// Check for collisions with other Enemies
		std::map<UINT, EnemyInfo>::iterator iterCollisionEnemy = m_pEnemies->begin();
		while (iterCollisionEnemy != m_pEnemies->end())
		{
			if (CollisionCheck(Avatar->second.BBox, iterCollisionEnemy->second.BBox) == true)
			{
				bCollisionDetected = true;
				break;
			}

			iterCollisionEnemy++;
		}
	}

	if (bCollisionDetected == true)
	{
		// Revert the Enemy back to their original position before moving
		Avatar->second.v3Pos -= Avatar->second.v3Vel;

		// Revert the Bounding box to before the move
		Avatar->second.BBox.v3Max = Avatar->second.v3Pos + (1.1f * kfAvatarSize);
		Avatar->second.BBox.v3Min = Avatar->second.v3Pos - (1.1f * kfAvatarSize);
	}

	// Check for collisions with PowerUps
	std::map<UINT, PowerUpInfo>::iterator iterCollisionPowerUp = m_pPowerUps->begin();
	while (iterCollisionPowerUp != m_pPowerUps->end())
	{
		if (CollisionCheck(Avatar->second.BBox, iterCollisionPowerUp->second.BBox) == true)
		{
			// TO DO - ADD the powerUp effect

			//Delete the PowerUp
			m_pDeletedPowerUps->push(iterCollisionPowerUp->second);

			break;
		}

		iterCollisionPowerUp++;
	}

}

void CMechanics_Server::ProcessProjectiles()
{
	float fDT = m_pClock->GetDeltaTick();

	std::map<UINT, ProjectileInfo>::iterator iterProjectile = m_pProjectiles->begin();
	while (iterProjectile != m_pProjectiles->end())
	{
		// Update the Projectiles Position

		if (fDT == 0.0f)
		{
			int i = 0;
		}
		if (iterProjectile->second.fMaxSpeed == 0.0f)
		{
			int i = 0;
		}

		iterProjectile->second.v3Vel = iterProjectile->second.v3Dir.Normalise() * (iterProjectile->second.fMaxSpeed * fDT);
		iterProjectile->second.v3Pos += iterProjectile->second.v3Vel;

		// Calculate the Projectile updated Bounding Box
		iterProjectile->second.BBox.v3Max = iterProjectile->second.v3Pos + (1.1f * kfProjectileSize);
		iterProjectile->second.BBox.v3Min = iterProjectile->second.v3Pos - (1.1f * kfProjectileSize);

		// Check for collisions with Enemies
		std::map<UINT, EnemyInfo>::iterator iterCollisionEnemy = m_pEnemies->begin();
		while (iterCollisionEnemy != m_pEnemies->end())
		{
			if (CollisionCheck(iterProjectile->second.BBox, iterCollisionEnemy->second.BBox) == true)
			{
				// Damage the Enemy
				iterCollisionEnemy->second.iHealth -= iterProjectile->second.iDamage;

				// Check if the Enemy is a Sentinel
				if (iterCollisionEnemy->second.eType == ET_SENTINEL)
				{
					// Set the Target to the first Avatar to hit this sentinel
					if ((std::string)iterCollisionEnemy->second.cTargetPlayer == "")
					{
						StringToStruct(iterProjectile->second.cUserName, iterCollisionEnemy->second.cTargetPlayer, network::MAX_USERNAME_LENGTH);
					}
				}

				if (iterCollisionEnemy->second.iHealth <= 0)
				{
					// Add points to the players score
					std::map < std::string, AvatarInfo>::iterator Avatar = m_pAvatars->find(iterProjectile->second.cUserName);
					if (Avatar != m_pAvatars->end())
					{
						// Provided the Avatar still exists
						Avatar->second.iScore += iterCollisionEnemy->second.iPoints;
					}

					// delete the enemy if their health drops to zero
					m_pDeletedEnemies->push(iterCollisionEnemy->second);
				}
				// delete the projectile
				m_pDeletedProjectiles->push(iterProjectile->second);

				break;
			}
			iterCollisionEnemy++;
		}

		iterProjectile++;
	}
}

void CMechanics_Server::ProcessFlare()
{
	if (m_Flare.bActive == true)
	{
		// Get the current Delta Tick
		float fDT = m_pClock->GetDeltaTick();

		// Calculate the time left for the flare
		m_Flare.fTimeLeft -= fDT;
		if (m_Flare.fTimeLeft < 0)
		{
			// Flare has timed out, deactivate the flare.
			m_Flare.bActive = false;
			m_bFlareActive = false;
		}

		if (m_Flare.fRange < m_Flare.fMaxRange)
		{
			m_Flare.fRange += (m_Flare.fMaxSpeed * fDT);
		}
	}
}

void CMechanics_Server::ProcessPowerUps()
{
	float fDT = m_pClock->GetDeltaTick();

	std::map<UINT, PowerUpInfo>::iterator iterPowerUp = m_pPowerUps->begin();
	while (iterPowerUp != m_pPowerUps->end())
	{
		Wander(&iterPowerUp->second, fDT);

		// Calculate the Enemies updated Bounding Box
		iterPowerUp->second.BBox.v3Max = iterPowerUp->second.v3Pos + (1.1f * kfPowerUpSize);
		iterPowerUp->second.BBox.v3Min = iterPowerUp->second.v3Pos - (1.1f * kfPowerUpSize);

		// TO DO - calculate edge of terrain collision


		iterPowerUp++;
	}
}

void CMechanics_Server::ProcessEnemies(float _fDT)
{
	float fEnemySize;
	std::map<UINT, EnemyInfo>::iterator iterEnemy = m_pEnemies->begin();
	while (iterEnemy != m_pEnemies->end())
	{
		switch (iterEnemy->second.eType)
		{
			case ET_DEMON:
			{
				ProcessDemon(&iterEnemy->second, _fDT);
				fEnemySize = kfDemonSize;
				break;
			}
			case ET_SENTINEL:
			{
				ProcessSentinel(&iterEnemy->second, _fDT);
				fEnemySize = kfSentinelSize;
				break;
			}
		}	// End Switch

		// Calculate the Enemies updated Bounding Box
		iterEnemy->second.BBox.v3Max = iterEnemy->second.v3Pos + (1.1f * fEnemySize);
		iterEnemy->second.BBox.v3Min = iterEnemy->second.v3Pos - (1.1f * fEnemySize);
		
		bool bCollisionDetected = false;
		// Check for Collisions with Avatars
		std::map<std::string, AvatarInfo>::iterator iterCollisionAvatar = m_pAvatars->begin();
		while (iterCollisionAvatar != m_pAvatars->end())
		{
			if (CollisionCheck(iterEnemy->second.BBox, iterCollisionAvatar->second.BBox) == true)
			{
				bCollisionDetected = true;
				break;
			}
			iterCollisionAvatar++;
		}
		
		// Check only if a collision hasn't already been detected
		if (bCollisionDetected == false)
		{
			// Check for collisions with other Enemies
			std::map<UINT, EnemyInfo>::iterator iterCollisionEnemy = m_pEnemies->begin();
			while (iterCollisionEnemy != m_pEnemies->end())
			{
				if (iterEnemy->second.iID != iterCollisionEnemy->second.iID)
				{
					if (CollisionCheck(iterEnemy->second.BBox, iterCollisionEnemy->second.BBox) == true)
					{
						bCollisionDetected = true;
						break;
					}
				}
				iterCollisionEnemy++;
			}
		}
		
		if (bCollisionDetected == true)
		{
			// Revert the Enemy back to their original position before moving
			iterEnemy->second.v3Pos -= iterEnemy->second.v3Vel;

			// Revert the Bounding box to before the move
			iterEnemy->second.BBox.v3Max = iterEnemy->second.v3Pos + (1.1f * fEnemySize);
			iterEnemy->second.BBox.v3Min = iterEnemy->second.v3Pos - (1.1f * fEnemySize);
		}

		iterEnemy++;
	}
}

void CMechanics_Server::ProcessDemon(EnemyInfo* _enemyInfo, float _fDT)
{
	if (m_pAvatars->size() > 0)
	{
		// Find Avatar Target
		std::map<std::string, AvatarInfo>::iterator iterAvatar = m_pAvatars->begin();
		std::map<std::string, AvatarInfo>::iterator closestAvatar = iterAvatar;

		// Calculate the distance from the first avatar
		float fDistance = abs((iterAvatar->second.v3Pos - _enemyInfo->v3Pos).Magnitude());

		while (iterAvatar != m_pAvatars->end())
		{
			// Caclulate the distance from the enemy to the current
			float fNewDistance = abs((iterAvatar->second.v3Pos - _enemyInfo->v3Pos).Magnitude());
			if (fNewDistance < fDistance)
			{
				// Set the closest avatar to the current avatar and update the distance for checking
				closestAvatar = iterAvatar;
				fDistance = fNewDistance;
			}
			iterAvatar++;
		}
		// Set the demons target to be the position of the closest avatar
		_enemyInfo->v3Target = closestAvatar->second.v3Pos;

		// Steerings
		Seek(_enemyInfo, _fDT);
	}
}

void CMechanics_Server::ProcessSentinel(EnemyInfo* _enemyInfo, float _fDT)
{
	if ((std::string)_enemyInfo->cTargetPlayer != "")
	{
		std::map<std::string, AvatarInfo>::iterator TargetAvatar = m_pAvatars->find((std::string)_enemyInfo->cTargetPlayer);
		if (TargetAvatar != m_pAvatars->end())
		{
			_enemyInfo->v3Target = TargetAvatar->second.v3Pos;
		}
		else
		{
			StringToStruct("", _enemyInfo->cTargetPlayer, 1);
		}

		// Steerings
		Seek(_enemyInfo, _fDT);
	}
	else
	{
		// TO DO - WANDER AI
		Wander(_enemyInfo, _fDT);
	}
}

void CMechanics_Server::SpawnNextWave()
{
	// Increment The Wave Count
	m_iWaveNumber++;

	m_iDemonCount = (m_iWaveNumber * 2) + 10;
	m_iSentinelCount = m_iWaveNumber;

	for (UINT i = 0; i < m_iDemonCount; i++)
	{
		// TO DO - base on the terrain sizes
		float fRandomX = float(rand() % 50 - 25);
		float fRandomZ = float(rand() % 50 - 25);
		
		EnemyInfo tempEnemyInfo;
		tempEnemyInfo.eType = ET_DEMON;
		tempEnemyInfo.iHealth = 100;
		tempEnemyInfo.iID = m_iNextObjectID++;
		tempEnemyInfo.v3Dir = { fRandomZ, 0.0f, fRandomX };
		tempEnemyInfo.v3Pos = { fRandomX, 0.0f, fRandomZ };
		tempEnemyInfo.v3Vel = { 0.0f, 0.0f, 0.0f };
		tempEnemyInfo.v3Acceleration = { 0.0f, 0.0f, 0.0f };
		tempEnemyInfo.fMaxSpeed = 3.0f;
		tempEnemyInfo.fMaxForce = 30.0f;
		tempEnemyInfo.iPoints = 50;
		tempEnemyInfo.fWanderAngle = 0;
		tempEnemyInfo.fSize = kfDemonSize;
		tempEnemyInfo.BBox.v3Max = tempEnemyInfo.v3Pos + kfDemonSize;
		tempEnemyInfo.BBox.v3Min = tempEnemyInfo.v3Pos - kfDemonSize;
		m_pCreatedEnemies->push(tempEnemyInfo);
		m_pEnemies->insert(std::pair<UINT, EnemyInfo>(tempEnemyInfo.iID, tempEnemyInfo));
	}

	for (UINT i = 0; i < m_iSentinelCount; i++)
	{
		// TO DO - base on the terrain sizes
		float fRandomX = float(rand() % 50 - 25);
		float fRandomZ = float(rand() % 50 - 25);

		EnemyInfo tempEnemyInfo;
		tempEnemyInfo.eType = ET_SENTINEL;
		tempEnemyInfo.iHealth = 500;
		tempEnemyInfo.iID = m_iNextObjectID++;
		tempEnemyInfo.v3Dir = { fRandomZ, 0.0f, fRandomX };
		tempEnemyInfo.v3Pos = { fRandomX, 0.0f, fRandomZ };
		tempEnemyInfo.v3Vel = { 0.0f, 0.0f, 0.0f };
		tempEnemyInfo.v3Acceleration = { 0.0f, 0.0f, 0.0f };
		tempEnemyInfo.fMaxSpeed = 5.0f;
		tempEnemyInfo.fMaxForce = 1.0f;
		tempEnemyInfo.iPoints = 500;
		tempEnemyInfo.fWanderAngle = 0;
		tempEnemyInfo.fSize = kfSentinelSize;
		StringToStruct("", tempEnemyInfo.cTargetPlayer, network::MAX_USERNAME_LENGTH);
		tempEnemyInfo.BBox.v3Max = tempEnemyInfo.v3Pos + kfSentinelSize;
		tempEnemyInfo.BBox.v3Min = tempEnemyInfo.v3Pos - kfSentinelSize;
		m_pCreatedEnemies->push(tempEnemyInfo);
		m_pEnemies->insert(std::pair<UINT, EnemyInfo>(tempEnemyInfo.iID, tempEnemyInfo));
	}
}

void CMechanics_Server::SpawnNextPowerUp()
{
	if (m_pPowerUps->size() < network::MAX_POWERUPS_SPAWNED)
	{
		// TO DO - base on the terrain sizes
		float fRandomX = float(rand() % 50 - 25);
		float fRandomZ = float(rand() % 50 - 25);
	
		PowerUpInfo tempPowerInfo;
		tempPowerInfo.eType = PT_HEALTH;
		tempPowerInfo.iID = m_iNextObjectID++;
		tempPowerInfo.v3Dir = { fRandomZ, 0.0f, fRandomX };
		tempPowerInfo.v3Pos = { fRandomX, 0.0f, fRandomZ };
		tempPowerInfo.v3Vel = { 0.0f, 0.0f, 0.0f };
		tempPowerInfo.v3Acceleration = { 0.0f, 0.0f, 0.0f };
		tempPowerInfo.fMaxSpeed = 1.0f;
		tempPowerInfo.fMaxForce = 5.0f;
		tempPowerInfo.iPoints = 10;
		tempPowerInfo.fWanderAngle = 0;
		tempPowerInfo.BBox.v3Max = tempPowerInfo.v3Pos + kfPowerUpSize;
		tempPowerInfo.BBox.v3Min = tempPowerInfo.v3Pos - kfPowerUpSize;
		m_pCreatedPowerUps->push(tempPowerInfo);
		m_pPowerUps->insert(std::pair<UINT, PowerUpInfo>(tempPowerInfo.iID, tempPowerInfo));
	}
}

bool CMechanics_Server::CreateDataPacket(ServerToClient* _pServerPacket)
{
	// Erase old data
	ZeroMemory(*(&_pServerPacket), sizeof(*_pServerPacket));

	// Fill basic information of the packet
	_pServerPacket->bCommand = false;
	_pServerPacket->eCommand = ERROR_COMMAND;
	_pServerPacket->iCurrentUserCount = (int)(m_pAvatars->size());
	_pServerPacket->iCurrentEnemyCount = (int)(m_pEnemies->size());
	_pServerPacket->iCurrentPowerUpCount = (int)(m_pPowerUps->size());
	_pServerPacket->iCurrentProjectileCount = (int)(m_pProjectiles->size());

	// Add the Server as the username to the Packet structure
	if (!(StringToStruct(m_strServerName.c_str(), _pServerPacket->cServerName, network::MAX_SERVERNAME_LENGTH)))
	{
		return false;	// Data invalid - Data Packet failed to create
	}

	// Add all the Avatars to the Packet with their information status
	std::map<std::string, AvatarInfo>::iterator iterAvatar = m_pAvatars->begin();
	int iIndex = 0;
	while (iterAvatar != m_pAvatars->end())
	{
		_pServerPacket->Avatars[iIndex] = iterAvatar->second;

		iIndex++;
		iterAvatar++;
	}

	// Add all the Enemies to the Packet with their information status
	std::map<UINT, EnemyInfo>::iterator iterEnemy = m_pEnemies->begin();
	iIndex = 0;
	while (iterEnemy != m_pEnemies->end())
	{
		_pServerPacket->Enemies[iIndex] = iterEnemy->second;

		iIndex++;
		iterEnemy++;
	}

	// Add all the PowerUps to the Packet with their information status
	std::map<UINT, PowerUpInfo>::iterator iterPowerUp = m_pPowerUps->begin();
	iIndex = 0;
	while (iterPowerUp != m_pPowerUps->end())
	{
		_pServerPacket->PowerUps[iIndex] = iterPowerUp->second;

		iIndex++;
		iterPowerUp++;
	}

	// Add all the Projectiles to the Packet with their information status
	std::map<UINT, ProjectileInfo>::iterator iterProjectile = m_pProjectiles->begin();
	iIndex = 0;
	while (iterProjectile != m_pProjectiles->end())
	{
		_pServerPacket->Projectiles[iIndex] = iterProjectile->second;

		iIndex++;
		iterProjectile++;
	}

	// Add the Flare to the Data Packet
	_pServerPacket->Flare = m_Flare;

	return true;
}

void CMechanics_Server::AddAvatar(ClientToServer* _pClientPacket)
{
	// Retrieve the number of the current users
	int iNumber = (int)(m_pAvatars->size());

	// Create a new UserInfo with default information for the new user
	AvatarInfo tempAvatarInfo;
	tempAvatarInfo.bAlive = false;
	StringToStruct(_pClientPacket->cUserName, tempAvatarInfo.cUserName, network::MAX_USERNAME_LENGTH);

	// Create the starting position based on the current number of users
	tempAvatarInfo.v3Pos = { (float)iNumber * 5, 0, 5 };
	tempAvatarInfo.iID = m_iNextObjectID++;
	tempAvatarInfo.fMaxSpeed = 40.0f;
	tempAvatarInfo.fRateOfFire = 0.2f;
	tempAvatarInfo.fFireCountDown = 0.0f;
	tempAvatarInfo.iDamage = 50;
	tempAvatarInfo.iHealth = 100;
	tempAvatarInfo.iScore = 0;

	tempAvatarInfo.BBox.v3Max = tempAvatarInfo.v3Pos + kfAvatarSize;
	tempAvatarInfo.BBox.v3Min = tempAvatarInfo.v3Pos - kfAvatarSize;

	std::pair<std::string, AvatarInfo> newAvatar((std::string)_pClientPacket->cUserName, tempAvatarInfo);
	m_pAvatars->insert(newAvatar);
}

void CMechanics_Server::RemoveAvatar(std::string _strUserName)
{
	m_pAvatars->erase(_strUserName);
}

bool CMechanics_Server::AddAvatarsToPacket(ServerToClient* _pServerPacket)
{
	if (m_pAvatars->empty() == false)
	{
		// Add all the Users to the Packet with their information status
		std::map<std::string, AvatarInfo>::iterator iterCurrent = m_pAvatars->begin();
		int iIndex = 0;
		while (iterCurrent != m_pAvatars->end())
		{
			_pServerPacket->Avatars[iIndex] = iterCurrent->second;

			iIndex++;
			iterCurrent++;
		}
	}

	return true;
}

bool CMechanics_Server::CheckAllAvatarsReady()
{
	// Start the check to say all Avatars are ready
	bool bAllReady = true;

	std::map < std::string, AvatarInfo>::iterator iterAvatar = m_pAvatars->begin();

	// Cycle through all Avatars
	while (iterAvatar != m_pAvatars->end())
	{
		// Check the Ready state
		if (iterAvatar->second.bAlive == false)
		{

			// If one Avatar is not ready then change the All Ready state to false and break the loop 
			bAllReady = false;
			break;
		}

		iterAvatar++;
	}

	return bAllReady;
}

bool CMechanics_Server::CollisionCheck(BoundingBox _BBoxCaller, BoundingBox _BBoxTarget)
{

	if (	(_BBoxCaller.v3Max.x >= _BBoxTarget.v3Min.x)
		&&	(_BBoxCaller.v3Max.y >= _BBoxTarget.v3Min.y)
		&&	(_BBoxCaller.v3Max.z >= _BBoxTarget.v3Min.z)
		&&	(_BBoxCaller.v3Min.x <= _BBoxTarget.v3Max.x)
		&&	(_BBoxCaller.v3Min.y <= _BBoxTarget.v3Max.y)
		&&	(_BBoxCaller.v3Min.z <= _BBoxTarget.v3Max.z)
		)
	{
		// Collision has been detected
		return true;
	}
	else
	{
		// Collision NOT detected
		return false;
	}
}