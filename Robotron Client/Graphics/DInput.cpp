/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : DInput.cpp
* Description : Handles all input from external devices
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Includes
#include "DInput.h"

CDInput::CDInput()
{
}

CDInput::~CDInput()
{
}

Controls CDInput::GetControls()
{
	return m_activatedControls;
}

POINT CDInput::GetMousePos()
{
	return m_ptMousePos;
}

bool CDInput::Initialise(HINSTANCE _hInstance, HWND _hWnd, int _iScreenWidth, int _iScreenHeight)
{
	// Start the Input using Keyboard and Mouse
	m_bControllerInput = false;

	// Store the Window variables
	m_iScreenWidth = _iScreenWidth;
	m_iScreenHeight = _iScreenHeight;
	m_hWnd = _hWnd;

	// Create the DirectInput Interface
	HRESULT hResult = DirectInput8Create(_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDirectInput, NULL);
	VALIDATE((SUCCEEDED(hResult)));

	// Initialise the Keyboard
	if (InitialiseKeyboard(_hWnd) == false)
	{
		m_pKeyboard->Release();
		return false;
	}

	// Initialise the Mouse
	if (InitialiseMouse(_hWnd) == false)
	{
		m_pMouse->Release();
		return false;
	}

	m_activatedControls.bAction = false;
	m_activatedControls.bEsc = false;
	m_activatedControls.bUp = false;
	m_activatedControls.bDown = false;
	m_activatedControls.bLeft = false;
	m_activatedControls.bRight = false;

	return true;
}

bool CDInput::InitialiseKeyboard(HWND _hWnd)
{
	HRESULT hResult;

	// Initialise the Direct Input device for the Keyboard
	hResult = m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, NULL);
	VALIDATE((SUCCEEDED(hResult)));

	// Set the Data format to the predefined keyboard format
	hResult = m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	VALIDATE((SUCCEEDED(hResult)));

	// Set the Cooperative level of the Keyboard to allow other programs access
	hResult = m_pKeyboard->SetCooperativeLevel(_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	VALIDATE((SUCCEEDED(hResult)));

	// Aquire the keyboard to gain access 
	hResult = m_pKeyboard->Acquire();
	VALIDATE((SUCCEEDED(hResult)));

	return true;
}

bool CDInput::InitialiseMouse(HWND _hWnd)
{
	HRESULT hResult;

	// Initialise the mouse to the center of the screen
	m_ptMousePos = { (m_iScreenWidth / 2), (m_iScreenHeight / 2) };

	// Initialise the Direct Input device for the Keyboard
	hResult = m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouse, NULL);
	VALIDATE((SUCCEEDED(hResult)));

	// Set the Data format to the predefined keyboard format
	hResult = m_pMouse->SetDataFormat(&c_dfDIMouse);
	VALIDATE((SUCCEEDED(hResult)));

	// Set the Cooperative level of the Mouse to allow other programs access
	hResult = m_pMouse->SetCooperativeLevel(_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	VALIDATE((SUCCEEDED(hResult)));

	// Aquire the keyboard to gain access 
	hResult = m_pMouse->Acquire();
	VALIDATE((SUCCEEDED(hResult)));

	return true;
}

void CDInput::Process()
{
	ReadDevice(m_pMouse, &m_mouseState, sizeof(m_mouseState));
	ReadDevice(m_pKeyboard, &m_keyboardState, sizeof(m_keyboardState));

	ProcessInput();
}

void CDInput::Shutdown()
{
	// Release the Direct Input Interface
	if (m_pDirectInput)
	{
		m_pDirectInput->Release();
		m_pDirectInput = 0;
	}

	// Release the Keyboard
	if (m_pKeyboard)
	{
		m_pKeyboard->Unacquire();
		m_pKeyboard->Release();
		m_pKeyboard = 0;
	}

	// Release the Mouse
	if (m_pMouse)
	{
		m_pMouse->Unacquire();
		m_pMouse->Release();
		m_pMouse = 0;
	}
}

bool CDInput::ReadDevice(IDirectInputDevice8* _pDIDevice, void* _pDataBuffer, int _iBufferSize)
{
	HRESULT hr;
	while (true)
	{
		if (_pDIDevice == 0)
		{
			return false;
		}

		// Poll the Device
		_pDIDevice->Poll();

		// Read the devices state
		hr = _pDIDevice->GetDeviceState(_iBufferSize, (LPVOID)_pDataBuffer);
		if (SUCCEEDED(hr))
		{
			break;
		}
		// Unknown error encountered
		if (hr != DIERR_INPUTLOST && hr != DIERR_NOTACQUIRED)
		{
			return false;
		}
		// Try again after reacquiring the device
		if (FAILED(_pDIDevice->Acquire()))
		{
			return false;
		}
	}
	return true;
}

void CDInput::ProcessInput()
{
	if (m_bControllerInput == true)
	{
		// Xbox 360 Controller
		// ProcessController();
	}
	else
	{
		// Using Keyboard and Mouse
		ProcessKeyboard();
		ProcessMouse();
	}
}

void CDInput::ProcessKeyboard()
{
	// Normal Player Input controls
	(m_keyboardState[DIK_E] & 0x80)		 ? m_activatedControls.bFlare = true : m_activatedControls.bFlare = false;
	(m_keyboardState[DIK_W] & 0x80)		 ? m_activatedControls.bUp = true : m_activatedControls.bUp = false;
	(m_keyboardState[DIK_A] & 0x80)		 ? m_activatedControls.bLeft = true : m_activatedControls.bLeft = false;
	(m_keyboardState[DIK_S] & 0x80)		 ? m_activatedControls.bDown = true : m_activatedControls.bDown = false;
	(m_keyboardState[DIK_D] & 0x80)		 ? m_activatedControls.bRight = true : m_activatedControls.bRight = false;
	(m_keyboardState[DIK_ESCAPE] & 0x80) ? m_activatedControls.bEsc = true : m_activatedControls.bEsc = false;
	(m_keyboardState[DIK_TAB] & 0x80)	 ? m_activatedControls.bTab = true : m_activatedControls.bTab = false;

	// Debug Camera Controls
	(m_keyboardState[DIK_NUMPAD5] & 0x80) ? m_activatedControls.bDebugForward = true : m_activatedControls.bDebugForward = false;
	(m_keyboardState[DIK_NUMPAD0] & 0x80) ? m_activatedControls.bDebugBackwards = true : m_activatedControls.bDebugBackwards = false;
	(m_keyboardState[DIK_NUMPAD4] & 0x80) ? m_activatedControls.bDebugLeft = true : m_activatedControls.bDebugLeft = false;
	(m_keyboardState[DIK_NUMPAD6] & 0x80) ? m_activatedControls.bDebugRight = true : m_activatedControls.bDebugRight = false;
	(m_keyboardState[DIK_NUMPAD8] & 0x80) ? m_activatedControls.bDebugUp = true : m_activatedControls.bDebugUp = false;
	(m_keyboardState[DIK_NUMPAD2] & 0x80) ? m_activatedControls.bDebugDown = true : m_activatedControls.bDebugDown = false;

	(m_keyboardState[DIK_F1] & 0x80) ? m_activatedControls.bCameraViewToggle = true : m_activatedControls.bCameraViewToggle = false;
	(m_keyboardState[DIK_F4] & 0x80) ? m_activatedControls.bDebugToggle = true : m_activatedControls.bDebugToggle = false;

}

void CDInput::ProcessMouse()
{
	// Update the position of the mouse using the difference of the mouse location from the last process
	//m_iMouseX += m_mouseState.lX;
	//m_ptMouse.y += m_mouseState.lY;
	GetCursorPos(&m_ptMousePos);
	ScreenToClient(m_hWnd, &m_ptMousePos);
	m_activatedControls.ptMouse = m_ptMousePos;

	// Get the state of the left click
	(m_mouseState.rgbButtons[0] & 0x80) ? m_activatedControls.bAction = true : m_activatedControls.bAction = false;


	// Ensure that the mouse stays within the screen borders
	if (m_ptMousePos.x < 0)	// Left Border
	{
		m_ptMousePos.x = 0;
	}
	if (m_ptMousePos.y < 0)  // Top Border
	{
		m_ptMousePos.y = 0;
	}
	if (m_ptMousePos.x > m_iScreenWidth)		// Right Border
	{
		m_ptMousePos.x = m_iScreenWidth;
	}
	if (m_ptMousePos.y > m_iScreenHeight)	// Bottom Border
	{
		m_ptMousePos.y = m_iScreenHeight;
	}

	return;
}
