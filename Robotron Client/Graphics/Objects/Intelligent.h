/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Intelligent.h
* Description : Declarations for the functions of the Intelligent Object
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __INTELLIGENT_H__
#define __INTELLIGENT_H__

// Local Includes
#include "C3DObject.h"

class CIntelligent :
	public C3DObject
{
public:
	// Constructors / Destructors

	/***********************
	* CIntelligent: Default Contructor for Intelligent class
	* @author: Callan Moore
	********************/
	CIntelligent();

	/***********************
	* ~CIntelligent: Default Destructor for Intelligent class
	* @author: Callan Moore
	********************/
	~CIntelligent();

	// Getters
	
	// Setters

	// Prototypes

protected:
	// Member Variables
};
#endif // __INTELLIGENT_H__