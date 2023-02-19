#pragma once
#include "../Misc/VFunc.hpp"
#include "../Misc/UserCmd.hpp"

#define MULTIPLAYER_BACKUP 150

class bf_write;
class bf_read;

class IInput
{
public:
	char pad0[ 0xC ]; // 0x0
	bool m_bTrackIrAvailable; // 0xC
	bool m_bMouseInitialized; // 0xD
	bool m_bMouseActive; // 0xE
	char pad1[ 0x9E ]; // 0xF
	bool m_bCameraInThirdPerson; // 0xAD
	char pad2[ 0x2 ]; // 0xAE
	Vector m_vecCameraOffset; // 0xB0
	char pad3[ 0x38 ]; // 0xBC
	C_UserCmd* m_Commands; // 0xF4
	C_VerifiedUserCmd* m_VerifiedCommands; // 0xF8

	VFunc( GetUserCmd( int nSequence ), 8, C_UserCmd* ( __thiscall* )( void*, int, int ), 0, nSequence );
	VFunc( GetUserCmd( int nSlot, int nSequence ), 8, C_UserCmd* ( __thiscall* )( void*, int, int ), nSlot, nSequence );

	C_VerifiedUserCmd* GetVerifiedCmd( int nSequence )
	{
		return &m_VerifiedCommands[ nSequence % MULTIPLAYER_BACKUP ];
	}
};