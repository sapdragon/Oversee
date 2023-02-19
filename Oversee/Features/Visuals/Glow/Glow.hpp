#pragma once
#include "../../../SDK/SDK.hpp"

class C_Glow
{
public:
	virtual void Instance( );
};

inline C_Glow* g_Glow = new C_Glow( );