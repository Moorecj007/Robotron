/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Intelligent.cpp
* Description : Implementations for the functions of the Intelligent Object
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// Local Includes
#include "Intelligent.h"

CIntelligent::CIntelligent()
{
	m_iPointScore = 10;
	m_fMaxForce = 5;
	m_v3Acceleration = { 0.0f, 0.0f, 0.0f };
	m_v3DesiredVelocity = { 0.0f, 0.0f, 0.0f };
}

CIntelligent::~CIntelligent()
{

}