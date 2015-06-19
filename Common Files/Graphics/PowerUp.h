#pragma once
#include "C3DObject.h"
class CPowerUp :
	public C3DObject
{
public:
	CPowerUp(ePowerUpType _eType);
	~CPowerUp();

private:
	ePowerUpType m_eType;
};

