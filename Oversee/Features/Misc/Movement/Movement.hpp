#pragma once
#include "../../../SDK/SDK.hpp"

class C_Movement
{
public:
	virtual void BunnyHop( );
	virtual void AutoStrafe( );
	virtual void MouseCorrections( );
	virtual void EdgeJump( );
	virtual void FastAutoStop( );
	virtual void QuickStop( );
};

inline C_Movement* g_Movement = new C_Movement();