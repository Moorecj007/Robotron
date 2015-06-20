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

	// Delete the container of Enemies
	delete m_pEnemies;
	m_pEnemies = 0;

	// Delete the container of PowerUps
	delete m_pPowerUps;
	m_pPowerUps = 0;

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
bool CMechanics_Server::GetNextDeletedEnemy(EnemyInfo* _enemyInfo)
{
	// Check if any enemies are in queue to be created
	if (m_pDeletedEnemies->empty() == false)
	{
		// Find the info of the Created enemy
		*_enemyInfo = m_pDeletedEnemies->front();

		// remove the queued enemy
		m_pCreatedEnemies->pop();
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

	m_strServerName = _strServerName;

	// Set up the Object ID
	m_iNextObjectID = 0;

	// Create the Container for the Avatars
	m_pAvatars = new std::map<std::string, AvatarInfo>;

	// Create the Container for the Enemies
	m_pEnemies = new std::map<UINT, EnemyInfo>;

	// Create the Container for the PowerUps
	m_pPowerUps = new std::map<UINT, PowerUpInfo>;

	// Create the Queues for Created and Deleted Enemies
	m_pDeletedEnemies = new std::queue<EnemyInfo>;
	m_pCreatedEnemies = new std::queue<EnemyInfo>;

	// Create the Queues for Created and deleted Powerups
	m_pDeletedPowerUps = new std::queue<PowerUpInfo>;
	m_pCreatedPowerUps = new std::queue<PowerUpInfo>;

	// TO DO - remove to wave spawing function
	EnemyInfo tempEnemyInfo;
	tempEnemyInfo.eType = ET_DEMON;
	tempEnemyInfo.iID = m_iNextObjectID++;
	tempEnemyInfo.v3Dir = { 0.0f, 0.0f, 1.0f };
	tempEnemyInfo.v3Pos = { 0.0f, 0.0f, 10.0f };
	tempEnemyInfo.v3Vel = { 0.0f, 0.0f, 0.0f };
	tempEnemyInfo.v3Acceleration = { 0.0f, 0.0f, 0.0f };
	tempEnemyInfo.fMaxSpeed = 0.1f;
	tempEnemyInfo.fMaxForce = 0.01f;
	m_pCreatedEnemies->push(tempEnemyInfo);
	m_pEnemies->insert(std::pair<UINT, EnemyInfo>(tempEnemyInfo.iID, tempEnemyInfo));

	// TO DO - remove to wave spawing function
	PowerUpInfo tempPowerInfo;
	tempPowerInfo.eType = PT_HEALTH;
	tempPowerInfo.iID = m_iNextObjectID++;
	tempPowerInfo.v3Dir = { 1.0f, 0.0f, 0.0f };
	tempPowerInfo.v3Pos = { 0.0f, 0.0f, 0.0f };
	tempPowerInfo.v3Vel = { 0.0f, 0.0f, 0.0f };
	m_pCreatedPowerUps->push(tempPowerInfo);

	return true;
}

void CMechanics_Server::Process()
{
	m_pClock->Process();
	float fDT = m_pClock->GetDeltaTick();

	ProcessEnemies();
}

void CMechanics_Server::ProcessAvatarMovement(ClientToServer* _pClientPacket)
{
	std::map<std::string, AvatarInfo>::iterator Avatar = m_pAvatars->find(_pClientPacket->cUserName);

	// Process the Avatars movement for Position
	v3float v3Movement = { 0, 0, 0 };
	if (_pClientPacket->activatedControls.bUp == true)
	{
		v3Movement.z += 1;
	}
	if (_pClientPacket->activatedControls.bDown == true)
	{
		v3Movement.z += -1;
	}
	if (_pClientPacket->activatedControls.bRight == true)
	{
		v3Movement.x += 1;
	}
	if (_pClientPacket->activatedControls.bLeft == true)
	{
		v3Movement.x += -1;
	}
	NormaliseV3Float(&v3Movement);
	Avatar->second.v3Pos += v3Movement;

	// Calculate the Avatars Look direction
	POINT pt = _pClientPacket->activatedControls.ptMouse;
	pt.x -= 500;
	pt.y -= 500;

	v3float v3Dir = { (float)pt.x, 0.0f, (float)(-pt.y) };
	NormaliseV3Float(&v3Dir);
	Avatar->second.v3Dir = v3Dir;
}

void CMechanics_Server::ProcessEnemies()
{
	std::map<UINT, EnemyInfo>::iterator iterEnemy = m_pEnemies->begin();
	while (iterEnemy != m_pEnemies->end())
	{
		switch (iterEnemy->second.eType)
		{
			case ET_DEMON:
			{
				ProcessDemonAI(&iterEnemy->second);
				break;
			}
		}	// End Switch
		iterEnemy++;
	}
}

void CMechanics_Server::ProcessDemonAI(EnemyInfo* _enemyInfo)
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
		Seek(_enemyInfo);

		
	}
}

bool CMechanics_Server::CreateDataPacket(ServerToClient* _pServerPacket)
{
	// Erase old data
	ZeroMemory(*(&_pServerPacket), sizeof(*_pServerPacket));

	// Fill basic information of the packet
	_pServerPacket->bCommand = false;
	_pServerPacket->eCommand = ERROR_COMMAND;
	_pServerPacket->CurrentUserCount = (int)(m_pAvatars->size());
	_pServerPacket->CurrentEnemyCount = (int)(m_pEnemies->size());

	// Add the Server as the username to the Packet structure
	if (!(StringToStruct(m_strServerName.c_str(), _pServerPacket->cServerName, network::MAX_SERVERNAME_LENGTH)))
	{
		return false;	// Data invalid - Data Packet failed to create
	}

	// Add all the Users to the Packet with their information status
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

