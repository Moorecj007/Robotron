/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : ClientMechanics.cpp
* Description : Handles all the mechanics on the client
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __CLIENT_MECHANICS_H__
#define __CLIENT_MECHANICS_H__

// Library Incudes
#include <vector>

// Local Includes
#include "Client.h"
#include "C3DObject.h"
#include "Mesh.h"
#include "IRenderer.h"
#include "Terrain.h"
#include "StaticCamera.h"


class CClientMechanics
{
public:
	// Constructors / Destructors

	/***********************
	* Game: Default Constructor for GameMechanics class
	* @author: Callan Moore
	********************/
	CClientMechanics();

	/***********************
	* ~Game: Default Destructor for GameMechanics class
	* @author: Callan Moore
	********************/
	~CClientMechanics();

	// Getters

	// Setters

	// Prototypes

	/***********************
	* Initialise: Initialise the GameMechanics for use
	* @author: Callan Moore
	* @parameter: _pRenderer: The pointer to the Renderer
	* @parameter: _pServerPacket: The current Server Packet
	* @parameter: _strUserName: The Username of the client user
	* @return: void
	********************/
	bool Initialise(IRenderer* _pRenderer, ServerToClient* _pServerPacket, std::string _strUserName);

	/***********************
	* CreateCubeMesh: Creates a Cube Mesh with origin in its very center
	* @author: Callan Moore
	* @parameter: _fDT: The Current Delta Tick
	* @parameter: _ServerPacket: The current Packet from the Server
	* @return: void
	********************/
	void Process(float _fDT, ServerToClient* _pServerPacket);

	/***********************
	* Draw: Draws all the game world
	* @author: Callan Moore
	* @return: void
	********************/
	void Draw();

	/***********************
	* CreateAvatars: Create the Controllable Avatar for the game
	* @author: Callan Moore
	* @return: void
	********************/
	void CreateAvatars();

	/***********************
	* CreateCubeMesh: Creates a Cube Mesh with origin in its very center
	* @author: Callan Moore
	* @parameter: _fSize: Size of the cube mesh to create from origin to edge
	* @return: CMesh*: Pointer to a created Cube Mesh
	********************/
	CMesh* CreateCubeMesh(float _fSize);

	/***********************
	* UpdateAvatars: Update the Avatar Information from the Packet from the server
	* @author: Callan Moore
	* @return: void
	********************/
	void UpdateAvatars();

	/***********************
	* AddAvatar: Add an Avatar to the Container
	* @author: Callan Moore
	* @parameter: _pServerPacket: Packet that contains the data needed to create the new Avatar
	* @return: void
	********************/
	void AddAvatar(ServerToClient* _pServerPacket);

	/***********************
	* AddAllAvatars: Adds an avatar for all current server users for a user that just joined
	* @author: Callan Moore
	* @parameter: _pServerPacket: Packet that contains the data needed to create the new Avatars
	* @return: void
	********************/
	void AddAllAvatars(ServerToClient* _pServerPacket);

	/***********************
	* RemoveAvatar: Remove an Avatar from the Container
	* @author: Callan Moore
	* @parameter: _strUserName: The username of the avatar to remove
	* @return: void
	********************/
	void RemoveAvatar(std::string _strUserName);


private:
	// Member Variables
	IRenderer* m_pRenderer;
	ServerToClient* m_pServerPacket;
	float m_fDT;

	std::string m_strUserName;
	CTerrain* m_pTerrain;
	CStaticCamera* m_pCamera;
	CMesh* m_pAvatarMesh;
	std::map<std::string, C3DObject*>* m_pAvatars;

};

#endif // __CLIENT_MECHANICS_H__