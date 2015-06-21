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

CD3D9Renderer::CD3D9Renderer()
{
	m_pDirect3D = 0;
	m_pDevice = 0;
	m_pVecBuffers = 0;
	m_pScreenFont = 0;
	m_pTitleFont = 0;
	m_pMenuFont = 0;

	// Map of Surfaces Setup
	m_pMapSurfaces = 0;
	m_iNextSurfaceKey = 0;

	// Map of materials Setup
	m_pMapMaterials = 0;
	m_iNextMaterialKey = 0;
	m_iNextTextureKey = 0;
}

CD3D9Renderer::~CD3D9Renderer()
{
	// Release all the Graphics devices
	m_pDevice->Release();
	m_pDevice = 0;
	m_pDirect3D->Release();
	m_pDirect3D = 0;
	m_pScreenFont->Release();
	m_pScreenFont = 0;
	m_pTitleFont->Release();
	m_pTitleFont = 0;
	m_pMenuFont->Release();
	m_pMenuFont = 0;
	m_pSubtitleFont->Release();
	m_pSubtitleFont = 0;

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

	// Delete the Map of Textures
	if (m_pMapTextures != 0)
	{
		std::map<int, IDirect3DTexture9*>::iterator iterCurrent = m_pMapTextures->begin();
		std::map<int, IDirect3DTexture9*>::iterator iterEnd = m_pMapTextures->end();

		while (iterCurrent != iterEnd)
		{
			iterCurrent->second->Release();
			iterCurrent->second = 0;
			iterCurrent++;
		}
		delete m_pMapTextures;
		m_pMapTextures = 0;
	}

	// Delete the Map of Lights
	if (m_pMapLight != 0)
	{
		std::map<int, D3DLIGHT9*>::iterator iterCurrent = m_pMapLight->begin();
		std::map<int, D3DLIGHT9*>::iterator iterEnd = m_pMapLight->end();

		while (iterCurrent != iterEnd)
		{
			delete iterCurrent->second;
			iterCurrent->second = 0;
			iterCurrent++;
		}
		delete m_pMapLight;
		m_pMapLight = 0;
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
}

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
	DWORD dwVertProcessing = (caps.VertexProcessingCaps != 0) ? D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED | D3DCREATE_PUREDEVICE : D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;

	if (PopulatePresentParams(d3dpp, displayMode) == false)
	{
		return false;	// Device unable to be created
	}

	// Create the device
	m_pDirect3D->CreateDevice(iAdapter, m_devType, m_hWindow, dwVertProcessing, &d3dpp, &m_pDevice);

	//Set the Clear Color to Yellow
	SetClearColour(1, 1, 0);

	// Create Containers
	m_pVecBuffers = new std::vector<CStaticBuffer*>;
	m_pMapSurfaces = new std::map<int, IDirect3DSurface9*>;
	m_pMapMaterials = new std::map<int, D3DMATERIAL9*>;
	m_pMapTextures = new std::map<int, IDirect3DTexture9*>;
	m_pMapLight = new std::map<int, D3DLIGHT9*>;

	// Create a font
	CreateScreenFont();
	CreateTitleFont();
	CreateMenuFont();
	CreateSubtitleFont();

	// Setup the Device to handle lighting
	m_pDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
	//m_pDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50, 50, 50));
	m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// Create and set up a global Material to be used
	D3DMATERIAL9 material;    
	ZeroMemory(&material, sizeof(D3DMATERIAL9));
	material.Diffuse = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);    
	material.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);    
	material.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	m_pDevice->SetMaterial(&material);

	// TO DO - put this somewhere else
	ZeroMemory((&m_DirectionLight), sizeof(m_DirectionLight));
	m_DirectionLight.Type = D3DLIGHT_DIRECTIONAL;
	m_DirectionLight.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_DirectionLight.Direction = D3DXVECTOR3(-1.0f, -1.0f, 0);

	//m_pDevice->SetLight(0, &m_DirectionLight);
	//m_pDevice->LightEnable(0, TRUE);

	// Create the initial index for the torches 
	//(0 is reserved for directional light)
	//(1 is reserved for point light)
	m_iNextTorchID = 2;

	return true;
}

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
	_rd3dpp.BackBufferHeight = m_iScreenWidth;
	_rd3dpp.BackBufferWidth = m_iScreenHeight;
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

	D3DXCreateFontIndirectA(m_pDevice, &fontDesc, &m_pScreenFont);
}

void CD3D9Renderer::SetClearColour(float _fRed, float _fGreen, float _fBlue)
{
	m_ClearColor = D3DCOLOR_COLORVALUE(_fRed, _fGreen, _fBlue, 1.0f);
}

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

void CD3D9Renderer::StartRender(bool _bTarget, bool _bDepth, bool _bStencil)
{
	Clear(_bTarget, _bDepth, _bStencil);
	m_pDevice->BeginScene();
}

void CD3D9Renderer::EndRender()
{
	m_pDevice->EndScene();
	m_pDevice->Present(NULL, NULL, NULL, NULL);
}

void CD3D9Renderer::CreateViewMatrix(D3DXVECTOR3 _vPosition, D3DXVECTOR3 _vLookAt, D3DXVECTOR3 _vUp)
{
	// Populate the View Matrix
	D3DXMatrixLookAtLH(	&m_matView, &_vPosition, &_vLookAt, &_vUp);			

	// Setting the View Matrix for this Object on the Device
	m_pDevice->SetTransform(D3DTS_VIEW, &m_matView);
}

void CD3D9Renderer::CalculateProjectionMatrix(float _fFOV, float _fNear, float _fFar)
{
	// Calculate the Aspect ratio
	float fAspectRatio = ((float)m_iScreenWidth / (float)m_iScreenHeight);

	// Calculate the Projection Matrix for the Device
	D3DXMatrixPerspectiveFovLH(	&m_matProjection,
								_fFOV,					// Horizontal Field of View
								(float)fAspectRatio,	// The Aspect ratio
								_fNear,					// Near view plane
								_fFar);					// Far view plane

	// Set the Projection Matrix for the D3D Device
	m_pDevice->SetTransform(D3DTS_PROJECTION, &m_matProjection);
}

void CD3D9Renderer::CalculateOrthogonalMatrix(float _fNear, float _fFar)
{
	// Calculate the Othogonal Projection Matrix for the Device
	D3DXMatrixOrthoLH(&m_matProjection, (float)m_iScreenWidth, (float)m_iScreenHeight, _fNear, _fFar);

	// Set the Othogonal Projection Matrix for the Device
	m_pDevice->SetTransform(D3DTS_PROJECTION, &m_matProjection);
}

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

void CD3D9Renderer::Render(unsigned int _uiStaticId)
{
	(*m_pVecBuffers)[_uiStaticId]->Render(m_pDevice);
}

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

void CD3D9Renderer::RetrieveSurfaceVertices(std::vector<CVertex>* _pVertices, int _iSurfaceID, D3DXIMAGE_INFO& _pImageInfo, VertexScalar _VertexScalar)
{
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

			v2float v3UV = { ((1.0f / (float)_pImageInfo.Width) * fWidth), ((1.0f / (float)_pImageInfo.Height) * fDepth) };
			CVertex pTempVertex({ fWidth, fHeight, fDepth }, { v3Normal.x, v3Normal.y, v3Normal.z }, v3UV);
			_pVertices->push_back(pTempVertex);
		}
	}

	// unlock the Rect after use
	pSurface->UnlockRect();
}

void CD3D9Renderer::SetWorldMatrix(D3DXMATRIX& _rWorld)
{
	m_matWorld = _rWorld;
	m_pDevice->SetTransform(D3DTS_WORLD, &m_matWorld);
}

D3DXMATRIX& CD3D9Renderer::GetProjectionMatrix()
{
	return m_matProjection;
}

D3DXMATRIX& CD3D9Renderer::GetViewMatrix()
{
	return m_matView;
}

D3DXMATRIX& CD3D9Renderer::GetWorldMatrix()
{
	return m_matWorld;
}

int CD3D9Renderer::CreateFlareLight()
{
	int FlareID = 1;

	D3DLIGHT9* pFlareLight = new D3DLIGHT9;
	ZeroMemory(*(&pFlareLight), sizeof(*pFlareLight));

	pFlareLight->Type = D3DLIGHT_POINT;
	pFlareLight->Diffuse = { 1.0f, 1.0f, 0.5f, 1.0f };
	pFlareLight->Position = { 0.0f, 0.0f, 0.0f };
	pFlareLight->Range = 10.0f;
	pFlareLight->Attenuation0 = 0.0f;
	pFlareLight->Attenuation1 = 0.001f;
	pFlareLight->Attenuation2 = 0.0f;

	m_pDevice->SetLight(FlareID, pFlareLight);
	m_pDevice->LightEnable(FlareID, true);

	std::pair<int, D3DLIGHT9*> newLight(FlareID, pFlareLight);
	m_pMapLight->insert(newLight);

	return FlareID;
}

int CD3D9Renderer::CreateTorchLight()
{
	D3DLIGHT9* pTorchLight = new D3DLIGHT9;
	ZeroMemory(*(&pTorchLight), sizeof(*pTorchLight));

	pTorchLight->Type = D3DLIGHT_SPOT;
	pTorchLight->Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	pTorchLight->Position = { 0.0f, 0.0f, 0.0f };
	pTorchLight->Direction = { 0.0f, -0.1f, 1.0f };
	pTorchLight->Range = 50.0f;
	pTorchLight->Attenuation0 = 0.0f;
	pTorchLight->Attenuation1 = 0.001f;
	pTorchLight->Attenuation2 = 0.0f;
	pTorchLight->Phi = D3DXToRadian(60.0f);
	pTorchLight->Theta = D3DXToRadian(30.0f);
	pTorchLight->Falloff = 1.0f;

	m_pDevice->SetLight(m_iNextTorchID, pTorchLight);
	m_pDevice->LightEnable(m_iNextTorchID, true);

	std::pair<int, D3DLIGHT9*> newLight(m_iNextTorchID, pTorchLight);
	m_pMapLight->insert(newLight);

	return m_iNextTorchID++;
}

void CD3D9Renderer::SetLight(int _iLightID)
{
	
}

void CD3D9Renderer::LightEnable(int _iLightID, bool bOn)
{
	m_pDevice->LightEnable(_iLightID, bOn);
}

void CD3D9Renderer::UpdateSpotLight(int _iLightID, v3float _v3Pos, v3float _v3Dir)
{
	std::map<int, D3DLIGHT9*>::iterator iterLight = m_pMapLight->find(_iLightID);

	// Create D3DX vectors from the v3Floats
	D3DXVECTOR3 v3Pos = { _v3Pos.x, _v3Pos.y + 0.5f, _v3Pos.z };
	D3DXVECTOR3 v3Dir = { _v3Dir.x, _v3Dir.y, _v3Dir.z };

	iterLight->second->Position = v3Pos;
	iterLight->second->Direction = v3Dir;

	m_pDevice->SetLight(_iLightID, iterLight->second);
}

void CD3D9Renderer::UpdateFlareLight(int _iLightID, v3float _v3Pos, float _fRange)
{
	std::map<int, D3DLIGHT9*>::iterator iterLight = m_pMapLight->find(_iLightID);

	// Create D3DX vectors from the v3Floats
	D3DXVECTOR3 v3Pos = { _v3Pos.x, _v3Pos.y + 5.0f, _v3Pos.z };
	iterLight->second->Position = v3Pos;

	iterLight->second->Range = _fRange;

	m_pDevice->SetLight(_iLightID, iterLight->second);
}

int CD3D9Renderer::CreateMaterial(MaterialComposition _MatComp)
{
	D3DXCOLOR ambient = D3DXCOLOR(	_MatComp.ambient.r,
									_MatComp.ambient.g,
									_MatComp.ambient.b,
									_MatComp.ambient.a);
	
	D3DXCOLOR diffuse = D3DXCOLOR(	_MatComp.diffuse.r,
									_MatComp.diffuse.r,
									_MatComp.diffuse.r,
									_MatComp.diffuse.r);

	D3DXCOLOR emissive = D3DXCOLOR(	_MatComp.emissive.r,
									_MatComp.emissive.r,
									_MatComp.emissive.r,
									_MatComp.emissive.r);

	float power = _MatComp.power;

	D3DXCOLOR specular = D3DXCOLOR(	_MatComp.specular.r,
									_MatComp.specular.r,
									_MatComp.specular.r,
									_MatComp.specular.r);

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

std::string CD3D9Renderer::RenderText(bool _bSelectable, POINT _ptMousePos, std::string _str, int _iYpos, eFontType _font, D3DXCOLOR _color, eAllignmentH _eAllignmentH)
{
	ID3DXFont* pFont;
	DWORD dwAllignment;
	std::string strSelection;

	switch (_eAllignmentH)
	{
		case H_LEFT:
		{
			dwAllignment = DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP;
			break;
		}
		case H_CENTER:
		{
			dwAllignment = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP;
			break;
		}
		case H_RIGHT:
		{
			dwAllignment = DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP;
			break;
		}
	}	// End Switch (_eAllignment)

	// Get pointer to the correct font
	switch (_font)
	{
		case TITLE_FONT:
		{
			pFont = m_pTitleFont;
			break;
		}
		case MENU_FONT:
		{
			pFont = m_pMenuFont;
			break;
		}
		case SUBTITLE_FONT:
		{
			pFont = m_pSubtitleFont;
			break;
		}
		case SCREEN_FONT:
		{
			pFont = m_pScreenFont;
			break;
		}
		default:
		{
			pFont = m_pScreenFont;
			break;
		}
	} // End Switch (_font)

	// Create a rect based on the string and font type in relation to the Y position
	RECT rect = RectfromString(_iYpos, _str, pFont);

	// If the string is selectable
	if (_bSelectable == true)
	{
		// Check if the mouse position is within the Text Rect
		if (	_ptMousePos.x >= rect.left && _ptMousePos.x <= rect.right
			&&	_ptMousePos.y >= rect.top && _ptMousePos.y <= rect.bottom )
		{
			// Highlight the Text to a differect color to show selection is available
			_color = 0xff0000ff;
			// Save the string if this item to show the mouse is in it
			strSelection = _str;
		}

	}

	// Draw the Text
	pFont->DrawTextA(NULL,
		_str.c_str(),
		-1,
		&rect,
		dwAllignment,
		_color);

	return strSelection;
}

void CD3D9Renderer::RenderColor(D3DXCOLOR _color)
{
	// Color the Backbuffer
	IDirect3DSurface9* pBackBuffer;
	m_pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	m_pDevice->ColorFill(pBackBuffer, 0, _color);
	pBackBuffer->Release();
}

int CD3D9Renderer::CreateTexture(std::string strFilePath)
{
	IDirect3DTexture9* pTempTexture;
	if (FAILED(D3DXCreateTextureFromFileA(m_pDevice, strFilePath.c_str(), &pTempTexture)))
	{
		bool UnableToCreateTexture = false;
		assert(UnableToCreateTexture);
		return -1;
	}

	// Add the material to the Map containing all surfaces
	m_pMapTextures->insert(std::pair<int, IDirect3DTexture9*>(++m_iNextTextureKey, pTempTexture));
	return m_iNextTextureKey;
}

bool CD3D9Renderer::SetTexture(int _iTexID)
{
	std::map<int, IDirect3DTexture9*>::iterator iterTexture = m_pMapTextures->find(_iTexID);

	if (iterTexture == m_pMapTextures->end())
	{
		bool UnableToFindTexture= false;
		assert(UnableToFindTexture);
		return false;
	}

	IDirect3DTexture9* pTexture = iterTexture->second;
	m_pDevice->SetTexture(0, pTexture);

	//m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	//m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	//m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
  
	return true;
}

bool CD3D9Renderer::SetMaterial(int _iMaterialID)
{
	std::map<int, D3DMATERIAL9*>::iterator iterMaterial = m_pMapMaterials->find(_iMaterialID);

	if (iterMaterial == m_pMapMaterials->end())
	{
		bool UnableToFindMaterial = false;
		assert(UnableToFindMaterial);
		return false;
	}

	D3DMATERIAL9* pMaterial = iterMaterial->second;
	m_pDevice->SetMaterial(pMaterial);
	return true;
}

int CD3D9Renderer::ExtractRed(D3DCOLOR _color)
{
	return ((_color >> 16) & 255);
}

void CD3D9Renderer::CreateTitleFont()
{
	// Creating a font to draw text to screen
	D3DXFONT_DESCA fontDesc;
	ZeroMemory(&fontDesc, sizeof(D3DXFONT_DESCA));
	fontDesc.Height = 150;
	fontDesc.Width = 75;
	fontDesc.Weight = 1;
	fontDesc.MipLevels = D3DX_DEFAULT;
	fontDesc.Italic = false;
	fontDesc.CharSet = 0;
	fontDesc.OutputPrecision = 100;
	fontDesc.Quality = 100;
	fontDesc.PitchAndFamily = 0;
	strcpy_s(fontDesc.FaceName, "Verdana");

	D3DXCreateFontIndirectA(m_pDevice, &fontDesc, &m_pTitleFont);
}

void CD3D9Renderer::CreateMenuFont()
{
	// Creating a font to draw text to screen
	D3DXFONT_DESCA fontDesc;
	ZeroMemory(&fontDesc, sizeof(D3DXFONT_DESCA));
	fontDesc.Height = 44;
	fontDesc.Width = 22;
	fontDesc.Weight = 1;
	fontDesc.MipLevels = D3DX_DEFAULT;
	fontDesc.Italic = false;
	fontDesc.CharSet = 0;
	fontDesc.OutputPrecision = 100;
	fontDesc.Quality = 100;
	fontDesc.PitchAndFamily = 0;
	strcpy_s(fontDesc.FaceName, "Verdana");

	D3DXCreateFontIndirectA(m_pDevice, &fontDesc, &m_pMenuFont);
}

void CD3D9Renderer::CreateSubtitleFont()
{
	// Creating a font to draw text to screen
	D3DXFONT_DESCA fontDesc;
	ZeroMemory(&fontDesc, sizeof(D3DXFONT_DESCA));
	fontDesc.Height = 80;
	fontDesc.Width = 40;
	fontDesc.Weight = 1;
	fontDesc.MipLevels = D3DX_DEFAULT;
	fontDesc.Italic = false;
	fontDesc.CharSet = 0;
	fontDesc.OutputPrecision = 100;
	fontDesc.Quality = 100;
	fontDesc.PitchAndFamily = 0;
	strcpy_s(fontDesc.FaceName, "Verdana");

	D3DXCreateFontIndirectA(m_pDevice, &fontDesc, &m_pSubtitleFont);
}

RECT CD3D9Renderer::RectfromString(int _iYPos, std::string _str, ID3DXFont* _pFont)
{
	int iOffset = 20;
	D3DXFONT_DESCA fontDesc;
	_pFont->GetDescA(&fontDesc);
	float fWidthCenter = (float)(m_iScreenWidth / 2);
	RECT rect;

	rect.top = (LONG)_iYPos;
	rect.bottom = (LONG)(_iYPos + fontDesc.Height);
	rect.left = 0 + iOffset;
	rect.right = m_iScreenWidth - iOffset;


	return rect;
}