/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : MySemaphore.h
* Description : Declarations for the functions of the MySemaphore class
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __MYSEMAPHORE_H__
#define __MYSEMAPHORE_H__

//Library includes
#include <condition_variable>
#include <mutex>

class CMySemaphore
{
public:
	// Constructors / Destructors

	/***********************
	* CMySemaphore: Default Constructor for the MySemaphore Class
	* @author: Callan Moore
	********************/
	CMySemaphore();

	/***********************
	* CMySemaphore: Constructor taking in a value for the MySemaphore Class
	* @author: Callan Moore
	********************/
	CMySemaphore(int _iValue);

	/***********************
	* ~CMySemaphore: Default Destructor for the MySemaphore Class
	* @author: Callan Moore
	********************/
	~CMySemaphore();

	// Prototypes

	/***********************
	* Wait: Atomic operation that waits for the semaphore value to be positive and then decrements it by one.
	* @author: Callan Moore
	* @author: Asma Shakil
	* @return: void
	********************/
	void Wait();

	/***********************
	* Signal: Atomic operation that increments the semaphore value by one. Which when positive, wakes up a waiting Semaphore
	* @author: Callan Moore
	* @author: Asma Shakil
	* @return: void
	********************/
	void Signal();

	/***********************
	* isPositive: Checks if the Value is positive
	* @author: Callan Moore
	* @return: bool: Returns whether the Value is positive
	********************/
	bool CMySemaphore::isPositive();

private:
	// Member Variables
	int m_iValue;
	std::condition_variable m_Condition; 
	std::mutex m_Mutex;
};

#endif // __MYSEMAPHORE_H__