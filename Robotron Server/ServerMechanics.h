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

#pragma once

#ifndef __SERVER_MECHANICS_H__
#define __SERVER_MECHANICS_H__

// Library Incudes


// Local Includes
#include "../Common Files/Clock.h"
#include "../Common Files/Generic_Defines.h"

class CServerMechanics
{
public:
	// Constructors / Destructors

	/***********************
	* CServerMechanics: Default Constructor for Server Mechanics class
	* @author: Callan Moore
	********************/
	CServerMechanics();

	/***********************
	* ~CServerMechanics: Default Destructor for Server Mechanics class
	* @author: Callan Moore
	********************/
	~CServerMechanics();

	// Getters

	// Setters

	// Prototypes

	/***********************
	* Initialise: Initialise the Server mechanics class for use
	* @author: Callan Moore
	* @parameter: bool: Successful initialisation (or not)
	********************/
	bool Initialise();

	/***********************
	* Process: Process the mechanics on the server
	* @author: Callan Moore
	* @parameter: bool: Successful initialisation (or not)
	********************/
	void Process();




private:
	// Member Variables
	CClock* m_pClock;

};

#endif // __SERVER_MECHANICS_H__