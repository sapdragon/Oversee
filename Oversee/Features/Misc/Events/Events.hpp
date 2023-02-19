#pragma once
#include "../../../SDK/SDK.hpp"

class C_CheatEvents
{
public:
	virtual void HandleGameEvent( IGameEvent* m_GameEvent );
	virtual void ResetCheatData( );
private:
	int m_nVolume = 100;
};

inline C_CheatEvents* g_CheatEvents = new C_CheatEvents( );