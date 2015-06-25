#include "PowerUp.h"


CPowerUp::CPowerUp(ePowerUpType _eType)
{
	m_eType = _eType;
	switch (_eType)
	{
	case PT_HEALTH:
	{

		//m_iPointScore = 10;
		break;
	}
	default: break;
	}
}


CPowerUp::~CPowerUp()
{
}
