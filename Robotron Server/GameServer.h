/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : GameServer.h
* Description : Declaration file for the main game functions of the server
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __GAMESERVER_H__
#define __GAMESERVER_H__

// Library Includes
#include <Windows.h>

class CGameServer
{
public:
	// Constructors / Destructors

	/***********************
	* ~Game: Default Destructor for Game class
	* @author: Callan Moore
	********************/
	~CGameServer();

	// Singleton Methods

	/***********************
	* GetInstance: Returns the singleton instance of the game, if it doesnt exist creates it.
	* @author: Callan Moore
	* @return: CGame&: The current instance of the game
	********************/
	static CGameServer& GetInstance();

	/***********************
	* DestroyInstance: Deletes the instance of the game.
	* @author: Callan Moore
	* @return: void
	********************/
	static void DestroyInstance();

	// Getters

	// Setters

	// Prototypes

	/***********************
	* Initialize: Initialise the Game Object for Use
	* @author: Callan Moore
	* @parameters: _hWnd: Handle to The creating window
	* @parameters: _iScreenWidth: The Height of the Screen
	* @parameters: _iScreenHeight: The Width of the Screen
	* @return: void
	********************/
	void Initialise(HWND _hWnd, int _iScreenWidth, int _iScreenHeight);
	
	/***********************
	* RenderOneFrame: Renders one frame
	* @author: Callan Moore
	* @return: void
	********************/
	void RenderOneFrame();

	/***********************
	* Process: Processes the Game for the Delta tick
	* @author: Callan Moore
	* @return: void
	********************/
	void Process();

	/***********************
	* Draw: Draws all the game world
	* @author: Callan Moore
	* @return: void
	********************/
	void Draw();

	/***********************
	* MainMenu: Displays the Games Main Menu
	* @author: Callan Moore
	* @return: void
	********************/
	void MainMenu();

private:
	//Disallowing copies and extra constructions
	CGameServer();
	CGameServer(const CGameServer& _kr);
	CGameServer& operator= (const CGameServer& _kr);

private:
	// Singleton Instance
	static CGameServer* s_pGame;

	// Window Variables
	HWND m_hWnd;
	int m_iScreenWidth;
	int m_iScreenHeight;

	
};

#endif //__GAMESERVER_H__