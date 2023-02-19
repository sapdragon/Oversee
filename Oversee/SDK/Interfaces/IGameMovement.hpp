#pragma once
#include "../Misc/MoveData.hpp"
#include "../Misc/VFunc.hpp"

class C_BasePlayer;
class IGameMovement
{
public:
    inline C_BasePlayer*& m_Player( )
    {
        return *( C_BasePlayer** ) ( ( DWORD ) ( this ) + 0x4 );
    }
    inline C_MoveData*& m_MoveData( )
    {
        return *( C_MoveData** ) ( ( DWORD ) ( this ) + 0x8 );
    }
    inline Vector& m_vecForward( )
    {
        return *( Vector* ) ( ( DWORD ) ( this ) + 0x18 );
    }
    inline Vector& m_vecRight( )
    {
        return *( Vector* ) ( ( DWORD ) ( this ) + 0x24 );
    }
    inline Vector& m_vecUp( )
    {
        return *( Vector* ) ( ( DWORD ) ( this ) + 0x30 );
    }
    inline int& m_nTraceCount( )
    {
        return *( int* ) ( ( DWORD ) ( this ) + 0xE50 );
    }
    inline int& m_iSpeedCropped( )
    {
        return *( int* ) ( ( DWORD ) ( this ) + 0xC3C );
    }
    inline bool& m_bProcessingMovement( )
    {
        return *( bool* ) ( ( DWORD ) ( this ) + 0xC40 );
    }

    VFunc( ProcessMovement( C_BasePlayer* pPlayer, C_MoveData* pMoveData ), 1, void( __thiscall* )( void*, C_BasePlayer*, C_MoveData* ), pPlayer, pMoveData );
    VFunc( Reset( ), 2, void( __thiscall* )( void* ) );
    VFunc( GetPlayerMins( bool bDucked ), 6, Vector& ( __thiscall* )( void*, bool ), bDucked );
    VFunc( GetPlayerMaxs( bool bDucked ), 7, Vector& ( __thiscall* )( void*, bool ), bDucked );
    VFunc( GetPlayerViewOffset( bool bDucked ), 8, Vector&( __thiscall* )( void*, bool ), bDucked );
    VFunc( StartTrackPredictionErrors( C_BasePlayer* pPlayer ), 3, void( __thiscall* )( void*, C_BasePlayer* ), pPlayer );
    VFunc( FinishTrackPredictionErrors( C_BasePlayer* pPlayer ), 4, void( __thiscall* )( void*, C_BasePlayer* ), pPlayer );
};