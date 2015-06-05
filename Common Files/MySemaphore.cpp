/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : MySemaphore.cpp
* Description : Implementation for the functions of the MySemaphore class
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

//This include
#include "MySemaphore.h"

CMySemaphore::CMySemaphore()
{
	m_iValue = 0;
}

CMySemaphore::CMySemaphore(int _iValue)
{
	m_iValue = _iValue;
}

CMySemaphore::~CMySemaphore()
{

}

void CMySemaphore::Wait()
{
	// Value is the Critical section. Lock the Mutex with automatic unlocking
	std::unique_lock<std::mutex> _unqlckguard(m_Mutex);

	m_Condition.wait(_unqlckguard, [this](){return (m_iValue > 0); });
	
	m_iValue--;
}

void CMySemaphore::Signal()
{
	std::lock_guard<std::mutex> _lckguard(m_Mutex);
	m_iValue++;
	m_Condition.notify_one();
}

bool CMySemaphore::isPositive()
{
	return (m_iValue > 0);
}