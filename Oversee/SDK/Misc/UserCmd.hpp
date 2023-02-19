#pragma once
#include <cstdint>

#include "checksum_crc.hpp"
#include "checksum_md5.hpp"
#include "../Math/Vector.hpp"
#include "../Math/QAngle.hpp"

#define IN_ATTACK  (1 << 0)
#define IN_JUMP   (1 << 1)
#define IN_DUCK   (1 << 2)
#define IN_FORWARD  (1 << 3)
#define IN_BACK   (1 << 4)
#define IN_USE   (1 << 5)
#define IN_CANCEL  (1 << 6)
#define IN_LEFT   (1 << 7)
#define IN_RIGHT  (1 << 8)
#define IN_MOVELEFT  (1 << 9)
#define IN_MOVERIGHT (1 << 10)
#define IN_ATTACK2  (1 << 11)
#define IN_RUN   (1 << 12)
#define IN_RELOAD  (1 << 13)
#define IN_ALT1   (1 << 14)
#define IN_ALT2   (1 << 15)
#define IN_SCORE  (1 << 16)   // Used by client.dll for when scoreboard is held down
#define IN_SPEED  (1 << 17) // Player is holding the speed key
#define IN_WALK   (1 << 18) // Player holding walk key
#define IN_ZOOM   (1 << 19) // Zoom key for HUD zoom
#define IN_WEAPON1  (1 << 20) // weapon defines these bits
#define IN_WEAPON2  (1 << 21) // weapon defines these bits
#define IN_BULLRUSH  (1 << 22)
#define IN_GRENADE1  (1 << 23) // grenade 1
#define IN_GRENADE2  (1 << 24) // grenade 2
#define IN_LOOKSPIN  (1 << 25)

class C_UserCmd
{
public:
    CRC32_t GetChecksum( void ) const
    {
        CRC32_t crc;
        CRC32_Init( &crc );

        CRC32_ProcessBuffer( &crc, &m_nCommand, 4 );
        CRC32_ProcessBuffer( &crc, &m_nTickCount, 4 );
        CRC32_ProcessBuffer( &crc, &m_angViewAngles, sizeof( m_angViewAngles ) );
        CRC32_ProcessBuffer( &crc, &m_vecAimDirection, sizeof( m_vecAimDirection ) );
        CRC32_ProcessBuffer( &crc, &m_flForwardMove, sizeof( m_flForwardMove ) );
        CRC32_ProcessBuffer( &crc, &m_flSideMove, sizeof( m_flSideMove ) );
        CRC32_ProcessBuffer( &crc, &m_flUpMove, sizeof( m_flUpMove ) );
        CRC32_ProcessBuffer( &crc, &m_nButtons, sizeof( m_nButtons ) );
        CRC32_ProcessBuffer( &crc, &m_nImpulse, sizeof( m_nImpulse ) );
        CRC32_ProcessBuffer( &crc, &m_iWeaponSelect, sizeof( m_iWeaponSelect ) );
        CRC32_ProcessBuffer( &crc, &m_iWeaponSubType, sizeof( m_iWeaponSubType ) );
        CRC32_ProcessBuffer( &crc, &m_iRandomSeed, sizeof( m_iRandomSeed ) );
        CRC32_ProcessBuffer( &crc, &m_nMouseDirectionX, sizeof( m_nMouseDirectionX ) );
        CRC32_ProcessBuffer( &crc, &m_nMouseDirectionY, sizeof( m_nMouseDirectionY ) );

        CRC32_Final(&crc);
        return crc;
    }

    void*   m_pVTable;              // 0x00
    int     m_nCommand;             // 0x04
    int     m_nTickCount;           // 0x08
    QAngle  m_angViewAngles;        // 0x0C Player instantaneous view angles.
    Vector  m_vecAimDirection;      // 0x18
    float   m_flForwardMove;        // 0x24
    float   m_flSideMove;           // 0x28
    float   m_flUpMove;             // 0x2C
    int     m_nButtons;             // 0x30 Attack button states
    char    m_nImpulse;             // 0x34
    int     m_iWeaponSelect;        // 0x38 Current weapon id
    int     m_iWeaponSubType;       // 0x3C
    int     m_iRandomSeed;          // 0x40 For shared random functions
    short   m_nMouseDirectionX;     // 0x44 mouse accum in x from create move
    short   m_nMouseDirectionY;     // 0x46 mouse accum in y from create move
    bool    m_bHasBeenPredicted;    // 0x48 Client only, tracks whether we've predicted this command at least once
    char    pad[ 0x14 ];
    int     m_nTickBase;
}; // sizeof 0x60

class C_VerifiedUserCmd
{
public:
    C_UserCmd m_Cmd;
    CRC32_t  m_CRC;
};