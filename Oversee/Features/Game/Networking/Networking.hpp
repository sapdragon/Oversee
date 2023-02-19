#pragma once
#include "../../../SDK/SDK.hpp"

class C_Networking
{
public:
	virtual void StartNetwork( );
	virtual void FinishNetwork( );
	virtual void PingReducer( );
	virtual void FinishPacket( int iSequence );
	virtual void OnFakeDuck( );
	virtual void OnNetShowFragments( DWORD dwReturnAddress );
	virtual void OnSendDatagram( int nOutSequenceNr ) 
	{
		PushCommand( nOutSequenceNr );
	};
	virtual void PushCommand( int nOutSequenceNr )
	{
		m_NetworkData.m_SequenceList.push_back( nOutSequenceNr );
	}

	virtual void SetMaxChoke( int nMaxChoke ) { m_NetworkData.m_nMaximumChoke = nMaxChoke; };
	virtual bool PacketStart( int nCommandAck );
	virtual bool ReadPackets( ) { return m_NetworkData.m_bReadPackets; };

	virtual int GetLagCompensationTick( );
	virtual int GetMaximumChoke( );
	virtual int GetTickRate( );
	virtual int GetServerTick( );
	virtual float GetLatency( );
	virtual void CleanSequenceList( )
	{
		m_NetworkData.m_SequenceList.clear( );
	}
	virtual void ResetData( )
	{
		m_NetworkData.m_nSequence = 0;
		m_NetworkData.m_nMaximumChoke = 0;
		m_NetworkData.m_nServerTick = 0;
		m_NetworkData.m_nCompensatedServerTick = 0;
		m_NetworkData.m_nTickRate = 0;
		m_NetworkData.m_nLatestCommand = 0;
		m_NetworkData.m_nLastFragment = 0;
		m_NetworkData.m_fLatency = 0.0f;
		m_NetworkData.m_bReadPackets = true;
		m_NetworkData.m_bSkipDatagram = false;
		m_NetworkData.m_SequenceList.clear( );
	}
private:
	struct
	{
		int m_nSequence = 0;
		int m_nMaximumChoke = 0;
		int m_nServerTick = 0;
		int m_nCompensatedServerTick = 0;
		int m_nTickRate = 0;
		int m_nLatestCommand = 0;
		int m_nLastFragment = 0;

		float m_fLatency = 0.0f;
		bool m_bReadPackets = true;
		bool m_bSkipDatagram = false;

		std::vector < int > m_SequenceList;
	} m_NetworkData;
};

inline C_Networking* g_Networking = new C_Networking( );