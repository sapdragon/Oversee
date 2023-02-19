#include "LagCompensation.hpp"
#include "Animations.hpp"
#include "../Networking/Networking.hpp"
#include "../Exploits/Exploits.hpp"
#include "../Prediction/EnginePrediction.hpp"
#include "../../../SDK/Utils/Utils.hpp"

void C_LagCompensation::Instance( ClientFrameStage_t Stage )
{
	if ( Stage != ClientFrameStage_t::FRAME_NET_UPDATE_END )
		return;

	/* Iterate all players */
	for ( int nPlayerID = 1; nPlayerID <= SDK::Interfaces::ClientState->m_nMaxClients; nPlayerID++ )
	{
		/* Get player data of this player */
		auto m_PlayerData = &m_LagCompensationPlayerData[ nPlayerID - 1 ];

		/* Get player and check its data */
		C_BasePlayer* pPlayer = C_BasePlayer::GetPlayerByIndex( nPlayerID );
		if ( !pPlayer || !pPlayer->IsPlayer( ) || !pPlayer->IsAlive( ) || pPlayer->IsFriend( ) || pPlayer == g_Globals->m_LocalPlayer )
		{
			m_PlayerData->m_bLeftDormancy = true;
			continue;
		}

		/* Get record of this player */
		auto m_LagRecords = &m_LagCompensationRecords[ nPlayerID - 1 ];

		/* Check data changed */
		if ( pPlayer->m_flOldSimulationTime( ) == pPlayer->m_flSimulationTime( ) )
			continue;

		/* Set left dormancy */
		if ( pPlayer->IsDormant( ) )
		{
			m_PlayerData->m_bLeftDormancy = true;
			continue;
		}

		/* define previous record of this player */
		LagRecord_t m_PreviousRecord;
		m_PreviousRecord.m_bRestoreData = false;
		if ( !m_LagRecords->empty( ) )
		{
			m_PlayerData->m_LagRecord = m_LagRecords->back( );
			m_PreviousRecord = m_PlayerData->m_LagRecord;
			m_PreviousRecord.m_bRestoreData = true;
		}

		/* Store data */
		LagRecord_t m_Record;
		g_LagCompensation->StoreRecordData( pPlayer, &m_Record, &m_PreviousRecord );

		/* check for fake update */
		if ( m_PreviousRecord.m_bRestoreData )
		{
			if ( m_PreviousRecord.m_Layers[ 11 ].m_flCycle == m_Record.m_Layers[ 11 ].m_flCycle )
			{
				pPlayer->m_flSimulationTime( ) = m_PreviousRecord.m_flSimulationTime;
				continue;
			}
		}

		/* Check tickbase exploits */
		if ( m_PreviousRecord.m_flSimulationTime > m_Record.m_flSimulationTime )
			g_LagCompensation->HandleTickbaseShift( pPlayer, &m_PreviousRecord );

		/* Player who left dormancy right now shouldn't have any records */
		if ( m_PlayerData->m_bLeftDormancy )
			g_LagCompensation->CleanPlayer( pPlayer );

		/* Invalidate records for defensive and break lc */
		if ( m_Record.m_flSimulationTime <= m_PlayerData->m_flExploitTime )
		{
			/* Don't care at all about this cases if we have anti-exploit */
			m_Record.m_bIsInvalid = true;
			m_Record.m_bHasBrokenLC = true;
		}

		/* handle break lagcompensation by high speed and fakelags */
		if ( m_PreviousRecord.m_bRestoreData )
		{
			if ( ( m_Record.m_vecOrigin - m_PreviousRecord.m_vecOrigin ).Length2DSqr( ) > 4096.0f )
			{
				m_Record.m_bHasBrokenLC = true;
				g_LagCompensation->CleanPlayer( pPlayer );
			}
		}

		/* Determine simulation ticks */
		/* Another code for tickbase shift */
		if ( m_Record.m_flSimulationTime < m_PreviousRecord.m_flSimulationTime )
		{
			m_Record.m_bIsInvalid = true;
			m_Record.m_bHasBrokenLC = true;

			if ( m_PreviousRecord.m_bRestoreData )
				m_Record.m_nSimulationTicks = g_Utils->TimeToTicks( m_Record.m_flOldSimulationTime - m_PreviousRecord.m_flOldSimulationTime );
			else
				m_Record.m_nSimulationTicks = m_PreviousRecord.m_nSimulationTicks;
		}
		else
		{
			if ( m_PreviousRecord.m_bRestoreData )
				m_Record.m_nSimulationTicks = g_Utils->TimeToTicks( m_Record.m_flSimulationTime - m_PreviousRecord.m_flSimulationTime );
			else
				m_Record.m_nSimulationTicks = g_Utils->TimeToTicks( m_Record.m_flSimulationTime - m_Record.m_flOldSimulationTime );
		}


		/* Set this record as first after dormant */
		if ( m_PlayerData->m_bLeftDormancy )
			m_Record.m_bFirstAfterDormant = true;

		/* Push record and erase extra records */
		m_LagRecords->emplace_back( m_Record );
		while ( m_LagRecords->size( ) > 32 )
			m_LagRecords->pop_front( );

		/* Simulate player animations */
		g_PlayerAnimations->SimulatePlayerAnimations( pPlayer, &m_LagRecords->back( ), m_PlayerData );

		/* Dormancy handled */
		m_PlayerData->m_bLeftDormancy = false;
	}
}
void C_LagCompensation::HandleTickbaseShift( C_BasePlayer* pPlayer, LagRecord_t* m_PreviousRecord )
{
	float flOldSimulationTime = pPlayer->m_flOldSimulationTime( );
	if ( m_PreviousRecord )
		flOldSimulationTime = m_PreviousRecord->m_flSimulationTime;

	m_LagCompensationPlayerData[ pPlayer->EntIndex( ) - 1 ].m_flExploitTime = pPlayer->m_flSimulationTime( );
	g_LagCompensation->CleanPlayer( pPlayer );
}
void C_LagCompensation::ForcePlayerRecord( C_BasePlayer* Player, LagRecord_t* m_LagRecord )
{
	Player->m_fFlags( ) = m_LagRecord->m_nFlags;
	Player->m_flSimulationTime( ) = m_LagRecord->m_flSimulationTime;
	Player->m_vecOrigin( ) = m_LagRecord->m_vecOrigin;
	Player->m_angEyeAngles( ) = m_LagRecord->m_angEyeAngles;
	Player->SetAbsOrigin( m_LagRecord->m_vecOrigin );
	Player->SetAbsAngles( m_LagRecord->m_angAbsAngles );

	std::memcpy( Player->GetAnimationLayers( ), m_LagRecord->m_Layers.data( ), sizeof( C_AnimationLayer ) * ANIMATION_LAYER_COUNT );
	std::memcpy( Player->m_aPoseParameters( ).data( ), m_LagRecord->m_PoseParameters.data( ), sizeof( float ) * MAXSTUDIOPOSEPARAM );

	/* set time */
	float flCurTime = SDK::Interfaces::GlobalVars->m_flCurTime;
	SDK::Interfaces::GlobalVars->m_flCurTime = g_Utils->TimeToTicks( g_Exploits->GetPredictedTickBase( ) );
	Player->SetCollisionBounds( m_LagRecord->m_vecMins, m_LagRecord->m_vecMaxs );
	SDK::Interfaces::GlobalVars->m_flCurTime = flCurTime;

	std::memcpy( Player->m_CachedBoneData( ).Base( ), m_LagRecord->m_Matricies[ EBoneMatrix::Aimbot ].data( ), sizeof( matrix3x4_t ) * Player->m_CachedBoneData( ).Count( ) );
	return Player->InvalidateBoneCache( );
}
float C_LagCompensation::GetLerpTime( )
{
	float flUpdateRate = std::clamp( SDK::EngineData::m_ConvarList[ CheatConvarList::ClUpdateRate ]->GetFloat( ), SDK::EngineData::m_ConvarList[ CheatConvarList::SvMinUpdateRate ]->GetFloat( ), SDK::EngineData::m_ConvarList[ CheatConvarList::SvMaxUpdateRate ]->GetFloat( ) );
	float flLerpRatio = std::clamp( SDK::EngineData::m_ConvarList[ CheatConvarList::ClInterpRatio ]->GetFloat( ), SDK::EngineData::m_ConvarList[ CheatConvarList::SvClientMinInterpRatio ]->GetFloat( ), SDK::EngineData::m_ConvarList[ CheatConvarList::SvClientMaxInterpRatio ]->GetFloat( ) );
	return std::clamp( flLerpRatio / flUpdateRate, SDK::EngineData::m_ConvarList[ CheatConvarList::ClInterp ]->GetFloat( ), 1.0f );
}
void C_LagCompensation::StoreRecordData( C_BasePlayer* pPlayer, LagRecord_t* m_LagRecord, LagRecord_t* m_PreviousRecord )
{
	std::memcpy( m_LagRecord->m_Layers.data( ), pPlayer->GetAnimationLayers( ), sizeof( C_AnimationLayer ) * ANIMATION_LAYER_COUNT );

	m_LagRecord->m_flSimulationTime = pPlayer->m_flSimulationTime( );
	m_LagRecord->m_flOldSimulationTime = pPlayer->m_flOldSimulationTime( );
	m_LagRecord->m_nFlags = pPlayer->m_fFlags( );
	m_LagRecord->m_angEyeAngles = pPlayer->m_angEyeAngles( );
	m_LagRecord->m_flDuckAmount = pPlayer->m_flDuckAmount( );
	m_LagRecord->m_flThirdPersonRecoil = pPlayer->m_flThirdpersonRecoil( );
	m_LagRecord->m_flMaxSpeed = g_EnginePrediction->GetMaxSpeed( pPlayer );
	m_LagRecord->m_flLowerBodyYaw = pPlayer->m_flLowerBodyYaw( );
	m_LagRecord->m_vecOrigin = pPlayer->m_vecOrigin( );
	m_LagRecord->m_vecVelocity = pPlayer->m_vecVelocity( );
	m_LagRecord->m_bIsWalking = pPlayer->m_bIsWalking( );
	m_LagRecord->m_vecAbsOrigin = pPlayer->GetAbsOrigin( );;

	C_BaseCombatWeapon* pWeapon = pPlayer->m_hActiveWeapon( );
	if ( pWeapon )
	{
		if ( pWeapon->m_flLastShotTime( ) <= pPlayer->m_flSimulationTime( ) && pWeapon->m_flLastShotTime( ) > pPlayer->m_flOldSimulationTime( ) )
		{
			m_LagRecord->m_bDidShot = true;
			m_LagRecord->m_nShotTick = g_Utils->TimeToTicks( pWeapon->m_flLastShotTime( ) );
		}
	}

	/* Get player info */
	PlayerInfo_t m_PlayerInfo;
	SDK::Interfaces::EngineClient->GetPlayerInfo( pPlayer->EntIndex( ), &m_PlayerInfo );

	if ( m_PlayerInfo.m_bIsFakePlayer )
	{
		m_LagRecord->m_nSimulationTicks = 1;
		m_LagRecord->m_bIsFakePlayer = true;
	}
}
void C_LagCompensation::InterpolatePlayers( )
{
	float flInterpolationAmount = SDK::Interfaces::GlobalVars->m_flIntervalPerTick;
	if ( g_SettingsManager->B[ _S( "misc.visual_interpolation" ) ] )
		flInterpolationAmount = g_Utils->TicksToTime( g_SettingsManager->I[ _S( "misc.visual_interpolation_amount" ) ] );

	float flLocalDeadInterpolation = flInterpolationAmount;
	if ( g_Globals->m_LocalPlayer->IsAlive( ) )
		if ( g_SettingsManager->B[ _S( "misc.visual_interpolation_dead_only" ) ] )
			flInterpolationAmount = SDK::Interfaces::GlobalVars->m_flIntervalPerTick;

	for ( int nPlayerID = 1; nPlayerID <= 64; nPlayerID++ )
	{
		C_BasePlayer* pPlayer = C_BasePlayer::GetPlayerByIndex( nPlayerID );
		if ( !pPlayer || !pPlayer->IsPlayer( ) || pPlayer->m_iTeamNum( ) == g_Globals->m_LocalPlayer->m_iTeamNum( ) || !pPlayer->IsAlive( ) )
			continue;

		void* m_VarMap = *( void** ) ( ( DWORD ) ( pPlayer ) + 0x24 );
		if ( m_VarMap )
		{
			*( float* ) ( *( DWORD* ) ( ( DWORD ) ( m_VarMap ) + 0x8 ) + 0x24 ) = flInterpolationAmount;
			*( float* ) ( *( DWORD* ) ( ( DWORD ) ( m_VarMap ) + 0x44 ) + 0x24 ) = flInterpolationAmount;
		}
	}
}
void C_LagCompensation::InvalidatePlayer( C_BasePlayer* pPlayer )
{
	if ( !pPlayer )
		return;

	auto m_LagRecords = &m_LagCompensationRecords[ pPlayer->EntIndex( ) - 1 ];
	if ( !m_LagRecords )
		return;

	for ( auto& Record : *m_LagRecords )
		Record.m_bIsInvalid = true;
}
void C_LagCompensation::CleanPlayer( C_BasePlayer* pPlayer )
{
	if ( !pPlayer )
		return;

	auto m_LagRecords = &m_LagCompensationRecords[ pPlayer->EntIndex( ) - 1 ];
	if ( !m_LagRecords || m_LagRecords->empty( ) )
		return;

	m_LagRecords->clear( );
}
bool C_LagCompensation::WasPlayerDataChanged( C_BasePlayer* pPlayer, LagRecord_t* m_PreviousRecord )
{
	if ( m_PreviousRecord )
		if ( pPlayer->GetAnimationLayers( )[ ANIMATION_LAYER_ALIVELOOP ].m_flCycle == m_PreviousRecord->m_Layers[ ANIMATION_LAYER_ALIVELOOP ].m_flCycle )
			pPlayer->m_flSimulationTime( ) = pPlayer->m_flOldSimulationTime( );

	return pPlayer->m_flSimulationTime( ) != pPlayer->m_flOldSimulationTime( );
}
void C_LagCompensation::StartLagCompensation( )
{
	for ( int nPlayerID = 1; nPlayerID <= SDK::Interfaces::GlobalVars->m_iMaxClients; nPlayerID++ )
	{
		C_BasePlayer* pPlayer = C_BasePlayer::GetPlayerByIndex( nPlayerID );
		if ( !pPlayer || !pPlayer->IsPlayer( ) || !pPlayer->IsAlive( ) || pPlayer->IsFriend( ) || pPlayer->IsDormant( ) )
			continue;

		LagRecord_t* Data = &m_BackupData[ nPlayerID - 1 ];
		if ( !Data )
			continue;

		Data->m_angAbsAngles = pPlayer->GetAbsAngles( );
		Data->m_vecAbsOrigin = pPlayer->GetAbsOrigin( );
		Data->m_angEyeAngles = pPlayer->m_angEyeAngles( );
		Data->m_vecOrigin = pPlayer->m_vecOrigin( );
		Data->m_flSimulationTime = pPlayer->m_flSimulationTime( );
		Data->m_nFlags = pPlayer->m_fFlags( );
		Data->m_vecMins = pPlayer->GetCollideable( )->OBBMins( );
		Data->m_vecMaxs = pPlayer->GetCollideable( )->OBBMaxs( );
		Data->m_flCollisionChangeTime = pPlayer->m_flCollisionChangeTime( );
		Data->m_flCollisionZ = pPlayer->m_flCollisionChangeOrigin( );
		Data->m_bRestoreData = true;

		std::memcpy( Data->m_Layers.data( ), pPlayer->GetAnimationLayers( ), sizeof( C_AnimationLayer ) * ANIMATION_LAYER_COUNT );
		std::memcpy( Data->m_PoseParameters.data( ), pPlayer->m_aPoseParameters( ).data( ), sizeof( float ) * MAXSTUDIOPOSEPARAM );
		std::memcpy( Data->m_Matricies[ 0 ].data( ), pPlayer->m_CachedBoneData( ).Base( ), sizeof( matrix3x4_t ) * pPlayer->m_CachedBoneData( ).Count( ) );
	}
}
void C_LagCompensation::FinishLagCompensation( )
{
	for ( int nPlayerID = 1; nPlayerID <= SDK::Interfaces::GlobalVars->m_iMaxClients; nPlayerID++ )
	{
		C_BasePlayer* pPlayer = C_BasePlayer::GetPlayerByIndex( nPlayerID );
		if ( !pPlayer || !pPlayer->IsPlayer( ) || !pPlayer->IsAlive( ) || pPlayer->IsFriend( ) || pPlayer->IsDormant( ) )
			continue;

		LagRecord_t* Data = &m_BackupData[ nPlayerID - 1 ];
		if ( !Data || !Data->m_bRestoreData )
			continue;

		std::memcpy( pPlayer->GetAnimationLayers( ), Data->m_Layers.data( ), sizeof( C_AnimationLayer ) * ANIMATION_LAYER_COUNT );
		std::memcpy( pPlayer->m_aPoseParameters( ).data( ), Data->m_PoseParameters.data( ), sizeof( float ) * MAXSTUDIOPOSEPARAM );
		std::memcpy( pPlayer->m_CachedBoneData( ).Base( ), Data->m_Matricies[ 0 ].data( ), sizeof( matrix3x4_t ) * pPlayer->m_CachedBoneData( ).Count( ) );

		pPlayer->GetCollideable( )->OBBMins( ) = Data->m_vecMins;
		pPlayer->GetCollideable( )->OBBMaxs( ) = Data->m_vecMaxs;
		pPlayer->m_angEyeAngles( ) = Data->m_angEyeAngles;
		pPlayer->SetAbsAngles( Data->m_angAbsAngles );
		pPlayer->SetAbsOrigin( Data->m_vecAbsOrigin );
		pPlayer->m_vecOrigin( ) = Data->m_vecOrigin;
		pPlayer->m_flSimulationTime( ) = Data->m_flSimulationTime;
		pPlayer->m_fFlags( ) = Data->m_nFlags;
		pPlayer->m_flCollisionChangeTime( ) = Data->m_flCollisionChangeTime;
		pPlayer->m_flCollisionChangeOrigin( ) = Data->m_flCollisionZ;

		Data->m_bRestoreData = false;
	}
}

int C_LagCompensation::GetRecordPriority( LagRecord_t* m_Record )
{
	int nTotalPoints = 0;
	if ( m_Record->m_bDidShot )
		nTotalPoints += 2;

	if ( m_Record->m_bIsResolved )
		nTotalPoints += 2;

	if ( m_Record->m_flAnimationVelocity > 0.0f )
		nTotalPoints++;

	if ( !( m_Record->m_nFlags & FL_ONGROUND ) )
		nTotalPoints++;

	if ( m_Record->m_flDesyncDelta <= 30.0f )
		nTotalPoints++;

	if ( m_Record->m_nSimulationTicks <= 1 )
		nTotalPoints++;

	return nTotalPoints;
}
bool C_LagCompensation::GetBacktrackMatrix( C_BasePlayer* pPlayer, matrix3x4_t* aMatrix )
{
	if ( g_Globals->m_Packet.m_bAntiExploit )
		return false;

	auto aLagRecords = m_LagCompensationRecords[ pPlayer->EntIndex( ) - 1 ];
	if ( aLagRecords.size( ) < 2 )
		return false;

	float flCorrectTime = g_Networking->GetLatency( ) + fmax( SDK::EngineData::m_ConvarList[ CheatConvarList::ClInterp ]->GetFloat( ), SDK::EngineData::m_ConvarList[ CheatConvarList::ClInterpRatio ]->GetFloat( ) / SDK::EngineData::m_ConvarList[ CheatConvarList::ClUpdateRate ]->GetFloat( ) );
	for ( auto m_Record = aLagRecords.rend( ) - 1; m_Record > aLagRecords.rbegin( ); m_Record-- )
	{
		if ( !g_LagCompensation->IsRecordValid( pPlayer, m_Record.operator->( ) ) )
			continue;

		if ( m_Record->m_vecAbsOrigin.DistTo( pPlayer->GetAbsOrigin( ) ) < 1.f )
			return false;

		float flTimeDelta = ( m_Record - 1 )->m_flSimulationTime - m_Record->m_flSimulationTime;
		float flDeadTime = ( m_Record->m_flSimulationTime + flCorrectTime + flTimeDelta ) - g_Utils->TicksToTime( g_Exploits->GetPredictedTickBase( ) );

		std::array < matrix3x4_t, MAXSTUDIOBONES > m_BoneArray;
		std::memcpy( m_BoneArray.data( ), m_Record->m_Matricies[ EBoneMatrix::Visual ].data( ), sizeof( matrix3x4_t ) * MAXSTUDIOBONES );

		Vector vecInterpolatedOrigin = Math::Interpolate( ( m_Record - 1 )->m_vecAbsOrigin, m_Record->m_vecAbsOrigin, std::clamp( flDeadTime * ( 1.0f / flTimeDelta ), 0.f, 1.f ) );
		for ( int nBone; nBone < MAXSTUDIOBONES; nBone++ )
			m_BoneArray[ nBone ].SetOrigin( m_Record->m_Matricies[ EBoneMatrix::Visual ][ nBone ].GetOrigin( ) - m_Record->m_vecAbsOrigin + vecInterpolatedOrigin );

		std::memcpy( aMatrix, m_BoneArray.data( ), sizeof( matrix3x4_t ) * MAXSTUDIOBONES );
		return true;
	}

	return false;
}
bool C_LagCompensation::IsRecordValid( C_BasePlayer* pPlayer, LagRecord_t* m_LagRecord )
{
	if ( !m_LagRecord || ( ( m_LagRecord->m_bIsInvalid || m_LagRecord->m_bHasBrokenLC ) && !g_Globals->m_Packet.m_bAntiExploit ) )
		return false;

	return this->IsTimeValid( m_LagRecord->m_flSimulationTime, g_Exploits->GetPredictedTickBase( ) );
}
bool C_LagCompensation::IsTimeValid( float flSimulationTime, int nTickBase )
{
	const float flLerpTime = g_LagCompensation->GetLerpTime( );

	float flDeltaTime = fmin( g_Networking->GetLatency( ) + flLerpTime, 0.2f ) - g_Utils->TicksToTime( nTickBase - g_Utils->TimeToTicks( flSimulationTime ) );
	if ( fabs( flDeltaTime ) > 0.2f )
		return false;

	int nDeadTime = ( int ) ( ( float ) ( g_Utils->TicksToTime( g_Networking->GetLagCompensationTick( ) ) ) - 0.2f );
	if ( g_Utils->TimeToTicks( flSimulationTime + flLerpTime ) < nDeadTime )
		return false;

	return true;
}