/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Mesh.cpp
* Description : Implementation for the functions of the Mesh class
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Includes
#include "Mesh.h"

/***********************
* CMesh: Contructor for Mesh class
* @author: Callan Moore
* @parameter: _pRenderer: Pointer to the Renderer for this application
********************/
CMesh::CMesh(IRenderer* _pRenderer)
{
	// Initialise all Pointers
	m_pRenderer = _pRenderer;

	// Initial Primitive Type
	m_primitiveType = IGPT_TRIANGLELIST;
}

/***********************
* ~CMesh: Default Destructor for Mesh class
* @author: Callan Moore
********************/
CMesh::~CMesh()
{
}

/***********************
* SetPrimitiveType: Set the Primitive Type for this Mesh
* @author: Callan Moore
* @parameter: _primType: New Primitive Type
* @return: void
********************/
void CMesh::SetPrimitiveType(eIGPrimitiveType _primType)
{
	m_primitiveType = _primType;
}

/***********************
* AddVertex: Add a vertex to this Mesh
* @author: Callan Moore
* @parameter: _vert: Vertex to be added
* @return: void
********************/
void CMesh::AddVertex(CVertex _vert)
{
	m_vecVertices.push_back(_vert);
}

/***********************
* AddIndices: Add a Indice list(vector) to this mesh
* @author: Callan Moore
* @parameter: _vecIndices: List of Indices to become the Meshes Indices
* @return: void
********************/
void CMesh::AddIndices(std::vector<int> _vecIndices)
{
	m_vecIndices = _vecIndices;
}

/***********************
* CreateStaticBuffer: Creates the Static Buffer for the Mesh
* @author: Callan Moore
* @return: void
********************/
void CMesh::CreateStaticBuffer()
{
	// Determine the Index Type
	eIGIndexType indexType;
	if (m_vecIndices.size() == 0)
	{
		// There will be no Index Buffer
		indexType = IGIT_NONE;
	}
	else
	{
		// Index Type depends on the Indice format
		indexType = ((sizeof(m_vecIndices.front())) >= 4) ? IGIT_32 : IGIT_16;
	}

	// Create a Static buffer in the Renderer for this Mesh
	m_iBufferID = m_pRenderer->CreateStaticBuffer(	sizeof(CVertex),
													m_primitiveType,
													m_vecVertices.size(),
													m_vecIndices.size(),
													sizeof(CVertex),
													&m_vecVertices,
													indexType,
													&m_vecIndices);
										
}

/***********************
* Draw: Draws the Mesh 
* @author: Callan Moore
* @return: void
********************/
void CMesh::Draw()
{
	m_pRenderer->Render(m_iBufferID);
}


