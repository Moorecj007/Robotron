/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Clock.cpp
* Description : Keeps track of time using delta ticks
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Includes
#include "Clock.h"

CClock::CClock()	
{
	m_fTimeElapsed = 0.0f;
	m_fDeltaTime = 0.0f;
	m_fLastTime = 0.0f;
	m_fCurrentTime = 0.0f;
}

CClock::~CClock()
{
}

bool CClock::Initialise()
{
	return (true);
}

void CClock::Process()
{
	m_fLastTime = m_fCurrentTime;
	m_fCurrentTime = static_cast<float>(timeGetTime());
	if (m_fLastTime == 0.0f)
	{
		m_fLastTime = m_fCurrentTime;
	}
	m_fDeltaTime = m_fCurrentTime - m_fLastTime;
	m_fTimeElapsed += m_fDeltaTime;
}

float CClock::GetDeltaTick()
{
	// Return Delta Tick in Seconds
	return (m_fDeltaTime / 1000.0f);
}