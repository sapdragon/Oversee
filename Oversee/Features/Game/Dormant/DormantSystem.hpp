#pragma once
#include "../../../SDK/SDK.hpp"

struct C_SharedESPData;
class C_DormantSystem
{
public:
	virtual void OnItemEquip( IGameEvent* m_EventInfo );
	virtual void OnPostNetworkDataReceived( );
	
	virtual void OnRadarDataReceived( CCSUsrMsg_ProcessSpottedEntityUpdate_SpottedEntityUpdate* pMsg );
	virtual void OnSharedESPReceived( C_SharedESPData Data );

	virtual void ResetPlayer( C_BasePlayer* Player );
	virtual void ResetData( );

	virtual int GetDormantHealth( int nPlayerIndex );
	virtual int GetLastActiveWeapon( int nPlayerIndex );
	virtual int GetLastActiveWeaponType( int nPlayerIndex );
	virtual float GetTimeInDormant( int nPlayerIndex );
	virtual float GetTimeSinceLastData( int nPlayerIndex );
	virtual float GetLastDormantTime( int nPlayerIndex );
	virtual float GetLastActiveTime( int nPlayerIndex );
	virtual bool IsValidSoundData( SndInfo_t m_Sound );

	virtual Vector GetLastNetworkOrigin( int nPlayerIndex );
	virtual Vector GetLastDormantOrigin( int nPlayerIndex );
private:
	struct
	{
		CUtlVector < SndInfo_t > m_CurrentSoundData;
		CUtlVector < SndInfo_t > m_CachedSoundData;
	} m_SoundData;

	std::array < DormantPlayer_t, 64 > m_DormantPlayers;
};

inline C_DormantSystem* g_DormantSystem = new C_DormantSystem( );