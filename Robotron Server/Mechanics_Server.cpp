/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Mechanics_Server.cpp
* Description : Handles all the Mechanics on the server
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Includes
#include "Mechanics_Server.h"

CMechanics_Server::CMechanics_Server()
{
}

CMechanics_Server::~CMechanics_Server()
{
	// Delete the clock
	delete m_pClock;
	m_pClock = 0;
}

bool CMechanics_Server::Initialise()
{
	// Create and initialise the clock
	m_pClock = new CClock();
	m_pClock->Initialise();

	return true;
}

void CMechanics_Server::Process()
{
	m_pClock->Process();
	float fDT = m_pClock->GetDeltaTick();
}

void CMechanics_Server::AddAvatar(ClientToServer* _pClientPacket)
{
	
}

void CMechanics_Server::RemoveAvatar(std::string _strUserName)
{
	
}