#pragma once
#include "../../../SDK/SDK.hpp"

class C_ThirdPerson
{
public:
	virtual void Instance( C_ViewSetup* pViewSetup );
private:
	// thirdperson animation progress
	float m_flThirdpersonDistance = 0.0f;

	// should we reset the observer mode?
	bool m_bReset = false;
};

inline C_ThirdPerson* g_ThirdPerson = new C_ThirdPerson( );