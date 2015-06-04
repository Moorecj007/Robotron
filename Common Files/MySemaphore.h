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
	CMySemaphore();
	CMySemaphore(int _iValue);
	~CMySemaphore();

	// Prototypes
	void Wait();
	void Signal();
	bool CMySemaphore::isPositive();

private:
	// Member Variables
	int m_iValue;
	std::condition_variable m_Condition; 
	std::mutex m_Mutex;
};

#endif // __MYSEMAPHORE_H__