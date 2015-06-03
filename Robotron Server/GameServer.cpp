/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : GameServer.cpp
* Description : Runs the main game functions for the Server - The game manager
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Includes
#include "GameServer.h"

// Static Variables
CGameServer* CGameServer::s_pGame = 0;

CGameServer::CGameServer()
{
}

CGameServer::~CGameServer()
{
}

CGameServer& CGameServer::GetInstance()
{
	if (s_pGame == 0)
	{
		s_pGame = new CGameServer();
	}
	return (*s_pGame);
}

void CGameServer::DestroyInstance()
{
	delete s_pGame;
	s_pGame = 0;
}

void CGameServer::Initialise(HWND _hWnd, int _iScreenWidth, int _iScreenHeight)
{
	// Initialise window variables
	m_hWnd = _hWnd;
	m_iScreenWidth = _iScreenWidth;
	m_iScreenHeight = _iScreenHeight;

	int i = 0;
}

void CGameServer::RenderOneFrame()
{
	
}

void CGameServer::Process()
{

}

void CGameServer::Draw()
{

}

void CGameServer::MainMenu()
{

}