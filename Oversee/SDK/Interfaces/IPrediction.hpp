#pragma once
#include "../Misc/VFunc.hpp"
#include "../Misc/MoveData.hpp"
#include "IGameMovement.hpp"
#include "IMoveHelper.hpp"

class C_BasePlayer;
class C_UserCmd;

class IPrediction
{
public:
	inline bool& m_bInPrediction( )
	{
		return *( bool* )( ( uintptr_t )( this ) + 0x8 );
	}
	inline bool& m_bIsFirstTimePredicted( )
	{
		return *( bool* )( ( uintptr_t )( this ) + 0x18 );
	}
	inline bool& m_bIsEnginePaused( )
	{
		return *( bool* )( ( uintptr_t )( this ) + 0xA );
	}
	inline bool& m_bPreviousAckHadErrors( )
	{
		return *( bool* )( ( uintptr_t )( this ) + 0x24 );
	}
	inline bool& m_bPerformedTickShift( )
	{
		return *( bool* ) ( ( uintptr_t ) ( this ) + 0x48 );
	}
	inline int& m_iPredictedCommands( )
	{
		return *( int* )( ( uintptr_t )( this ) + 0x1C );
	}
	inline int& m_iPreviousStartFrame( )
	{
		return *( int* )( ( uintptr_t )( this ) + 0xC );
	}
	inline int& m_nServerCommandsAcknowledged( )
	{
		return *( int* )( ( uintptr_t )( this ) + 0x20 );
	}

	// vfuncs
	VFunc( Update( int iServerTick, bool bIsValid, int iAcknowledged, int iOutgoingCmd ), 3, void( __thiscall* )( void*, int, bool, int, int ), iServerTick, bIsValid, iAcknowledged, iOutgoingCmd );
	VFunc( SetupMove( C_BasePlayer* pPlayer, C_UserCmd* pCmd, IMoveHelper* pMoveHelper, C_MoveData* MoveData ), 20, void( __thiscall* )( void*, void*, void*, void*, C_MoveData* ), pPlayer, pCmd, pMoveHelper, MoveData );
	VFunc( FinishMove( C_BasePlayer* pPlayer, C_UserCmd* pCmd, C_MoveData* MoveData ), 21, void( __thiscall* )( void*, void*, void*, C_MoveData* ), pPlayer, pCmd, MoveData );
	VFunc( RunCommand( C_BasePlayer* pPlayer, C_UserCmd* pCmd, IMoveHelper* pMoveHelper ), 19, void( __thiscall* )( void*, void*, void*, IMoveHelper* ), pPlayer, pCmd, pMoveHelper );
	VFunc( CheckMovingGround( C_BasePlayer* pPlayer, double flFrameTime ), 18, void( __thiscall* )( void*, void*, double ), pPlayer, flFrameTime );
};