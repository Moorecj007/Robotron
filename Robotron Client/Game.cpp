/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Source.cpp
* Description : Sets up the Window for the Program and processes all the messages
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Includes
#include "Game.h"

// Static Variables
CGame* CGame::s_pGame = 0;

CGame::CGame()
{
}

CGame::~CGame()
{
}

CGame& CGame::GetInstance()
{
	if (s_pGame == 0)
	{
		s_pGame = new CGame();
	}
	return (*s_pGame);
}

void CGame::DestroyInstance()
{
	delete s_pGame;
	s_pGame = 0;
}

void CGame::Initialise(HWND _hWnd, int _iScreenWidth, int _iScreenHeight)
{
	// Initialise window variables
	m_hWnd = _hWnd;
	m_iScreenWidth = _iScreenWidth;
	m_iScreenHeight = _iScreenHeight;

}

void CGame::RenderOneFrame()
{
	
}

void CGame::Process()
{

}

void CGame::Draw()
{

}

void CGame::MainMenu()
{

}