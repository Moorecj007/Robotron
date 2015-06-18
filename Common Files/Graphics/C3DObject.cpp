/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : 3DObject.cpp
* Description : Implementation for the functions of the 3DObject class
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Includes
#include "C3DObject.h"

// Static Variables
UINT C3DObject::m_siNextID = 0;

C3DObject::C3DObject()
{
	// Set the Pointers to NULL
	m_pRenderer = 0;
	m_pMesh = 0;

	// Set the Vectors to generic starting values
	m_v3Position = { 0.0f, 0.0f, 0.0f };
	m_v3Direction = { 0.0f, 0.0f, 1.0f };
	m_v3Velocity = { 0.0f, 0.0f, 0.0f };

	// Set the floating point variables top generic starting values
	m_fSize = 0.0f;
	m_fRotation = 0.0f;
	m_fMaxSpeed = 10.0f;	// TO DO - something

	// Set the material ID to -1 ( doesn't exist)
	m_iMaterialID = -1;

	// Set the ID of the Object
	m_iID = m_siNextID++;
}

C3DObject::~C3DObject()
{
}

void C3DObject::Initialise(IRenderer* _pRenderer, CMesh* _pMesh, int _MaterialID, v3float _v3Pos)
{ 
	// Initialise the Pointers
	m_pRenderer = _pRenderer;
	m_pMesh = _pMesh;

	// Initialise the Vectors
	m_v3Position = _v3Pos;

	// Initialise the floating point variables
	m_fSize = m_pMesh->GetSize();

	// Set the Material
	m_iMaterialID = _MaterialID;
}

bool C3DObject::CreateTexture(std::string strFilePath)
{
	return true;
}

void C3DObject::Draw()
{
	// Calculates the World Matrix for this 3D Object
	CalcWorldMatrix();
	m_pRenderer->SetMaterial(m_iMaterialID);
	m_pMesh->Draw();
}

void C3DObject::Process(float _fDT)
{
	
}

void C3DObject::CalcWorldMatrix()
{
	// Matrices to make up the World Matrix
	D3DXMATRIX matRotateYaw;
	D3DXMATRIX matTranslation;
	D3DXMATRIX matWorld;

	D3DXMatrixIdentity(&matRotateYaw);

	// Reset the Objects World Matrix for new Calculation
	D3DXMatrixIdentity(&matWorld);

	// Calculate a Rotation Matrix around the Pitch and Yaw
	D3DXMatrixRotationY(&matRotateYaw, m_fRotation);

	// Calculate the Full Rotation Matrix and store in the World Matrix
	matWorld = matRotateYaw;

	// Calculate a translation Matrix based on the Objects current position in world space
	D3DXMatrixTranslation(&matTranslation, m_v3Position.x, m_v3Position.y, m_v3Position.z);

	// Calculate the Objects World Matrix
	matWorld *= matTranslation;

	// Change the Renderers World Matrix to the Objects World Matrix
	m_pRenderer->SetWorldMatrix(matWorld);
}