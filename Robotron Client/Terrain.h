/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Terrain.h
* Description : Declarations for the functions of the Terrain class
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __TERRAIN_H__
#define __TERRAIN_H__

// Library Includes
#include <vector>
#include <stdlib.h>

// Local Includes
#include "IRenderer.h"
#include "StaticBuffer.h"
#include "Vertex.h"

class CTerrain
{
public:
	// Constructors / Destructors
	CTerrain();
	~CTerrain();

	// Prototypes
	bool Initialise(IRenderer* _pRenderer, std::string _strImagePath, VertexScalar _VertexScalar);
	void Process(float _fDT);
	void Draw(IRenderer* _rRenderer);
	int GenerateStripIndices(std::vector<int>* ppIndices, UINT _uiWidth, UINT _uiDepth);

	// Setters
	void SetScaleWidth(float _fWidth);
	void SetScaleHeight(float _fHeight);
	void SetScaleDepth(float _fDepth);
	void SetCenter(float _fX, float _fY, float _fZ);

protected:
	void CalcWorldMatrix(IRenderer* _pRenderer);

private:
	// Member Variables
	VertexScalar m_VertexScalar;
	float m_fCenter;

	int m_iSurfaceID;
	int m_iBufferID;

	float m_fSurfaceWidth;
	float m_fSurfaceDepth;

	// World Matrix Variables
	float m_fDT;
	float m_fRotationPitch;
	float m_fRotationYaw;
	float m_fRotationRoll;
	float m_fX;
	float m_fY;
	float m_fZ;

	int m_iMaterialID;
};

#endif // __TERRAIN_H__

