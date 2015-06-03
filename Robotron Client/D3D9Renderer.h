/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : D3D9Renderer.h
* Description : Declaration file for D3D9Renderer class
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __D3D9RENDERER_H__
#define __D3D9RENDERER_H__

// Library Includes
#include <d3d9.h>
#include <vector>
#include <map>

// Local Includes
#include "Defines.h"
#include "IRenderer.h"
#include "StaticBuffer.h"
#include "DX9Lighting.h"

class CD3D9Renderer 
	: public CIRenderer
{
public:
	// Constructors / Destructors
	CD3D9Renderer();
	~CD3D9Renderer();

	bool Initialise(int _iWidth, int _iHeight, HWND _hWindow, bool _bFullscreen);
	virtual bool Shutdown();

	// Prototypes
	virtual bool PopulatePresentParams(D3DPRESENT_PARAMETERS& _rd3dpp, D3DDISPLAYMODE& _displayMode);
	virtual void SetClearColour(float _fRed, float _fGreen, float _fBlue);
	virtual void CreateScreenFont();

	virtual void Clear(bool _bTarget, bool _bDepth, bool _bStencil);
	virtual void StartRender(bool _bTarget, bool _bDepth, bool _bStencil);
	virtual void EndRender();

	virtual void CreateViewMatrix(D3DXVECTOR3 _vPosition, D3DXVECTOR3 _vLookAt, D3DXVECTOR3 _vUp);
	virtual void CalculateProjectionMatrix(float _fFOV, float _fNear, float _fFar);
	virtual void CalculateOrthogonalMatrix(float _fNear, float _fFar);

	virtual int CreateStaticBuffer(	VertexType _VertexType,
									eIGPrimitiveType _ePrimitiveType,
									unsigned int _uiTotalVerts,
									unsigned int _uiTotalIndices,
									int _iStride,
									void* _pData,
									eIGIndexType _eIndexType,
									void* _pIndices );

	virtual void Render(unsigned int _uiStaticId);

	virtual int CreateOffscreenSurface(std::string _strFileName, D3DXIMAGE_INFO& _pImageInfo);
	virtual void RetrieveSurfaceVertices(std::vector<CVertex>*, int _iSurfaceID, D3DXIMAGE_INFO& _pImageInfo, VertexScalar _VertexScalar);

	virtual void RenderDebugOutput(std::string, int _iYPos, D3DCOLOR _color);
	virtual void RenderLightingOutput(int _iYPos, int _iIncrement);

	virtual void ReadLightingData();
	virtual void SetLight(int _iLightID);
	virtual void LightEnable(int _iLightID, bool bOn);

	virtual int CreateMaterial(MaterialComponents _MaterialComponents);

	// Setters
	virtual void SetWorldMatrix(D3DXMATRIX& _rWorld);
	virtual void SetMaterial(int _iMaterialID);

	// Getters
	virtual D3DXMATRIX& GetProjectionMatrix();
	virtual D3DXMATRIX& GetViewMatrix();
	virtual D3DXMATRIX& GetWorldMatrix();

protected:
	int ExtractRed(D3DCOLOR _color);

protected:
	// Member Variables
	HWND m_hWindow;
	IDirect3D9* m_pDirect3D;
	IDirect3DDevice9* m_pDevice;
	D3DCOLOR m_ClearColor;
	D3DDEVTYPE m_devType;

	std::vector<CStaticBuffer*>* m_pVecBuffers;
	
	int m_iScreenWidth;
	int m_iScreenHeight;

	D3DXMATRIX m_matWorld;
	D3DXMATRIX m_matView;
	D3DXMATRIX m_matProjection;

	bool m_bFullscreen;
	bool m_bRenderingScene;

	// Map Surfaces Variables
	std::map<int, IDirect3DSurface9*>* m_pMapSurfaces;
	unsigned int m_iNextSurfaceKey;
	
	std::map<int, D3DMATERIAL9*>* m_pMapMaterials;
	unsigned int m_iNextMaterialKey;

	ID3DXFont* m_pFont;

	std::vector<D3DLIGHT9*>* m_pVecLights;
	CDX9Lighting* m_pLighting;
};
#endif __D3D9RENDERER_H__