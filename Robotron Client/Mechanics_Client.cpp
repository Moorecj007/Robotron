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
	delete m_pDemonMesh;
	m_pDemonMesh = 0;
	delete m_pHealthMesh;
	m_pHealthMesh = 0;

	// Delete the Graphics 
	delete m_pCamera;
	m_pCamera = 0;
	delete m_pTerrain;
	m_pTerrain = 0;
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
	m_pEnemies = new std::map < UINT, CEnemy*>;
	m_pPowerUps = new std::map < UINT, CPowerUp*>;

	// Create the required Assets
	CreateAvatarAsset();
	CreateDemonAsset();
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
	UpdateEnemies();
	UpdateFlare();

	// Process all the Avatars
	std::map<std::string, CAvatar*>::iterator currentAvatar = m_pAvatars->begin();
	while (currentAvatar != m_pAvatars->end())
	{
		currentAvatar->second->Process(_fDT);
		currentAvatar++;
	}

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

	// Draw all the Enemies
	std::map<UINT, CEnemy*>::iterator iterEnemy= m_pEnemies->begin();
	while (iterEnemy != m_pEnemies->end())
	{
		iterEnemy->second->Draw();
		iterEnemy++;
	}
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

	for (int i = 0; i < m_pServerPacket->CurrentUserCount; i++)
	{
		// Retrieve the username from the current UserInfo
		AvatarInfo avatarInfo = (*m_pServerPacket).Avatars[i];
		std::string strUserName = (std::string)(avatarInfo.cUserName);
		iterAvatar = m_pAvatars->find(strUserName);

		iterAvatar->second->SetPosition({ avatarInfo.v3Pos.x, avatarInfo.v3Pos.y, avatarInfo.v3Pos.z });
		iterAvatar->second->SetDirection({ avatarInfo.v3Dir.x, avatarInfo.v3Dir.y, avatarInfo.v3Dir.z });

		m_pRenderer->UpdateSpotLight(iterAvatar->second->GetTorchID(), *(iterAvatar->second->GetPosition()), *(iterAvatar->second->GetDirection()));
	}
}

void CMechanics_Client::UpdateEnemies()
{
	// Iterator to point at the found Enemy
	std::map<UINT, CEnemy*>::iterator iterEnemy;
	
	for (int i = 0; i < m_pServerPacket->CurrentEnemyCount; i++)
	{
		// Retrieve the username from the current UserInfo
		EnemyInfo enemyInfo = (*m_pServerPacket).Enemies[i];
		UINT iID = enemyInfo.iID;
		iterEnemy = m_pEnemies->find(iID);

		iterEnemy->second->SetPosition({ enemyInfo.v3Pos.x, enemyInfo.v3Pos.y, enemyInfo.v3Pos.z });
		iterEnemy->second->SetDirection({ enemyInfo.v3Dir.x, enemyInfo.v3Dir.y, enemyInfo.v3Dir.z });
	}
}

void CMechanics_Client::UpdateFlare()
{
	if (m_pServerPacket->Flare.bActive == true)
	{
		m_pRenderer->LightEnable(m_iFlareLightID, true);
		m_pRenderer->UpdateFlareLight(m_iFlareLightID, m_pServerPacket->Flare.v3Pos, m_pServerPacket->Flare.fRange);
	}
	else
	{
		m_pRenderer->LightEnable(m_iFlareLightID, false);
	}
}

void CMechanics_Client::AddAvatar(ServerToClient* _pServerPacket)
{
	AvatarInfo currentAvatarInfo;
	// Temporarily store the user data 
	for (int i = 0; i < _pServerPacket->CurrentUserCount; i++)
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
	for (int i = 0; i < _pServerPacket->CurrentUserCount; i++)
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
	AvatarMatComp.specular = { 1.0f, 1.0f, 0.0f, 1.0f };
	AvatarMatComp.power = 0;
	m_iAvatarMaterialID = m_pRenderer->CreateMaterial(AvatarMatComp);

	// Avatar Mesh and Texture
	m_iAvatarTexID = m_pRenderer->CreateTexture("Assets//Crate Side.bmp");
	m_pAvatarMesh = CreateCubeMesh(0.5f, m_iAvatarTexID);
}

void CMechanics_Client::CreateDemonAsset()
{
	// Create the Material for the Demons to use
	MaterialComposition DemonMatComp;
	DemonMatComp.ambient = { 0.0f, 0.0f, 0.0f, 0.0f };
	DemonMatComp.diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	DemonMatComp.emissive = { 0.0f, 0.0f, 0.0f, 0.0f };
	DemonMatComp.specular = { 1.0f, 1.0f, 0.0f, 1.0f };
	DemonMatComp.power = 0;
	m_iDemonMaterialID = m_pRenderer->CreateMaterial(DemonMatComp);

	// Demon Enemy Mesh and Texture
	m_iDemonTexID = m_pRenderer->CreateTexture("Assets//Demon.bmp");
	m_pDemonMesh = CreateCubeMesh(1.0f, m_iDemonTexID);
}

void CMechanics_Client::CreateHealthAsset()
{
	// Create the Material for the Demons to use
	MaterialComposition HealthMatComp;
	HealthMatComp.ambient = { 0.0f, 0.0f, 0.0f, 0.0f };
	HealthMatComp.diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	HealthMatComp.emissive = { 0.0f, 0.0f, 0.0f, 0.0f };
	HealthMatComp.specular = { 1.0f, 1.0f, 0.0f, 1.0f };
	HealthMatComp.power = 0;
	m_iHealthMaterialID = m_pRenderer->CreateMaterial(HealthMatComp);

	// Demon Enemy Mesh and Texture
	m_iHealthTexID = m_pRenderer->CreateTexture("Assets//Health.bmp");
	m_pHealthMesh = CreateCubeMesh(0.4f, m_iHealthTexID);
}

void CMechanics_Client::CreateFlareAsset()
{
	ZeroMemory(&m_Flare, sizeof(m_Flare));
	m_iFlareLightID = m_pRenderer->CreateFlareLight();
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
	// TO DO
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
	// TO DO
}