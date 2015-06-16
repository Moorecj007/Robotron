/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Terrain.cpp
* Description : Implementation for the functions of the Terrain class
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Include
#include "Terrain.h"

/***********************
* CTerrain: Default Contructor for Terrain class
* @author: Callan Moore
********************/
CTerrain::CTerrain()
{
	m_fDT = 0;
	m_fRotationPitch = 0;
	m_fRotationYaw = 0;
	m_fRotationRoll = 0;
	m_fX = 0;
	m_fY = 0;
	m_fZ = 0;
}

/***********************
* ~CTerrain: Default Destructor for Terrain class
* @author: Callan Moore
********************/
CTerrain::~CTerrain()
{
}

/***********************
* Initialise: Initialise the Terrain for use
* @author: Callan Moore
* @parameter: _pRenderer: Pointer to the Renderer for the application
* @parameter: _strImagePath: String containing the file path and name of the Image
* @parameter: _VertexScalar: Structure holding the scale data for vertices
* @return: bool: Successful Initialisation or not
********************/
bool CTerrain::Initialise(IRenderer* _pRenderer, std::string _strImagePath, VertexScalar _VertexScalar)
{
	// Initial Scalar values
	m_VertexScalar = _VertexScalar;

	// Create a struct to hold Image information
	D3DXIMAGE_INFO imageInfo;
	ZeroMemory(&imageInfo, sizeof(D3DXIMAGE_INFO));

	// Create an surface
	m_iSurfaceID = _pRenderer->CreateOffscreenSurface(_strImagePath, imageInfo);
	m_fSurfaceWidth = (float)imageInfo.Height;
	m_fSurfaceDepth = (float)imageInfo.Width;

	std::vector<CVertex> pVertices;
	_pRenderer->RetrieveSurfaceVertices(&pVertices, m_iSurfaceID, imageInfo, m_VertexScalar);

	// Generate Indices using TriangleStrip
	std::vector<int> pIndices;
	int iIndicesNum = GenerateStripIndices(&pIndices, imageInfo.Width, imageInfo.Height);

	// Set the Index Type Format
	eIGIndexType indexType = (sizeof(pIndices.front()) >= 4) ? IGIT_32 : IGIT_16;

	if (m_iBufferID = _pRenderer->CreateStaticBuffer(	sizeof(CVertex),
														IGPT_TRIANGLESTRIP,
														(&pVertices)->size(),
														iIndicesNum,
														sizeof(CVertex),
														(&pVertices),
														indexType,
														&pIndices) == -1)
	{
		// Buffer creation failed
		return false;
	}

	// Create Material for terrain
	MaterialComposition matComp;

	matComp.ambient = { 0.0f, 0.2f, 0.0f, 0.0f };
	matComp.diffuse = { 0.85f, 1.0f, 1.0f, 0.0f };
	matComp.emissive = { 0.0f, 0.0f, 0.0f, 0.0f };
	matComp.specular = { 1.0f, 0.2f, 0.1f, 0.0f };
	matComp.power = 0;

	m_iMaterialID = _pRenderer->CreateMaterial(matComp);

	std::string strTexturePath = "Assets\\Basic Terrain.bmp";
	m_iTextureID = _pRenderer->CreateTexture(strTexturePath);


	return true;
}

/***********************
* GenerateStripIndices: Generate and TriangleStrip indice list
* @author: Callan Moore
* @parameter: _pVecIndices: Pointer to store the Generated Indices
* @parameter: _uiWidth: Width(X) of the Image
* @parameter: _uiDepth: Depth(Z) of the Image
* @return: bool: Successful Initialisation or not
********************/
int CTerrain::GenerateStripIndices(std::vector<int>* _pVecIndices, UINT _uiWidth, UINT _uiDepth)
{
	// Cycle through all rows except last one
	for (unsigned int iRow = 0; iRow < _uiDepth - 1; iRow++)
	{
		if (iRow % 2 == 0)	// Even Row
		{
			// Even Rows go Right to Left for Clockwise winding order
			for (int iCol = (int)(_uiWidth - 1); iCol >= 0; iCol--)
			{
				_pVecIndices->push_back(iCol + (iRow * _uiWidth));
				_pVecIndices->push_back(iCol + ((iRow + 1) * _uiWidth));
			}

			// Add Degenerate triangle at end of each row
			if (iRow != _uiDepth - 2)
			{
				_pVecIndices->push_back(0 + ((iRow + 1) * _uiWidth));
			}
		}
		else	// Odd Row
		{
			// Even Rows go Left to Right for Clockwise winding order
			for (unsigned int iCol = 0; iCol < _uiWidth; iCol++)
			{
				_pVecIndices->push_back(iCol + (iRow * _uiWidth));
				_pVecIndices->push_back(iCol + ((iRow + 1) * _uiWidth));
			}

			// Add Degenerate triangle at end of each row
			if (iRow != _uiDepth - 2)
			{
				_pVecIndices->push_back((_uiWidth - 1) + ((iRow + 1)  * _uiWidth));
			}
		}
	}
	
	// return the Number of Indices created
	return ((int)_pVecIndices->size());
}

/***********************
* Process: Process the Terrain for the current Delta Tick
* @author: Callan Moore
* @author: _fDT: The current Delta Tick
* @return: void
********************/
void CTerrain::Process(float _fDT)
{
	m_fDT = _fDT;
}

/***********************
* Draw: Draws the Terrain
* @author: Callan Moore
* @author: _pRenderer: Renderer to use to draw the Terrain
* @return: void
********************/
void CTerrain::Draw(IRenderer* _pRenderer)
{
	// Calculate and Set on the Renderer the Terrains World Matrix
	CalcWorldMatrix(_pRenderer);
	_pRenderer->SetMaterial(m_iMaterialID);

	// Set the Texture for the Terrain
	_pRenderer->SetTexture(m_iTextureID);

	_pRenderer->Render(m_iBufferID);
}

/***********************
* SetScaleWidth: Set a new Scale Width
* @author: Callan Moore
* @author: _fWidth: The new Scale Width
* @return: void
********************/
void CTerrain::SetScaleWidth(float _fWidth)
{
	m_VertexScalar.Width = _fWidth;
}

/***********************
* SetScaleHeight: Set a new Scale Height
* @author: Callan Moore
* @author: _fHeight: The new Scale Height
* @return: void
********************/
void CTerrain::SetScaleHeight(float _fHeight)
{
	m_VertexScalar.Height = _fHeight;
}

/***********************
* SetScaleDepth: Set a new Scale Depth
* @author: Callan Moore
* @author: _fDepth: The new Scale Depth
* @return: void
********************/
void CTerrain::SetScaleDepth(float _fDepth)
{
	m_VertexScalar.Depth = _fDepth;
}

/***********************
* SetCenter: Set the Center of the Terrain to be this coordinate
* @author: Callan Moore
* @parameter: _fX: X coordinate for the new center
* @parameter: _fY: Y coordinate for the new center
* @parameter: _fX: Z coordinate for the new center
* @return: void
********************/
void CTerrain::SetCenter(float _fX, float _fY, float _fZ)
{
	m_fX = _fX - ((m_fSurfaceWidth * m_VertexScalar.Width) / 2);
	m_fY = _fY;
	m_fZ = _fZ - ((m_fSurfaceDepth * m_VertexScalar.Depth) / 2);
}

/***********************
* CalcWorldMatrix: Calculates the World Matrix for this Terrain
* @author: Callan Moore
* @parameter: _pRenderer: Renderer for this application
* @return: void
********************/
void CTerrain::CalcWorldMatrix(IRenderer* _pRenderer)
{
	// Matrices to make up the World Matrix
	D3DXMATRIX matRotateX;
	D3DXMATRIX matRotateY;
	D3DXMATRIX matRotateZ;
	D3DXMATRIX matTranslation;
	D3DXMATRIX matWorld;

	// Calculate Rotational Matrices for this Object based on its starting Yaw/Pitch/Roll and the increments
	D3DXMatrixRotationX(&matRotateX, m_fRotationPitch);
	D3DXMatrixRotationY(&matRotateY, m_fRotationYaw);
	D3DXMatrixRotationZ(&matRotateZ, m_fRotationRoll);

	// Calculate the Translation Matrix from the Objects Position coordinates
	D3DXMatrixTranslation(&matTranslation, m_fX, m_fY, m_fZ);

	// Calculate the World Matrix based on the above Matrices
	matWorld = (matRotateX * matRotateY * matRotateZ * matTranslation);

	// Set The World Matrix for this Terrain on the Renderer
	_pRenderer->SetWorldMatrix(matWorld);
}
