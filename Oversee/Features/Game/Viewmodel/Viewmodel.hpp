#pragma once
#include "../../../SDK/SDK.hpp"

class C_ViewmodelAnimation
{
public:
	virtual void SaveViewmodelData( );
	virtual void ForceViewmodelData( );
	virtual void ResolveSequences( );
	virtual void ModifyOrigin( );
private:
	struct
	{
		float m_flCycle = 0.f;
		float m_flAnimTime = 0.f;

		int m_nSequence = 0;
		int m_nAnimationParity = 0;

		DWORD m_dwWeaponLong = 0;
	} m_Data;
};

inline C_ViewmodelAnimation* g_ViewmodelAnimation = new C_ViewmodelAnimation( );