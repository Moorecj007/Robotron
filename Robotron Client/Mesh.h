/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Mesh.h
* Description : Declarations for the functions of the Mesh class
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __MESH_H__
#define __MESH_H__

// Library Includes 
#include <d3dx9.h>
#include <vector>

// Local Includes
#include "IRenderer.h"
#include "Vertex.h"

class CMesh
{
public:
	// Constructors / Destructors
	CMesh(IRenderer* _pRenderer);
	~CMesh();

	// Setters
	void SetPrimitiveType(eIGPrimitiveType _primType);

	//Prototypes
	void AddVertex(CVertex _vert);
	void AddIndices(std::vector<int> _vecIndices);
	void CreateStaticBuffer();
	void Draw();

protected:
	// Member Variables
	IRenderer* m_pRenderer;
	int m_iBufferID;
	std::vector<CVertex> m_vecVertices;
	std::vector<int> m_vecIndices;
	eIGPrimitiveType m_primitiveType;
};

#endif // __MESH_H__ 