/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Vertex.h
* Description : Declarations for the functions of the Vertex class
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __VERTEX_H__
#define __VERTEX_H__

// Library Includes
#include <d3d9.h>

// Local Includes
#include "../../Common Files/Generic_Defines.h"

class CVertex
{
public:
	// Constructors / Destructors

	/***********************
	* CVertex: Default Contructor for Vertex class
	* @author: Callan Moore
	********************/
	CVertex();

	/***********************
	* ~CVertex: Default Destructor for Vertex class
	* @author: Callan Moore
	********************/
	~CVertex();

	/***********************
	* CVertex: Contructor for Vertex class
	* @author: Callan Moore
	* @parameter: _v3Pos: The position of the vertex relative to the center of the mesh it is a part of
	* @parameter: _v3Normal: The normal of the vertex for lighting calculations
	* @parameter: _v2UV: The UV coordinates of the vertex for Texture mapping
	********************/
	CVertex(v3float _v3Pos, v3float _v3Normal, v2float _v2UV);

	// Getters

	/***********************
	* GetPos: Retrieve the position vector of the vertex
	* @author: Callan Moore
	* @return: v3float: the position vector as 3 floats
	********************/
	v3float GetPos() { return m_v3Pos; };

	/***********************
	* GetUV: Retrieve the UV coordinates of the vertex
	* @author: Callan Moore
	* @return: v2float: The coodinates of UV as 2 floats
	********************/
	v2float GetUV() { return m_v2UV; };

	// Setters

	/***********************
	* SetPosition: Sets the position of the vertex relative to the center of the mesh it is part of
	* @author: Callan Moore
	* @parameter: _v3Pos: The position of the vertex relative to the center
	* @return: void
	********************/
	void SetPosition(v3float _v3Pos) { m_v3Pos = _v3Pos; };

private:
	// Member Variables
	v3float m_v3Pos;
	v3float m_v3Normal;
	v2float m_v2UV;
};

#endif //__VERTEX_H__

