/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : StaticBuffer.h
* Description : Declaration file for StaticBuffer class
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __STATICBUFFER_H__
#define __STATICBUFFER_H__

// Library Includes
#include <d3d9.h>
#include <vector>

// Local Includes
#include "../Common Files/Graphics_Defines.h"
#include "Vertex.h"


// Constants
const int g_kiInvalidStaticBuffer = -1;

// Prototypes
class CStaticBuffer
{
public:
	// Constructors / Destructors
	CStaticBuffer();
	~CStaticBuffer();

	// Prototypes
	bool Initialise(	IDirect3DDevice9* _pDevice,
						int _iID,
						VertexType _VertexType,
						eIGPrimitiveType _ePrimitiveType,
						unsigned int _uiTotalVerts,
						unsigned int _uiTotalIndices,
						int _iStride,
						void* _pVertexData,
						eIGIndexType _eIndexType,
						void* _pIndices);
	void Render(IDirect3DDevice9* _pDevice);

	// Getters
	int GetNumPrimitives() const;
	int GetID() const;

protected:
	DWORD GetD3DFvf();
	D3DPRIMITIVETYPE GetD3DPT();

private:
	// Disallowing Copies
	CStaticBuffer(const CStaticBuffer& _kr);
	CStaticBuffer& operator=(const CStaticBuffer& _kr);

	// Member Variables
protected:
	IDirect3DVertexBuffer9* m_pVertexBuffer;
	IDirect3DIndexBuffer9* m_pIndexBuffer;

	int m_iNumVerts;
	int m_iNumIndices;

	int m_iStride;
	DWORD m_dwFvf;

	eIGPrimitiveType m_ePrimitiveType;

	int m_iID;
	static int s_iActiveStaticBuffer;
};

#endif //__STATICBUFFER_H__
