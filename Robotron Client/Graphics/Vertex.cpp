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


CVertex::CVertex()
{
	m_v3Pos = { 0.0f, 0.0f, 0.0f };
	m_v3Normal = { 0.0f, 0.0f, 0.0f };
	m_v2UV = { 0.0f, 0.0f};
}


CVertex::~CVertex()
{
}


CVertex::CVertex(v3float _v3Pos, v3float _v3Normal, v2float _v2UV)
{
	m_v3Pos = _v3Pos;
	m_v3Normal = _v3Normal;
	m_v2UV = _v2UV;
}
