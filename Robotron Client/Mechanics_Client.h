/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Mechanics_Client.h
* Description : Handles all the game mechanics on the client
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __MECHANICSCLIENT_H__
#define __MECHANICSCLIENT_H__

// Library Incudes
#include <vector>

// Local Includes
#include "Networking/Network_Client.h"
#include "../Common Files/Graphics/Mesh.h"
#include "../Common Files/Graphics/IRenderer.h"
#include "../Common Files/Graphics/Avatar.h"
#include "../Common Files/Graphics/Enemy.h"
#include "../Common Files/Graphics/PowerUp.h"
#include "Graphics/Terrain.h"
#include "Graphics/StaticCamera.h"

class CMechanics_Client
{
public:
	// Constructors / Destructors

	/***********************
	* Game: Default Constructor for GameMechanics class
	* @author: Callan Moore
	********************/
	CMechanics_Client();

	/***********************
	* ~Game: Default Destructor for GameMechanics class
	* @author: Callan Moore
	********************/
	~CMechanics_Client();

	// Getters

	// Setters

	// Prototypes

	/***********************
	* Initialise: Initialise the GameMechanics for use
	* @author: Callan Moore
	* @parameter: _pRenderer: The pointer to the Renderer
	* @parameter: _strUserName: The Username of the client user
	* @return: void
	********************/
	bool Initialise(IRenderer* _pRenderer , std::string _strUserName);

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
	* CreateCubeMesh: Creates a Cube Mesh with origin in its very center
	* @author: Callan Moore
	* @parameter: _fSize: Size of the cube mesh to create from origin to edge
	* @parameter: _iTexID: ID of the Texure for the cube mesh
	* @return: CMesh*: Pointer to a created Cube Mesh
	********************/
	CMesh* CreateCubeMesh(float _fSize, int _iTexID);

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

	/***********************
	* CreateAvatarAsset: Create all required assets for the creation of Avatars
	* @author: Callan Moore
	* @return: void
	********************/
	void CreateAvatarAsset();

	/***********************
	* CreateDemonAsset: Create all required assets for the creation of Demon Enemies
	* @author: Callan Moore
	* @return: void
	********************/
	void CreateDemonAsset();

	/***********************
	* CreateHealthAsset: Create all required assets for the creation of Health PowerUps
	* @author: Callan Moore
	* @return: void
	********************/
	void CreateHealthAsset();

	/***********************
	* SpawnEnemy: Spawn a new enemy
	* @author: Callan Moore
	* @parameter: _pServerPacket: Packet containing the details about the new enemy
	* @return: void
	********************/
	void SpawnEnemy(ServerToClient* _pServerPacket);

	/***********************
	* DeleteEnemy: Delete an existing enemy
	* @author: Callan Moore
	* @parameter: _pServerPacket: Packet containing the details about the enemy to delete
	* @return: void
	********************/
	void DeleteEnemy(ServerToClient* _pServerPacket);

	/***********************
	* SpawnPower: Spawn a new PowerUp
	* @author: Callan Moore
	* @parameter: _pServerPacket: Packet containing the details about the new PowerUp
	* @return: void
	********************/
	void SpawnPower(ServerToClient* _pServerPacket);

	/***********************
	* DeletePower: Delete an existing PowerUp
	* @author: Callan Moore
	* @parameter: _pServerPacket: Packet containing the details about the PowerUp to delete
	* @return: void
	********************/
	void DeletePower(ServerToClient* _pServerPacket);


private:
	// Member Variables
	IRenderer* m_pRenderer;
	ServerToClient* m_pServerPacket;
	float m_fDT;

	std::string m_strUserName;
	CTerrain* m_pTerrain;
	CStaticCamera* m_pCamera;

	// TO DO - Create Blank material and texture
	// Put safe guard on draw methods if tex or mat ID = -1

	// Avatars
	CMesh* m_pAvatarMesh;
	int m_iAvatarMaterialID;
	int m_iAvatarTexID;
	std::map<std::string, CAvatar*>* m_pAvatars;

	// Enemies
	std::map<UINT, CEnemy*>* m_pEnemies;
	CMesh* m_pDemonMesh;
	int m_iDemonMaterialID;
	int m_iDemonTexID;

	// PowerUps
	std::map<UINT, CPowerUp*>* m_pPowerUps;
	CMesh* m_pHealthMesh;
	int m_iHealthMaterialID;
	int m_iHealthTexID;

	
};

#endif // __MECHANICSCLIENT_H__