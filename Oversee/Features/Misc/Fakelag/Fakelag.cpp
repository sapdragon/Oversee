#include "Fakelag.hpp"
#include "../../Game/RageBot/AutoStop.hpp"
#include "../../Game/Prediction/EnginePrediction.hpp"
#include "../../Game/Networking/Networking.hpp"

void C_FakeLag::Instance( )
{
	if ( !g_Globals->m_Packet.m_bIsValidPacket )
	{
		*g_Globals->m_Packet.m_bSendPacket = true;
		return;
	}

	bool bIsInAttack = false;

	C_BaseCombatWeapon* pCombatWeapon = g_Globals->m_LocalPlayer->m_hActiveWeapon( );
	if ( pCombatWeapon )
	{
		if ( pCombatWeapon->IsGrenade( ) )
		{
			if ( pCombatWeapon->m_fThrowTime( ) > 0.0f && !pCombatWeapon->m_bPinPulled( ) )
				bIsInAttack = true;
		}
		else
		{
			if ( g_EnginePrediction->CanAttack( true ) )
				if ( ( g_Globals->m_Packet.m_pCmd->m_nButtons & IN_ATTACK ) || ( ( g_Globals->m_Packet.m_pCmd->m_nButtons & IN_ATTACK2 ) && pCombatWeapon->IsKnife( ) ) )
					bIsInAttack = true;
		}
	}

	m_FakeLag.m_nChokeAmount = 0;
	if ( g_SettingsManager->B[ _S( "antiaims.enable" ) ] )
		m_FakeLag.m_nChokeAmount = 1;

	if ( m_FakeLag.m_bFakeLag )
		m_FakeLag.m_nChokeAmount = max( g_SettingsManager->I[ _S( "misc.fake_lag.amount" ) ], 1 );

	if ( m_FakeLag.m_nCustomChoke >= 0 )
		m_FakeLag.m_nChokeAmount = m_FakeLag.m_nCustomChoke;

	if ( g_Globals->m_Packet.m_bFakeDucking )
	{
		m_FakeLag.m_bIsChoking = true;
		return;
	}

	bool bLocalFrozen = false;
	if ( g_Globals->m_LocalPlayer->IsFrozen( ) )
		bLocalFrozen = true;

	/* 06.02.2022 fix antiaim with exploits after respawn */
	if ( g_Networking->GetMaximumChoke( ) == 1 )
		m_FakeLag.m_bReset = false;

	m_FakeLag.m_nChokedCommands = SDK::Interfaces::ClientState->m_nChokedCommands;
	if ( m_FakeLag.m_bReset )
	{
		if ( m_FakeLag.m_nChokedCommands < 14 )
		{
			*g_Globals->m_Packet.m_bSendPacket = false;
			return;
		}

		m_FakeLag.m_bReset = false;
	}

	if ( m_FakeLag.m_bPeeking )
		m_FakeLag.m_nChokeAmount = 14;

	if (
		m_FakeLag.m_nChokedCommands >= g_Networking->GetMaximumChoke( )
		||
		m_FakeLag.m_nPacketMode == EPacketMode::Send
		||
		( m_FakeLag.m_nChokedCommands >= m_FakeLag.m_nChokeAmount && m_FakeLag.m_nPacketMode != EPacketMode::Choke )
		||
		( g_Globals->m_LocalPlayer->m_vecOrigin( ) - m_FakeLag.m_vecNetworkedOrigin ).Length2DSqr( ) > 4096.0f
		||
		bIsInAttack
		||
		( g_Globals->m_LocalPlayer->m_vecVelocity( ).Length2D( ) < 5.0f && m_FakeLag.m_nPacketMode != EPacketMode::Choke && ( SDK::Interfaces::ClientState->m_nChokedCommands > 0 || !g_SettingsManager->B[ _S( "antiaims.enable" ) ] ) )
			||
			bLocalFrozen
			|| ( g_AutoStop->IsPeeking( ) && !m_FakeLag.m_bPeeking )
			)
	{
		m_FakeLag.m_bIsChoking = false;
		if ( m_FakeLag.m_bPeeking )
			m_FakeLag.m_bPeeking = false;
		else if ( g_AutoStop->IsPeeking( ) )
			m_FakeLag.m_bPeeking = true;

		*g_Globals->m_Packet.m_bSendPacket = true;
		m_FakeLag.m_vecNetworkedOrigin = g_Globals->m_LocalPlayer->m_vecOrigin( );
	}
	else
	{
		m_FakeLag.m_bIsChoking = true;
		*g_Globals->m_Packet.m_bSendPacket = false;
	}
}
void C_FakeLag::FakeDuck( )
{
	if ( g_Globals->m_LocalPlayer->IsFrozen( ) || !( g_Globals->m_LocalPlayer->m_fFlags( ) & FL_ONGROUND ) || !g_Globals->m_Packet.m_bIsValidPacket )
	{
		g_Globals->m_Packet.m_bFakeDucking = false;
		return;
	}

	bool bShouldFakeDuck = g_SettingsManager->B[ _S( "misc.fake_duck" ) ];
	bool bFakeDuckBackup = g_Globals->m_Packet.m_bFakeDucking;

	if ( bShouldFakeDuck )
		g_Globals->m_Packet.m_bVisualFakeDucking = true;
	else if ( g_Globals->m_LocalPlayer->m_flDuckAmount( ) == 0.0f || g_Globals->m_LocalPlayer->m_flDuckAmount( ) >= 1.0f )
		g_Globals->m_Packet.m_bVisualFakeDucking = false;

	g_Globals->m_Packet.m_bFakeDucking = bShouldFakeDuck;
	if ( !bFakeDuckBackup && bShouldFakeDuck )
	{
		g_Globals->m_Packet.m_bFakeDucking = true;
		if ( g_FakeLag->GetChokedPackets( ) )
			g_Globals->m_Packet.m_bFakeDucking = false;
		else
			g_Globals->m_Packet.m_pCmd->m_nButtons &= ~IN_DUCK;

		return;
	}
	else if ( !bShouldFakeDuck )
	{
		float m_flAwaitedDuck = ( g_Globals->m_Packet.m_pCmd->m_nButtons & IN_DUCK ) ? 1.0f : 0.0f;
		if ( m_flAwaitedDuck != g_Globals->m_LocalPlayer->m_flDuckAmount( ) )
		{
			g_Globals->m_Packet.m_pCmd->m_nButtons |= IN_BULLRUSH;

			if ( m_flAwaitedDuck < g_Globals->m_LocalPlayer->m_flDuckAmount( ) )
				g_Globals->m_Packet.m_pCmd->m_nButtons &= ~IN_DUCK;
			else
				g_Globals->m_Packet.m_pCmd->m_nButtons |= IN_DUCK;

			g_Globals->m_Packet.m_bFakeDucking = true;
		}

		if ( !g_Globals->m_Packet.m_bFakeDucking )
			return;
	}
	else
	{
		g_Globals->m_Packet.m_pCmd->m_nButtons |= IN_BULLRUSH;
		if ( g_FakeLag->GetChokedPackets( ) < 7 )
			g_Globals->m_Packet.m_pCmd->m_nButtons &= ~IN_DUCK;
		else
			g_Globals->m_Packet.m_pCmd->m_nButtons |= IN_DUCK;
	}

	if ( !bShouldFakeDuck )
	{
		g_Globals->m_Packet.m_bFakeDucking = false;
		return;
	}

	if ( g_FakeLag->GetChokedPackets( ) < 14 )
	{
		*g_Globals->m_Packet.m_bSendPacket = false;
		return;
	}

	*g_Globals->m_Packet.m_bSendPacket = true;
}