/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : ServerMechanics.cpp
* Description : Handles all the Mechanics on the server
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Includes
#include "ServerMechanics.h"

CServerMechanics::CServerMechanics()
{
}


CServerMechanics::~CServerMechanics()
{
	// Delete the clock
	delete m_pClock;
	m_pClock = 0;
}

bool CServerMechanics::Initialise()
{
	// Create and initialise the clock
	m_pClock = new CClock();
	m_pClock->Initialise();

	return true;
}

void CServerMechanics::Process()
{
	m_pClock->Process();
	float fDT = m_pClock->GetDeltaTick();


}