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
	delete m_pDemonMesh;
	m_pDemonMesh = 0;
	delete m_pSentinelMesh;
	m_pSentinelMesh = 0;
	delete m_pHealthMesh;
	m_pHealthMesh = 0;
	delete m_pFlareMesh;
	m_pFlareMesh = 0;


	// Delete the Graphics Objects
	delete m_pCamera;
	m_pCamera = 0;
	delete m_pTerrain;
	m_pTerrain = 0;
	delete m_pFlare;
	m_pFlare = 0;
}

bool CMechanics_Client::Initialise(IRenderer* _pRenderer, std::string _strUserName)
{
	// Store the pointer to the renderer
	m_pRenderer = _pRenderer;
	m_strUserName = _strUserName;

	// Create the initial Projection Matrices and set them on the Renderer
	m_pRenderer->CalculateProjectionMatrix(D3DXToRadian(45), 0.1f, 10000.0f);

	// Create a Terrain for the Game
	m_pTerrain = new CTerrain();
	VertexScalar TerrainScalar = { 0.2f, 0.0f, 0.2f };
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
	CreateDemonAsset();
	CreateSentinelAsset();
	CreateHealthAsset();
	CreateFlareAsset();

	return true;
}

void CMechanics_Client::Process( float _fDT, ServerToClient* _pServerPacket)
{
	// save the current states of the delta tick and the Packet from the server
	m_fDT = _fDT;
	m_pServerPacket = _pServerPacket;

	UpdateAvatars();
	UpdateProjectiles();
	UpdateEnemies();
	UpdateFlare();

	// Process the camera to keep it following the Avatar
	std::map<std::string, CAvatar*>::iterator Avatar = m_pAvatars->find(m_strUserName);
	v3float v3Pos = *(Avatar->second->GetPosition());
	m_pCamera->SetCamera({ v3Pos.x, v3Pos.y, v3Pos.z }, { v3Pos.x, 50, v3Pos.z}, { 0, 0, 1 }, { 0, -1, 0 });
	m_pCamera->Process(m_pRenderer);
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
		iterAvatar->second->Draw();
		iterAvatar++;
	}

	// Draw all the Projectiles
	std::map<UINT, CProjectile*>::iterator iterProjectile = m_pProjectiles->begin();
	while (iterProjectile != m_pProjectiles->end())
	{
		iterProjectile->second->Draw();
		iterProjectile++;
	}

	// Draw all the Enemies
	std::map<UINT, CEnemy*>::iterator iterEnemy= m_pEnemies->begin();
	while (iterEnemy != m_pEnemies->end())
	{
		iterEnemy->second->Draw();
		iterEnemy++;
	}

	// Draw all the Enemies
	if (m_pFlare->GetActive() == true)
	{
		m_pFlare->Draw();
	}

	// Render the HUD
	RenderHUD();
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
			iterAvatar->second->SetPosition({ avatarInfo.v3Pos.x, avatarInfo.v3Pos.y, avatarInfo.v3Pos.z });
			iterAvatar->second->SetDirection({ avatarInfo.v3Dir.x, avatarInfo.v3Dir.y, avatarInfo.v3Dir.z });
			iterAvatar->second->SetHealth(avatarInfo.iHealth);
			iterAvatar->second->SetScore(avatarInfo.iScore);

			m_pRenderer->UpdateSpotLight(iterAvatar->second->GetTorchID(), *(iterAvatar->second->GetPosition()), *(iterAvatar->second->GetDirection()));
		}
	}
}

void CMechanics_Client::UpdateProjectiles()
{
	if (m_pServerPacket->bCommand == false)
	{
		// Iterator to point at the found Projectile
		std::map<UINT, CProjectile*>::iterator iterProjectile;

		int iDifference = m_pProjectiles->size() - m_pServerPacket->iCurrentProjectileCount;
		if (iDifference > 0)
		{
			// Delete any extra Projectiles
			for (int i = 0; i < iDifference; i++)
			{
				delete m_pProjectiles->begin()->second;
				m_pProjectiles->erase(m_pProjectiles->begin(), ++m_pProjectiles->begin());
			}
		}

		for (int i = 0; i < m_pServerPacket->iCurrentProjectileCount; i++)
		{
			// Retrieve the username from the current UserInfo
			ProjectileInfo projectileInfo = (*m_pServerPacket).Projectiles[i];
			std::string strUserName = (std::string)(projectileInfo.cUserName);
			iterProjectile = m_pProjectiles->find(projectileInfo.iID);

			if (iterProjectile != m_pProjectiles->end())
			{
				//If the Projectile was not updated from the last frame the Projectile should be deleted
				//if (*(iterProjectile->second->GetPosition()) == projectileInfo.v3Pos)
				//{
				//	delete iterProjectile->second;
				//	m_pProjectiles->erase(projectileInfo.iID);
				//}
				//else
				//{
					iterProjectile->second->SetPosition({ projectileInfo.v3Pos.x, projectileInfo.v3Pos.y, projectileInfo.v3Pos.z });
					iterProjectile->second->SetDirection({ projectileInfo.v3Dir.x, projectileInfo.v3Dir.y, projectileInfo.v3Dir.z });
				//}
			}

			
		}
	}
}

void CMechanics_Client::UpdateEnemies()
{
	// Iterator to point at the found Enemy
	std::map<UINT, CEnemy*>::iterator iterEnemy;
	
	for (int i = 0; i < m_pServerPacket->iCurrentEnemyCount; i++)
	{
		// Retrieve the username from the current UserInfo
		EnemyInfo enemyInfo = (*m_pServerPacket).Enemies[i];
		UINT iID = enemyInfo.iID;
		iterEnemy = m_pEnemies->find(iID);

		if (iterEnemy != m_pEnemies->end())
		{
			iterEnemy->second->SetPosition({ enemyInfo.v3Pos.x, enemyInfo.v3Pos.y, enemyInfo.v3Pos.z });
			iterEnemy->second->SetDirection({ enemyInfo.v3Dir.x, enemyInfo.v3Dir.y, enemyInfo.v3Dir.z });
		}
	}
}

void CMechanics_Client::UpdateFlare()
{
	if (m_pServerPacket->Flare.bActive == true)
	{
		if (m_pServerPacket->Flare.iID != m_pFlare->GetID())
		{
			m_pFlare->ReactivateFlare(m_pServerPacket->Flare.v3Pos, m_pServerPacket->Flare.fRange, m_pServerPacket->Flare.iID);
		}
		else
		{
			m_pFlare->UpdateFlareLight(m_pServerPacket->Flare.v3Pos, m_pServerPacket->Flare.fRange, m_fDT);
		}
	}
	else
	{
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
	pTempAvatar->Initialise(m_pRenderer, m_pAvatarMesh, currentAvatarInfo.iID, m_iAvatarMaterialID, currentAvatarInfo.v3Pos);

	// Save the avatar in a vector
	std::pair<std::string, CAvatar*> pairAvatar((std::string)(currentAvatarInfo.cUserName), pTempAvatar);
	m_pAvatars->insert(pairAvatar);

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
	// Temporarily store the user data 
	for (int i = 0; i < _pServerPacket->iCurrentUserCount; i++)
	{
		currentAvatarInfo = _pServerPacket->Avatars[i];

		// Create a new avatar object
		CAvatar* pTempAvatar = new  CAvatar(m_pRenderer);
		v3float v3Pos = { currentAvatarInfo.v3Pos.x, currentAvatarInfo.v3Pos.y, currentAvatarInfo.v3Pos.z };
		pTempAvatar->Initialise(m_pRenderer, m_pAvatarMesh, currentAvatarInfo.iID, m_iAvatarMaterialID, v3Pos);

		// Save the avatar in a vector
		std::pair<std::string, CAvatar*> pairAvatar((std::string)(currentAvatarInfo.cUserName), pTempAvatar);
		m_pAvatars->insert(pairAvatar);

		if ((std::string)(currentAvatarInfo.cUserName) == m_strUserName)
		{
			// Create and inititalise the Camera for the Game
			m_pCamera = new CStaticCamera();
			std::map< std::string, CAvatar*>::iterator Avatar = m_pAvatars->find(m_strUserName);
			v3float v3Pos = *(Avatar->second->GetPosition());
			m_pCamera->Initialise({ v3Pos.x, 100, v3Pos.z }, { 0, -1, 0 }, true);
			m_pCamera->Process(m_pRenderer);
		}
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
	m_iAvatarTexID = m_pRenderer->CreateTexture("Assets//Crate Side.bmp");
	m_pAvatarMesh = CreateCubeMesh(kfAvatarSize, m_iAvatarTexID);
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
}

void CMechanics_Client::CreateHealthAsset()
{
	// Create the Material for the Demons to use
	MaterialComposition HealthMatComp;
	HealthMatComp.ambient = { 0.0f, 0.0f, 0.0f, 0.0f };
	HealthMatComp.diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	HealthMatComp.emissive = { 0.0f, 0.0f, 0.0f, 0.0f };
	HealthMatComp.specular = { 0.0f, 0.0f, 0.0f, 0.0f };
	HealthMatComp.power = 0;
	m_iHealthMaterialID = m_pRenderer->CreateMaterial(HealthMatComp);

	// Demon Enemy Mesh and Texture
	m_iHealthTexID = m_pRenderer->CreateTexture("Assets//Health.bmp");
	m_pHealthMesh = CreateCubeMesh(kfPowerUpSize, m_iHealthTexID);
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
	m_pFlare->Initialise(m_pRenderer, m_pFlareMesh, 0, m_iFlareMaterialID, { 0.0f, 1.0f, 0.0f });

}

void CMechanics_Client::SpawnProjectile(ServerToClient* _pServerPacket)
{
	// Copy the info from the Packet
	ProjectileInfo projectileInfo = _pServerPacket->projectileInfo;

	// Create a new Projectile using the information from the Packet
	CProjectile* tempProjectile = new CProjectile((std::string)projectileInfo.cUserName, projectileInfo.v3Dir, projectileInfo.iDamage);
	tempProjectile->Initialise(m_pRenderer, m_pProjectileMesh, projectileInfo.iID, m_iProjectileMaterialID, projectileInfo.v3Pos);
	tempProjectile->SetDirection(projectileInfo.v3Dir);

	// Add the new Porjectile to the Map
	std::pair<UINT, CProjectile*> newProjectile(projectileInfo.iID, tempProjectile);
	m_pProjectiles->insert(newProjectile);
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
	default:
		break;
	}

	CEnemy* tempEnemy = new CEnemy(enemyInfo.eType);
	tempEnemy->Initialise(m_pRenderer, pMesh, enemyInfo.iID, iMatID, enemyInfo.v3Pos);
	tempEnemy->SetDirection(enemyInfo.v3Dir);

	std::pair<UINT, CEnemy*> newEnemy(enemyInfo.iID, tempEnemy);
	m_pEnemies->insert(newEnemy);
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

void CMechanics_Client::SpawnPower(ServerToClient* _pServerPacket)
{
	PowerUpInfo powerInfo = _pServerPacket->powerInfo;
	CMesh* pMesh = 0;
	int iMatID;

	switch (powerInfo.eType)
	{
	case PT_HEALTH:
	{
		pMesh = m_pHealthMesh;
		iMatID = m_iHealthMaterialID;
		break;
	}
	default:
		break;
	}

	CPowerUp* tempPower = new CPowerUp(powerInfo.eType);
	tempPower->Initialise(m_pRenderer, pMesh, powerInfo.iID, iMatID, powerInfo.v3Pos);
	tempPower->SetDirection(powerInfo.v3Dir);

	std::pair<UINT, CPowerUp*> newPower(powerInfo.iID, tempPower);
	m_pPowerUps->insert(newPower);
}

void CMechanics_Client::DeletePower(ServerToClient* _pServerPacket)
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

void CMechanics_Client::RenderHUD()
{
	std::map<std::string, CAvatar*>::iterator Avatar = m_pAvatars->find(m_strUserName);

	int iYpos = 0;
	D3DXCOLOR textBlue = 0xff0000ff;

	// Render the text to the Top left Corner
	m_pRenderer->RenderText(false, { 0, 0 }, "Health: " + std::to_string(Avatar->second->GetHealth()), (iYpos += 10), SCREEN_FONT, textBlue, H_LEFT);
	m_pRenderer->RenderText(false, { 0, 0 }, "Score: " + std::to_string(Avatar->second->GetScore()), (iYpos += 14), SCREEN_FONT, textBlue, H_LEFT);
}