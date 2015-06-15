/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Vertex..cpp
* Description : Implementaion for the functions of the Vertex class
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Includes
#include "Vertex.h"

/***********************
* CVertex: Default Contructor for Vertex class
* @author: Callan Moore
********************/
CVertex::CVertex()
{
	m_fX = 0;
	m_fY = 0;
	m_fZ = 0;
}

/***********************
* ~CVertex: Default Destructor for Vertex class
* @author: Callan Moore
********************/
CVertex::~CVertex()
{
}

/***********************
* CVertex: Contructor for Vertex class
* @author: Callan Moore
* @parameter: _fX: X Coordinate for the Vertex
* @parameter: _fY: Y Coordinate for the Vertex
* @parameter: _fZ: Z Coordinate for the Vertex
* @parameter: _dwColor: The Color as a 32-bit Unsigned integer
* @parameter: _normal: 3D vector representing the Vertex Normal
********************/
CVertex::CVertex(float _fX, float _fY, float _fZ, float _normalX, float _normalY, float _normalZ)
{
	m_fX = _fX;
	m_fY = _fY;
	m_fZ = _fZ;
	m_NormalX = _normalX;
	m_NormalY = _normalY;
	m_NormalZ = _normalZ;
}

/***********************
* GetX: Retreive the X coordinate of the Vertex
* @author: Callan Moore
* @return: float: The X coordinate
********************/
float CVertex::GetX()
{
	return m_fX;
}

/***********************
* GetY: Retreive the Y coordinate of the Vertex
* @author: Callan Moore
* @return: float: The Y coordinate
********************/
float CVertex::GetY()
{
	return m_fY;
}

/***********************
* GetZ: Retreive the Z coordinate of the Vertex
* @author: Callan Moore
* @return: float: The Z coordinate
********************/
float CVertex::GetZ()
{
	return m_fZ;
}

/***********************
* SetX: Set a new X coordinate for the Vertex
* @author: Callan Moore
* @parameter: _fX: The new value for the X coordinate
* @return: void
********************/
void CVertex::SetX(float _fX)
{
	m_fX = _fX;
}

/***********************
* SetY: Set a new Y coordinate for the Vertex
* @author: Callan Moore
* @parameter: _fY: The new value for the Y coordinate
* @return: void
********************/
void CVertex::SetY(float _fY)
{
	m_fY = _fY;
}

/***********************
* SetZ: Set a new Z coordinate for the Vertex
* @author: Callan Moore
* @parameter: _fZ: The new value for the Z coordinate
* @return: void
********************/
void CVertex::SetZ(float _fZ)
{
	m_fZ = _fZ;
}
