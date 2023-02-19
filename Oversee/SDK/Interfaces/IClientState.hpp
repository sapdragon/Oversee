#pragma once
#include <cstdint>
#include "../Misc/NetMessage.hpp"
#include "../Misc/EventInfo.hpp"
#include "../Misc/ClockDriftMgr.hpp"

#define MIN_SIMULATION_TICKS 1
#define MAX_SIMULATION_TICKS 17

enum SIGNONSTATE
{
	SIGNONSTATE_NONE = 0,	// no state yet; about to connect
	SIGNONSTATE_CHALLENGE = 1,	// client challenging server; all OOB packets
	SIGNONSTATE_CONNECTED = 2,	// client is connected to server; netchans ready
	SIGNONSTATE_NEW = 3,	// just got serverinfo and string tables
	SIGNONSTATE_PRESPAWN = 4,	// received signon buffers
	SIGNONSTATE_SPAWN = 5,	// ready to receive entity packets
	SIGNONSTATE_FULL = 6,	// we are fully connected; first non-delta packet received
	SIGNONSTATE_CHANGELEVEL = 7,	// server is changing level; please wait
};
class IClientState
{
public:
	void ForceFullUpdate( )
	{
		m_nDeltaTick = -1;
	}
	char pad_0000[ 156 ];
	void* m_NetChannel;
	int m_nChallengeNr;
	char pad_00A4[ 100 ];
	int m_nSignonState;
	int signon_pads[ 2 ];
	float m_flNextCmdTime;
	int m_nServerCount;
	int m_nCurrentSequence;
	int musor_pads[ 2 ];
	C_ClockDriftMgr m_ClockDriftMgr;
	int m_nDeltaTick;
	bool m_bPaused;
	char paused_align[ 3 ];
	int m_nViewEntity;
	int m_nPlayerSlot;
	int bruh;
	char m_szLevelName[ 260 ];
	char m_szLevelNameShort[ 80 ];
	char m_szGroupName[ 80 ];
	char pad_032[ 92 ];
	int m_nMaxClients;
	char pad_0314[ 18828 ];
	float m_nLastServerTickTime;
	bool m_bInSimulation;
	char pad_4C9D[ 3 ];
	int m_nOldTickCount;
	float m_flTickReminder;
	float m_flFrametime;
	int m_nLastOutgoingCommand;
	int m_nChokedCommands;
	int m_nLastCommandAck;
	int m_nPacketEndTickUpdate;
	int m_nCommandAck;
	int m_nSoundSequence;
	char pad_4CCD[ 76 ];
	QAngle m_angViewAngles;
	int pads[ 54 ];
	C_EventInfo* m_pEventInfo;

	bool& m_bIsHLTV( )
	{
		return *( bool* )( ( uintptr_t )( this ) + 0x4D48 );
	}
};