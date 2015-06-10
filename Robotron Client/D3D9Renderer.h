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
#include "IRenderer.h"
#include "StaticBuffer.h"

class CD3D9Renderer 
	: public IRenderer
{
public:
	// Constructors / Destructors

	/***********************
	* CD3D9Renderer: Default Constructor for D3D9Renderer
	* @author: Callan Moore
	********************/
	CD3D9Renderer();

	/***********************
	* ~CD3D9Renderer: Default Destructor for D3D9Renderer
	* @author: Callan Moore
	********************/
	~CD3D9Renderer();

	/***********************
	* Initialise: Initialise the D3D9 Renderer for use
	* @author: Callan Moore
	* @parameter: _iWidth: The width of the screen
	* @parameter: _iHeight: The height of the screen
	* @parameter: _hWindow: Handle to the Window
	* @parameter: _bFullscreen: Whether the screen is fullscreen or not
	* @return: bool: Successful Initialisation or not
	********************/
	bool Initialise(int _iWidth, int _iHeight, HWND _hWindow, bool _bFullscreen);

	/***********************
	* Shutdown: Shuts down the Renderer
	* @author: Callan Moore
	* @return: bool: Successful Shutdown or not
	********************/
	virtual bool Shutdown();

	// Prototypes

	/***********************
	* PopulatePresentParams: Populates the Present Parameters struct for thew device
	* @author: Callan Moore
	* @return: _rd3dpp: Structure to hold (return) the generated Present Parameters
	* @return: _displayMode: The current display mode for the current adapter
	* @return: void
	********************/
	virtual bool PopulatePresentParams(D3DPRESENT_PARAMETERS& _rd3dpp, D3DDISPLAYMODE& _displayMode);
	
	/***********************
	* Shutdown: Shuts down the Renderer
	* @author: Callan Moore
	* @return: _fRed: The red component for the clear color
	* @return: _fGreen: The green component for the clear color
	* @return: _fBlue: The blue component for the clear color
	* @return: void
	********************/
	virtual void SetClearColour(float _fRed, float _fGreen, float _fBlue);

	/***********************
	* Clear: Clear the Back Buffer
	* @author: Callan Moore
	* @parameter: _bTarget: Variable to determine whether to clear a Render Target
	* @parameter: _bDepth: Variable to determine whether to clear the Depth Buffer
	* @parameter: _bStencil: Variable to determine whether to clear the Stencil Buffer
	* @return: void:
	********************/
	virtual void Clear(bool _bTarget, bool _bDepth, bool _bStencil);
	
	/***********************
	* StartRender: Sets the Device up to begin Rendering
	* @author: Callan Moore
	* @parameter: _bTarget: Variable to determine whether to clear a Render Target
	* @parameter: _bDepth: Variable to determine whether to clear the Depth Buffer
	* @parameter: _bStencil: Variable to determine whether to clear the Stencil Buffer
	* @return: void:
	********************/
	virtual void StartRender(bool _bTarget, bool _bDepth, bool _bStencil);
	
	/***********************
	* EndRender: Ends the Rendering sequence and presents the Back Buffer
	* @author: Callan Moore
	* @return: void:
	********************/
	virtual void EndRender();

	/***********************
	* CreateViewMatrix: Creates a View Matrix and sets the Device to use this View Matrix
	* @author: Callan Moore
	* @parameters: _vPosition: The Camera Position
	* @parameters: _vLookAt: Model space coodinate the camera is looking
	* @parameters: _vUp: Which direction is up
	* @return: void:
	********************/
	virtual void CreateViewMatrix(D3DXVECTOR3 _vPosition, D3DXVECTOR3 _vLookAt, D3DXVECTOR3 _vUp);
	
	/***********************
	* CalculateProjectionMatrix: Calculates the the Projection Matrix
	* @author: Callan Moore
	* @parameter: _fFOV: The Field of View in radians
	* @parameter: _fNear: Closest Point for Projecting Images
	* @parameter: _fFar: Farthest Point for Projecting Images
	* @return: void:
	********************/
	virtual void CalculateProjectionMatrix(float _fFOV, float _fNear, float _fFar);
	
	/***********************
	* CalculateOrthogonalMatrix: Calculates the the Orthogonal Projection Matrix
	* @author: Callan Moore
	* @parameter: _fNear: The closest the projection can be
	* @parameter: _fFar: The Furtherest the projection can be
	* @return: void:
	********************/
	virtual void CalculateOrthogonalMatrix(float _fNear, float _fFar);

	/***********************
	* CreateStaticBuffer: Create a Static Buffer
	* @author: Callan Moore
	* parameter: _VertexType: Long number to define the Vertex Type
	* @parameter: _ePrimitiveType: The Primitive Type used
	* @parameter: _uiTotalVerts: The Total number of Verts in the Buffer
	* @parameter: _uiTotalIndices: The Total number of Indices in the Buffer
	* @parameter: _iStride: The size of the Vertex
	* parameter: _pData: Pointer to data to be assigned to the Vertex Buffer
	* @parameter: _eIndexType: Index type used
	* parameter: _pIndices: Pointer to data to be assigned to the Index Buffer
	* @return: int: ID of the newly created Static Buffer
	********************/
	virtual int CreateStaticBuffer(	VertexType _VertexType,
									eIGPrimitiveType _ePrimitiveType,
									unsigned int _uiTotalVerts,
									unsigned int _uiTotalIndices,
									int _iStride,
									void* _pData,
									eIGIndexType _eIndexType,
									void* _pIndices );

	/***********************
	* SetWorldMatrix: Sets the Renderers World Matrix
	* @author: Callan Moore
	* @parameter: _uiStaticId: ID number of the Static Buffer to Render
	* @return: void
	********************/
	virtual void Render(unsigned int _uiStaticId);

	/***********************
	* CreateSurface: Creates a Offscreen Surface using the Device
	* @author: Callan Moore
	* @parameter: _strFileName: Path and File name of an image
	* @return: void
	********************/
	virtual int CreateOffscreenSurface(std::string _strFileName, D3DXIMAGE_INFO& _pImageInfo);
	
	/***********************
	* RetrieveSurfaceVertices: Retrieve the Vertices of the Surface of the ID given
	* @author: Callan Moore
	* @parameter: _pVertices: Vector to hold the Retrieved Vertices
	* @parameter: _iSurfaceID: ID of the Surface to render
	* @parameter: _pImageInfo: Struture to hold the retrieved Image Info
	* @parameter: _VertexScalar: Scalar for the Vertices for the Surface
	* @return: void
	********************/
	virtual void RetrieveSurfaceVertices(std::vector<CVertex>*, int _iSurfaceID, D3DXIMAGE_INFO& _pImageInfo, VertexScalar _VertexScalar);

	/***********************
	* SetLight: Sets a Light Lighting structure to the device
	* @author: Callan Moore
	* @parameter: _iLightID: ID of the Light to Set
	* @return: void
	********************/
	virtual void SetLight(int _iLightID);
	
	/***********************
	* LightEnable: Turns a light on or off
	* @author: Callan Moore
	* @parameter: _iLightID: ID of the Light to turn on/off
	* @parameter: bOn: On of off indication
	* @return: void
	********************/
	virtual void LightEnable(int _iLightID, bool bOn);

	/***********************
	* CreateMaterial: Creates a Material and stores it witht the Renderer
	* @author: Callan Moore
	* @parameter: _MaterialComponents: Struct holding all necessary values to create a DX9 Material
	* @return: int: ID value for the map where the material is stored
	********************/
	virtual int CreateMaterial(MaterialComponents _MaterialComponents);

	/***********************
	* RenderText: Render Text to the Screen
	* @author: Callan Moore
	* @parameter: _bSelectable: Allows the text to be selectable
	* @parameter: _iMouseY: The mouse Y coordinate on the screen
	* @parameter: _str: The string to draw
	* @parameter: _iYpos: TThe Y position to start drawing the text
	* @parameter: _font: The font to use to draw the text
	* @parameter: _color: Default color for the font
	* @parameter: _eAllignmentH: Horizonal Allignment
	* @return: std::string: The string of the selected item
	********************/
	virtual std::string RenderText(bool _bSelectable, int _iMouseY, std::string _str, int _iYpos, eFontType _font, D3DXCOLOR _color, eAllignmentH _eAllignmentH);

	/***********************
	* RenderColor: Color fill the backbuffer to the input color
	* @author: Callan Moore
	* @parameter: _color: Color to use
	* @return: void
	********************/
	virtual void RenderColor(D3DXCOLOR _color);
	
	// Setters

	/***********************
	* SetWorldMatrix: Sets the Renderers World Matrix
	* @author: Callan Moore
	* @parameter: _rWorld: Memory address holding a World Matrix
	* @return: void
	********************/
	virtual void SetWorldMatrix(D3DXMATRIX& _rWorld);

	/***********************
	* SetMaterial: Set the Material on the Device
	* @author: Callan Moore
	* @parameter: _iMaterialID: The ID key for the Material
	* @return: void
	********************/
	virtual void SetMaterial(int _iMaterialID);

	// Getters

	/***********************
	* GetProjectionMatrix: Retrieves the location of the Projection Matrix stored in memory
	* @author: Callan Moore
	* @return: D3DXMATRIX&: Memory address of the Devices Projection Matrix
	********************/
	virtual D3DXMATRIX& GetProjectionMatrix();

	/***********************
	* GetViewMatrix: Retrieves the location of the View Matrix stored in memory
	* @author: Callan Moore
	* @return: D3DXMATRIX&: Memory address of the Devices View Matrix
	********************/
	virtual D3DXMATRIX& GetViewMatrix();

	/***********************
	* GetWorldMatrix: Retrieves the location of the World Matrix stored in memory
	* @author: Callan Moore
	* @return: D3DXMATRIX&: Memory address of the Devices World Matrix
	********************/
	virtual D3DXMATRIX& GetWorldMatrix();

private:

	/***********************
	* ExtractRed: Extract the Red channel from a color
	* @author: Callan Moore
	* @parameter: _color: Color to take the red channel from
	* @return: int: 0 - 255 value of the red channel
	********************/
	int ExtractRed(D3DCOLOR _color);

	/***********************
	* CreateScreenFont: Creates a font for drawing onto the screen
	* @author: Callan Moore
	* @return: void
	********************/
	virtual void CreateScreenFont();

	/***********************
	* CreateTitleFont: Create a font to be used for Titles
	* @author: Callan Moore
	* @return: void
	********************/
	virtual void CreateTitleFont();

	/***********************
	* CreateMenuFont: Create a font to be used for menu text
	* @author: Callan Moore
	* @return: void
	********************/
	virtual void CreateMenuFont();

	/***********************
	* CreateMenuFont: Create a font to be used for subtitle text
	* @author: Callan Moore
	* @return: void
	********************/
	virtual void CreateSubtitleFont();

	/***********************
	* CreateMenuFont: Create a font to be used for menu text
	* @author: Callan Moore
	* @parameter: _iYPos: THe y Position ot start the Rect
	* @parameter: _str: string that needs a RECT structure to contain it
	* @parameter: _pFont: Font that will be used to write the string
	* @return: RECT: Rect structure that can contain the string
	********************/
	RECT RectfromString(int _iYPos, std::string _str, ID3DXFont* _pFont);

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

	ID3DXFont* m_pScreenFont;
	ID3DXFont* m_pTitleFont;
	ID3DXFont* m_pMenuFont;
	ID3DXFont* m_pSubtitleFont;

	// TO DO - delete
	D3DLIGHT9 m_DirectionLight;
};
#endif __D3D9RENDERER_H__