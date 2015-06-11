/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Graphics_Defines.h
* Description : Declaration file to holds structs, enums and various containers/variables to be shared among the Graphics files
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __GRAPHICSDEFINES_H__
#define __GRAPHICSDEFINES_H__

// Local Includes
#include "Generic_Defines.h"

// Library Includes
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#include <d3dx9.h>

// Types
typedef long VertexType;



enum eIGPrimitiveType
{
	INVALID_IGPT,
	IGPT_POINTLIST,
	IGPT_LINELIST,
	IGPT_LINESTRIP,
	IGPT_TRIANGLELIST,
	IGPT_TRIANGLESTRIP,
	IGPT_TRIANGLEFAN
};

enum eIGIndexType
{
	IGIT_NONE,
	IGIT_16,
	IGIT_32
};

enum eCameraType
{
	TOPDOWN,
	FIRSTPERSON,
	DEBUG
};

// Structs
struct VertexScalar
{
	float Width;	// X
	float Height;	// Y
	float Depth;	// Z
};

struct MaterialComponents
{
	float fAmbientRed;
	float fAmbientGreen;
	float fAmbientBlue;
	float fAmbientAlpha;

	float fDiffuseRed;
	float fDiffuseGreen;
	float fDiffuseBlue;
	float fDiffuseAlpha;

	float fEmissiveRed;
	float fEmissiveGreen;
	float fEmissiveBlue;
	float fEmissiveAlpha;

	float fPower;

	float fSpecularRed;
	float fSpecularGreen;
	float fSpecularBlue;
	float fSpecularAlpha;
};

// Defines
#define VALIDATE(a) if (!a) return (false)

#endif //__GRAPHICSDEFINES_H__
