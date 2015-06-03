/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : D3D9Renderer.cpp
* Description : Implementation file for D3D9Renderer class
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Includes
#include "D3D9Renderer.h"

/***********************
* CD3D9Renderer: Default Constructor for D3D9Renderer
* @author: Callan Moore
********************/
CD3D9Renderer::CD3D9Renderer()
{
	m_pDirect3D = 0;
	m_pDevice = 0;
	m_pVecBuffers = 0;
	m_pFont = 0;

	// Map of Surfaces Setup
	m_pMapSurfaces = 0;
	m_iNextSurfaceKey = 0;

	// Map of materials Setup
	m_pMapMaterials = 0;
	m_iNextMaterialKey = 0;

	m_pLighting = 0;
	m_pVecLights = 0;
}

/***********************
* ~CD3D9Renderer: Default Destructor for D3D9Renderer
* @author: Callan Moore
********************/
CD3D9Renderer::~CD3D9Renderer()
{
	// Release all the Graphics devices
	m_pDevice->Release();
	m_pDevice = 0;
	m_pDirect3D->Release();
	m_pDirect3D = 0;
	m_pFont->Release();
	m_pFont = 0;

	// Delete all surfaces
	if (m_pMapSurfaces != 0)
	{
		std::map<int, IDirect3DSurface9*>::iterator iterCurrent = m_pMapSurfaces->begin();
		std::map<int, IDirect3DSurface9*>::iterator iterEnd = m_pMapSurfaces->end();

		while (iterCurrent != iterEnd)
		{
			iterCurrent->second->Release();
			iterCurrent->second = 0;
			iterCurrent++;
		}
		delete m_pMapSurfaces;
		m_pMapSurfaces = 0;
	}

	// Delete all Materials
	if (m_pMapMaterials != 0)
	{
		std::map<int, D3DMATERIAL9*>::iterator iterCurrent = m_pMapMaterials->begin();
		std::map<int, D3DMATERIAL9*>::iterator iterEnd = m_pMapMaterials->end();

		while (iterCurrent != iterEnd)
		{
			delete iterCurrent->second;
			iterCurrent->second = 0;
			iterCurrent++;
		}
		delete m_pMapMaterials;
		m_pMapMaterials = 0;
	}

	// Delete all the static buffers
	if (m_pVecBuffers != 0)
	{
		while (m_pVecBuffers->empty() == false)
		{
			delete m_pVecBuffers->back();
			m_pVecBuffers->back() = 0;
			m_pVecBuffers->pop_back();
		}
		delete m_pVecBuffers;
		m_pVecBuffers = 0;
	}

	// Delete the Lights
	if (m_pVecLights != 0)
	{
		while (m_pVecLights->empty() == false)
		{
			delete m_pVecLights->back();
			m_pVecLights->back() = 0;
			m_pVecLights->pop_back();
		}
		delete m_pVecLights;
		m_pVecLights = 0;
	}

	// Delete the Lighting handler
	delete m_pLighting;
	m_pLighting = 0;
}

/***********************
* Initialise: Initialise the D3D9 Renderer for use
* @author: Callan Moore
* @parameter: _iWidth: The width of the screen
* @parameter: _iHeight: The height of the screen
* @parameter: _hWindow: Handle to the Window
* @parameter: _bFullscreen: Whether the screen is fullscreen or not
* @return: bool: Successful Initialisation or not
********************/
bool CD3D9Renderer::Initialise(int _iWidth, int _iHeight, HWND _hWindow, bool _bFullscreen)
{
	// Assign member variables
	m_iScreenWidth = _iWidth;
	m_iScreenHeight = _iHeight;
	m_hWindow = _hWindow;
	m_bFullscreen = _bFullscreen;

	// Decide the Device type
	m_devType = D3DDEVTYPE_HAL;

	// Declare Variables structs
	D3DDISPLAYMODE displayMode;
	D3DCAPS9 caps;
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	// Create the Direct3D
	m_pDirect3D = Direct3DCreate9(D3D_SDK_VERSION);

	// Get the default Adapter
	int iAdapter = D3DADAPTER_DEFAULT;

	// Get the default adapters display mode
	m_pDirect3D->GetAdapterDisplayMode(iAdapter, &displayMode);

	// Get the Caps for the device
	m_pDirect3D->GetDeviceCaps(iAdapter, m_devType, &caps);

	// Determine the Vertex processing Hardware or software
	DWORD dwVertProcessing = (caps.VertexProcessingCaps != 0) ? D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE : D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	if (PopulatePresentParams(d3dpp, displayMode) == false)
	{
		return false;	// Device unable to be created
	}

	// Create the device
	m_pDirect3D->CreateDevice(iAdapter, m_devType, m_hWindow, dwVertProcessing, &d3dpp, &m_pDevice);

	//Set the Clear Color to black
	SetClearColour(1, 20.0f / 255.0f, 147.0f / 255.0f);

	// Create Containers
	m_pVecBuffers = new std::vector < CStaticBuffer* > ;
	m_pMapSurfaces = new std::map < int, IDirect3DSurface9* > ;
	m_pMapMaterials = new std::map < int, D3DMATERIAL9* > ;

	// Create a font
	CreateScreenFont();

	// Set up Lighting Vector
	m_pVecLights = new std::vector < D3DLIGHT9* > ;
	for (int i = 0; i < 8; i++)
	{
		D3DLIGHT9* tempLight = new D3DLIGHT9;
		ZeroMemory(tempLight, sizeof(tempLight));
		m_pVecLights->push_back(tempLight);

	}
	// Create a Lighting Handler
	m_pLighting = new CDX9Lighting();
	m_pLighting->Initialise(this, m_pVecLights);
	ReadLightingData();

	for (unsigned int i = 0; i < m_pVecLights->size(); i++)
	{
		if ((*m_pVecLights)[i]->Type == D3DLIGHT_DIRECTIONAL
			|| (*m_pVecLights)[i]->Type == D3DLIGHT_POINT
			|| (*m_pVecLights)[i]->Type == D3DLIGHT_SPOT)
		{
			m_pDevice->SetLight(i, (*m_pVecLights)[i]);
		}
	}
	
	// Setup the Device to handle lighting
	m_pDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL);
	m_pDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50, 50, 50));
	m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// Create and set up a global Material to be used
	D3DMATERIAL9 material;    
	ZeroMemory(&material, sizeof(D3DMATERIAL9));
	material.Diffuse = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);    
	material.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);    
	material.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	m_pDevice->SetMaterial(&material);

	return true;
}

/***********************
* Shutdown: Shuts down the Renderer
* @author: Callan Moore
* @return: bool: Successful Shutdown or not
********************/
bool CD3D9Renderer::Shutdown()
{
	// Iterate through the Static buffer container and delete them
	if (m_pVecBuffers != 0)
	{
		// Cycle through until all Static Buffers have been deleted
		while (m_pVecBuffers->empty() == false)
		{
			delete m_pVecBuffers->back();
			m_pVecBuffers->back() = 0;
			m_pVecBuffers->pop_back();
		}
		delete m_pVecBuffers;
		m_pVecBuffers = 0;
	}

	// Release the Device and Direct3D
	m_pDevice->Release();
	m_pDirect3D->Release();

	return true;
}

/***********************
* PopulatePresentParams: Populates the Present Parameters struct for thew device
* @author: Callan Moore
* @return: _rd3dpp: Structure to hold (return) the generated Present Parameters
* @return: _displayMode: The current display mode for the current adapter
* @return: void
********************/
bool CD3D9Renderer::PopulatePresentParams(D3DPRESENT_PARAMETERS& _rd3dpp, D3DDISPLAYMODE& _displayMode)
{
	// Fill out the D3D Present Parameters structure
	if (m_bFullscreen == true)
	{
		_rd3dpp.FullScreen_RefreshRateInHz = _displayMode.RefreshRate;
		_rd3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	}
	else
	{
		_rd3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}
	_rd3dpp.BackBufferFormat = _displayMode.Format;
	_rd3dpp.BackBufferCount = 1;
	_rd3dpp.EnableAutoDepthStencil = TRUE;
	_rd3dpp.hDeviceWindow = m_hWindow;
	_rd3dpp.BackBufferHeight = m_iScreenHeight;
	_rd3dpp.BackBufferWidth = m_iScreenWidth;
	_rd3dpp.Windowed = (!m_bFullscreen);
	_rd3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

	// Check if the Device supports certain surface formats - Starting with the best and going down the list until one is accepted
	D3DFORMAT adapterFormat = _displayMode.Format;
	if ((m_pDirect3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, m_devType, adapterFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24S8)) == D3D_OK)
	{
		// 32-bit Z-Buffer
		// 24-bit Depth Channel
		//  8-bit Stencil Channel
		_rd3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	}
	else if ((m_pDirect3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, m_devType, adapterFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X4S4)) == D3D_OK)
	{
		// 32-bit Z-Buffer
		// 24-bit Depth Channel
		//	4-bit Stencil Channel
		_rd3dpp.AutoDepthStencilFormat = D3DFMT_D24X4S4;
	}
	else if ((m_pDirect3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, m_devType, adapterFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X8)) == D3D_OK)
	{
		// 32-bit Z-Buffer
		// 24-bit Depth Channel
		//	   no Stencil Channel
		_rd3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;
	}
	else if ((m_pDirect3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, m_devType, adapterFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D32)) == D3D_OK)
	{
		// 32-bit Z-Buffer
		//     no Depth Channel
		//     no Stencil Channel
		_rd3dpp.AutoDepthStencilFormat = D3DFMT_D32;
	}
	else if ((m_pDirect3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, m_devType, adapterFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D16)) == D3D_OK)
	{
		// 16-bit Z-Buffer
		//     no Depth Channel
		//     no Stencil Channel
		_rd3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	}
	else
	{
		return false;
	}

	return true;
}

/***********************
* CreateScreenFont: Creates a font for drawing onto the screen
* @author: Callan Moore
* @return: void
********************/
void CD3D9Renderer::CreateScreenFont()
{
	// Creating a font to draw text to screen
	D3DXFONT_DESCA fontDesc;
	ZeroMemory(&fontDesc, sizeof(D3DXFONT_DESCA));
	fontDesc.Height = 16;
	fontDesc.Width = 8;
	fontDesc.Weight = 1;
	fontDesc.MipLevels = D3DX_DEFAULT;
	fontDesc.Italic = false;
	fontDesc.CharSet = 0;
	fontDesc.OutputPrecision = 100;
	fontDesc.Quality = 100;
	fontDesc.PitchAndFamily = 0;
	strcpy_s(fontDesc.FaceName, "Times New Roman");

	D3DXCreateFontIndirectA(m_pDevice, &fontDesc, &m_pFont);
}

/***********************
* Shutdown: Shuts down the Renderer
* @author: Callan Moore
* @return: _fRed: The red component for the clear color
* @return: _fGreen: The green component for the clear color
* @return: _fBlue: The blue component for the clear color
* @return: void
********************/
void CD3D9Renderer::SetClearColour(float _fRed, float _fGreen, float _fBlue)
{
	m_ClearColor = D3DCOLOR_COLORVALUE(_fRed, _fGreen, _fBlue, 1.0f);
}

/***********************
* Clear: Clear the Back Buffer
* @author: Callan Moore
* @parameter: _bTarget: Variable to determine whether to clear a Render Target
* @parameter: _bDepth: Variable to determine whether to clear the Depth Buffer
* @parameter: _bStencil: Variable to determine whether to clear the Stencil Buffer
* @return: void:
********************/
void CD3D9Renderer::Clear(bool _bTarget, bool _bDepth, bool _bStencil)
{
	DWORD dwClearFlags = 0;

	// Add each Clear Flag if input is set to true
	if (_bStencil == true)
	{
		dwClearFlags = dwClearFlags | D3DCLEAR_STENCIL;
	}
	if (_bTarget == true)
	{
		dwClearFlags = dwClearFlags | D3DCLEAR_TARGET;
	}
	if (_bDepth == true)
	{
		dwClearFlags = dwClearFlags | D3DCLEAR_ZBUFFER;
	}

	// Clear the Back Buffer
	if (dwClearFlags != 0)
	{
		m_pDevice->Clear(0, NULL, dwClearFlags, m_ClearColor, 1.0f, 0);
	}
}

/***********************
* StartRender: Sets the Device up to begin Rendering
* @author: Callan Moore
* @parameter: _bTarget: Variable to determine whether to clear a Render Target
* @parameter: _bDepth: Variable to determine whether to clear the Depth Buffer
* @parameter: _bStencil: Variable to determine whether to clear the Stencil Buffer
* @return: void:
********************/
void CD3D9Renderer::StartRender(bool _bTarget, bool _bDepth, bool _bStencil)
{
	Clear(_bTarget, _bDepth, _bStencil);
	m_pDevice->BeginScene();
}

/***********************
* EndRender: Ends the Rendering sequence and presents the Back Buffer
* @author: Callan Moore
* @return: void:
********************/
void CD3D9Renderer::EndRender()
{
	m_pDevice->EndScene();
	m_pDevice->Present(NULL, NULL, NULL, NULL);
}

/***********************
* CreateViewMatrix: Creates a View Matrix and sets the Device to use this View Matrix
* @author: Callan Moore
* @parameters: _vPosition: The Camera Position
* @parameters: _vLookAt: Model space coodinate the camera is looking
* @parameters: _vUp: Which direction is up
* @return: void:
********************/
void CD3D9Renderer::CreateViewMatrix(D3DXVECTOR3 _vPosition, D3DXVECTOR3 _vLookAt, D3DXVECTOR3 _vUp)
{
	// Populate the View Matrix
	D3DXMatrixLookAtLH(	&m_matView, &_vPosition, &_vLookAt, &_vUp);			

	// Setting the View Matrix for this Object on the Device
	m_pDevice->SetTransform(D3DTS_VIEW, &m_matView);
}

/***********************
* CalculateProjectionMatrix: Calculates the the Projection Matrix
* @author: Callan Moore
* @parameter: _fFOV: The Field of View in radians
* @parameter: _fNear: Closest Point for Projecting Images
* @parameter: _fFar: Farthest Point for Projecting Images
* @return: void:
********************/
void CD3D9Renderer::CalculateProjectionMatrix(float _fFOV, float _fNear, float _fFar)
{
	// Calculate the Aspect ratio
	float fAspectRatio = (float)(m_iScreenWidth / m_iScreenHeight);

	// Calculate the Projection Matrix for the Device
	D3DXMatrixPerspectiveFovLH(	&m_matProjection,
								_fFOV,					// Horizontal Field of View
								(float)fAspectRatio,	// The Aspect ratio
								_fNear,					// Near view plane
								_fFar);					// Far view plane

	// Set the Projection Matrix for the D3D Device
	m_pDevice->SetTransform(D3DTS_PROJECTION, &m_matProjection);
}
/***********************
* CalculateOrthogonalMatrix: Calculates the the Orthogonal Projection Matrix
* @author: Callan Moore
* @parameter: _fNear: The closest the projection can be
* @parameter: _fFar: The Furtherest the projection can be
* @return: void:
********************/
void CD3D9Renderer::CalculateOrthogonalMatrix(float _fNear, float _fFar)
{
	// Calculate the Othogonal Projection Matrix for the Device
	D3DXMatrixOrthoLH(&m_matProjection, (float)m_iScreenWidth, (float)m_iScreenHeight, _fNear, _fFar);

	// Set the Othogonal Projection Matrix for the Device
	m_pDevice->SetTransform(D3DTS_PROJECTION, &m_matProjection);
}

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
int CD3D9Renderer::CreateStaticBuffer(	VertexType _VertexType,
										eIGPrimitiveType _ePrimitiveType,
										unsigned int _uiTotalVerts,
										unsigned int _uiTotalIndices,
										int _iStride,
										void* _pData,
										eIGIndexType _eIndexType,
										void* _pIndices)
{
	// Set the ID to Invalid
	int iID = g_kiInvalidStaticBuffer;

	// Create and Initialise a new Static Buffer
	CStaticBuffer* pNewBuffer = new CStaticBuffer();
	if (pNewBuffer->Initialise(	m_pDevice,
								m_pVecBuffers->size(),
								_VertexType,
								_ePrimitiveType,
								_uiTotalVerts,
								_uiTotalIndices,
								_iStride,
								_pData,
								_eIndexType,
								_pIndices))
	{
		// Add new Buffer to the Static Buffer Container
		m_pVecBuffers->push_back(pNewBuffer);
		iID = pNewBuffer->GetID();
	}
	else
	{
		// Delete allocated memory if the initialisation failed
		delete pNewBuffer;
		pNewBuffer = 0;
	}

	return iID;
}

/***********************
* SetWorldMatrix: Sets the Renderers World Matrix
* @author: Callan Moore
* @parameter: _uiStaticId: ID number of the Static Buffer to Render
* @return: void
********************/
void CD3D9Renderer::Render(unsigned int _uiStaticId)
{
	(*m_pVecBuffers)[_uiStaticId]->Render(m_pDevice);
}

/***********************
* CreateSurface: Creates a Offscreen Surface using the Device
* @author: Callan Moore
* @parameter: _strFileName: Path and File name of an image
* @return: void
********************/
int CD3D9Renderer::CreateOffscreenSurface(std::string _strFileName, D3DXIMAGE_INFO& _pImageInfo)
{
	// Create a DX9 Surface pointer
	IDirect3DSurface9* pSurface = 0;
	
	// Populate the imageInfo from a image file
	HRESULT hr = D3DXGetImageInfoFromFileA(_strFileName.c_str(), &_pImageInfo);
	
	// Create the Offscreen Surface saving it to the Surface Pointer
	m_pDevice->CreateOffscreenPlainSurface(_pImageInfo.Width, _pImageInfo.Height, D3DFMT_X8R8G8B8, D3DPOOL_SCRATCH, &pSurface, 0);
	
	// Load the Image into the surface
	if (D3DXLoadSurfaceFromFileA(pSurface, 0, 0, _strFileName.c_str(), 0, D3DX_FILTER_NONE, 0, &_pImageInfo) != D3D_OK)
	{
		return 0;	// Identifier for Invalid Surface
	}

	// Add the Surface to the Map containing all surfaces
	m_pMapSurfaces->insert(std::pair<int, IDirect3DSurface9*>(++m_iNextSurfaceKey, pSurface));
	return m_iNextSurfaceKey;
}

/***********************
* RetrieveVertices: Retrieve the Vertices of the Surface of the ID given
* @author: Callan Moore
* @parameter: _pVertices: Vector to hold the Retrieved Vertices
* @parameter: _iSurfaceID: ID of the Surface to render 
* @parameter: _pImageInfo: Struture to hold the retrieved Image Info
* @parameter: _VertexScalar: Scalar for the Vertices for the Surface
* @return: void
********************/
void CD3D9Renderer::RetrieveSurfaceVertices(std::vector<CVertex>* _pVertices, int _iSurfaceID, D3DXIMAGE_INFO& _pImageInfo, VertexScalar _VertexScalar)
{

	// TO DO - Comment
	D3DLOCKED_RECT lockRect;
	ZeroMemory(&lockRect, sizeof(D3DLOCKED_RECT));

	// Find the Surface within the Renderers Map of surfaces
	std::map<int, IDirect3DSurface9*>::iterator iterSurface = m_pMapSurfaces->find(_iSurfaceID);
	IDirect3DSurface9* pSurface = iterSurface->second;

	pSurface->LockRect(&lockRect, 0, D3DLOCK_READONLY);;
	int iNumPixels = _pImageInfo.Width * _pImageInfo.Height; // Height becomes Depth

	// Get bits pointer and cast to D3DCOLOR*
	D3DCOLOR* pPixel = reinterpret_cast<D3DCOLOR*>(lockRect.pBits);

	// Cycle through all the Columns of the Surface image
	for (int iCol = 0; iCol < (int)_pImageInfo.Height; iCol++)
	{
		// Cycle through all the Rows of the Surface Image
		for (int iRow = 0; iRow < (int)_pImageInfo.Width; iRow++)
		{
			
			int iPixelNum = (iCol * _pImageInfo.Width) + iRow;	// Find the Pixel Number
			D3DCOLOR PixelColor = pPixel[iPixelNum];			// Retrieve the Color of that Pixel
			int iRedChannel = ExtractRed(PixelColor);			// Extract the Red Channel

			float fWidth = iCol * _VertexScalar.Depth;			// X
			float fHeight = iRedChannel * _VertexScalar.Height;	// Y
			float fDepth = iRow * _VertexScalar.Width;			// Z

			// Create variables to hold values during Normal Calculations
			std::vector<D3DVECTOR> vecDirections;
			D3DVECTOR tempDirection;
			D3DXVECTOR3 tempNormal = { 0, 0, 0 };
			D3DXVECTOR3 v3Normal = { 0, 0, 0 };

			if (iCol % 2 == 0)	// Even Columns
			{		
				// Get the Direction for the Vertex one positive Column over
				if (iCol != (_pImageInfo.Width - 1))
				{
					tempDirection.x = 1 * _VertexScalar.Width;
					tempDirection.y = (ExtractRed(pPixel[(iRow + ((iCol + 1) * _pImageInfo.Width))]) * _VertexScalar.Height) - fHeight;
					tempDirection.z = 0 * _VertexScalar.Depth;
					vecDirections.push_back(tempDirection);
				}

				// Get the Direction for the Vertex one positive Column over and one positive Row over
				if (iCol != (_pImageInfo.Width - 1)
					&& iRow != (_pImageInfo.Width - 1))
				{
					tempDirection.x = 1 * _VertexScalar.Width;
					tempDirection.y = (ExtractRed(pPixel[((iRow + 1) + ((iCol + 1) * _pImageInfo.Width))]) * _VertexScalar.Height) - fHeight;
					tempDirection.z = 1 * _VertexScalar.Depth;
					vecDirections.push_back(tempDirection);
				}

				// Get the Direction for the Vertex one positive Row over
				if (iRow != (_pImageInfo.Width - 1))
				{
					tempDirection.x = 0 * _VertexScalar.Width;
					tempDirection.y = (ExtractRed(pPixel[((iRow + 1) + (iCol * _pImageInfo.Width))]) * _VertexScalar.Height) - fHeight;
					tempDirection.z = 1 * _VertexScalar.Depth;
					vecDirections.push_back(tempDirection);
				}
					
				// Get the Direction for the Vertex one positive Column over and one positive Row over
				if (iCol != 0
					&& iRow != (_pImageInfo.Width - 1))
				{
					tempDirection.x = -1 * _VertexScalar.Width;
					tempDirection.y = (ExtractRed(pPixel[((iRow + 1) + ((iCol - 1) * _pImageInfo.Width))]) * _VertexScalar.Height) - fHeight;
					tempDirection.z = 1 * _VertexScalar.Depth;
					vecDirections.push_back(tempDirection);
				}
					
				// Get the Direction for the Vertex one negative Column over
				if (iCol != 0
					&& iRow != 0)
				{
					tempDirection.x = -1 * _VertexScalar.Width;
					tempDirection.y = (ExtractRed(pPixel[(iRow + ((iCol - 1) * _pImageInfo.Width))]) * _VertexScalar.Height) - fHeight;
					tempDirection.z = 0 * _VertexScalar.Depth;
					vecDirections.push_back(tempDirection);
				}
					
				// Get the Direction for the Vertex one negative Row over
				if (iRow != 0)
				{
					tempDirection.x = 0 * _VertexScalar.Width;
					tempDirection.y = (ExtractRed(pPixel[((iRow - 1) + (iCol * _pImageInfo.Width))]) * _VertexScalar.Height) - fHeight;
					tempDirection.z = -1 * _VertexScalar.Depth;
					vecDirections.push_back(tempDirection);
				}

				
			}
			else	// Odd Columns
			{
				// Get the Direction for the Vertex one positive Column over
				if (iCol != (_pImageInfo.Width - 1))
				{
					tempDirection.x = 1 * _VertexScalar.Width;
					tempDirection.y = (ExtractRed(pPixel[(iRow + ((iCol + 1) * _pImageInfo.Width))]) * _VertexScalar.Height) - fHeight;
					tempDirection.z = 0 * _VertexScalar.Depth;
					vecDirections.push_back(tempDirection);
				}
				
				// Get the Direction for the Vertex one positive Row over
				if (iRow != (_pImageInfo.Width - 1))
				{
					tempDirection.x = 0 * _VertexScalar.Width;
					tempDirection.y = (ExtractRed(pPixel[((iRow + 1) + (iCol * _pImageInfo.Width))]) * _VertexScalar.Height) - fHeight;
					tempDirection.z = 1 * _VertexScalar.Depth;
					vecDirections.push_back(tempDirection);
				}
					
				// Get the Direction for the Vertex one negative Column over
				if (iCol != 0
					&& iRow != 0)
				{
					tempDirection.x = -1 * _VertexScalar.Width;
					tempDirection.y = (ExtractRed(pPixel[(iRow + ((iCol - 1) * _pImageInfo.Width))]) * _VertexScalar.Height) - fHeight;
					tempDirection.z = 0 * _VertexScalar.Depth;
					vecDirections.push_back(tempDirection);
				}
					
				// Get the Direction for the Vertex one negative Column over and one negative Row over
				if (iCol != 0
					&& iRow != 0)
				{
					tempDirection.x = -1 * _VertexScalar.Width;
					tempDirection.y = (ExtractRed(pPixel[((iRow - 1) + ((iCol - 1) * _pImageInfo.Width))]) * _VertexScalar.Height) - fHeight;
					tempDirection.z = -1 * _VertexScalar.Depth;
					vecDirections.push_back(tempDirection);
				}
					
				// Get the Direction for the Vertex one negative Row over
				if (iRow != 0)
				{
					tempDirection.x = 0 * _VertexScalar.Width;
					tempDirection.y = (ExtractRed(pPixel[((iRow - 1) + (iCol * _pImageInfo.Width))]) * _VertexScalar.Height) - fHeight;
					tempDirection.z = -1 * _VertexScalar.Depth;
					vecDirections.push_back(tempDirection);
				}
			
				// Get the Direction for the Vertex one positive Column over and one negative Row over
				if ( iCol != (_pImageInfo.Width - 1)
					&& iRow != 0)
				{
					tempDirection.x = 1 * _VertexScalar.Width;
					tempDirection.y = (ExtractRed(pPixel[((iRow - 1) + ((iCol + 1) * _pImageInfo.Width))]) * _VertexScalar.Height) - fHeight;
					tempDirection.z = -1 * _VertexScalar.Depth;
					vecDirections.push_back(tempDirection);
				}
			}
				
			// Get the two direction vectors for each polygon and calculate the cross product to get the normal
			for (int i = (int)(vecDirections.size() - 1); i >= 0; i--)
			{
				if (i == 0)	// Need to calculate first vector with last vector
				{
					if (vecDirections.size() == 6)	// Vertex is not on the edge so has six polygons
					{
						tempNormal.x += ((vecDirections[i].y * vecDirections[(vecDirections.size() - 1)].z) - (vecDirections[i].z * vecDirections[(vecDirections.size() - 1)].y));
						tempNormal.y += ((vecDirections[i].z * vecDirections[(vecDirections.size() - 1)].x) - (vecDirections[i].x * vecDirections[(vecDirections.size() - 1)].z));
						tempNormal.z += ((vecDirections[i].x * vecDirections[(vecDirections.size() - 1)].y) - (vecDirections[i].y * vecDirections[(vecDirections.size() - 1)].x));
					}
				}
				else
				{
					tempNormal.x += ((vecDirections[i].y * vecDirections[i - 1].z) - (vecDirections[i].z * vecDirections[i - 1].y));
					tempNormal.y += ((vecDirections[i].z * vecDirections[i - 1].x) - (vecDirections[i].x * vecDirections[i - 1].z));
					tempNormal.z += ((vecDirections[i].x * vecDirections[i - 1].y) - (vecDirections[i].y * vecDirections[i - 1].x));
				}
			}

			// Normalise all the added normals together to get the averaged normal for the vertex
			D3DXVec3Normalize(&v3Normal, &tempNormal);

			// Create a Vertex
			CVertex pTempVertex(fWidth, fHeight, fDepth, v3Normal.x, v3Normal.y, v3Normal.z);
			_pVertices->push_back(pTempVertex);
		}
	}

	// unlock the Rect after use
	pSurface->UnlockRect();
}

/***********************
* RenderDebugOutput: Render Text to the screen for debug purposes
* @author: Callan Moore
* @parameter: _strInfo: String of text to be rendered on screen
* @parameter: _iYPos: Y Postion of the text in screen coordinates
* @parameter: _color: Color to make the Text
* @return: void
********************/
void CD3D9Renderer::RenderDebugOutput(std::string _strInfo, int _iYPos, D3DCOLOR _color)
{
	// Retrieve the Font Description
	D3DXFONT_DESCA fontDesc;
	m_pFont->GetDescA(&fontDesc);
	
	// Create a rectangle based on the string length and character sizes
	RECT rect = { 0, _iYPos, _strInfo.length() * (fontDesc.Width + 1), (_iYPos + fontDesc.Height) };
		
	m_pFont->DrawTextA(	NULL,				// Not used
						_strInfo.c_str(),	// The String to draw to the screen
						-1,					// Representation to state the the string is null terminated
						&rect,				// Rectangle to draw the text into
						DT_TOP | DT_LEFT,	// Justifies the Text to the Top and Aligns Text to the Left
						_color);			// The Color to make the text
}

/***********************
* RenderDebugOutput: Render Text to the screen for debug purposes
* @author: Callan Moore
* @parameter: _iYPos: Y Postion of the text in screen coordinates
* @parameter: _iIncrement: Increment Number for each line
* @return: void
********************/
void CD3D9Renderer::RenderLightingOutput(int _iYPos, int _iIncrement)
{
	// TO DO - Comment
	for (unsigned int i = 0; i < m_pVecLights->size(); i++)
	{
		std::string strLightInfo = "";
		strLightInfo.append("Light ID: " + std::to_string(i));

		BOOL bLightEnabled;
		m_pDevice->GetLightEnable(i, &bLightEnabled);
		if (bLightEnabled > 0)
		{
			strLightInfo.append(": Enabled:  True");
		}
		else
		{
			strLightInfo.append(": Enabled: False");
		}

		if ((*m_pVecLights)[i]->Type == D3DLIGHT_DIRECTIONAL)
		{
			strLightInfo.append(": Type: Directional");
		}
		else if ((*m_pVecLights)[i]->Type == D3DLIGHT_SPOT)
		{
			strLightInfo.append(": Type: Spot");
		}
		else if ((*m_pVecLights)[i]->Type == D3DLIGHT_POINT)
		{
			strLightInfo.append(": Type: Point");
		}
		else
		{
			strLightInfo.append(": Type: Undefined");
		}

		RenderDebugOutput(strLightInfo, _iYPos, 0xffffff00);
		_iYPos += _iIncrement;
	}
}

/***********************
* SetWorldMatrix: Sets the Renderers World Matrix
* @author: Callan Moore
* @parameter: _rWorld: Memory address holding a World Matrix
* @return: void
********************/
void CD3D9Renderer::SetWorldMatrix(D3DXMATRIX& _rWorld)
{
	m_matWorld = _rWorld;
	m_pDevice->SetTransform(D3DTS_WORLD, &m_matWorld);
}

/***********************
* GetProjectionMatrix: Retrieves the location of the Projection Matrix stored in memory
* @author: Callan Moore
* @return: D3DXMATRIX&: Memory address of the Devices Projection Matrix
********************/
D3DXMATRIX& CD3D9Renderer::GetProjectionMatrix()
{
	return m_matProjection;
}

/***********************
* GetViewMatrix: Retrieves the location of the View Matrix stored in memory
* @author: Callan Moore
* @return: D3DXMATRIX&: Memory address of the Devices View Matrix
********************/
D3DXMATRIX& CD3D9Renderer::GetViewMatrix()
{
	return m_matView;
}

/***********************
* GetWorldMatrix: Retrieves the location of the World Matrix stored in memory
* @author: Callan Moore
* @return: D3DXMATRIX&: Memory address of the Devices World Matrix
********************/
D3DXMATRIX& CD3D9Renderer::GetWorldMatrix()
{
	return m_matWorld;
}

/***********************
* ReadLightingData: Reads the Lighting Data from file and loads the data to the Renderer
* @author: Callan Moore
* @return: void
********************/
void CD3D9Renderer::ReadLightingData()
{
	if (m_pLighting->ReadLightingData() == true)	// True that the file is corrupt
	{
		int iMsgReturn = MessageBoxA(m_hWindow, "Your Lighting File has become corrupted. would you like to reset to default values?", "Corrupted Data", MB_YESNO | MB_ICONEXCLAMATION);

		switch (iMsgReturn)
		{
			case IDYES:
			{
				m_pLighting->CreateDefaultLightingFile();
				ReadLightingData();
			}
			break;
			case IDNO:
			{

			}
			default: break;
		}	// End Switch
	}
	else
	{
		for (int i = 0; i < 8; i++)
		{
			SetLight(i);
		}
	}
}

/***********************
* SetLight: Sets a Light Lighting structure to the device
* @author: Callan Moore
* @parameter: _iLightID: ID of the Light to Set
* @return: void
********************/
void CD3D9Renderer::SetLight(int _iLightID)
{
	if (	(*m_pVecLights)[_iLightID]->Type == D3DLIGHT_DIRECTIONAL
		||	(*m_pVecLights)[_iLightID]->Type == D3DLIGHT_POINT
		||	(*m_pVecLights)[_iLightID]->Type == D3DLIGHT_SPOT)
	{
		m_pDevice->SetLight(_iLightID, (*m_pVecLights)[_iLightID]);
	}
}

/***********************
* LightEnable: Turns a light on or off
* @author: Callan Moore
* @parameter: _iLightID: ID of the Light to turn on/off
* @parameter: bOn: On of off indication
* @return: void
********************/
void CD3D9Renderer::LightEnable(int _iLightID, bool bOn)
{
	m_pDevice->LightEnable(_iLightID, bOn);
}

/***********************
* CreateMaterial: Creates a Material and stores it witht the Renderer
* @author: Callan Moore
* @parameter: _MaterialComponents: Struct holding all necessary values to create a DX9 Material
* @return: int: ID value for the map where the material is stored
********************/
int CD3D9Renderer::CreateMaterial(MaterialComponents _MaterialComponents)
{
	D3DXCOLOR ambient = D3DXCOLOR(	_MaterialComponents.fAmbientRed,
									_MaterialComponents.fAmbientGreen,
									_MaterialComponents.fAmbientBlue,
									_MaterialComponents.fAmbientAlpha);
	
	D3DXCOLOR diffuse = D3DXCOLOR(	_MaterialComponents.fDiffuseRed,
									_MaterialComponents.fDiffuseGreen,
									_MaterialComponents.fDiffuseBlue,
									_MaterialComponents.fDiffuseAlpha);

	D3DXCOLOR emissive = D3DXCOLOR(	_MaterialComponents.fEmissiveRed,
									_MaterialComponents.fEmissiveGreen,
									_MaterialComponents.fEmissiveBlue,
									_MaterialComponents.fEmissiveAlpha);

	float power = _MaterialComponents.fPower;

	D3DXCOLOR specular = D3DXCOLOR(	_MaterialComponents.fSpecularRed,
									_MaterialComponents.fSpecularGreen,
									_MaterialComponents.fSpecularBlue,
									_MaterialComponents.fSpecularAlpha);

	// Create the Material
	D3DMATERIAL9* pMaterial = new D3DMATERIAL9;
	ZeroMemory(pMaterial, sizeof(pMaterial));
	pMaterial->Ambient = ambient;
	pMaterial->Diffuse = diffuse;
	pMaterial->Emissive = emissive;
	pMaterial->Power = power;
	pMaterial->Specular = specular;


	// Add the material to the Map containing all surfaces
	m_pMapMaterials->insert(std::pair<int, D3DMATERIAL9*>(++m_iNextMaterialKey, pMaterial));
	return m_iNextMaterialKey;
}

/***********************
* SetMaterial: Set the Material on the Device
* @author: Callan Moore
* @parameter: _iMaterialID: The ID key for the Material
* @return: void
********************/
void CD3D9Renderer::SetMaterial(int _iMaterialID)
{
	std::map<int, D3DMATERIAL9*>::iterator iterMaterial = m_pMapMaterials->find(_iMaterialID);
	D3DMATERIAL9* pMaterial = iterMaterial->second;

	m_pDevice->SetMaterial(pMaterial);
}

/***********************
* ExtractRed: Extract the Red channel from a color
* @author: Callan Moore
* @parameter: _color: Color to take the red channel from
* @return: int: 0 - 255 value of the red channel
********************/
int CD3D9Renderer::ExtractRed(D3DCOLOR _color)
{
	return ((_color >> 16) & 255);
}