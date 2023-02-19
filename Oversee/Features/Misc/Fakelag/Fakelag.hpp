#pragma once
#include "../../../SDK/SDK.hpp"

enum EPacketMode : int
{
	Defualt = 0,
	Send,
	Choke
};
class C_FakeLag
{
public:
	virtual void Instance( );
	virtual void FakeDuck( );
	virtual void ResetData( )
	{
		// reset packet
		m_FakeLag.m_bFakeLag = g_SettingsManager->B[ _S( "misc.fake_lag.enable" ) ];
		m_FakeLag.m_nChokeAmount = g_SettingsManager->I[ _S( "misc.fake_lag.amount" ) ];
		m_FakeLag.m_nPacketMode = EPacketMode::Defualt;
		m_FakeLag.m_bIsChoking = false;
		m_FakeLag.m_nCustomChoke = -1;

		// set reset as true
		m_FakeLag.m_bReset = true;
	}

	inline auto GetData( )
	{
		return &m_FakeLag;
	}

	virtual void SetPacketMode( int nMode )
	{
		m_FakeLag.m_nPacketMode = nMode;
	}
	virtual void SetChokeAmount( int nChokeAmount )
	{
		m_FakeLag.m_nCustomChoke = nChokeAmount;
	}
	virtual int GetChokedPackets( )
	{
		return SDK::Interfaces::ClientState->m_nChokedCommands;
	}
private:
	struct
	{
		Vector m_vecNetworkedOrigin = Vector( 0, 0, 0 );

		int m_nCustomChoke = -1;
		int m_nChokedCommands = 0;
		int m_nChokeAmount = 0;
		int m_nPacketMode = EPacketMode::Defualt;

		bool m_bIsChoking = false;
		bool m_bReset = true;
		bool m_bPeeking = false;
		bool m_bFakeLag = false;
	} m_FakeLag;
};

inline C_FakeLag* g_FakeLag = new C_FakeLag( );