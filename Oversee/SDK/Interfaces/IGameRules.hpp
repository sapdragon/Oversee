#pragma once
#include <cstdint>

enum EGameMode : int
{
	GAMEMODE_UNKNOWN = 0,
	GAMEMODE_CASUAL,
	GAMEMODE_COMPETITIVE,
	GAMEMODE_WINGMAN,
	GAMEMODE_ARMSRACE,
	GAMEMODE_DEMOLITION,
	GAMEMODE_DEATHMATCH,
	GAMEMODE_GUARDIAN,
	GAMEMODE_COOPSTRIKE,
	GAMEMODE_DANGERZONE
};

class IGameRules
{
public:
	inline bool IsFreezePeriod( )
	{
		return *( bool* )( ( DWORD )( this ) + 0x20 );
	}
	inline bool IsValveDS( )
	{
		return *( bool* )( ( DWORD )( this ) + 0x7C );
	}
	inline bool IsBombPlanted( )
	{
		return *( bool* )( ( DWORD )( this ) + 0x9A5 );
	}
};