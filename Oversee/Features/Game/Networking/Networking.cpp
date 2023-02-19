#include "Networking.hpp"
#include "../../Misc/Movement/Movement.hpp"
#include "../Exploits/Exploits.hpp"
#include "../RageBot/AntiAim.hpp"
#include "../RageBot/LagCompensation.hpp"
#include "../RageBot/LocalAnimations.hpp"
#include "../Prediction/EnginePrediction.hpp"
#include "../Viewmodel/Viewmodel.hpp"
#include "../../../Hooks/Hooks.hpp"
#include "../../../SDK/Utils/Utils.hpp"

void C_Networking::FinishPacket( int iSequence )
{
	/* don't overchoke */
	if ( SDK::Interfaces::ClientState->m_nChokedCommands >= m_NetworkData.m_nMaximumChoke )
		*g_Globals->m_Packet.m_bSendPacket = true;
	
	/* run datagram fix */
	if ( !*g_Globals->m_Packet.m_bSendPacket )
		m_NetworkData.m_bSkipDatagram = false;

	/* fix movement */
	{
		Math::Normalize3( g_Globals->m_Packet.m_pCmd->m_angViewAngles );
		Math::ClampAngles( g_Globals->m_Packet.m_pCmd->m_angViewAngles );
		Math::FixMovement( g_Globals->m_Packet.m_pCmd );
	}

	/* verify cmd ( it is const after this code part and couldn't be changed without new verification )*/
	{
		C_VerifiedUserCmd* m_pVerifiedCmd = SDK::Interfaces::Input->GetVerifiedCmd( g_Globals->m_Packet.m_nSequence );
		if ( m_pVerifiedCmd )
		{
			m_pVerifiedCmd->m_Cmd = *g_Globals->m_Packet.m_pCmd;
			m_pVerifiedCmd->m_CRC = m_pVerifiedCmd->m_Cmd.GetChecksum( );
		}
	}

	// restore all players affected by hitscan in ragebot
	g_LagCompensation->FinishLagCompensation( );

	// run animations and restore prediction
	g_LocalAnimations->OnCreateMove( );

	// reset useful variable used for a several fixes
	g_Globals->m_Packet.m_bInCreateMove = false;
}
void C_Networking::PingReducer( )
{
	/* Check SignonState */
	if ( !SDK::Interfaces::ClientState || SDK::Interfaces::ClientState->m_nSignonState < SIGNONSTATE_FULL )
	{
		m_NetworkData.m_bReadPackets = true;
		return;
	}

	/* Check localplayer */
	if ( !g_Globals->m_LocalPlayer || !g_Globals->m_LocalPlayer->IsAlive( ) )
	{
		m_NetworkData.m_bReadPackets = true;
		return;
	}

	/* Save viewmodel animation data */
	g_ViewmodelAnimation->SaveViewmodelData( );

	/* Check netchannel, so skip local servers */
	INetChannel* m_NetChannel = SDK::Interfaces::EngineClient->GetNetChannel( );
	if ( !m_NetChannel || m_NetChannel->IsLoopback( ) )
	{
		m_NetworkData.m_bReadPackets = true;
		return;
	}

	/* save players origins */
	std::array < Vector, 64 > m_Origins;
	for ( int nPlayer = 1; nPlayer <= SDK::Interfaces::ClientState->m_nMaxClients; nPlayer++ )
	{
		C_BasePlayer* Player = C_BasePlayer::GetPlayerByIndex( nPlayer );
		if ( !Player || !Player->IsAlive( ) )
			continue;

		m_Origins[ nPlayer - 1 ] = Player->GetAbsOrigin( );
	}

	/* call CL_ReadPackets */
	m_NetworkData.m_bReadPackets = true;
	( ( Hooks::CL_ReadPacket_t ) ( SDK::EngineData::m_AddressList[ CheatAddressList::CL_ReadPackets ] ) )( true );
	m_NetworkData.m_bReadPackets = false;

	/* restore players origins */
	for ( int nPlayer = 1; nPlayer <= SDK::Interfaces::ClientState->m_nMaxClients; nPlayer++ )
	{
		C_BasePlayer* Player = C_BasePlayer::GetPlayerByIndex( nPlayer );
		if ( !Player || !Player->IsAlive( ) )
			continue;

		Player->SetAbsOrigin( m_Origins[ nPlayer - 1 ] );
	}
}
void C_Networking::StartNetwork( )
{
	/* set networking data */
	m_NetworkData.m_nTickRate = ( int ) ( 1.0f / SDK::Interfaces::GlobalVars->m_flIntervalPerTick );
	m_NetworkData.m_nMaximumChoke = 14;
	m_NetworkData.m_bSkipDatagram = true;

	/* calc latency */
	INetChannel* m_NetChannel = SDK::Interfaces::EngineClient->GetNetChannel( );
	if ( m_NetChannel )
	{
		/* set latency */
		m_NetworkData.m_fLatency = m_NetChannel->GetLatency( FLOW_OUTGOING ) + m_NetChannel->GetLatency( FLOW_INCOMING );
		
		/* set sequence */
		m_NetworkData.m_nSequence = m_NetChannel->m_iOutSequenceNr;
	}

	/* set servertick */
	m_NetworkData.m_nServerTick = SDK::Interfaces::GlobalVars->m_iTickCount + g_Utils->TimeToTicks( m_NetworkData.m_fLatency );
	m_NetworkData.m_nCompensatedServerTick = m_NetworkData.m_nServerTick;
}
void C_Networking::FinishNetwork( )
{
	/* check player */
	if ( !SDK::Interfaces::ClientState || SDK::Interfaces::ClientState->m_nSignonState < SIGNONSTATE_FULL )
		return;

	/* Check localplayer */
	if ( !g_Globals->m_LocalPlayer || !g_Globals->m_LocalPlayer->IsAlive( ) || g_Globals->m_LocalPlayer->IsFrozen( ) )
		return;

	/* get netchannel */
	INetChannel* m_NetChannel = SDK::Interfaces::EngineClient->GetNetChannel( );
	if ( !m_NetChannel )
		return;

	/* skip datagram */
	if ( m_NetChannel->m_iChokedCommands % 4 || m_NetworkData.m_bSkipDatagram )
		return;

	/* run network fix */
	{
		/* store netchannel */
		int nSequenceNr = m_NetChannel->m_iOutSequenceNr;
		int nChokedCommands = m_NetChannel->m_iChokedCommands;

		/* fix net channel */
		m_NetChannel->m_iChokedCommands = 0;
		m_NetChannel->m_iOutSequenceNr = m_NetworkData.m_nSequence;

		/* send datagram */
		m_NetChannel->SendDatagram( nullptr );

		/* restore netchannel */
		m_NetChannel->m_iOutSequenceNr = nSequenceNr;
		m_NetChannel->m_iChokedCommands = nChokedCommands;
	}
}
void C_Networking::OnFakeDuck( )
{
	/* fakeduck servertick && maxchoke fix */
	if ( g_Globals->m_Packet.m_bFakeDucking )
	{
		m_NetworkData.m_nMaximumChoke = 14;
		m_NetworkData.m_nCompensatedServerTick = m_NetworkData.m_nServerTick + m_NetworkData.m_nMaximumChoke - SDK::Interfaces::ClientState->m_nChokedCommands;
	}
}
void C_Networking::OnNetShowFragments( DWORD dwReturnAddress )
{
	if ( !SDK::Interfaces::ClientState || !SDK::Interfaces::ClientState->m_NetChannel )
		return;

	uint32_t* m_NetData = &reinterpret_cast< uint32_t* >( SDK::Interfaces::ClientState->m_NetChannel )[ 0x56 ];
	if ( !m_NetData )
		return;

	if ( _rotr( dwReturnAddress, 32 ) == _rotr( ( DWORD )( SDK::EngineData::m_AddressList[ CheatAddressList::CheckReceivingList_Return ] ), 32 ) )
	{
		if ( m_NetworkData.m_nLastFragment > 0 )
		{
			uint32_t uBytesFragments = reinterpret_cast< uint32_t* >( m_NetData )[ 0x43 ];
			if ( uBytesFragments == m_NetworkData.m_nLastFragment )
				reinterpret_cast< uint32_t* >( m_NetData )[ 0x42 ] = 0;
		}
	}

	if ( _rotr( dwReturnAddress, 32 ) == _rotr( ( DWORD )( SDK::EngineData::m_AddressList[ CheatAddressList::ReadSubChannelData_Return ] ), 32 ) )
		m_NetworkData.m_nLastFragment = reinterpret_cast< uint32_t* >( m_NetData )[ 0x43 ];
}
float C_Networking::GetLatency( )
{
	return m_NetworkData.m_fLatency;
}
int C_Networking::GetMaximumChoke( )
{
	return m_NetworkData.m_nMaximumChoke;
}
int C_Networking::GetLagCompensationTick( )
{
	return m_NetworkData.m_nCompensatedServerTick;
}
int C_Networking::GetServerTick( )
{
	return m_NetworkData.m_nServerTick;
}
int C_Networking::GetTickRate( )
{
	return m_NetworkData.m_nTickRate;
}
bool C_Networking::PacketStart( int nCommandAck )
{
	/* Check clientstate and connection */
	if ( !SDK::Interfaces::ClientState || SDK::Interfaces::ClientState->m_nSignonState < SIGNONSTATE_FULL )
		return true;

	/* Check localplayer */
	if ( !g_Globals->m_LocalPlayer || !g_Globals->m_LocalPlayer->IsAlive( ) || g_Globals->m_LocalPlayer->IsFrozen( ) )
		return true;

	/* Process sequences */
	for ( auto Begin = m_NetworkData.m_SequenceList.begin( ); Begin != m_NetworkData.m_SequenceList.end( ); Begin++ )
	{
		if ( *Begin == nCommandAck )
		{
			m_NetworkData.m_SequenceList.erase( Begin );
			return true;
		}
	}

	return false;
}