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
//#include <Windows.h>	// for: DWORD
#include <d3d9.h>

class CVertex
{
public:
	// Constructors / Destructors
	CVertex();
	~CVertex();
	CVertex(float _fX, float _fY, float _fZ, float _normalX, float _normalY, float _normalZ);

	// Getters
	float GetX();
	float GetY();
	float GetZ();

	// Setters
	void SetX(float _fX);
	void SetY(float _fX);
	void SetZ(float _fX);

private:
	// Member Variables
	float m_fX;
	float m_fY;
	float m_fZ;
	float m_NormalX;
	float m_NormalY;
	float m_NormalZ;
};

#endif //__VERTEX_H__

