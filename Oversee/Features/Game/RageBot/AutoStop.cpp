#include "AutoStop.hpp"
#include "RageBot.hpp"
#include "Autowall.hpp"
#include "AntiAim.hpp"
#include "LocalAnimations.hpp"
#include "LagCompensation.hpp"

#include "../../../SDK/Utils/Utils.hpp"
#include "../Prediction/EnginePrediction.hpp"
#include "../Networking/Networking.hpp"

void C_AutoStop::Initialize( bool bIsForced )
{
	/* reset auto-stop data */
	m_Movement.m_nTicksToStop = 0;
	m_Movement.m_bCanPredictMovement = false;
	m_Movement.m_bIsPeeking = false;

	/* ragebot must be turned on and local mustn't be frozen */
	if ( !g_SettingsManager->B[ _S( "ragebot.enable" ) ] || g_Globals->m_LocalPlayer->IsFrozen( ) )
	{
		g_AutoStop->CleanUp( );
		return;
	}

	/* we must be able to fire or shift tickbase */
	if ( !g_Globals->m_LocalPlayer->m_hActiveWeapon( ) || !g_Globals->m_LocalPlayer->m_hActiveWeapon( )->IsGun( ) || ( !g_Globals->m_Packet.m_bCanFire && !bIsForced ) )
	{
		g_AutoStop->CleanUp( );
		return;
	}

	/* check movetype and water level */
	if ( g_Globals->m_LocalPlayer->m_nMoveType( ) != MOVETYPE_WALK || g_Globals->m_LocalPlayer->GetWaterLevel( ) >= WL_Waist )
	{
		g_AutoStop->CleanUp( );
		return;
	}

	/* check water jump time */
	if ( *( float* ) ( ( DWORD ) ( g_Globals->m_LocalPlayer ) + 0x321C ) > 0.0f )
	{
		g_AutoStop->CleanUp( );
		return;
	}

	/* we can predict movement because everything is ok */
	m_Movement.m_bCanPredictMovement = true;
}
void C_AutoStop::RunAutoStop( )
{
	/* remove movement buttons */
	g_Globals->m_Packet.m_pCmd->m_nButtons &= ~( IN_MOVERIGHT | IN_MOVELEFT | IN_FORWARD | IN_BACK | IN_SPEED );

	/* get velocity data */
	const float flSpeed2D = g_Globals->m_LocalPlayer->m_vecVelocity( ).Length2D( );
	const float flMaxSpeed = g_EnginePrediction->GetMaxSpeed( );
	const float flSlowSpeed = flMaxSpeed * 0.03f;

	/* if Minimal accurated speed is higher then our velocity, then run slowwalk */
	if ( g_Globals->m_LocalPlayer->m_vecVelocity( ).Length2D( ) <= ( flMaxSpeed * 0.34f ) )
	{
		// force movement
		g_Globals->m_Packet.m_pCmd->m_flSideMove /= flSpeed2D / flSlowSpeed;
		g_Globals->m_Packet.m_pCmd->m_flForwardMove /= flSpeed2D / flSlowSpeed;

		// clamp movement
		g_Globals->m_Packet.m_pCmd->m_flSideMove = std::clamp( g_Globals->m_Packet.m_pCmd->m_flSideMove, -450.0f, 450.0f );
		g_Globals->m_Packet.m_pCmd->m_flForwardMove = std::clamp( g_Globals->m_Packet.m_pCmd->m_flForwardMove, -450.0f, 450.0f );
	}
	else
	{
		/* resistance vector from velocity */
		QAngle angResistance;
		Math::VectorAngles( ( g_Globals->m_LocalPlayer->m_vecVelocity( ) * -1.f ), angResistance );

		/* calc yaw */
		angResistance.yaw = g_Globals->m_Packet.m_pCmd->m_angViewAngles.yaw - angResistance.yaw;

		/* angle vectors */
		Vector vecResistance = Vector( 0, 0, 0 );
		Math::AngleVectors( angResistance, vecResistance );

		/* set movement */
		g_Globals->m_Packet.m_pCmd->m_flForwardMove = std::clamp( vecResistance.x, -450.f, 450.0f );
		g_Globals->m_Packet.m_pCmd->m_flSideMove = std::clamp( vecResistance.y, -450.f, 450.0f );
	}
}
void C_AutoStop::RunEarlyAutoStop( bool bForced )
{
	RageSettings_t* m_Settings = g_RageBot->GetSettings( );
	if ( ( !bForced && ( !m_Settings->m_bAutoStop || !m_Settings->m_bEarlyAutoStop ) ) || !m_Movement.m_bCanPredictMovement )
		return;

	const float flMaxSpeed = g_EnginePrediction->GetMaxSpeed( );
	if ( g_Globals->m_LocalPlayer->m_vecVelocity( ).Length2D( ) < flMaxSpeed * 0.3f )
		return;

	float flFrameTime = SDK::Interfaces::GlobalVars->m_flFrameTime;
	float flCurTime = SDK::Interfaces::GlobalVars->m_flCurTime;

	/* backup movement data */
	std::tuple < C_BasePlayer*, C_MoveData*, Vector, Vector, Vector > m_MovementData
		=
		std::make_tuple
		(
			SDK::Interfaces::GameMovement->m_Player( ),
			SDK::Interfaces::GameMovement->m_MoveData( ),
			SDK::Interfaces::GameMovement->m_vecForward( ),
			SDK::Interfaces::GameMovement->m_vecRight( ),
			SDK::Interfaces::GameMovement->m_vecUp( )
		);

	/* save localplayer data */
	std::tuple < float, float, float, Vector, Vector > m_LocalData
		=
		std::make_tuple
		(
			g_Globals->m_Packet.m_pCmd->m_flSideMove,
			g_Globals->m_Packet.m_pCmd->m_flForwardMove,
			*( float* ) ( ( DWORD ) ( g_Globals->m_LocalPlayer ) + 0x103F0 ),
			g_Globals->m_LocalPlayer->m_vecVelocity( ),
			g_Globals->m_LocalPlayer->m_vecAbsVelocity( )
		);

	/* normalize and clamp angles */
	Math::Normalize3( g_Globals->m_Packet.m_pCmd->m_angViewAngles );
	Math::ClampAngles( g_Globals->m_Packet.m_pCmd->m_angViewAngles );

	/* SetAbsVelocity */
	g_Globals->m_LocalPlayer->m_vecAbsVelocity( ) = g_Globals->m_LocalPlayer->m_vecVelocity( );

	/* predict ticks */
	for ( m_Movement.m_nTicksToStop = 1; m_Movement.m_nTicksToStop < 32; m_Movement.m_nTicksToStop++ )
	{
		/* run autostop */
		g_AutoStop->RunAutoStop( );

		/* setup move */
		SDK::Interfaces::Prediction->SetupMove( g_Globals->m_LocalPlayer, g_Globals->m_Packet.m_pCmd, SDK::Interfaces::MoveHelper, m_PredData.MoveData );
		Math::AngleVectors( QAngle( m_PredData.MoveData->m_vecViewAngles.x, m_PredData.MoveData->m_vecViewAngles.y, m_PredData.MoveData->m_vecViewAngles.z ), m_PredData.m_vecForward, m_PredData.m_vecRight, m_PredData.m_vecUp );

		/* calc max speed */
		m_PredData.MoveData->m_flMaxSpeed = fmin( flMaxSpeed, m_PredData.MoveData->m_flClientMaxSpeed );

		/* set move data */
		SDK::Interfaces::GameMovement->m_Player( ) = g_Globals->m_LocalPlayer;
		SDK::Interfaces::GameMovement->m_MoveData( ) = m_PredData.MoveData;
		SDK::Interfaces::GameMovement->m_vecForward( ) = m_PredData.m_vecForward;
		SDK::Interfaces::GameMovement->m_vecRight( ) = m_PredData.m_vecRight;
		SDK::Interfaces::GameMovement->m_vecUp( ) = m_PredData.m_vecUp;

		/* set globals */
		SDK::Interfaces::GlobalVars->m_flCurTime = g_Utils->TicksToTime( g_Globals->m_LocalPlayer->m_nTickBase( ) + m_Movement.m_nTicksToStop );
		SDK::Interfaces::GlobalVars->m_flFrameTime = SDK::Interfaces::GlobalVars->m_flIntervalPerTick;

		/* simulate process movement */
		g_AutoStop->ProcessMovement( );

		/* set abs velocity */
		g_Globals->m_LocalPlayer->m_vecAbsVelocity( ) = m_PredData.MoveData->m_vecVelocity;

		/* break the cycle if we match good velocity */
		if ( g_Globals->m_LocalPlayer->m_vecAbsVelocity( ).Length2D( ) <= flMaxSpeed * 0.34f )
			break;
	}

	/* restore velocity */
	g_Globals->m_LocalPlayer->m_vecVelocity( ) = std::get < 3 >( m_LocalData );
	g_Globals->m_LocalPlayer->m_vecAbsVelocity( ) = std::get < 4 >( m_LocalData );

	/* restore cmd data */
	g_Globals->m_Packet.m_pCmd->m_flSideMove = std::get < 0 >( m_LocalData );
	g_Globals->m_Packet.m_pCmd->m_flForwardMove = std::get < 1 >( m_LocalData );

	/* restore ground fraction acceleration */
	*( float* ) ( ( DWORD ) ( g_Globals->m_LocalPlayer ) + 0x103F0 ) = std::get < 2 >( m_LocalData );

	/* restore movement data */
	SDK::Interfaces::GameMovement->m_Player( ) = std::get < 0 >( m_MovementData );
	SDK::Interfaces::GameMovement->m_MoveData( ) = std::get < 1 >( m_MovementData );
	SDK::Interfaces::GameMovement->m_vecForward( ) = std::get < 2 >( m_MovementData );
	SDK::Interfaces::GameMovement->m_vecRight( ) = std::get < 3 >( m_MovementData );
	SDK::Interfaces::GameMovement->m_vecUp( ) = std::get < 4 >( m_MovementData );

	/* restore globals */
	SDK::Interfaces::GlobalVars->m_flCurTime = flCurTime;
	SDK::Interfaces::GlobalVars->m_flFrameTime = flFrameTime;
}
void C_AutoStop::RunMovementPrediction( )
{
	/* if ragebot is running, then skip because there's more optimized code in ragebob */
	if ( !g_SettingsManager->B[ _S( "ragebot.enable" ) ] || !m_Movement.m_bCanPredictMovement || !m_Movement.m_nTicksToStop )
		return;

	/* Get weapon */
	C_BaseCombatWeapon* pWeapon = g_Globals->m_LocalPlayer->m_hActiveWeapon( );
	if ( !pWeapon )
		return;

	/* revolver time check */
	if ( pWeapon->m_nItemID( ) == WEAPON_REVOLVER )
	{
		if ( pWeapon->m_flPostponeFireReadyTime( ) > SDK::Interfaces::GlobalVars->m_flCurTime + g_Utils->TicksToTime( m_Movement.m_nTicksToStop ) )
			return;
	}

	/* iterate all players*/
	std::vector < TempTarget_t > m_TempTargetList;
	for ( int nTarget = 1; nTarget <= SDK::Interfaces::ClientState->m_nMaxClients; nTarget++ )
	{
		/* get player and check him */
		C_BasePlayer* m_Player = C_BasePlayer::GetPlayerByIndex( nTarget );
		if ( !m_Player || !m_Player->IsPlayer( ) || !m_Player->IsAlive( ) || m_Player->IsFriend( ) || m_Player->m_bGunGameImmunity( ) || m_Player->IsFrozen( ) || m_Player == g_Globals->m_LocalPlayer || m_Scan.m_BadPlayers[ nTarget ] )
			continue;

		/* get player records */
		std::deque < LagRecord_t > m_Records = g_LagCompensation->GetPlayerRecords( m_Player );

		/* skip players without records */
		if ( m_Records.empty( ) )
			continue;

		/* find first record */
		LagRecord_t m_Record = g_LagCompensation->FindFirstRecord( m_Player, m_Records );
		if ( m_Record.m_bIsInvalid )
			continue;

		/* calc record's priority */
		int nLocalPriority = g_LagCompensation->GetRecordPriority( &m_Record );

		/* increase priority for prev target */
		if ( nTarget == m_LastPlayer )
			if ( nLocalPriority )
				nLocalPriority++;

		/* push target */
		m_TempTargetList.push_back( TempTarget_t( m_Player, m_Record, m_Record, nLocalPriority, 0 ) );
	}

	if ( m_TempTargetList.empty( ) )
	{
		memset( m_Scan.m_BadPlayers.data( ), 0, m_Scan.m_BadPlayers.size( ) );
		return;
	}

	/* sort targets */
	std::sort
	(
		m_TempTargetList.begin( ),
		m_TempTargetList.end( ),
		[ ] ( const TempTarget_t& First, const TempTarget_t& Second ) -> bool
	{
		return First.m_nLocalVulnerablePoints > Second.m_nLocalVulnerablePoints;
	}
	);

	/* scan player smart function */
	auto ScanPlayer = [ ] ( C_BasePlayer* Player, LagRecord_t* Record, Vector& vecShootPosition ) -> AimPointData_t
	{
		/* get the fuck out */
		if ( Record->m_bIsInvalid )
			return AimPointData_t( );

		/* set record */
		g_LagCompensation->ForcePlayerRecord( Player, Record );

		/* return result */
		return g_RageBot->CalculateTargetPoint( Player, vecShootPosition, Record );
	};

	/* predicted shoot position */
	Vector vecPredictedShootPosition = g_LocalAnimations->GetShootPosition( ) + ( g_Globals->m_LocalPlayer->m_vecVelocity( ) * SDK::Interfaces::GlobalVars->m_flIntervalPerTick ) * g_AutoStop->GetTicksToStop( );

	/* scanned targets amt */
	int nScannedTargets = 0;

	/* scan targets */
	for ( auto& Target : m_TempTargetList )
	{
		++nScannedTargets;
		if ( nScannedTargets > 3 )
			break;

		/* backtrack scan state used to prevent double record scan */
		bool bHasBacktrackScanned = false;

		/* scan record */
		AimPointData_t m_Data = ScanPlayer( Target.m_pPlayer, &Target.m_LocalRecord, vecPredictedShootPosition );
		if ( !m_Data.m_bIsInvalid )
		{
			/* save autostop */
			if ( g_RageBot->IsAutoStopEnabled( ) && g_RageBot->GetSettings( )->m_bEarlyAutoStop )
			{
				/* force autostop */
				g_AutoStop->RunAutoStop( );

				/* repredict */
				g_EnginePrediction->RePredict( );
			}

			/* Predictive zoom */
			if ( pWeapon->m_nWeaponMode( ) == CSWeaponMode::Primary_Mode )
			{
				/* only if sniper */
				if ( pWeapon->IsSniper( ) )
				{
					/* for autosniper we use another distance */
					float flZoomDistance = pWeapon->m_nItemID( ) == WEAPON_SCAR20 || pWeapon->m_nItemID( ) == WEAPON_G3SG1 ? 500.0f : 200.0f;
					if ( g_Globals->m_LocalPlayer->m_vecOrigin( ).DistTo( Target.m_LocalRecord.m_vecOrigin ) > flZoomDistance )
					{
						/* we cannot zoom and attack at the same time */
						g_Globals->m_Packet.m_pCmd->m_nButtons |= IN_ATTACK2;
						g_Globals->m_Packet.m_pCmd->m_nButtons &= ~IN_ATTACK;

						/* force repredict due to forced zoom */
						g_EnginePrediction->RePredict( );

						/* break ( we cannot fire this tick anyway ) */
						break;
					}
				}
			}

			/* set as peek */
			g_AutoStop->SetAsPeeking( );

			/* mark this target prioritized */
			m_LastPlayer = Target.m_pPlayer->EntIndex( );

			/* break */;
			break;
		}

		/* invalid */
		m_Scan.m_BadPlayers[ Target.m_pPlayer->EntIndex( ) ] = false;
	}
}
void C_AutoStop::Friction( )
{
	float flSpeed = m_PredData.MoveData->m_vecVelocity.Length( );
	if ( flSpeed < 0.1f )
		return;

	float flStopSpeed = SDK::EngineData::m_ConvarList[ CheatConvarList::StopSpeed ]->GetFloat( );
	float flDrop = fmax( flStopSpeed, flSpeed ) * SDK::EngineData::m_ConvarList[ CheatConvarList::Friction ]->GetFloat( ) * g_Globals->m_LocalPlayer->m_surfaceFriction( ) * SDK::Interfaces::GlobalVars->m_flIntervalPerTick;
	float flNewSpeed = fmax( flSpeed - flDrop, 0.0f ) / flSpeed;

	Math::VectorMultiply( m_PredData.MoveData->m_vecVelocity, flNewSpeed, m_PredData.MoveData->m_vecVelocity );

	m_PredData.MoveData->m_vecOutWishVel -= m_PredData.MoveData->m_vecVelocity * ( 1.0f - flNewSpeed );
}
float C_AutoStop::CalcRoll( const QAngle& angAngles, const Vector& vecVelocity, float flRollAngle, float flRollSpeed )
{
	Vector  vecForward, vecRight, vecUp;
	Math::AngleVectors( angAngles, vecForward, vecRight, vecUp );

	float flSide = std::fabs( Math::DotProduct( vecVelocity, vecRight ) );
	float flSign = flSide < 0 ? -1 : 1;

	if ( flSide < flRollSpeed )
		flSide = flSide * flRollAngle / flRollSpeed;
	else
		flSide = flRollAngle;

	return flSide * flSign;
}
float C_AutoStop::ComputeConstraintSpeedFactor( )
{
	// If we have a constraint, slow down because of that too.
	if ( !m_PredData.MoveData || m_PredData.MoveData->m_flConstraintRadius < FLT_EPSILON )
		return 1.0f;

	float flDistSq = m_PredData.MoveData->m_vecAbsOrigin.DistToSqr( m_PredData.MoveData->m_vecConstraintCenter );

	float flOuterRadiusSq = m_PredData.MoveData->m_flConstraintRadius * m_PredData.MoveData->m_flConstraintRadius;
	float flInnerRadiusSq = m_PredData.MoveData->m_flConstraintRadius - m_PredData.MoveData->m_flConstraintWidth;
	flInnerRadiusSq *= flInnerRadiusSq;

	// Only slow us down if we're inside the constraint ring
	if ( ( flDistSq <= flInnerRadiusSq ) || ( flDistSq >= flOuterRadiusSq ) )
		return 1.0f;

	// Only slow us down if we're running away from the center
	Vector vecDesired;
	Math::VectorMultiply( m_PredData.m_vecForward, m_PredData.MoveData->m_flForwardMove, vecDesired );
	Math::VectorMA( vecDesired, m_PredData.MoveData->m_flSideMove, m_PredData.m_vecRight, vecDesired );
	Math::VectorMA( vecDesired, m_PredData.MoveData->m_flUpMove, m_PredData.m_vecUp, vecDesired );

	Vector vecDelta;
	Math::VectorSubtract( m_PredData.MoveData->m_vecAbsOrigin, m_PredData.MoveData->m_vecConstraintCenter, vecDelta );
	Math::VectorNormalize( vecDelta );
	Math::VectorNormalize( vecDesired );
	if ( Math::DotProduct( vecDelta, vecDesired ) < 0.0f )
		return 1.0f;

	float flFrac = ( sqrt( flDistSq ) - ( m_PredData.MoveData->m_flConstraintRadius - m_PredData.MoveData->m_flConstraintWidth ) ) / m_PredData.MoveData->m_flConstraintWidth;
	return Math::Lerp( flFrac, 1.0f, m_PredData.MoveData->m_flConstraintSpeedFactor );
}
void C_AutoStop::ProcessMovement( )
{
	const float flRollSpeed = SDK::EngineData::m_ConvarList[ CheatConvarList::RollSpeed ]->GetFloat( );
	{
		float flSpeedFactor = 1.0f;
		if ( g_Globals->m_LocalPlayer->GetSurfaceData( ) )
			flSpeedFactor = g_Globals->m_LocalPlayer->GetSurfaceData( )->GetMaxSpeedFactor( );

		float flConstraintSpeedFactor = 1.0f;
		if ( flConstraintSpeedFactor < flSpeedFactor )
			flSpeedFactor = flConstraintSpeedFactor;

		m_PredData.MoveData->m_flMaxSpeed *= flSpeedFactor;

		float flSpeed = sqrtf( m_PredData.MoveData->m_flForwardMove ) + sqrtf( m_PredData.MoveData->m_flSideMove ) + sqrtf( m_PredData.MoveData->m_flUpMove );
		if ( flSpeed > 0.0f )
		{
			if ( flSpeed > sqrtf( m_PredData.MoveData->m_flMaxSpeed ) )
			{
				float fRatio = m_PredData.MoveData->m_flMaxSpeed / sqrt( flSpeed );
				m_PredData.MoveData->m_flForwardMove *= fRatio;
				m_PredData.MoveData->m_flSideMove *= fRatio;
				m_PredData.MoveData->m_flUpMove *= fRatio;
			}
		}

		QAngle m_vecViewAngles = QAngle( m_PredData.MoveData->m_vecAngles.x, m_PredData.MoveData->m_vecAngles.y, m_PredData.MoveData->m_vecAngles.z ) + g_Globals->m_LocalPlayer->m_viewPunchAngle( );

		m_PredData.MoveData->m_vecAngles.x = m_vecViewAngles.pitch;
		m_PredData.MoveData->m_vecAngles.y = m_vecViewAngles.yaw;
		m_PredData.MoveData->m_vecAngles.z = g_AutoStop->CalcRoll( m_vecViewAngles, m_PredData.MoveData->m_vecVelocity, flRollSpeed, flRollSpeed );
	}

	g_AutoStop->Friction( );
	return ( ( void( __thiscall* )( void* ) )( SDK::EngineData::m_AddressList[ CheatAddressList::WalkMove ] ) )( SDK::Interfaces::GameMovement );
}