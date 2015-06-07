/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : IRenderer.h
* Description : Interface for Renderer
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __RENDERER_H__
#define __RENDERER_H__

// Library Includes
#include <string>
#include <vector>

// Local Includes
#include "../Common Files/Graphics_Defines.h"

class CVertex;
class IRenderer
{
public:
	// Constructors / Destructors
	IRenderer()
	{
	}
	virtual ~IRenderer()
	{
	}

	virtual bool Initialise(int _iWidth, int _iHeight, HWND _hWindow, bool _bFullscreen) = 0;
	virtual bool Shutdown() = 0;

	// Prototypes
	virtual bool PopulatePresentParams(D3DPRESENT_PARAMETERS& _rd3dpp, D3DDISPLAYMODE& _displayMode) = 0;
	virtual void SetClearColour(float _fRed, float _fGreen, float _fBlue) = 0;
	virtual void CreateScreenFont() = 0;

	virtual void Clear(bool _bTarget, bool _bDepth, bool _bStencil) = 0;
	virtual void StartRender(bool _bTarget, bool _bDepth, bool _bStencil) = 0;
	virtual void EndRender() = 0;

	virtual void CreateViewMatrix(D3DXVECTOR3 _vPosition, D3DXVECTOR3 _vLookAt, D3DXVECTOR3 _vUp) = 0;
	virtual void CalculateProjectionMatrix(float _fFov, float _fNear, float _fFar) = 0;
	virtual void CalculateOrthogonalMatrix(float _fNear, float _fFar) = 0;

	virtual int CreateStaticBuffer(		VertexType _VertexType,
										eIGPrimitiveType _ePrimitiveType,
										unsigned int _uiTotalVerts,
										unsigned int _uiTotalIndices,
										int _iStride,
										void* _pData,
										eIGIndexType _eIndexType,
										void* _pIndices) = 0;

	virtual void Render(unsigned int _uiStaticId) = 0;

	virtual int CreateOffscreenSurface(std::string _strFileName, D3DXIMAGE_INFO& _pImageInfo) = 0;
	virtual void RetrieveSurfaceVertices(std::vector<CVertex>*, int _iSurfaceID, D3DXIMAGE_INFO& _pImageInfo, VertexScalar _VertexScalar) = 0;

	virtual void SetLight(int _iLightID) = 0;
	virtual void LightEnable(int _iLightID, bool bOn) = 0;

	virtual int CreateMaterial(MaterialComponents _MaterialComponents) = 0;
	virtual void SetMaterial(int _iMaterialID) = 0;

	virtual eMenuSelection RenderText(eMenuSelection _eSelection, int _iMouseY, std::string _str, int _iYpos, eFontType _font, D3DXCOLOR _color, bool _bAllignLeft) = 0;
	virtual void RenderColor(D3DXCOLOR _color) = 0;

	// Setters
	virtual void SetWorldMatrix(D3DXMATRIX& _rWorld) = 0;

	// Getters
	virtual D3DXMATRIX& GetProjectionMatrix() = 0;
	virtual D3DXMATRIX& GetViewMatrix() = 0;
	virtual D3DXMATRIX& GetWorldMatrix() = 0;

private:
	// Disallowing copies
	IRenderer(const IRenderer& _kr);
	IRenderer& operator= (const IRenderer& _kr);
};

#endif //__RENDERER_H__
