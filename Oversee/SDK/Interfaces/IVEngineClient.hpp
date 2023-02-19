#pragma once
#include "../Misc/VFunc.hpp"

#include "../Misc/NetChannel.hpp"
#include "../Misc/PlayerInfo.hpp"

#include "../Math/QAngle.hpp"
#include "../Math/VMatrix.hpp"
#include "../Math/Vector2D.hpp"
#include "../Math/Vector4D.hpp"

#define FLOW_OUTGOING	0		
#define FLOW_INCOMING	1
#define MAX_FLOWS		2		// in & out

class IVEngineClient
{
public:
	VFunc( FireEvents( ), 57, void ( __thiscall* )( void* ) );
	VFunc( GetBSPTreeQuery( ), 43, void* ( __thiscall* )( void* ) );
	VFunc( GetScreenSize(int& width, int& height), 5, void(__thiscall*)(void*, int&, int&), width, height);
	VFunc( GetLastTimeStamp(), 14, float(__thiscall*)(void*));
	VFunc( GetLocalPlayer( ), 12, int( __thiscall* )( void* ) );
	VFunc( GetPlayerForUserID( int nUserID ), 9, int( __thiscall* )( void*, int ), nUserID );
	VFunc( GetPlayerInfo( int nEntIndex, PlayerInfo_t* PlayerInfo ), 8, bool( __thiscall* )( void*, int, PlayerInfo_t* ), nEntIndex, PlayerInfo );
	VFunc( GetViewAngles( QAngle* angAngles ), 18, void( __thiscall* )( void*, QAngle* ), angAngles );
	VFunc( SetViewAngles( QAngle* angAngles ), 19, void( __thiscall* )( void*, QAngle* ), angAngles );
	VFunc( IsActiveApp( ), 211, bool( __thiscall* )( void* ) );
	VFunc( IsConsoleVisible( ), 11, bool( __thiscall* )( void* ) );
	VFunc( IsConnected( ), 27, bool( __thiscall* )( void* ) );
	VFunc( IsInGame( ), 26, bool( __thiscall* )( void* ) );
	VFunc( GetLevelName( ), 52, char const*( __thiscall* )( void* ) );
	VFunc( GetLevelNameShort( ), 53, char const*( __thiscall* )( void* ) );
	VFunc( GetNetChannel( ), 78, INetChannel*( __thiscall* )( void* ) );
	VFunc( ExecuteClientCmd( const char* szCmdString ), 108, void( __thiscall* )( void*, const char* ), szCmdString );

	const VMatrix& WorldToScreenMatrix( )
	{
		return reinterpret_cast < const VMatrix & ( __thiscall* )( void* ) >( GetVFunction_NoObf( this, 37 ) )( this );
	}
};
