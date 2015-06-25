/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Mechanics_Client.cpp
* Description : Handles all the game mechanics for the Client
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Include
#include "Mechanics_Client.h"

CMechanics_Client::CMechanics_Client()
{
}

CMechanics_Client::~CMechanics_Client()
{
	// Delete the Container of Avatars
	if (m_pAvatars != 0)
	{
		std::map<std::string, CAvatar*>::iterator currentAvatar = m_pAvatars->begin();
		while (currentAvatar != m_pAvatars->end())
		{
			delete currentAvatar->second;
			currentAvatar->second = 0;
			currentAvatar++;
		}

		delete m_pAvatars;
		m_pAvatars = 0;
	}

	// Delete the Container of Projectiles
	if (m_pProjectiles != 0)
	{
		std::map<UINT, CProjectile*>::iterator currentProjectile = m_pProjectiles->begin();
		while (currentProjectile != m_pProjectiles->end())
		{
			delete currentProjectile->second;
			currentProjectile->second = 0;
			currentProjectile++;
		}

		delete m_pProjectiles;
		m_pProjectiles = 0;
	}

	// Delete the Container of Enemies
	if (m_pEnemies != 0)
	{
		std::map<UINT, CEnemy*>::iterator currentEnemy = m_pEnemies->begin();
		while (currentEnemy != m_pEnemies->end())
		{
			delete currentEnemy->second;
			currentEnemy->second = 0;
			currentEnemy++;
		}

		delete m_pEnemies;
		m_pEnemies = 0;
	}

	// Delete the Container of PowerUps
	if (m_pPowerUps != 0)
	{
		std::map<UINT, CPowerUp*>::iterator currentPowerUp = m_pPowerUps->begin();
		while (currentPowerUp != m_pPowerUps->end())
		{
			delete currentPowerUp->second;
			currentPowerUp->second = 0;
			currentPowerUp++;
		}

		delete m_pPowerUps;
		m_pPowerUps = 0;
	}

	// Delete the created meshes
	delete m_pAvatarMesh;
	m_pAvatarMesh = 0;
	delete m_pProjectileMesh;
	m_pProjectileMesh = 0;
	delete m_pFlareMesh;
	m_pFlareMesh = 0;

	delete m_pDemonMesh;
	m_pDemonMesh = 0;
	delete m_pSentinelMesh;
	m_pSentinelMesh = 0;
	delete m_pShadowMesh;
	m_pShadowMesh = 0;

	delete m_pHealthPowerMesh;
	m_pHealthPowerMesh = 0;
	delete m_pFlarePowerMesh;
	m_pFlarePowerMesh = 0;
	delete m_pGoldenPowerMesh;
	m_pGoldenPowerMesh = 0;


	// Delete the Graphics Objects
	delete m_pCamera;
	m_pCamera = 0;
	delete m_pTerrain;
	m_pTerrain = 0;
	delete m_pFlare;
	m_pFlare = 0;
}

bool CMechanics_Client::Initialise(IRenderer* _pRenderer, std::string _strUserName, bool _bSinglePlayer)
{
	// Store the pointer to the renderer
	m_pRenderer = _pRenderer;
	m_strUserName = _strUserName;

	// Create the initial Projection Matrices and set them on the Renderer
	m_pRenderer->CalculateProjectionMatrix(D3DXToRadian(45), 0.1f, 10000.0f);

	// Create a Terrain for the Game
	m_pTerrain = new CTerrain();
	VertexScalar TerrainScalar = { kfTerrainScalarX, kfTerrainScalarY, kfTerrainScalarZ };
	std::string strImagePath = "Assets\\Basic Terrain.bmp";
	m_pTerrain->Initialise(m_pRenderer, strImagePath, TerrainScalar);
	m_pTerrain->SetCenter(0, 0, 0);

	// Create the the containers for assets
	m_pAvatars = new std::map < std::string, CAvatar*>;
	m_pProjectiles = new std::map < UINT, CProjectile*>;
	m_pEnemies = new std::map < UINT, CEnemy*>;
	m_pPowerUps = new std::map < UINT, CPowerUp*>;

	// Create the required Assets
	CreateAvatarAsset();
	CreateProjectileAsset();
	CreateFlareAsset();

	CreateDemonAsset();
	CreateSentinelAsset();
	CreateShadowAsset();

	CreateHealthPowerAsset();
	CreateFlarePowerAsset();
	CreateGoldenPowerAsset();

	// Create game variables
	m_bToggle = 0;
	m_bSinglePlayer = _bSinglePlayer;

	return true;
}

void CMechanics_Client::Process( float _fDT, ServerToClient* _pServerPacket)
{
	// save the current states of the delta tick and the Packet from the server
	m_fDT = _fDT;
	m_pServerPacket = _pServerPacket;

	UpdateAvatars();
	UpdatePowerUps();
	UpdateProjectiles();
	UpdateEnemies();
	UpdateFlare();

	// Process the camera to keep it following the Avatar
	std::map<std::string, CAvatar*>::iterator Avatar = m_pAvatars->find(m_strUserName);
	v3float v3Pos = *(Avatar->second->GetPosition());
	m_pCamera->SetCamera({ v3Pos.x, v3Pos.y, v3Pos.z }, { v3Pos.x, 50, v3Pos.z}, { 0, 0, 1 }, { 0, -1, 0 });
	m_pCamera->Process(m_pRenderer);

	m_bToggle = !m_bToggle;
}

void CMechanics_Client::Draw()
{
	// Render the Terrain
	m_pTerrain->Draw(m_pRenderer);

	// Draw all the PowerUps
	std::map<UINT, CPowerUp*>::iterator iterPower = m_pPowerUps->begin();
	while (iterPower != m_pPowerUps->end())
	{
		iterPower->second->Draw();
		iterPower++;
	}

	// Draw all the Avatars
	std::map<std::string, CAvatar*>::iterator iterAvatar = m_pAvatars->begin();
	while (iterAvatar != m_pAvatars->end())
	{
		if (iterAvatar->second->GetAliveStatus() == true)
		{
			// Ensure the torch for the avatar is on
			m_pRenderer->LightEnable(iterAvatar->second->GetTorchID(), true);
			iterAvatar->second->Draw();
		}
		else
		{
			// Turn a dead players torch off
			m_pRenderer->LightEnable(iterAvatar->second->GetTorchID(), false);
		}
		iterAvatar++;
	}

	iterAvatar = m_pAvatars->find(m_strUserName);
	if (iterAvatar->second->GetAliveStatus() == true)
	{
		// Turn off the directional light
		m_pRenderer->LightEnable(0, false);
	}
	else
	{
		// Turn on the directional light
		m_pRenderer->LightEnable(0, true);
	}

	// Draw all the Projectiles
	std::map<UINT, CProjectile*>::iterator iterProjectile = m_pProjectiles->begin();
	while (iterProjectile != m_pProjectiles->end())
	{
		iterProjectile->second->Draw();
		iterProjectile++;
	}

	// Draw the Flare if it is active
	if (m_pFlare->GetActive() == true)
	{
		m_pFlare->Draw();
	}

	std::vector<CEnemy*> pShadows;
	// Draw all the Enemies
	std::map<UINT, CEnemy*>::iterator iterEnemy = m_pEnemies->begin();
	while (iterEnemy != m_pEnemies->end())
	{
		if (iterEnemy->second->GetEnemyType() == ET_SHADOW)
		{
			// Add the Shadows to a separate vector to render last
			pShadows.push_back(iterEnemy->second);
			iterEnemy++;
			continue;
		}
		iterEnemy->second->Draw();
		iterEnemy++;
	}

	m_pRenderer->AlphaBlend(true);
	// Draw all the Shadows
	for (UINT i = 0; i < pShadows.size(); i++)
	{
		pShadows[i]->Draw();
	}
	m_pRenderer->AlphaBlend(false);

	// Render the HUD
	OverlayHUD();

}

CMesh* CMechanics_Client::CreateCubeMesh(float _fSize, int _iTexID)
{
	float fVertexfromOrigin = _fSize;
	CMesh* meshCube = new CMesh(m_pRenderer, _fSize, _iTexID);

	// Add Vertices to the Mesh in shape of a Cube
	// Front Face
	meshCube->AddVertex(CVertex({ -fVertexfromOrigin, fVertexfromOrigin, -fVertexfromOrigin }, { -1.0f, 1.0f, -1.0f }, { 0.0f, 0.0f }));
	meshCube->AddVertex(CVertex({ fVertexfromOrigin, fVertexfromOrigin, -fVertexfromOrigin }, { 1.0f, 1.0f, -1.0f }, { 1.0f, 0.0f }));
	meshCube->AddVertex(CVertex({ fVertexfromOrigin, -fVertexfromOrigin, -fVertexfromOrigin }, { 1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f }));
	meshCube->AddVertex(CVertex({ -fVertexfromOrigin, -fVertexfromOrigin, -fVertexfromOrigin }, { -1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f }));

	// Left Face
	meshCube->AddVertex(CVertex({ -fVertexfromOrigin, fVertexfromOrigin, fVertexfromOrigin }, { -1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f }));
	meshCube->AddVertex(CVertex({ -fVertexfromOrigin, fVertexfromOrigin,- fVertexfromOrigin }, { -1.0f, 1.0f, -1.0f }, { 1.0f, 0.0f }));
	meshCube->AddVertex(CVertex({ -fVertexfromOrigin, -fVertexfromOrigin, -fVertexfromOrigin }, { -1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f }));
	meshCube->AddVertex(CVertex({ -fVertexfromOrigin, -fVertexfromOrigin, fVertexfromOrigin }, { -1.0f, -1.0f, 1.0f }, { 0.0f, 1.0f }));

	// Right Face
	meshCube->AddVertex(CVertex({ fVertexfromOrigin, fVertexfromOrigin, -fVertexfromOrigin }, { 1.0f, 1.0f, -1.0f }, { 0.0f, 0.0f }));
	meshCube->AddVertex(CVertex({ fVertexfromOrigin, fVertexfromOrigin, fVertexfromOrigin }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f }));
	meshCube->AddVertex(CVertex({ fVertexfromOrigin, -fVertexfromOrigin, fVertexfromOrigin }, { 1.0f, -1.0f, 1.0f }, { 1.0f, 1.0f }));
	meshCube->AddVertex(CVertex({ fVertexfromOrigin, -fVertexfromOrigin, -fVertexfromOrigin }, { 1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f }));

	// Back Face
	meshCube->AddVertex(CVertex({ fVertexfromOrigin, fVertexfromOrigin, fVertexfromOrigin }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f }));
	meshCube->AddVertex(CVertex({ -fVertexfromOrigin, fVertexfromOrigin, fVertexfromOrigin }, { -1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f }));
	meshCube->AddVertex(CVertex({ -fVertexfromOrigin, -fVertexfromOrigin, fVertexfromOrigin }, { -1.0f, -1.0f, 1.0f }, { 1.0f, 1.0f }));
	meshCube->AddVertex(CVertex({ fVertexfromOrigin, -fVertexfromOrigin, fVertexfromOrigin }, { 1.0f, -1.0f, 1.0f }, { 0.0f, 1.0f }));

	// Top Face
	meshCube->AddVertex(CVertex({ -fVertexfromOrigin, fVertexfromOrigin, fVertexfromOrigin }, { -1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f }));
	meshCube->AddVertex(CVertex({ fVertexfromOrigin, fVertexfromOrigin, fVertexfromOrigin }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f }));
	meshCube->AddVertex(CVertex({ fVertexfromOrigin, fVertexfromOrigin, -fVertexfromOrigin }, { 1.0f, 1.0f, -1.0f }, { 1.0f, 1.0f }));
	meshCube->AddVertex(CVertex({ -fVertexfromOrigin, fVertexfromOrigin, -fVertexfromOrigin }, { -1.0f, 1.0f, -1.0f }, { 0.0f, 1.0f }));

	// Bottom Face
	meshCube->AddVertex(CVertex({ -fVertexfromOrigin, -fVertexfromOrigin, -fVertexfromOrigin }, { -1.0f, -1.0f, -1.0f }, { 0.0f, 0.0f }));
	meshCube->AddVertex(CVertex({ fVertexfromOrigin, -fVertexfromOrigin, -fVertexfromOrigin }, { 1.0f, -1.0f, -1.0f }, { 1.0f, 0.0f }));
	meshCube->AddVertex(CVertex({ fVertexfromOrigin, -fVertexfromOrigin, fVertexfromOrigin }, { 1.0f, -1.0f, 1.0f }, { 1.0f, 1.0f }));
	meshCube->AddVertex(CVertex({ -fVertexfromOrigin, -fVertexfromOrigin, fVertexfromOrigin }, { -1.0f, -1.0f, 1.0f }, { 0.0f, 1.0f }));

	// Set the Indice of the Cube in a Triangle List format
	std::vector<int> vecIndices = { 
		// Front Face
		0, 1, 2,
		0, 2, 3,

		// Left Face
		4, 5, 6,
		4, 6, 7,

		// Right Face
		8, 9, 10,
		8, 10, 11,

		// Back Face
		12, 13, 14,
		12, 14, 15,

		// Top Face
		16, 17, 18,
		16, 18, 19,

		// Bottom Face
		20, 21, 22,
		20, 22, 23
	};
	meshCube->AddIndices(vecIndices);

	meshCube->SetPrimitiveType(IGPT_TRIANGLELIST);

	// Create a Static buffer in the Renderer for this Mesh
	meshCube->CreateStaticBuffer();
	return meshCube;
}

void CMechanics_Client::UpdateAvatars()
{
	// Iterator to point at the found Avatar
	std::map<std::string, CAvatar*>::iterator iterAvatar;

	for (int i = 0; i < m_pServerPacket->iCurrentUserCount; i++)
	{
		// Retrieve the username from the current UserInfo
		AvatarInfo avatarInfo = (*m_pServerPacket).Avatars[i];
		std::string strUserName = (std::string)(avatarInfo.cUserName);
		iterAvatar = m_pAvatars->find(strUserName);

		if (iterAvatar != m_pAvatars->end())
		{
			iterAvatar->second->SetPosition({ avatarInfo.v3Pos.x, avatarInfo.v3Pos.y + kfAvatarSize, avatarInfo.v3Pos.z });
			iterAvatar->second->SetDirection({ avatarInfo.v3Dir.x, avatarInfo.v3Dir.y, avatarInfo.v3Dir.z });
			iterAvatar->second->SetHealth(avatarInfo.iHealth);
			iterAvatar->second->SetAliveStatus(avatarInfo.bAlive);
			iterAvatar->second->SetScore(avatarInfo.iScore);
			iterAvatar->second->SetFlareCount(avatarInfo.iFlareCount);
			iterAvatar->second->SetLives(avatarInfo.iLives);
			iterAvatar->second->m_bToggle = m_bToggle;

			v3float v3TorchPos = iterAvatar->second->GetDirection()->Normalise();
			v3TorchPos = *(iterAvatar->second->GetPosition()) - (v3TorchPos * 2.25f);

			m_pRenderer->UpdateSpotLight(iterAvatar->second->GetTorchID(), v3TorchPos, *(iterAvatar->second->GetDirection()));
		}
		else
		{
			StringToStruct(strUserName.c_str(), m_pServerPacket->cUserName, network::MAX_USERNAME_LENGTH);
			AddAvatar(m_pServerPacket);
		}
	}

	// Check if any Avatars was not updated
	iterAvatar = m_pAvatars->begin();
	std::string strID = "";
	while (iterAvatar != m_pAvatars->end())
	{
		// If a Avatars was not updated the server has deleted it
		if (iterAvatar->second->m_bToggle != m_bToggle)
		{
			// Set the ID of the Avatars to Delete
			strID = iterAvatar->first;
			delete iterAvatar->second;
			break;
		}

		iterAvatar++;
	}
	// Delete the Avatars from the map
	m_pAvatars->erase(strID);
}

void CMechanics_Client::UpdateProjectiles()
{
	if (m_pServerPacket->bCommand == false)
	{
		// Iterator to point at the found Projectile
		std::map<UINT, CProjectile*>::iterator iterProjectile;

		for (int i = 0; i < m_pServerPacket->iCurrentProjectileCount; i++)
		{
			// Retrieve the username from the current UserInfo
			ProjectileInfo projectileInfo = (*m_pServerPacket).Projectiles[i];
			iterProjectile = m_pProjectiles->find(projectileInfo.iID);

			if (iterProjectile != m_pProjectiles->end())
			{
				// Update all projectiles
				iterProjectile->second->SetPosition({ projectileInfo.v3Pos.x, projectileInfo.v3Pos.y + kfProjectileSize, projectileInfo.v3Pos.z });
				iterProjectile->second->SetDirection({ projectileInfo.v3Dir.x, projectileInfo.v3Dir.y, projectileInfo.v3Dir.z });
				iterProjectile->second->m_bToggle = m_bToggle;
			}
			else
			{
				// Create Packet didn't arrive. Create missed Projectile
				m_pServerPacket->projectileInfo = projectileInfo;
				SpawnProjectile(m_pServerPacket);
			}
		}

		// Check if any Projectile was not updated
		iterProjectile = m_pProjectiles->begin();
		UINT iID = 0;
		while (iterProjectile != m_pProjectiles->end())
		{
			// If a projectile was not updated the server has deleted it
			if (iterProjectile->second->m_bToggle != m_bToggle)
			{
				// Set the ID of the Projectile to Delete
				iID = iterProjectile->second->GetID();
				delete iterProjectile->second;
				break;
			}

			iterProjectile++;
		}
		// Delete the Projectile from the map
		m_pProjectiles->erase(iID);

	}
}

void CMechanics_Client::UpdateEnemies()
{
	// Iterator to point at the found Enemy
	std::map<UINT, CEnemy*>::iterator iterEnemy;
	
	for (int i = 0; i < m_pServerPacket->iCurrentEnemyCount; i++)
	{
		// Retrieve the Enemy from the current enemyInfo
		EnemyInfo enemyInfo = (*m_pServerPacket).Enemies[i];
		UINT iID = enemyInfo.iID;
		iterEnemy = m_pEnemies->find(iID);

		float fSize = 0.0f;
		switch (enemyInfo.eType)
		{
			case ET_DEMON:
			{
				fSize = kfDemonSize;
				break;
			}
			case ET_SENTINEL:
			{
				fSize = kfSentinelSize;
				break;
			}
			case ET_SHADOW:
			{
				fSize = kfShadowSize;
				break;
			}
		}	// Ende Switch

		if (iterEnemy != m_pEnemies->end())
		{
			// update the Enemy
			iterEnemy->second->SetPosition({ enemyInfo.v3Pos.x, enemyInfo.v3Pos.y + fSize, enemyInfo.v3Pos.z });
			iterEnemy->second->SetDirection({ enemyInfo.v3Dir.x, enemyInfo.v3Dir.y, enemyInfo.v3Dir.z });
			iterEnemy->second->m_bToggle = m_bToggle;
		}
		else
		{
			// Create Packet was not processed. Create the missed Enemy
			m_pServerPacket->enemyInfo = enemyInfo;
			SpawnEnemy(m_pServerPacket);
		}
	}

	// Check if any Enemies was not updated
	iterEnemy = m_pEnemies->begin();
	UINT iID = 0;
	while (iterEnemy != m_pEnemies->end())
	{
		// If a Enemies was not updated the server has deleted it
		if (iterEnemy->second->m_bToggle != m_bToggle)
		{
			// Set the ID of the Enemies to Delete
			iID = iterEnemy->second->GetID();
			delete iterEnemy->second;
			break;
		}

		iterEnemy++;
	}
	// Delete the Enemies from the map
	m_pEnemies->erase(iID);
}

void CMechanics_Client::UpdatePowerUps()
{
	// Iterator to point at the found Enemy
	std::map<UINT, CPowerUp*>::iterator iterPowerUp;

	for (int i = 0; i < m_pServerPacket->iCurrentPowerUpCount; i++)
	{
		// Retrieve the Enemy from the current enemyInfo
		PowerUpInfo powerInfo = (*m_pServerPacket).PowerUps[i];
		UINT iID = powerInfo.iID;
		iterPowerUp = m_pPowerUps->find(iID);

		if (iterPowerUp != m_pPowerUps->end())
		{
			// Update the Enemy
			iterPowerUp->second->SetPosition({ powerInfo.v3Pos.x, powerInfo.v3Pos.y + kfPowerUpSize, powerInfo.v3Pos.z });
			iterPowerUp->second->SetDirection({ powerInfo.v3Dir.x, powerInfo.v3Dir.y, powerInfo.v3Dir.z });
			iterPowerUp->second->m_bToggle = m_bToggle;
		}
		else
		{
			// Create Packet was not processed. Create the missed Enemy
			m_pServerPacket->powerInfo = powerInfo;
			SpawnPowerUp(m_pServerPacket);
		}
	}

	// Check if any Enemies was not updated
	iterPowerUp = m_pPowerUps->begin();
	UINT iID = 0;
	while (iterPowerUp != m_pPowerUps->end())
	{
		// If a Enemies was not updated the server has deleted it
		if (iterPowerUp->second->m_bToggle != m_bToggle)
		{
			// Set the ID of the Enemies to Delete
			iID = iterPowerUp->second->GetID();
			delete iterPowerUp->second;
			break;
		}

		iterPowerUp++;
	}
	// Delete the Enemies from the map
	m_pPowerUps->erase(iID);
}

void CMechanics_Client::UpdateFlare()
{
	// Check if the Server has an active flare
	if (m_pServerPacket->Flare.bActive == true)
	{
		// Check if the flare is the same one as the Flare Object
		if (m_pServerPacket->Flare.iID != m_pFlare->GetID())
		{
			// Reactivate a new flare
			m_pFlare->ReactivateFlare(m_pServerPacket->Flare.v3Pos, m_pServerPacket->Flare.fRange, m_pServerPacket->Flare.iID);
		}
		else
		{
			// Update the current flare
			m_pFlare->UpdateFlareLight(m_pServerPacket->Flare.v3Pos, m_pServerPacket->Flare.fRange, m_fDT);
		}
	}
	else
	{
		// Flare light is set to off
		m_pFlare->SetActive(false);
	}
}

void CMechanics_Client::AddAvatar(ServerToClient* _pServerPacket)
{
	AvatarInfo currentAvatarInfo;
	// Temporarily store the user data 
	for (int i = 0; i < _pServerPacket->iCurrentUserCount; i++)
	{
		if ((std::string)(_pServerPacket->cUserName) == (std::string)(_pServerPacket->Avatars[i].cUserName))
		{
			currentAvatarInfo = _pServerPacket->Avatars[i];
		}
	}

	// Create a new avatar object
	CAvatar* pTempAvatar = new  CAvatar(m_pRenderer);
	pTempAvatar->Initialise(m_bToggle, m_pRenderer, m_pAvatarMesh, currentAvatarInfo.iID, m_iAvatarMaterialID, currentAvatarInfo.v3Pos);

	// Save the avatar in a map
	std::pair < std::map < std::string, CAvatar* > ::iterator, bool> pairCreateCheck;
	std::pair<std::string, CAvatar*> pairAvatar((std::string)(currentAvatarInfo.cUserName), pTempAvatar);
	pairCreateCheck = m_pAvatars->insert(pairAvatar);

	// Check if the New PowerUp was successfully inserted
	if (pairCreateCheck.second == false)
	{
		// Delete the Temp PowerUp if the insertion failed
		delete pTempAvatar;
		pTempAvatar = 0;
		return;
	}

	if ((std::string)(_pServerPacket->cUserName) == m_strUserName)
	{
		// Create and inititalise the Camera for the Game
		m_pCamera = new CStaticCamera();
		std::map< std::string, CAvatar*>::iterator Avatar = m_pAvatars->find(m_strUserName);
		v3float v3Pos = *(Avatar->second->GetPosition());
		m_pCamera->Initialise({ v3Pos.x, 100, v3Pos.z }, { 0, -1, 0 }, true);
		m_pCamera->Process(m_pRenderer);
	}
}

void CMechanics_Client::AddAllAvatars(ServerToClient* _pServerPacket)
{
	AvatarInfo currentAvatarInfo;
	//Temporarily store the user data 
	for (int i = 0; i < _pServerPacket->iCurrentUserCount; i++)
	{
		currentAvatarInfo = _pServerPacket->Avatars[i];
		StringToStruct(currentAvatarInfo.cUserName, _pServerPacket->cUserName, network::MAX_USERNAME_LENGTH);
		AddAvatar(_pServerPacket);
	}
}

void CMechanics_Client::RemoveAvatar(std::string _strUserName)
{
	std::map<std::string, CAvatar*>::iterator iterAvatar;
	iterAvatar = m_pAvatars->find(_strUserName);

	m_pRenderer->LightEnable(iterAvatar->second->GetTorchID(), false);

	delete iterAvatar->second;
	iterAvatar->second = 0;

	m_pAvatars->erase(_strUserName);
}

void CMechanics_Client::CreateAvatarAsset()
{
	// Create a material for the avatars to be made from
	MaterialComposition AvatarMatComp;
	AvatarMatComp.ambient = { 1.0f, 1.0f, 1.0f, 1.0f };
	AvatarMatComp.diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	AvatarMatComp.emissive = { 0.0f, 0.0f, 0.0f, 0.0f };
	AvatarMatComp.specular = { 0.0f, 0.0f, 0.0f, 0.0f };
	AvatarMatComp.power = 0;
	m_iAvatarMaterialID = m_pRenderer->CreateMaterial(AvatarMatComp);

	// Avatar Mesh and Texture
	m_iAvatarTexID = m_pRenderer->CreateTexture("Assets//Avatar.bmp");
	m_pAvatarMesh = CreateCubeMesh(kfAvatarSize, m_iAvatarTexID);

	assert(("Avatar Mesh Failed to Create", m_pAvatarMesh != 0));
}

void CMechanics_Client::CreateProjectileAsset()
{
	// Create a material for the avatars to be made from
	MaterialComposition ProjectileMatComp;
	ProjectileMatComp.ambient = { 1.0f, 1.0f, 1.0f, 1.0f };
	ProjectileMatComp.diffuse = { 0.0f, 0.0f, 1.0f, 1.0f };
	ProjectileMatComp.emissive = { 0.0f, 0.0f, 0.0f, 0.0f };
	ProjectileMatComp.specular = { 0.0f, 0.0f, 0.0f, 0.0f };
	ProjectileMatComp.power = 0;
	m_iProjectileMaterialID = m_pRenderer->CreateMaterial(ProjectileMatComp);

	// Avatar Mesh and Texture
	m_iProjectileTexID = m_pRenderer->CreateTexture("Assets//Projectile.bmp");
	m_pProjectileMesh = CreateCubeMesh(kfProjectileSize, m_iProjectileTexID);

	assert(("Projectile Mesh Failed to Create", m_pProjectileMesh != 0));
}

void CMechanics_Client::CreateFlareAsset()
{
	// Create the Material for the Demons to use
	MaterialComposition FlareMatComp;
	FlareMatComp.ambient = { 0.0f, 0.0f, 0.0f, 0.0f };
	FlareMatComp.diffuse = { 1.0f, 1.0f, 1.0f, 0.0f };
	FlareMatComp.emissive = { 1.0f, 1.0f, 0.0f, 0.0f };
	FlareMatComp.specular = { 1.0f, 1.0f, 0.0f, 1.0f };
	FlareMatComp.power = 100.0f;
	m_iFlareMaterialID = m_pRenderer->CreateMaterial(FlareMatComp);

	// Flare Enemy Mesh and Texture
	m_iFlareTexID = m_pRenderer->CreateTexture("Assets//Flare.bmp");
	m_pFlareMesh = CreateCubeMesh(kfFlareSize, m_iFlareTexID);

	m_pFlare = new CFlare(m_pRenderer);
	m_pFlare->Initialise(m_bToggle, m_pRenderer, m_pFlareMesh, 0, m_iFlareMaterialID, { 0.0f, 1.0f, 0.0f });

	assert(("Flare Mesh Failed to Create", m_pFlareMesh != 0));
}

void CMechanics_Client::CreateDemonAsset()
{
	// Create the Material for the Demons to use
	MaterialComposition DemonMatComp;
	DemonMatComp.ambient = { 0.0f, 0.0f, 0.0f, 0.0f };
	DemonMatComp.diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	DemonMatComp.emissive = { 0.0f, 0.0f, 0.0f, 0.0f };
	DemonMatComp.specular = { 0.0f, 0.0f, 0.0f, 0.0f };
	DemonMatComp.power = 0;
	m_iDemonMaterialID = m_pRenderer->CreateMaterial(DemonMatComp);

	// Demon Enemy Mesh and Texture
	m_iDemonTexID = m_pRenderer->CreateTexture("Assets//Demon.bmp");
	m_pDemonMesh = CreateCubeMesh(kfDemonSize, m_iDemonTexID);

	assert(("Demon Enemy Mesh Failed to Create", m_pDemonMesh != 0));
}

void CMechanics_Client::CreateSentinelAsset()
{
	// Create the Material for the Sentinels to use
	MaterialComposition SentinelMatComp;
	SentinelMatComp.ambient = { 0.0f, 0.0f, 0.0f, 0.0f };
	SentinelMatComp.diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	SentinelMatComp.emissive = { 0.0f, 0.0f, 0.0f, 0.0f };
	SentinelMatComp.specular = { 0.0f, 0.0f, 0.0f, 0.0f };
	SentinelMatComp.power = 0;
	m_iSentinelMaterialID = m_pRenderer->CreateMaterial(SentinelMatComp);

	// Sentinel Enemy Mesh and Texture
	m_iSentinelTexID = m_pRenderer->CreateTexture("Assets//Sentinel.bmp");
	m_pSentinelMesh = CreateCubeMesh(kfSentinelSize, m_iSentinelTexID);

	assert(("Sentinel Enemy Mesh Failed to Create", m_pSentinelMesh != 0));
}

void CMechanics_Client::CreateShadowAsset()
{
	// Create the Material for the Shadows to use
	MaterialComposition ShadowMatComp;
	ShadowMatComp.ambient = { 0.0f, 0.0f, 0.0f, 0.0f };
	ShadowMatComp.diffuse = { 1.0f, 1.0f, 1.0f, 0.0f };
	ShadowMatComp.emissive = { 0.0f, 0.0f, 0.0f, 0.0f };
	ShadowMatComp.specular = { 0.0f, 0.0f, 0.0f, 0.0f };
	ShadowMatComp.power = 0;
	m_iShadowMaterialID = m_pRenderer->CreateMaterial(ShadowMatComp);

	// Shadow Enemy Mesh and Texture
	m_iShadowTexID = m_pRenderer->CreateTexture("Assets//Shadow.bmp");
	m_pShadowMesh = CreateCubeMesh(kfShadowSize, m_iShadowTexID);

	assert(("Shadow Enemy Mesh Failed to Create", m_pShadowMesh != 0));
}

void CMechanics_Client::CreateHealthPowerAsset()
{
	// Create the Material for the Demons to use
	MaterialComposition HealthMatComp;
	HealthMatComp.ambient = { 0.0f, 0.0f, 0.0f, 0.0f };
	HealthMatComp.diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	HealthMatComp.emissive = { 1.0f, 1.0f, 1.0f, 0.0f };
	HealthMatComp.specular = { 0.0f, 0.0f, 0.0f, 0.0f };
	HealthMatComp.power = 0;
	m_iHealthPowerMaterialID = m_pRenderer->CreateMaterial(HealthMatComp);

	// Demon Enemy Mesh and Texture
	m_iHealthPowerTexID = m_pRenderer->CreateTexture("Assets//HealthPower.bmp");
	m_pHealthPowerMesh = CreateCubeMesh(kfPowerUpSize, m_iHealthPowerTexID);

	assert(("Health PowerUp Mesh Failed to Create", m_pHealthPowerMesh != 0));
}

void CMechanics_Client::CreateFlarePowerAsset()
{
	// Create the Material for the Demons to use
	MaterialComposition FlareMatComp;
	FlareMatComp.ambient = { 0.0f, 0.0f, 0.0f, 0.0f };
	FlareMatComp.diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	FlareMatComp.emissive = { 1.0f, 1.0f, 1.0f, 0.0f };
	FlareMatComp.specular = { 0.0f, 0.0f, 0.0f, 0.0f };
	FlareMatComp.power = 0;
	m_iFlarePowerMaterialID = m_pRenderer->CreateMaterial(FlareMatComp);

	// Demon Enemy Mesh and Texture
	m_iFlarePowerTexID = m_pRenderer->CreateTexture("Assets//FlarePower.bmp");
	m_pFlarePowerMesh = CreateCubeMesh(kfPowerUpSize, m_iFlarePowerTexID);

	assert(("Flare PowerUp Mesh Failed to Create", m_pFlarePowerMesh != 0));
}

void CMechanics_Client::CreateGoldenPowerAsset()
{
	// Create the Material for the Demons to use
	MaterialComposition GoldenMatComp;
	GoldenMatComp.ambient = { 0.0f, 0.0f, 0.0f, 0.0f };
	GoldenMatComp.diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	GoldenMatComp.emissive = { 1.0f, 1.0f, 1.0f, 0.0f };
	GoldenMatComp.specular = { 0.0f, 0.0f, 0.0f, 0.0f };
	GoldenMatComp.power = 0;
	m_iGoldenPowerMaterialID = m_pRenderer->CreateMaterial(GoldenMatComp);

	// Demon Enemy Mesh and Texture
	m_iGoldenPowerTexID = m_pRenderer->CreateTexture("Assets//GoldenPower.bmp");
	m_pGoldenPowerMesh = CreateCubeMesh(kfPowerUpSize, m_iGoldenPowerTexID);

	assert(("Golden PowerUp Mesh Failed to Create", m_pGoldenPowerMesh != 0));
}



void CMechanics_Client::SpawnProjectile(ServerToClient* _pServerPacket)
{
	// Copy the info from the Packet
	ProjectileInfo projectileInfo = _pServerPacket->projectileInfo;

	// Create a new Projectile using the information from the Packet
	CProjectile* tempProjectile = new CProjectile((std::string)projectileInfo.cUserName, projectileInfo.v3Dir, projectileInfo.iDamage);
	tempProjectile->Initialise(m_bToggle, m_pRenderer, m_pProjectileMesh, projectileInfo.iID, m_iProjectileMaterialID, projectileInfo.v3Pos);
	tempProjectile->SetDirection(projectileInfo.v3Dir);

	// Add the new Projectile to the Map
	std::pair < std::map < UINT, CProjectile* > ::iterator, bool> pairCreateCheck;
	std::pair<UINT, CProjectile*> newProjectile(projectileInfo.iID, tempProjectile);
	pairCreateCheck = m_pProjectiles->insert(newProjectile);

	// Check if the New Projectile was successfully inserted
	if (pairCreateCheck.second == false)
	{
		// Delete the Temp Projectile if the insertion failed
		delete tempProjectile;
		tempProjectile = 0;
	}
}

void CMechanics_Client::DeleteProjectile(ServerToClient* _pServerPacket)
{
	UINT iID = _pServerPacket->projectileInfo.iID;

	// Delete the Projectile using its ID
	std::map<UINT, CProjectile*>::iterator Projectile = m_pProjectiles->find(iID);

	if (Projectile != m_pProjectiles->end())
	{
		delete Projectile->second;

		// Erase the Projectile using its ID
		m_pProjectiles->erase(iID);
	}
}

void CMechanics_Client::SpawnEnemy(ServerToClient* _pServerPacket)
{
	EnemyInfo enemyInfo = _pServerPacket->enemyInfo;
	CMesh* pMesh = 0;
	int iMatID;

	switch (enemyInfo.eType)
	{
	case ET_DEMON:
	{
		pMesh = m_pDemonMesh;
		iMatID = m_iDemonMaterialID;
		break;
	}
	case ET_SENTINEL:
	{
		pMesh = m_pSentinelMesh;
		iMatID = m_iSentinelMaterialID;
		break;
	}
	case ET_SHADOW:
	{
		pMesh = m_pShadowMesh;
		iMatID = m_iShadowMaterialID;
		break;
	}
	default:
		break;
	}

	CEnemy* tempEnemy = new CEnemy(enemyInfo.eType);
	tempEnemy->Initialise(m_bToggle, m_pRenderer, pMesh, enemyInfo.iID, iMatID, enemyInfo.v3Pos);
	tempEnemy->SetDirection(enemyInfo.v3Dir);

	std::pair < std::map < UINT, CEnemy* > ::iterator, bool> pairCreateCheck;
	std::pair<UINT, CEnemy*> newEnemy(enemyInfo.iID, tempEnemy);
	pairCreateCheck = m_pEnemies->insert(newEnemy);

	// Check if the New Enemy was successfully inserted
	if (pairCreateCheck.second == false)
	{
		// Delete the Temp Enemy if the insertion failed
		delete tempEnemy;
		tempEnemy = 0;
	}
}

void CMechanics_Client::DeleteEnemy(ServerToClient* _pServerPacket)
{
	UINT iID = _pServerPacket->enemyInfo.iID;

	// Delete the Enemy using its ID
	std::map<UINT, CEnemy*>::iterator Enemy = m_pEnemies->find(iID);

	if (Enemy != m_pEnemies->end())
	{
		delete Enemy->second;

		// Erase the Enemy using its ID
		m_pEnemies->erase(iID);
	}

	
}

void CMechanics_Client::SpawnPowerUp(ServerToClient* _pServerPacket)
{
	PowerUpInfo powerInfo = _pServerPacket->powerInfo;
	CMesh* pMesh = 0;
	int iMatID;

	switch (powerInfo.eType)
	{
	case PT_HEALTH:
	{
		pMesh = m_pHealthPowerMesh;
		iMatID = m_iHealthPowerMaterialID;
		break;
	}
	case PT_FLARE:
	{
		pMesh = m_pFlarePowerMesh;
		iMatID = m_iFlarePowerMaterialID;
		break;
	}
	case PT_GOLDEN:
	{
		pMesh = m_pGoldenPowerMesh;
		iMatID = m_iGoldenPowerMaterialID;
		break;
	}
	default:
		break;
	}

	CPowerUp* tempPower = new CPowerUp(powerInfo.eType);
	tempPower->Initialise(m_bToggle, m_pRenderer, pMesh, powerInfo.iID, iMatID, powerInfo.v3Pos);
	tempPower->SetDirection(powerInfo.v3Dir);

	std::pair < std::map < UINT, CPowerUp* > ::iterator, bool> pairCreateCheck;
	std::pair<UINT, CPowerUp*> newPower(powerInfo.iID, tempPower);
	pairCreateCheck = m_pPowerUps->insert(newPower);

	// Check if the New PowerUp was successfully inserted
	if (pairCreateCheck.second == false)
	{
		// Delete the Temp PowerUp if the insertion failed
		delete tempPower;
		tempPower = 0;
	}
}

void CMechanics_Client::DeletePowerUp(ServerToClient* _pServerPacket)
{
	UINT iID = _pServerPacket->powerInfo.iID;

	// Delete the PowerUp using its ID
	std::map<UINT, CPowerUp*>::iterator PowerUp = m_pPowerUps->find(iID);

	if (PowerUp != m_pPowerUps->end())
	{
		delete PowerUp->second;

		// Erase the Enemy using its ID
		m_pPowerUps->erase(iID);
	}
}

void CMechanics_Client::OverlayHUD()
{
	std::map<std::string, CAvatar*>::iterator Avatar = m_pAvatars->find(m_strUserName);

	if (Avatar != m_pAvatars->end())
	{
		int iYpos = 0;
		D3DXCOLOR textBlue = 0xff0000ff;

		// Check the health status
		std::string strHealth;
		if (Avatar->second->GetHealth() <= 0)
		{
			strHealth = "Dead";
		}
		else
		{
			strHealth = std::to_string(Avatar->second->GetHealth());
		}

		// Render the text to the Top left Corner
		m_pRenderer->RenderText(false, { 0, 0 }, "Health: " + strHealth, (iYpos += 10), SCREEN_FONT, textBlue, H_LEFT);

		if (m_bSinglePlayer == true)
		{
			m_pRenderer->RenderText(false, { 0, 0 }, "Lives: " + std::to_string(Avatar->second->GetLives()), (iYpos += 14), SCREEN_FONT, textBlue, H_LEFT);
		}

		m_pRenderer->RenderText(false, { 0, 0 }, "Score: " + std::to_string(Avatar->second->GetScore()), (iYpos += 14), SCREEN_FONT, textBlue, H_LEFT);
		m_pRenderer->RenderText(false, { 0, 0 }, "Flares: " + std::to_string(Avatar->second->GetFlareCount()), (iYpos += 14), SCREEN_FONT, textBlue, H_LEFT);
	}
}

void CMechanics_Client::OverlayAvatarScores()
{
	int iYpos = 300;
	D3DXCOLOR textBlue = 0xff0000ff;

	m_pRenderer->RenderText(false, { 0, 0 }, "Avatar", (iYpos), MENU_FONT, textBlue, H_LEFT);
	m_pRenderer->RenderText(false, { 0, 0 }, "Health", (iYpos), MENU_FONT, textBlue, H_CENTER);
	m_pRenderer->RenderText(false, { 0, 0 }, "Score", (iYpos), MENU_FONT, textBlue, H_RIGHT);
	iYpos += 50;

	std::map<std::string, CAvatar*>::iterator Avatar = m_pAvatars->begin();

	while (Avatar != m_pAvatars->end())
	{
		// Check the health status
		std::string strHealth;
		if (Avatar->second->GetHealth() <= 0)
		{
			strHealth = "Dead";
		}
		else
		{
			strHealth = std::to_string(Avatar->second->GetHealth());
		}

		m_pRenderer->RenderText(false, { 0, 0 }, (Avatar->first), (iYpos), MENU_FONT, textBlue, H_LEFT);
		m_pRenderer->RenderText(false, { 0, 0 }, strHealth, (iYpos), MENU_FONT, textBlue, H_CENTER);
		m_pRenderer->RenderText(false, { 0, 0 }, std::to_string(Avatar->second->GetScore()), (iYpos), MENU_FONT, textBlue, H_RIGHT);
		iYpos += 50;

		Avatar++;
	}
}

void CMechanics_Client::OverlayPauseScreen(std::string* _strMenuInput, POINT _ptMouse, bool _bAction)
{
	int iYpos = 0;
	D3DXCOLOR colorRed = 0xffff0000;
	D3DXCOLOR colorWhite = 0xffffffff;
	std::string strSelection = "";

	// Print the Title Text
	m_pRenderer->RenderText(false, _ptMouse, "ROBOTRON", (iYpos += 50), SUBTITLE_FONT, 0xff450000, H_CENTER);
	m_pRenderer->RenderText(false, _ptMouse, "HORDES", (iYpos += 80), TITLE_FONT, 0xff650000, H_CENTER);
	m_pRenderer->RenderText(false, _ptMouse, "OF", (iYpos += 100), TITLE_FONT, 0xff7c0000, H_CENTER);
	m_pRenderer->RenderText(false, _ptMouse, "HELL", (iYpos += 100), TITLE_FONT, 0xffa50000, H_CENTER);

	m_pRenderer->RenderText(false, _ptMouse, "Pause Menu", (iYpos += 150), SUBTITLE_FONT, colorRed, H_CENTER);

	// Print the Menu Options
	strSelection += m_pRenderer->RenderText(true, _ptMouse, "Resume", (iYpos += 120), MENU_FONT, colorWhite, H_CENTER);
	strSelection += m_pRenderer->RenderText(true, _ptMouse, "Options", (iYpos += 60), MENU_FONT, colorWhite, H_CENTER);
	strSelection += m_pRenderer->RenderText(true, _ptMouse, "Exit Game", (iYpos += 60), MENU_FONT, colorWhite, H_CENTER);

	// Change the MenuSelection Variable to the menu enum
	if (_bAction == true)
	{
		*_strMenuInput = strSelection;
	}
	else
	{
		*_strMenuInput = "";
	}
}

void CMechanics_Client::OverlayGameOver(std::string* _strMenuInput, POINT _ptMouse, bool _bAction)
{
	int iYpos = 0;
	D3DXCOLOR colorRed = 0xffff0000;
	D3DXCOLOR colorWhite = 0xffffffff;
	std::string strSelection = "";

	// Print the Title Text
	m_pRenderer->RenderText(false, _ptMouse, "ROBOTRON", (iYpos += 50), SUBTITLE_FONT, 0xff450000, H_CENTER);
	m_pRenderer->RenderText(false, _ptMouse, "HORDES", (iYpos += 80), TITLE_FONT, 0xff650000, H_CENTER);
	m_pRenderer->RenderText(false, _ptMouse, "OF", (iYpos += 100), TITLE_FONT, 0xff7c0000, H_CENTER);
	m_pRenderer->RenderText(false, _ptMouse, "HELL", (iYpos += 100), TITLE_FONT, 0xffa50000, H_CENTER);

	m_pRenderer->RenderText(false, _ptMouse, "Game Over", (iYpos += 150), SUBTITLE_FONT, colorRed, H_CENTER);

	// Print the Menu Options
	strSelection += m_pRenderer->RenderText(true, _ptMouse, "Exit To Main Menu", (iYpos += 200), MENU_FONT, colorWhite, H_CENTER);
	
	// Change the MenuSelection Variable to the menu enum
	if (_bAction == true)
	{
		*_strMenuInput = strSelection;
	}
	else
	{
		*_strMenuInput = "";
	}
}

bool CMechanics_Client::CheckAvatarsAliveStatus()
{
	std::map < std::string, CAvatar* >::iterator iterAvatar = m_pAvatars->begin();
	while (iterAvatar != m_pAvatars->end())
	{
		if (iterAvatar->second->GetAliveStatus() == true)
		{
			// return true if one person or more is alive
			return true;
		}

		iterAvatar++;
	}

	// return false if no one is alive
	return false;
}