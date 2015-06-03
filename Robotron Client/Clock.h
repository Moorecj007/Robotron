/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Clock.h
* Description : Declarations for the functions of the Clock class
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __CLOCK_H__
#define __CLOCK_H__

class CClock
{
public:
	// Constructors / Destructors

	/***********************
	* CClock: Default Contructor for Clock class
	* @author: Callan Moore
	********************/
	CClock();

	/***********************
	* ~CClock: Default Destructor for Clock class
	* @author: Callan Moore
	********************/
	~CClock();

	// Prototypes

	/***********************
	* Initialise: Initialise the Clock class
	* @author: Callan Moore
	* @return: bool: Confirmation that the Initialise was successful
	********************/
	bool Initialise();

	/***********************
	* Process: Processes the Clock
	* @author: Callan Moore
	* @return: void
	********************/
	void Process();

	/***********************
	* GetDeltaTick: Retrieves the current delta tick
	* @author: Callan Moore
	* @return: float: One delta tick worth of time
	********************/
	float GetDeltaTick();

private:
	// Disallowing Copies
	CClock(const CClock& _kr);
	CClock& operator= (const CClock& _kr);
	
protected:
	// Member Variables
	float m_fTimeElapsed;
	float m_fDeltaTime;
	float m_fLastTime;
	float m_fCurrentTime;
};

#endif // __CLOCK_H__