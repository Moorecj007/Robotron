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

	// Delete the queues of enemies
	delete m_pDeletedEnemies;
	m_pDeletedEnemies = 0;
	delete m_pCreatedEnemies;
	m_pCreatedEnemies = 0;
}

// Getters
bool CMechanics_Server::GetNextDeletedEnemy(EnemyInfo* _enemyInfo)
{
	// Check if any enemies are in queue to be created
	if (m_pDeletedEnemies->empty() == false)
	{
		// Find the ID of the Created enemy
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
		// Find the ID of the Created enemy
		*_enemyInfo = m_pCreatedEnemies->front();

		// remove the queued enemy
		m_pCreatedEnemies->pop();
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

	// Create the Container for the Avatars
	m_pAvatars = new std::map<std::string, AvatarInfo>;

	// Create the Queue for Created and Deleted Enemies
	m_pDeletedEnemies = new std::queue<EnemyInfo>;
	m_pCreatedEnemies = new std::queue<EnemyInfo>;

	// TO DO - remove to wave spawing function
	EnemyInfo tempEnemyInfo;
	tempEnemyInfo.eType = ET_DEMON;
	tempEnemyInfo.iID = 0;
	tempEnemyInfo.v3Dir = { 0.0f, 0.0f, -1.0f };
	tempEnemyInfo.v3Pos = { 10.0f, 0.0f, 10.0f };
	tempEnemyInfo.v3Vel = { 0.0f, 0.0f, 0.0f };

	m_pCreatedEnemies->push(tempEnemyInfo);

	return true;
}

void CMechanics_Server::Process()
{
	m_pClock->Process();
	float fDT = m_pClock->GetDeltaTick();
}

void CMechanics_Server::ProcessAvatarMovement(ClientToServer* _pClientPacket)
{
	std::map<std::string, AvatarInfo>::iterator Avatar = m_pAvatars->find(_pClientPacket->cUserName);

	// TO DO - Move to another location
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
}

bool CMechanics_Server::CreateDataPacket(ServerToClient* _pServerPacket)
{
	// Erase old data
	ZeroMemory(*(&_pServerPacket), sizeof(*_pServerPacket));

	// Fill basic information of the packet
	_pServerPacket->bCommand = false;
	_pServerPacket->eCommand = ERROR_COMMAND;
	_pServerPacket->CurrentUserCount = (int)(m_pAvatars->size());

	// Add the Server as the username to the Packet structure
	if (!(StringToStruct(m_strServerName.c_str(), _pServerPacket->cServerName, network::MAX_SERVERNAME_LENGTH)))
	{
		return false;	// Data invalid - Data Packet failed to create
	}

	// Add all the Users to the Packet with their information status
	std::map<std::string, AvatarInfo>::iterator iterCurrent = m_pAvatars->begin();
	int iIndex = 0;
	while (iterCurrent != m_pAvatars->end())
	{
		_pServerPacket->Avatars[iIndex] = iterCurrent->second;

		iIndex++;
		iterCurrent++;
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

