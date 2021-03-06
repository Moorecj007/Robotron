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
#include "Graphics/Mesh.h"
#include "Graphics/IRenderer.h"
#include "Graphics/Objects/Avatar.h"
#include "Graphics/Objects/Projectile.h"
#include "Graphics/Objects/Enemy.h"
#include "Graphics/Objects/PowerUp.h"
#include "Graphics/Objects/Flare.h"
#include "Graphics/Terrain.h"
#include "Graphics/Camera.h"
#include "Graphics/DebugCamera.h"

class CMechanics_Client
{
public:
	// Constructors / Destructors

	/***********************
	* CMechanics_Client: Default Constructor for GameMechanics class
	* @author: Callan Moore
	********************/
	CMechanics_Client();

	/***********************
	* ~CMechanics_Client: Default Destructor for GameMechanics class
	* @author: Callan Moore
	********************/
	~CMechanics_Client();

	// Getters

	/***********************
	* GetDebugCam: Retrieve the Debug Camera
	* @author: Callan Moore
	* @return: CDebugCamera*: Pointer to the Debug Camera
	********************/
	CDebugCamera* GetDebugCam() { return m_pDebugCam; };

	/***********************
	* Initialise: Initialise the GameMechanics for use
	* @author: Callan Moore
	* @parameter: _pRenderer: The pointer to the Renderer
	* @parameter: _strUserName: The Username of the client user
	* @parameter: _bSinglePlayer: Single player version of the game
	* @return: void
	********************/
	bool Initialise(IRenderer* _pRenderer, std::string _strUserName, bool _bSinglePlayer);

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
	* UpdateAvatars: Update the Avatar Objects from the Packet from the server
	* @author: Callan Moore
	* @return: void
	********************/
	void UpdateAvatars();

	/***********************
	* UpdateProjectiles: Update the Projectile Objects from the Packet from the server
	* @author: Callan Moore
	* @return: void
	********************/
	void UpdateProjectiles();

	/***********************
	* UpdateEnemies: Update the Enemy Objects from the Packet from the server
	* @author: Callan Moore
	* @return: void
	********************/
	void UpdateEnemies();

	/***********************
	* UpdatePowerUps: Update the PowerUp Objects from the Packet from the server
	* @author: Callan Moore
	* @return: void
	********************/
	void UpdatePowerUps();

	/***********************
	* UpdateFlare: Update the Flare from the Packet from the server
	* @author: Callan Moore
	* @return: void
	********************/
	void UpdateFlare();

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
	* CreateProjectileAsset: Create all required assets for the creation of Projectiles
	* @author: Callan Moore
	* @return: void
	********************/
	void CreateProjectileAsset();

	/***********************
	* CreateFlareAsset: Create all required assets for the creation of Flares
	* @author: Callan Moore
	* @return: void
	********************/
	void CreateFlareAsset();

	/***********************
	* CreateDemonAsset: Create all required assets for the creation of Demon Enemies
	* @author: Callan Moore
	* @return: void
	********************/
	void CreateDemonAsset();

	/***********************
	* CreateSentinelAsset: Create all required assets for the creation of Sentinel Enemies
	* @author: Callan Moore
	* @return: void
	********************/
	void CreateSentinelAsset();

	/***********************
	* CreateShadowAsset: Create all required assets for the creation of Shadow Enemies
	* @author: Callan Moore
	* @return: void
	********************/
	void CreateShadowAsset();

	/***********************
	* CreateHealthPowerAsset: Create all required assets for the creation of Health PowerUps
	* @author: Callan Moore
	* @return: void
	********************/
	void CreateHealthPowerAsset();

	/***********************
	* CreateFlarePowerAsset: Create all required assets for the creation of Flare PowerUps
	* @author: Callan Moore
	* @return: void
	********************/
	void CreateFlarePowerAsset();

	/***********************
	* CreateGoldenPowerAsset: Create all required assets for the creation of Golden PowerUps
	* @author: Callan Moore
	* @return: void
	********************/
	void CreateGoldenPowerAsset();

	

	/***********************
	* SpawnProjectile: Spawn a new projectile
	* @author: Callan Moore
	* @parameter: _pServerPacket: Packet containing the details about the new projectile
	* @return: void
	********************/
	void SpawnProjectile(ServerToClient* _pServerPacket);

	/***********************
	* DeleteProjectile: Delete an existing projectile
	* @author: Callan Moore
	* @parameter: _pServerPacket: Packet containing the details about the projectile to delete
	* @return: void
	********************/
	void DeleteProjectile(ServerToClient* _pServerPacket);

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
	void SpawnPowerUp(ServerToClient* _pServerPacket);

	/***********************
	* DeletePower: Delete an existing PowerUp
	* @author: Callan Moore
	* @parameter: _pServerPacket: Packet containing the details about the PowerUp to delete
	* @return: void
	********************/
	void DeletePowerUp(ServerToClient* _pServerPacket);

	/***********************
	* OverlayHUD: Render the Avatars HUD to the screen on top of the game image
	* @author: Callan Moore
	* @return: void
	********************/
	void OverlayHUD();

	/***********************
	* OverlayAvatarScores: Render all Avatars scores to the screen on top of the game image
	* @author: Callan Moore
	* @parameter: _fFPS: The frames per second
	* @return: void
	********************/
	void OverlayAvatarScores(int _iFPS);

	/***********************
	* OverlayPauseScreen: Render the Pause game screen over the game image
	* @author: Callan Moore
	* @parameter: _strMenuInput: String to contain the selected input
	* @parameter: _ptMouse: The current mouse cursor position
	* @parameter: _bAction: The current state of the action key/button
	* @return: void
	********************/
	void OverlayPauseScreen(std::string* strMenuInput, POINT _ptMouse, bool _bAction);

	/***********************
	* OverlayGameOver: Render the Game Over screen over the game image
	* @author: Callan Moore
	* @parameter: _strMenuInput: String to contain the selected input
	* @parameter: _ptMouse: The current mouse cursor position
	* @parameter: _bAction: The current state of the action key/button
	* @return: void
	********************/
	void OverlayGameOver(std::string* _strMenuInput, POINT _ptMouse, bool _bAction);

	/***********************
	* CheckAvatarsAliveStatus: Checks the status of the avatars and returns false if they are all dead
	* @author: Callan Moore
	* @return: void
	********************/
	bool CheckAvatarsAliveStatus();

	/***********************
	* ToggleDebugMode: Toggle Debug Mode on and off
	* @author: Callan Moore
	* @return: void
	********************/
	void ToggleDebugMode();

	/***********************
	* ToggleViewMode: Toggle between first and third person camera mode
	* @author: Callan Moore
	* @return: void
	********************/
	void ToggleViewMode();

	/***********************
	* InitialiseGraphicsResources: Initialise Graphics resources on the renderer
	* @author: Callan Moore
	* @parameters: _bResetDevice: Is the Device in need of reset?
	* @return: void
	********************/
	void InitialiseGraphicsResources(bool _bResetDevice);

private:
	// Member Variables
	IRenderer* m_pRenderer;
	ServerToClient* m_pServerPacket;
	float m_fDT;

	std::string m_strUserName;
	CTerrain* m_pTerrain;
	CCamera* m_pCamera;
	CDebugCamera* m_pDebugCam;
	bool m_bDebugMode;
	bool m_bFirstPerson;

	// Blank/Alpha Assets
	int m_iBlankTextureID;

	// Game Variables
	bool m_bToggle;
	bool m_bSinglePlayer;

	// Avatars
	std::map<std::string, CAvatar*>* m_pAvatars;
	CMesh* m_pAvatarMesh;
	int m_iAvatarMaterialID;
	int m_iAvatarTexID;

	// Projectiles
	std::map<UINT, CProjectile*>* m_pProjectiles;
	CMesh* m_pProjectileMesh;
	int m_iProjectileMaterialID;
	int m_iProjectileTexID;

	// Enemies
	std::map<UINT, CEnemy*>* m_pEnemies;
	CMesh* m_pDemonMesh;
	int m_iDemonMaterialID;
	int m_iDemonTexID;
	CMesh* m_pSentinelMesh;
	int m_iSentinelMaterialID;
	int m_iSentinelTexID;
	CMesh* m_pShadowMesh;
	int m_iShadowMaterialID;
	int m_iShadowTexID;

	// PowerUps
	std::map<UINT, CPowerUp*>* m_pPowerUps;
	CMesh* m_pHealthPowerMesh;
	int m_iHealthPowerMaterialID;
	int m_iHealthPowerTexID;
	CMesh* m_pFlarePowerMesh;
	int m_iFlarePowerMaterialID;
	int m_iFlarePowerTexID;
	CMesh* m_pGoldenPowerMesh;
	int m_iGoldenPowerMaterialID;
	int m_iGoldenPowerTexID;

	// Flare
	CFlare* m_pFlare;
	CMesh* m_pFlareMesh;
	int m_iFlareMaterialID;
	int m_iFlareTexID;
};

#endif // __MECHANICSCLIENT_H__