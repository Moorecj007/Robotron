/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : StaticBuffer.cpp
* Description : Implementation file for StaticBuffer class
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Includes
#include "StaticBuffer.h"

// Initialise Static Variables
int CStaticBuffer::s_iActiveStaticBuffer = 0;

/***********************
* CStaticBuffer: Default Constructor for CStaticBuffer
* @author: Callan Moore
********************/
CStaticBuffer::CStaticBuffer()
{
}

/***********************
* ~CStaticBuffer: Default Destructor for CStaticBuffer
* @author: Callan Moore
********************/
CStaticBuffer::~CStaticBuffer()
{
	if (m_pVertexBuffer != 0)	// Release only if one exists
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = 0;
	}
	if (m_pIndexBuffer != 0)	// Release only if one exists
	{
		m_pIndexBuffer->Release();
		m_pIndexBuffer = 0;
	}
}

/***********************
* Initialise: Initialise the Static Buffer for use
* @author: Callan Moore
* @parameter: IDirect3DDevice9*: Pointer to the DirectX 9 Device
* @parameter: _iID: Unique ID for the Static Buffer
* parameter: _VertexType: Long number to define the Vertex Type
* @parameter: _ePrimitiveType: The Primitive Type used
* @parameter: _uiTotalVerts: The Total number of Verts in the Buffer
* @parameter: _uiTotalIndices: The Total number of Indices in the Buffer
* @parameter: _iStride: The size of the each Vertex in the Buffer
* parameter: _pVertexData: Pointer to data to be assigned to the Vertex Buffer
* @parameter: _eIndexType: Index type used
* parameter: _pIndices: Pointer to data to be assigned to the Index Buffer
* @return: bool: True if the initialisation was successful
********************/
bool CStaticBuffer::Initialise(	IDirect3DDevice9* _pDevice,
								int _iID,
								VertexType _VertexType,
								eIGPrimitiveType _ePrimitiveType,
								unsigned int _uiTotalVerts,
								unsigned int _uiTotalIndices,
								int _iStride,
								void* _pVertexData,
								eIGIndexType _eIndexType,
								void* _pIndices)
{
	// Initialise member variables
	m_iID = _iID;
	m_ePrimitiveType = _ePrimitiveType;
	m_iNumVerts = _uiTotalVerts;
	m_iNumIndices = _uiTotalIndices;
	m_iStride = _iStride;
	_pDevice->GetFVF(&m_dwFvf);
	m_pVertexBuffer = 0;
	m_pIndexBuffer = 0;

	// Check if the Buffer is actually creatable
	if (_uiTotalVerts == 0 || _pVertexData == 0)
	{
		return false;
	}
	
	// Create Vertex Buffer
	_pDevice->CreateVertexBuffer((_uiTotalVerts * _iStride), 0, m_dwFvf, D3DPOOL_MANAGED, &m_pVertexBuffer, NULL);
	VOID* pVoidVertex; // Creates a void pointer for locking memory

	// Cast The void Pointer to the Vertex vector to a usable known vector pointer
	std::vector<CVertex>* pVertices = static_cast<std::vector<CVertex>*>(_pVertexData);

	m_pVertexBuffer->Lock(0, (_uiTotalVerts * _iStride), (void**)&pVoidVertex, 0);	// Locks Memory from being moved or used by another process
	memcpy(pVoidVertex, &pVertices->front(), (_uiTotalVerts * _iStride));
	m_pVertexBuffer->Unlock();	// Unlocks the locked memory for general use again

	// Create Index Buffer
	D3DFORMAT indexType;
	switch (_eIndexType)
	{
		case IGIT_16:
		{
			indexType = D3DFMT_INDEX16;
		}
		break;
		case IGIT_32:
		{
			indexType = D3DFMT_INDEX32;
		}
		break;
		case IGIT_NONE:
		{
			m_pIndexBuffer = 0;
		}
		break;
		default:
		{
			indexType = D3DFMT_UNKNOWN;
		}
		break;

	}	// End Switch

	if (_eIndexType != IGIT_NONE)
	{
		_pDevice->CreateIndexBuffer((_uiTotalIndices * sizeof(int)), 0, indexType, D3DPOOL_MANAGED, &m_pIndexBuffer, NULL);
		VOID* pVoidIndex;

		// Cast The void Pointer to the Indice vector to a usable known vector pointer
		std::vector<int>* pIndices = static_cast<std::vector<int>*>(_pIndices);

		m_pIndexBuffer->Lock(0, (_uiTotalIndices * sizeof(int)), (void**)&pVoidIndex, 0);	// Locks Memory from being moved or used by another process
		memcpy(pVoidIndex, &pIndices->front(), (_uiTotalIndices * sizeof(int)));
		m_pIndexBuffer->Unlock();	// Unlocks the locked memory for general use again
	}
	
	// If the Vertex buffer has not been created return false
	if (m_pVertexBuffer == 0)
	{
		return false;
	}

	return true;
}

/***********************
* Render: Renders the Buffer using the input Device
* @author: Callan Moore
* @parameter: IDirect3DDevice9*: Pointer to the DirectX 9 Device
* @return: void
********************/
void CStaticBuffer::Render(IDirect3DDevice9* _pDevice)
{
	// TO DO - fix this

	// Check if the Active Buffer is this Buffer
	//if (s_iActiveStaticBuffer != m_iID)
	//{
		// Set the StreamSource to zero
		_pDevice->SetStreamSource(0, m_pVertexBuffer, 0, m_iStride);

		// Set the FVF
		_pDevice->SetFVF(m_dwFvf);

		// Set the Active buffer to this Buffer ID
		s_iActiveStaticBuffer = m_iID;
	//}

	if (m_pIndexBuffer != 0)	// has an Index Buffer
	{
		// Set the Indices
		_pDevice->SetIndices(m_pIndexBuffer);

		// Draw the Indexed primitives
		_pDevice->DrawIndexedPrimitive(GetD3DPT(), 0, 0, m_iNumVerts, 0, GetNumPrimitives());
	}
	else	// Does not have an Index Buffer
	{
		// Draw all Vertex Primitives
		_pDevice->DrawPrimitive(GetD3DPT(), 0, GetNumPrimitives());
	}
}

/***********************
* GetNumPrimitives: Retrieves the number of primitives in the Static Buffer
* @author: Callan Moore
* @return: int: Number of Primitives in the Static Buffer
********************/
int CStaticBuffer::GetNumPrimitives() const
{
	if (m_pIndexBuffer == 0)
	{
		switch (m_ePrimitiveType)
		{
		case IGPT_POINTLIST:
		{
			return (m_iNumVerts);
		}
			break;
		case IGPT_LINELIST:
		{
			return (m_iNumVerts / 2);
		}
			break;
		case IGPT_LINESTRIP:
		{
			return (m_iNumVerts - 1);
		}
			break;
		case IGPT_TRIANGLELIST:
		{
			return (m_iNumVerts / 3);
		}
			break;
		case IGPT_TRIANGLESTRIP:
		{
			return (m_iNumVerts - 2);
		}
			break;
		case D3DPT_TRIANGLEFAN:
		{
			return (m_iNumVerts - 2);
		}
			break;
		case INVALID_IGPT:	// Fall Through
		default:
		{
			return 0;
		}
			break;
		}	// End Switch
	}
	else
	{
		switch (m_ePrimitiveType)
		{
		case IGPT_POINTLIST:
		{
			return (m_iNumIndices);
		}
			break;
		case IGPT_LINELIST:
		{
			return (m_iNumIndices / 2);
		}
			break;
		case IGPT_LINESTRIP:
		{
			return (m_iNumIndices - 1);
		}
			break;
		case IGPT_TRIANGLELIST:
		{
			return (m_iNumIndices / 3);
		}
			break;
		case IGPT_TRIANGLESTRIP:
		{
			return (m_iNumIndices - 2);
		}
			break;
		case D3DPT_TRIANGLEFAN:
		{
			return (m_iNumIndices - 2);
		}
			break;
		case INVALID_IGPT:	// Fall Through
		default:
		{
			return 0;
		}
			break;
		}	// End Switch
	}
}

/***********************
* GetID: Retrieves the ID if the Static Buffer
* @author: Callan Moore
* @return: int: The ID if the Static Buffer
********************/
int CStaticBuffer::GetID() const 
{
	return m_iID;
}

/***********************
* GetD3DFvf: Retrieves the Flexible Vertex Format (FVF) of the Static Buffer
* @author: Callan Moore
* @return: DWORD: 32 bit representation of the Flexible Vertex Format (FVF)
********************/
DWORD CStaticBuffer::GetD3DFvf()
{
	return m_dwFvf;
}

/***********************
* GetD3DPT: Retrieves the Primitive type of the Static Buffer
* @author: Callan Moore
* @return: D3DPRIMITIVETYPE: Primitive Type
********************/
D3DPRIMITIVETYPE CStaticBuffer::GetD3DPT()
{
	switch( m_ePrimitiveType )
	{
		case IGPT_POINTLIST:
		{
			return D3DPT_POINTLIST;
		}
		break;
		case IGPT_LINELIST:
		{
			return D3DPT_LINELIST;
		}
		break;
		case IGPT_LINESTRIP:
		{
			return D3DPT_LINESTRIP;
		}
		break;
		case IGPT_TRIANGLELIST:
		{
			return D3DPT_TRIANGLELIST;
		}
		break;
		case IGPT_TRIANGLESTRIP:
		{
			return D3DPT_TRIANGLESTRIP;
		}
		break;
		case D3DPT_TRIANGLEFAN:
		{
			return D3DPT_POINTLIST;
		}
		break;
		case INVALID_IGPT:	// Fall Through
		default:
		{
			return D3DPT_FORCE_DWORD;
		}
		break;
	}	// End Switch
}
