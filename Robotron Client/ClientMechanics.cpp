/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : GameMechanics.h
* Description : Handles all the game mechanics
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Include
#include "ClientMechanics.h"

CClientMechanics::CClientMechanics()
{
}

CClientMechanics::~CClientMechanics()
{
	// Delete the Container of Avatars
	if (m_pAvatars != 0)
	{
		std::map<std::string, C3DObject*>::iterator currentAvatar = m_pAvatars->begin();
		while (currentAvatar != m_pAvatars->end())
		{
			delete currentAvatar->second;
			currentAvatar->second = 0;
			currentAvatar++;
		}

		delete m_pAvatars;
		m_pAvatars = 0;
	}

	// Delete the created meshes
	delete m_pAvatarMesh;
	m_pAvatarMesh = 0;

	// Delete the Graphics 
	delete m_pCamera;
	m_pCamera = 0;
	delete m_pTerrain;
	m_pTerrain = 0;
}

bool CClientMechanics::Initialise(IRenderer* _pRenderer, ServerToClient* _pServerPacket, std::string _strUserName)
{
	// Store the pointer to the renderer
	m_pRenderer = _pRenderer;
	m_pServerPacket = _pServerPacket;
	m_strUserName = _strUserName;

	// Create the initial Projection Matrices and set them on the Renderer
	m_pRenderer->CalculateProjectionMatrix(D3DXToRadian(45), 0.1f, 10000.0f);

	// Create a Terrain for the Game
	m_pTerrain = new CTerrain();
	VertexScalar TerrainScalar = { 1.0f, 0.05f, 1.0f };
	std::string strImagePath = "Assets\\Basic Terrain.bmp";
	m_pTerrain->Initialise(m_pRenderer, strImagePath, TerrainScalar);
	m_pTerrain->SetCenter(0, 0, 0);

	// Create the required meshes
	m_pAvatarMesh = CreateCubeMesh(0.5);

	// Create the the container of avatars and populate
	m_pAvatars = new std::map < std::string, C3DObject*>;
	//CreateAvatars();

	// Create and inititalise the Camera for the Game
	//m_pCamera = new CStaticCamera();
	//std::map< std::string, C3DObject*>::iterator Avatar = m_pAvatars->find(m_strUserName);
	//D3DXVECTOR3 v3Pos = *(Avatar->second->GetPosition());
	//m_pCamera->Initialise({ v3Pos.x, 100, v3Pos.z }, { 0, -1, 0 }, true);
	//m_pCamera->Process(m_pRenderer);

	return true;
}

void CClientMechanics::Process(float _fDT, ServerToClient* _pServerPacket)
{

	// save the current states of the delta tick and the Packet from the server
	m_fDT = _fDT;
	m_pServerPacket = _pServerPacket;

	UpdateAvatars();

	// Process all the Avatars
	std::map<std::string, C3DObject*>::iterator currentAvatar = m_pAvatars->begin();
	while (currentAvatar != m_pAvatars->end())
	{
		currentAvatar->second->Process(_fDT);
		currentAvatar++;
	}

	std::map<std::string, C3DObject*>::iterator Avatar = m_pAvatars->find(m_strUserName);
	D3DXVECTOR3 v3Pos = *(Avatar->second->GetPosition());
	m_pCamera->SetPosition({ v3Pos.x, 100, v3Pos.z});
	m_pCamera->SetCamera({ v3Pos.x, v3Pos.y, v3Pos.z }, { v3Pos.x, 100, v3Pos.z}, { 0, 0, 1 }, { 0, -1, 0 });
	m_pCamera->Process(m_pRenderer);
}

void CClientMechanics::Draw()
{
	// Render the Terrain
	m_pTerrain->Draw(m_pRenderer);

	// Draw all the Avatars
	std::map<std::string, C3DObject*>::iterator currentAvatar = m_pAvatars->begin();
	while (currentAvatar != m_pAvatars->end())
	{
		currentAvatar->second->Draw(m_pRenderer);
		currentAvatar++;
	}
}

void CClientMechanics::CreateAvatars()
{
	// Create a material for the avatars to be made from
	MaterialComponents matComponents;
	matComponents.fAmbientRed = 0;
	matComponents.fAmbientGreen = 0;
	matComponents.fAmbientBlue = 1;
	matComponents.fAmbientAlpha = 1;
	matComponents.fDiffuseRed = 0;
	matComponents.fDiffuseGreen = 0;
	matComponents.fDiffuseBlue = 1;
	matComponents.fDiffuseAlpha = 0;
	matComponents.fEmissiveRed = 0;
	matComponents.fEmissiveGreen = 0;
	matComponents.fEmissiveBlue = 0;
	matComponents.fEmissiveAlpha = 0;
	matComponents.fSpecularRed = 1;
	matComponents.fSpecularGreen = 1;
	matComponents.fSpecularBlue = 0;
	matComponents.fSpecularAlpha = 1;
	matComponents.fPower = 0;

	// Get the Count of the Avatars to be present in the game
	int iAvatarCount = m_pServerPacket->CurrentUserCount;

	// Create a avatar for each user
	for (int i = 0; i < iAvatarCount; i++)
	{
		// Temporarily store the user data 
		UserInfo currentUserInfo = m_pServerPacket->UserInfos[i];

		// Create a new avatar object
		C3DObject* pTempAvatar = new  C3DObject();
		pTempAvatar->Initialise(m_pAvatarMesh, currentUserInfo.fPosX, currentUserInfo.fPosY, currentUserInfo.fPosZ);	// Initialise the Avatar with the Cube Mesh and set its coordinates
		pTempAvatar->SetMaterial(m_pRenderer, matComponents);

		// Save the avatar in a vector
		std::pair<std::string, C3DObject*> pairAvatar((std::string)(currentUserInfo.cUserName), pTempAvatar);
		m_pAvatars->insert(pairAvatar);
	}
}

CMesh* CClientMechanics::CreateCubeMesh(float _fSize)
{
	float fVertexfromOrigin = _fSize;
	CMesh* meshCube = new CMesh(m_pRenderer);

	// Add Vertices to the Mesh in shape of a Cube
	meshCube->AddVertex(CVertex(-fVertexfromOrigin, fVertexfromOrigin, -fVertexfromOrigin, -1.0f, 1.0f, -1.0f));
	meshCube->AddVertex(CVertex(fVertexfromOrigin, fVertexfromOrigin, -fVertexfromOrigin, 1.0f, 1.0f, -1.0f));
	meshCube->AddVertex(CVertex(fVertexfromOrigin, -fVertexfromOrigin, -fVertexfromOrigin, 1.0f, -1.0f, -1.0f));
	meshCube->AddVertex(CVertex(-fVertexfromOrigin, -fVertexfromOrigin, -fVertexfromOrigin, -1.0f, -1.0f, -1.0f));
	meshCube->AddVertex(CVertex(-fVertexfromOrigin, fVertexfromOrigin, fVertexfromOrigin, -1.0f, 1.0f, 1.0f));
	meshCube->AddVertex(CVertex(fVertexfromOrigin, fVertexfromOrigin, fVertexfromOrigin, 1.0f, 1.0f, 1.0f));
	meshCube->AddVertex(CVertex(fVertexfromOrigin, -fVertexfromOrigin, fVertexfromOrigin, 1.0f, -1.0f, 1.0f));
	meshCube->AddVertex(CVertex(-fVertexfromOrigin, -fVertexfromOrigin, fVertexfromOrigin, -1.0f, -1.0f, 1.0f));

	// Set the Indice of the Cube in a Triangle List format
	std::vector<int> vecIndices = { 0, 1, 3,
		1, 2, 3,
		1, 5, 2,
		5, 6, 2,
		5, 4, 6,
		4, 7, 6,
		4, 0, 7,
		0, 3, 7,
		4, 5, 0,
		5, 1, 0,
		3, 2, 7,
		2, 6, 7 };
	meshCube->AddIndices(vecIndices);

	meshCube->SetPrimitiveType(IGPT_TRIANGLELIST);

	// Create a Static buffer in the Renderer for this Mesh
	meshCube->CreateStaticBuffer();
	return meshCube;
}

void CClientMechanics::UpdateAvatars()
{
	// Iterator to point at the found Avatar
	std::map<std::string, C3DObject*>::iterator iterAvatar;

	for (int i = 0; i < m_pServerPacket->CurrentUserCount; i++)
	{
		// Retrieve the username from the current UserInfo
		UserInfo userInfo = (*m_pServerPacket).UserInfos[i];
		std::string strUserName = (std::string)(userInfo.cUserName);
		iterAvatar = m_pAvatars->find(strUserName);

		iterAvatar->second->SetX(userInfo.fPosX);
		iterAvatar->second->SetY(userInfo.fPosY);
		iterAvatar->second->SetZ(userInfo.fPosZ);
	}
}

void CClientMechanics::AddAvatar(ServerToClient* _pServerPacket)
{
	// Create a material for the avatars to be made from
	MaterialComponents matComponents;
	matComponents.fAmbientRed = 0;
	matComponents.fAmbientGreen = 0;
	matComponents.fAmbientBlue = 1;
	matComponents.fAmbientAlpha = 1;
	matComponents.fDiffuseRed = 0;
	matComponents.fDiffuseGreen = 0;
	matComponents.fDiffuseBlue = 1;
	matComponents.fDiffuseAlpha = 0;
	matComponents.fEmissiveRed = 0;
	matComponents.fEmissiveGreen = 0;
	matComponents.fEmissiveBlue = 0;
	matComponents.fEmissiveAlpha = 0;
	matComponents.fSpecularRed = 1;
	matComponents.fSpecularGreen = 1;
	matComponents.fSpecularBlue = 0;
	matComponents.fSpecularAlpha = 1;
	matComponents.fPower = 0;

	UserInfo currentUserInfo;
	// Temporarily store the user data 
	for (int i = 0; i < _pServerPacket->CurrentUserCount; i++)
	{
		if ((std::string)(_pServerPacket->cUserName) == (std::string)(_pServerPacket->UserInfos[i].cUserName))
		{
			currentUserInfo = _pServerPacket->UserInfos[i];
		}
	}

	// Create a new avatar object
	C3DObject* pTempAvatar = new  C3DObject();
	pTempAvatar->SetMesh(m_pAvatarMesh);
	pTempAvatar->Initialise(m_pAvatarMesh, currentUserInfo.fPosX, currentUserInfo.fPosY, currentUserInfo.fPosZ);	// Initialise the Avatar with the Cube Mesh and set its coordinates
	pTempAvatar->SetMaterial(m_pRenderer, matComponents);

	// Save the avatar in a vector
	std::pair<std::string, C3DObject*> pairAvatar((std::string)(currentUserInfo.cUserName), pTempAvatar);
	m_pAvatars->insert(pairAvatar);

	if ((std::string)(_pServerPacket->cUserName) == m_strUserName)
	{
		// Create and inititalise the Camera for the Game
		m_pCamera = new CStaticCamera();
		std::map< std::string, C3DObject*>::iterator Avatar = m_pAvatars->find(m_strUserName);
		D3DXVECTOR3 v3Pos = *(Avatar->second->GetPosition());
		m_pCamera->Initialise({ v3Pos.x, 100, v3Pos.z }, { 0, -1, 0 }, true);
		m_pCamera->Process(m_pRenderer);
	}
}

void CClientMechanics::AddAllAvatars(ServerToClient* _pServerPacket)
{
	// Create a material for the avatars to be made from
	MaterialComponents matComponents;
	matComponents.fAmbientRed = 0;
	matComponents.fAmbientGreen = 0;
	matComponents.fAmbientBlue = 1;
	matComponents.fAmbientAlpha = 1;
	matComponents.fDiffuseRed = 0;
	matComponents.fDiffuseGreen = 0;
	matComponents.fDiffuseBlue = 1;
	matComponents.fDiffuseAlpha = 0;
	matComponents.fEmissiveRed = 0;
	matComponents.fEmissiveGreen = 0;
	matComponents.fEmissiveBlue = 0;
	matComponents.fEmissiveAlpha = 0;
	matComponents.fSpecularRed = 1;
	matComponents.fSpecularGreen = 1;
	matComponents.fSpecularBlue = 0;
	matComponents.fSpecularAlpha = 1;
	matComponents.fPower = 0;

	UserInfo currentUserInfo;
	// Temporarily store the user data 
	for (int i = 0; i < _pServerPacket->CurrentUserCount; i++)
	{
		currentUserInfo = _pServerPacket->UserInfos[i];

		// Create a new avatar object
		C3DObject* pTempAvatar = new  C3DObject();
		pTempAvatar->SetMesh(m_pAvatarMesh);
		pTempAvatar->Initialise(m_pAvatarMesh, currentUserInfo.fPosX, currentUserInfo.fPosY, currentUserInfo.fPosZ);	// Initialise the Avatar with the Cube Mesh and set its coordinates
		pTempAvatar->SetMaterial(m_pRenderer, matComponents);

		// Save the avatar in a vector
		std::pair<std::string, C3DObject*> pairAvatar((std::string)(currentUserInfo.cUserName), pTempAvatar);
		m_pAvatars->insert(pairAvatar);

		if ((std::string)(currentUserInfo.cUserName) == m_strUserName)
		{
			// Create and inititalise the Camera for the Game
			m_pCamera = new CStaticCamera();
			std::map< std::string, C3DObject*>::iterator Avatar = m_pAvatars->find(m_strUserName);
			D3DXVECTOR3 v3Pos = *(Avatar->second->GetPosition());
			m_pCamera->Initialise({ v3Pos.x, 100, v3Pos.z }, { 0, -1, 0 }, true);
			m_pCamera->Process(m_pRenderer);
		}
	}
}

void CClientMechanics::RemoveAvatar(std::string _strUserName)
{
	std::map<std::string, C3DObject*>::iterator iterAvatar;
	iterAvatar = m_pAvatars->find(_strUserName);

	delete iterAvatar->second;
	iterAvatar->second = 0;

	m_pAvatars->erase(_strUserName);
}

