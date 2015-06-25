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

	/***********************
	* CMesh: Contructor for Mesh class
	* @author: Callan Moore
	* @parameter: _pRenderer: Pointer to the Renderer for this application
	* @parameter: _fSize: Size of the mesh from center to the edge
	* @parameter: _iTexID: ID for the Texture to use
	********************/
	CMesh(IRenderer* _pRenderer, float _fSize, int _iTexID);

	/***********************
	* ~CMesh: Default Destructor for Mesh class
	* @author: Callan Moore
	********************/
	~CMesh();

	// Getters

	/***********************
	* GetSize: Retrieve the size of the mesh
	* @author: Callan Moore
	* @return: float: The size of the mesh from center to edge
	********************/
	float GetSize() { return m_fSize;};

	// Setters

	/***********************
	* SetPrimitiveType: Set the Primitive Type for this Mesh
	* @author: Callan Moore
	* @parameter: _primType: New Primitive Type
	* @return: void
	********************/
	void SetPrimitiveType(eIGPrimitiveType _primType) { m_primitiveType = _primType; };

	/***********************
	* SetTextureID: Set the Texture ID
	* @author: Callan Moore
	* @parameter: _iTextureID: The new Texture ID
	* @return: void
	********************/
	void SetTextureID(int _iTextureID) { m_iTextureID = _iTextureID; };

	//Prototypes

	/***********************
	* AddVertex: Add a vertex to this Mesh
	* @author: Callan Moore
	* @parameter: _vert: Vertex to be added
	* @return: void
	********************/
	void AddVertex(CVertex _vert);

	/***********************
	* AddIndices: Add a Indice list(vector) to this mesh
	* @author: Callan Moore
	* @parameter: _vecIndices: List of Indices to become the Meshes Indices
	* @return: void
	********************/
	void AddIndices(std::vector<int> _vecIndices);

	/***********************
	* CreateStaticBuffer: Creates the Static Buffer for the Mesh
	* @author: Callan Moore
	* @return: void
	********************/
	void CreateStaticBuffer();

	/***********************
	* Draw: Draws the Mesh
	* @author: Callan Moore
	* @return: void
	********************/
	void Draw();

private:
	// Member Variables
	IRenderer* m_pRenderer;
	int m_iBufferID;
	std::vector<CVertex> m_vecVertices;
	std::vector<int> m_vecIndices;
	eIGPrimitiveType m_primitiveType;

	int m_iTextureID;

	float m_fSize;
};

#endif // __MESH_H__ 