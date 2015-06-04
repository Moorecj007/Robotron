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

/***********************
* CMySemaphore: Default Constructor for the MySemaphore Class
* @author: Callan Moore
* @return:
********************/
CMySemaphore::CMySemaphore()
{
	m_iValue = 0;
}

/***********************
* CMySemaphore: Constructor taking in a value for the MySemaphore Class
* @author: Callan Moore
* @return:
********************/
CMySemaphore::CMySemaphore(int _iValue)
{
	m_iValue = _iValue;
}

/***********************
* ~CMySemaphore: Default Destructor for the MySemaphore Class
* @author: Callan Moore
* @return:
********************/
CMySemaphore::~CMySemaphore()
{

}

/***********************
* Wait: Atomic operation that waits for the semaphore value to be positive and then decrements it by one.
* @author: Callan Moore
* @author: Asma Shakil
* @return: void
********************/
void CMySemaphore::Wait()
{
	// Value is the Critical section. Lock the Mutex with automatic unlocking
	std::unique_lock<std::mutex> _unqlckguard(m_Mutex);

	m_Condition.wait(_unqlckguard, [this](){return (m_iValue > 0); });
	
	m_iValue--;
}

/***********************
* Signal: Atomic operation that increments the semaphore value by one. Which when positive, wakes up a waiting Semaphore
* @author: Callan Moore
* @author: Asma Shakil
* @return: void
********************/
void CMySemaphore::Signal()
{
	std::lock_guard<std::mutex> _lckguard(m_Mutex);
	m_iValue++;
	m_Condition.notify_one();
}

/***********************
* isPositive: Checks if the Value is positive
* @author: Callan Moore
* @return: bool: Returns whether the Value is positive
********************/
bool CMySemaphore::isPositive()
{
	return (m_iValue > 0);
}