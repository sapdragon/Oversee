#include <list>
#include "RageBot.hpp"
#include "AutoStop.hpp"
#include "AntiAim.hpp"
#include "AutoWall.hpp"
#include "Shots.hpp"
#include "LocalAnimations.hpp"
#include "../Revolver/Revolver.hpp"
#include "../../Visuals/Chams/ShotChams.hpp"
#include "../../Misc/Movement/Movement.hpp"
#include "../Prediction/EnginePrediction.hpp"
#include "../Networking/Networking.hpp"
#include "../Exploits/Exploits.hpp"
#include "../../../SDK/Utils/Utils.hpp"

/* ragebot configuration part */
void C_RageBot::SetupConfiguration( )
{
	/* reset cfg */
	m_Settings.m_Hitboxes.fill( false );

	/* get ragebot weapon */
	C_BaseCombatWeapon* pCombatWeapon = g_Globals->m_LocalPlayer->m_hActiveWeapon( );
	if ( !pCombatWeapon )
		return;

	/* select weapon type by item id */
	int iCurrentWeapon = -1;
	switch ( pCombatWeapon->m_nItemID( ) )
	{
		case WEAPON_AK47:
		case WEAPON_M4A1:
		case WEAPON_M4A1_SILENCER:
		case WEAPON_FAMAS:
		case WEAPON_SG553:
		case WEAPON_GALILAR:
			m_Settings = g_SettingsManager->GetRagebotSettings( _S( "rifle" ) ); 
			break;
		
		case WEAPON_MAG7:
		case WEAPON_NOVA:
		case WEAPON_XM1014:
		case WEAPON_SAWEDOFF:
			m_Settings = g_SettingsManager->GetRagebotSettings( _S( "shotgun" ) );
		break;
		
		case WEAPON_MP7:
		case WEAPON_MP9:
		case WEAPON_P90:
		case WEAPON_M249:
		case WEAPON_NEGEV:
		case WEAPON_UMP45:
			m_Settings = g_SettingsManager->GetRagebotSettings( _S( "smg" ) );
		break;
		
		case WEAPON_SCAR20:
		case WEAPON_G3SG1:
			m_Settings = g_SettingsManager->GetRagebotSettings( _S( "auto" ) ); 
			break;
		
		case WEAPON_GLOCK:
		case WEAPON_HKP2000:
		case WEAPON_USP_SILENCER:
		case WEAPON_CZ75A:
		case WEAPON_TEC9:
		case WEAPON_ELITE:
		case WEAPON_FIVESEVEN:
		case WEAPON_P250:
			m_Settings = g_SettingsManager->GetRagebotSettings( _S( "pistol" ) ); 
			break;
		
		case WEAPON_SSG08:
			m_Settings = g_SettingsManager->GetRagebotSettings( _S( "scout" ) ); 
			break;
		
		case WEAPON_AWP:
			m_Settings = g_SettingsManager->GetRagebotSettings( _S( "awp" ) ); 
			break;
		
		case WEAPON_DEAGLE:
			m_Settings = g_SettingsManager->GetRagebotSettings( _S( "deagle" ) ); 
			break;
		
		case WEAPON_REVOLVER:
			m_Settings = g_SettingsManager->GetRagebotSettings( _S( "revolver" ) );
			break;
		
		default: iCurrentWeapon = -1;
	}
}

/* ragebot main part */
void C_RageBot::Instance( )
{
	/* make previous aimbot target invalid */
	m_ScanData.m_AimTarget.m_Player = NULL;

	/* reset recurse state */
	m_bIsRecursed = false;

	/* check conditions */
	if ( g_Globals->m_LocalPlayer->IsFrozen( ) || !g_SettingsManager->B[ _S( "ragebot.enable" ) ] )
	{
		memset( m_ScanData.m_BadPlayers.data( ), 0, m_ScanData.m_BadPlayers.size( ) );
		return;
	}

	/* get weapon and check it */
	C_BaseCombatWeapon* pWeapon = g_Globals->m_LocalPlayer->m_hActiveWeapon( );
	if ( !pWeapon )
	{
		memset( m_ScanData.m_BadPlayers.data( ), 0, m_ScanData.m_BadPlayers.size( ) );
		return;
	}

	/* has no ammo */
	if ( pWeapon->m_iClip1( ) <= 0 )
	{
		memset( m_ScanData.m_BadPlayers.data( ), 0, m_ScanData.m_BadPlayers.size( ) );
		return;
	}

	/* get weapon data */
	const C_CSWeaponData* pWeaponData = pWeapon->GetWeaponData( );
	if ( !pWeaponData )
	{
		memset( m_ScanData.m_BadPlayers.data( ), 0, m_ScanData.m_BadPlayers.size( ) );
		return;
	}

	/* setup default ragebot accuracy */
	float flDefaultInaccuracy = 0.0f;
	if ( g_Globals->m_LocalPlayer->m_bDucked( ) || g_Globals->m_LocalPlayer->m_fFlags( ) & FL_DUCKING )
	{
		if ( pWeapon->m_nWeaponMode( ) != 0 )
			flDefaultInaccuracy = pWeaponData->m_flInaccuracyCrouchAlt;
		else
			flDefaultInaccuracy = pWeaponData->m_flInaccuracyCrouch;
	}
	else
	{
		if ( pWeapon->m_nWeaponMode( ) != 0 )
			flDefaultInaccuracy = pWeaponData->m_flInaccuracyStandAlt;
		else
			flDefaultInaccuracy = pWeaponData->m_flInaccuracyStand;
	}
	m_CachedData.m_flDefaultAccuracy = flDefaultInaccuracy;

	/* cache accuracy seeds */
	if ( m_CachedData.m_bShouldBeCached )
	{
		/* setup hitchance seeds */
		for ( int nSeed = 0; nSeed < 256; nSeed++ )
		{
			/* set random seed */
			g_Utils->RandomSeed( nSeed + 1 );

			/* generate random */
			m_CachedData.m_Seeds.m_Theta0[ nSeed ] = g_Utils->RandomFloat( 0.0f, pi / 0.5f );
			m_CachedData.m_Seeds.m_Theta1[ nSeed ] = g_Utils->RandomFloat( 0.0f, pi / 0.5f );
			m_CachedData.m_Seeds.m_Spread[ nSeed ] = g_Utils->RandomFloat( 0.0f, 1.0f );
			m_CachedData.m_Seeds.m_flInaccuracy = g_Utils->RandomFloat( 0.0f, 1.0f );
		}

		/* setup accuracy sin/cos */
		for ( int nSeed = 0; nSeed < 8; nSeed++ )
		{
			/* calcualte direction cos/sin */
			float flDirCos, flDirSin;
			DirectX::XMScalarSinCos( &flDirCos, &flDirSin, DirectX::XM_2PI * ( ( float ) ( nSeed ) / 8.0f ) );

			/* push the pair of direction sin/cos */
			m_CachedData.m_AccuracyBoost.m_Iterations[ nSeed ] = std::make_pair( flDirCos, flDirSin );
		}

		/* do not cache data twice */
		m_CachedData.m_bShouldBeCached = false;
	}

	/* force scope function */
	auto ForceWeaponZoom = [ & ] ( )
	{
		/* we cannot zoom and attack at the same time */
		g_Globals->m_Packet.m_pCmd->m_nButtons |= IN_ATTACK2;
		g_Globals->m_Packet.m_pCmd->m_nButtons &= ~IN_USE;
		g_Globals->m_Packet.m_pCmd->m_nButtons &= ~IN_ATTACK;

		/* force repredict due to forced zoom */
		g_EnginePrediction->RePredict( );
	};

	/* handle accuracy failure */
	auto HandleAccuracyFailure = [ & ] ( )
	{
		/* check autoscope */
		if ( !m_Settings.m_bAutoScope )
			return;

		/* hitchance has been failed. we can increase accuracy in the next tick in some cases */
		if ( pWeapon->IsSniper( ) )
		{
			/* if we are already scoped, then we can't do anything. So, if we ain't scoped then we have to force it */
			if ( pWeapon->m_nWeaponMode( ) == CSWeaponMode::Primary_Mode )
			{
				/* get distance to player */
				float flDistanceToPlayer = g_Globals->m_LocalPlayer->m_vecOrigin( ).DistTo( m_ScanData.m_AimTarget.m_Record.m_vecOrigin );

				/* check distance for a weapon isn't auto-sniper */
				if ( pWeapon->m_nItemID( ) != WEAPON_SCAR20 && pWeapon->m_nItemID( ) != WEAPON_G3SG1 )
				{
					/* distance is too long and we'll need to zoom and repredict */
					if ( flDistanceToPlayer > 200.0f )
						ForceWeaponZoom( );
				}
				else if ( flDistanceToPlayer > 500.0f )
					ForceWeaponZoom( );
			}
		}
	};
	
	if ( pWeapon->m_nItemID( ) == WEAPON_REVOLVER )
		g_Revolver->OnCreateMove( );

	/* get shoot position */
	Vector vecShootPosition = g_LocalAnimations->GetShootPosition( );

	/* continue state */
	bool bContinueRageBot = true;

	/* run ragebot */
	if ( pWeapon->m_nItemID( ) == WEAPON_TASER )
	{
		// g_RageBot->TaserBot( );
	}
	else if ( pWeapon->IsKnife( ) )
	{
		// g_RageBot->KnifeBot( );
	}
	else if ( pWeapon->IsGun( ) )
	{
		/* can fire check */
		bool bCanFire = g_Globals->m_Packet.m_bCanFire;
		if ( pWeaponData->m_flCycleTimeAlt > 0.2f )
			if ( !bCanFire )
				return;

		/* find target and check it */
		g_RageBot->FindTarget( vecShootPosition );
		if ( bCanFire && m_ScanData.m_AimTarget.m_Player )
		{
			if ( pWeapon->m_nItemID( ) == WEAPON_REVOLVER && !g_Globals->m_Packet.m_bCanFireRev )
			{
				g_Globals->m_Packet.m_pCmd->m_nButtons |= IN_ATTACK;
				return;
			}

			/* store buttons */
			int nButtons = g_Globals->m_Packet.m_pCmd->m_nButtons;

			/* auto-scope if needed */
			HandleAccuracyFailure( );

			/* did we scope */
			if ( nButtons != g_Globals->m_Packet.m_pCmd->m_nButtons )
				bContinueRageBot = false;

			/* continue ragebot only if we has no failures at previous checks */
			if ( bContinueRageBot )
			{
				/* now we'll have to check hitchance */
				float flHitChance = g_RageBot->GetHitchance( );
				if ( flHitChance < ( float ) ( m_Settings.m_nHitChance ) )
				{
					/* hitchance has been failed. we can increase accuracy in the next tick in some cases */
					HandleAccuracyFailure( );

					/* do not continue */
					bContinueRageBot = false;
				}

				/* if we have good accuracy then run ragebot */
				if ( bContinueRageBot )
				{
					/* calculate aimbot angles */
					QAngle angAimbotAngles = Math::CalcAngle( vecShootPosition, m_ScanData.m_AimTarget.m_Point.m_vecPoint );

					/* compensate recoil if we have it turned on */
					const float flWeaponRecoilScale = SDK::EngineData::m_ConvarList[ CheatConvarList::WeaponRecoilScale ]->GetFloat( );
					if ( flWeaponRecoilScale > 0.0f )
						angAimbotAngles -= g_Globals->m_LocalPlayer->m_aimPunchAngle( ) * flWeaponRecoilScale;

					/* calculate player tick */
					int nTickCount = g_Utils->TimeToTicks( m_ScanData.m_AimTarget.m_Record.m_flSimulationTime + g_LagCompensation->GetLerpTime( ) );

					/* set tick */
					g_Globals->m_Packet.m_pCmd->m_nTickCount = nTickCount;

					/* force attack */
					g_Globals->m_Packet.m_pCmd->m_nButtons &= ~IN_USE;
					g_Globals->m_Packet.m_pCmd->m_nButtons |= IN_ATTACK;

					/* set angles */
					g_Globals->m_Packet.m_pCmd->m_angViewAngles = angAimbotAngles;

					/* force sendpacket */
					if ( !g_Globals->m_Packet.m_bFakeDucking )
						*g_Globals->m_Packet.m_bSendPacket = true;

					/* get records */
					auto& Records = g_LagCompensation->GetPlayerRecords( m_ScanData.m_AimTarget.m_Player );

					/* Handle Ragebot fire */
					RageShot_t* RageShot = &g_ShotSystem->m_RageShots.emplace_front( );
					RageShot->m_Target = m_ScanData.m_AimTarget;
					RageShot->m_nTickCount = g_Networking->GetServerTick( );
					RageShot->m_nEstimate = g_Networking->GetServerTick( ) + g_Utils->TimeToTicks( g_Networking->GetLatency( ) ) + MAX_SIMULATION_TICKS;
					RageShot->m_flWeaponDamage = static_cast < float > ( pWeaponData->m_iDamage );
					RageShot->m_vecShootPosition = g_LocalAnimations->GetShootPosition( );
					RageShot->m_nTrackedTicks = g_Utils->TimeToTicks( Records.back( ).m_flSimulationTime - m_ScanData.m_AimTarget.m_Record.m_flSimulationTime );
				#ifdef OVERSEE_DEV
					auto GetHitboxNameFromId = [ & ] ( int id ) -> std::string
					{
						switch ( id )
						{
							case HITBOX_HEAD:
							return _S( "head" );
							case HITBOX_CHEST:
							return _S( "chest" );
							case HITBOX_STOMACH:
							return _S( "stomach" );
							case HITBOX_PELVIS:
							return _S( "pelvis" );
							case HITBOX_RIGHT_UPPER_ARM:
							case HITBOX_RIGHT_FOREARM:
							case HITBOX_RIGHT_HAND:
							return _S( "left arm" );
							case HITBOX_LEFT_UPPER_ARM:
							case HITBOX_LEFT_FOREARM:
							case HITBOX_LEFT_HAND:
							return _S( "right arm" );
							case HITBOX_RIGHT_THIGH:
							case HITBOX_RIGHT_CALF:
							return _S( "left leg" );
							case HITBOX_LEFT_THIGH:
							case HITBOX_LEFT_CALF:
							return _S( "right leg" );
							case HITBOX_RIGHT_FOOT:
							return _S( "left foot" );
							case HITBOX_LEFT_FOOT:
							return _S( "right foot" );
						}
					};
					
					PlayerInfo_t Data;
					SDK::Interfaces::EngineClient->GetPlayerInfo( m_ScanData.m_AimTarget.m_Player->EntIndex( ), &Data );

					const Vector shootpos = g_LocalAnimations->GetShootPosition( );

					/* build console message */
					std::string szConsoleMessage =
						( std::string ) ( _S( "[ RageBot ] Fired shot at " ) )
						+
						Data.m_strName
						+
						_S( " | ACC: " ) + std::to_string( ( int ) ( flHitChance ) )
						+
						_S( " | Lethal: " ) + std::to_string( ( int ) ( m_ScanData.m_AimTarget.m_Point.m_flDamage >= m_ScanData.m_AimTarget.m_Player->m_iHealth( ) ) )
						+
						_S( " | Safe: " ) + std::to_string( ( int ) ( this->IsSafePoint( m_ScanData.m_AimTarget.m_Player, &m_ScanData.m_AimTarget.m_Record, g_LocalAnimations->GetShootPosition( ), m_ScanData.m_AimTarget.m_Point.m_vecPoint, m_ScanData.m_AimTarget.m_Point.m_HitboxData ) ) )
						+
						_S( " | Velocity: " ) + std::to_string( ( int ) ( m_ScanData.m_AimTarget.m_Record.m_flAnimationVelocity ) )
						+
						_S( " | Backtrack: " ) + std::to_string( RageShot->m_nTrackedTicks )
						+
						_S( " | Damage: " ) + std::to_string( m_ScanData.m_AimTarget.m_Point.m_flDamage )
						+
						_S( " | Hitbox: " ) + GetHitboxNameFromId( m_ScanData.m_AimTarget.m_Point.m_HitboxData.m_nHitbox )
						+
						_S( "\n" );

					/* print console msg */
					SDK::Interfaces::CVar->ConsolePrintf( szConsoleMessage.c_str( ) );
				#endif

					/* Capture matrix for on shot chams */
					g_ShotChams->OnRageBotFire( m_ScanData.m_AimTarget.m_Player, m_ScanData.m_AimTarget.m_Record.m_Matricies[ EBoneMatrix::Visual ].data( ), m_ScanData.m_AimTarget.m_Record.m_vecAbsOrigin );
				}
			}
			else
			{
				/* force weapon zoom */
				HandleAccuracyFailure( );
			}
		}
	}
}
void C_RageBot::FindTarget( const Vector& vecShootPosition )
{
	/* get weapon */
	C_BaseCombatWeapon* pWeapon = g_Globals->m_LocalPlayer->m_hActiveWeapon( );
	C_CSWeaponData* pWeaponData = pWeapon->GetWeaponData( );

	/* rage optimization */
	int nTotalPoints = 0;

	/* iterate all players*/
	std::vector < TempTarget_t > m_TempTargetList;
	for ( int nTarget = 1; nTarget <= SDK::Interfaces::ClientState->m_nMaxClients; nTarget++ )
	{
		/* get player and check him */
		C_BasePlayer* m_Player = C_BasePlayer::GetPlayerByIndex( nTarget );
		if ( !m_Player || !m_Player->IsPlayer( ) || !m_Player->IsAlive( ) || m_Player->IsFriend( ) || m_Player->m_bGunGameImmunity( ) || m_ScanData.m_BadPlayers[ nTarget ] || m_Player->IsFrozen( ) || m_Player == g_Globals->m_LocalPlayer )
			continue;

		if ( m_TempTargetList.size( ) >= 3 ) // limit targets for 3 per tick
			break;

		/* get player records */
		std::deque < LagRecord_t > m_Records = g_LagCompensation->GetPlayerRecords( m_Player );

		/* skip players without records */
		if ( m_Records.empty( ) )
			continue;

		int nLocalPriority = 0;
		int nTrackPriority = 0;

		/* find first record */
		LagRecord_t m_Record = g_LagCompensation->FindFirstRecord( m_Player, m_Records );
		if ( !m_Record.m_bIsInvalid )
			nLocalPriority = g_LagCompensation->GetRecordPriority( &m_Record );

		/* find another record */
		LagRecord_t m_SecondRecord = g_LagCompensation->FindBestRecord( m_Player, m_Records, nTrackPriority, m_Record.m_flSimulationTime );

		/* increase priority for prev target */
		if ( nTarget == m_ScanData.m_nLastPlayer )
		{
			if ( nLocalPriority )
			{
				nLocalPriority++;
				if ( g_Globals->m_Packet.m_bDoubleTap )
					nLocalPriority += 9999;
			}

			if ( nTrackPriority )
			{
				nTrackPriority++;
				if ( g_Globals->m_Packet.m_bDoubleTap )
					nLocalPriority += 9999;
			}
		}

		/* push target */
		m_TempTargetList.emplace_back( TempTarget_t( m_Player, m_Record, m_SecondRecord, nLocalPriority, nTrackPriority ) );

		/* increase total priorty */
		nTotalPoints += nLocalPriority + nTrackPriority;
	}

	/* do not continue if we have no targets */
	if ( m_TempTargetList.empty( ) )
	{
		/* reset badplayers list */
		memset( m_ScanData.m_BadPlayers.data( ), 0, sizeof( m_ScanData.m_BadPlayers ) );

		/* reset last target */
		m_ScanData.m_nLastPlayer = 0;

		/* if this code is called at the first time this tick, then call it again */
		if ( !m_bIsRecursed )
		{
			/* to run this code we must be sure that we at least tried to scan players but found nothing because they all are bad!!! */
			bool bShouldRecurseThisCode = false;
			for ( int nPlayer = 0; nPlayer < sizeof( m_ScanData.m_BadPlayers ); nPlayer++ )
			{
				/* we just tried but failed, recurse and clean out bad players list */
				if ( m_ScanData.m_BadPlayers[ nPlayer ] )
				{
					bShouldRecurseThisCode = true;
					break;
				}
			}

			/* recurse this function */
			if ( bShouldRecurseThisCode )
				g_RageBot->FindTarget( vecShootPosition );

			/* set recurse status */
			m_bIsRecursed = true;
		}

		/* don't continue */
		return;
	}

	/* sort targets */
	std::sort
	(
		m_TempTargetList.begin( ),
		m_TempTargetList.end( ),
		[ ] ( const TempTarget_t& First, const TempTarget_t& Second ) -> bool
	{
		return ( First.m_nLocalVulnerablePoints + First.m_nTrackVulnerablePoints ) > ( Second.m_nLocalVulnerablePoints + Second.m_nTrackVulnerablePoints );
	}
	);

	/* scan player smart function */
	auto ScanPlayer = [ ] ( C_BasePlayer* Player, LagRecord_t* Record, const Vector& vecShootPosition ) -> AimPointData_t
	{
		/* get the fuck out */
		if ( Record->m_bIsInvalid || !( int ) ( Record->m_flSimulationTime ) )
			return AimPointData_t( );

		/* set record */
		g_LagCompensation->ForcePlayerRecord( Player, Record );

		/* return result */
		return g_RageBot->CalculateTargetPoint( Player, vecShootPosition, Record );
	};

	bool bShouldRunAutoStop = g_RageBot->ShouldRunAutoStop( pWeapon );

	/* reset last player */
	m_ScanData.m_nLastPlayer = 0;

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

		/* if backtrack has priority higher than main record, then we will scan it first */
		if ( Target.m_nLocalVulnerablePoints < Target.m_nTrackVulnerablePoints )
		{
			/* scan BT */
			AimPointData_t m_Data = ScanPlayer( Target.m_pPlayer, &Target.m_TrackRecord, vecShootPosition );
			if ( !m_Data.m_bIsInvalid )
			{
				/* run autostop */
				if ( bShouldRunAutoStop )
				{
					/* force autostop */
					g_AutoStop->RunAutoStop( );

					/* repredict */
					g_EnginePrediction->RePredict( );
				}

				/* exit find target because we found a target */
				m_ScanData.m_AimTarget.m_Player = Target.m_pPlayer;
				m_ScanData.m_AimTarget.m_Record = Target.m_TrackRecord;
				m_ScanData.m_AimTarget.m_Point = m_Data;

				/* mark this target prioritized */
				m_ScanData.m_nLastPlayer = Target.m_pPlayer->EntIndex( );

				/* exit */
				break;
			}

			/* we did scan backtrack */
			bHasBacktrackScanned = true;
		}


		/* scan default record */
		AimPointData_t m_Data = ScanPlayer( Target.m_pPlayer, &Target.m_LocalRecord, vecShootPosition );
		if ( !m_Data.m_bIsInvalid )
		{
			/* run autostop */
			if ( bShouldRunAutoStop )
			{
				/* force autostop */
				g_AutoStop->RunAutoStop( );

				/* repredict */
				g_EnginePrediction->RePredict( );
			}

			/* exit find target because we found a target */
			m_ScanData.m_AimTarget.m_Player = Target.m_pPlayer;
			m_ScanData.m_AimTarget.m_Record = Target.m_LocalRecord;
			m_ScanData.m_AimTarget.m_Point = m_Data;

			/* mark this target prioritized */
			m_ScanData.m_nLastPlayer = Target.m_pPlayer->EntIndex( );

			/* exit */
			break;
		}
		else if ( !bHasBacktrackScanned )
		{
			/* scan BT */
			AimPointData_t m_Data = ScanPlayer( Target.m_pPlayer, &Target.m_TrackRecord, vecShootPosition );
			if ( !m_Data.m_bIsInvalid )
			{
				/* Run autostop */
				if ( bShouldRunAutoStop )
				{
					/* force autostop */
					g_AutoStop->RunAutoStop( );

					/* repredict */
					g_EnginePrediction->RePredict( );
				}

				/* exit find target because we found a target */
				m_ScanData.m_AimTarget.m_Player = Target.m_pPlayer;
				m_ScanData.m_AimTarget.m_Record = Target.m_TrackRecord;
				m_ScanData.m_AimTarget.m_Point = m_Data;

				/* mark this target prioritized */
				m_ScanData.m_nLastPlayer = Target.m_pPlayer->EntIndex( );

				/* exit */
				break;
			}
		}

		/* set the player as bad */
		m_ScanData.m_BadPlayers[ Target.m_pPlayer->EntIndex( ) ] = true;
	}
}
void C_RageBot::ComputePointsPriority( C_BasePlayer* Player, LagRecord_t* m_Record, std::vector < PointData_t >& Data )
{
	/* Get weapon data */
	C_BaseCombatWeapon* m_pLocalWeapon = g_Globals->m_LocalPlayer->m_hActiveWeapon( );
	C_CSWeaponData* m_pWeaponData = m_pLocalWeapon->GetWeaponData( );

	/* Useful function to get avg dmg in this hitbox */
	auto GetHitboxAvgDmg = [ & ] ( int nHitBox ) -> int
	{
		CGameTrace Trace;
		Trace.hit_entity = Player;
		Trace.hitbox = nHitBox;

		float flDamage = static_cast < float > ( m_pWeaponData->m_iDamage );
		g_AutoWall->ScaleDamage( Trace, flDamage );

		return static_cast < int > ( std::floorf( flDamage ) );
	};

	/* fast weapon*/
	const bool bIsFastWeapon = g_Utils->TimeToTicks( m_pWeaponData->m_flCycleTime ) <= 16;

	/* Iterate points */
	for ( auto& Point : Data )
	{
		/* Prioritize points by hitgroup */
		switch ( Point.m_nHitGroup )
		{
			/* Setup Head Priority */
			case RageHitgroup::Head:
			{
				/* Increase head priority on shot or resolved */
				if ( m_Record->m_bDidShot || m_Record->m_bIsResolved )
					Point.m_nPriority += 2;

				/* Increse head priority in crouch non-lethal or walking, because there is velocity is perfect */
				if ( m_Record->m_bIsWalking || m_Record->m_nFlags & FL_DUCKING )
				{
					/* Check health */
					if ( Player->m_iHealth( ) > 80 )
						Point.m_nPriority += 2;
				}

				/* On low desync delta we might have additional priority on head */
				if ( m_Record->m_flAnimationVelocity > 0.0f )
					Point.m_nPriority += 2;
			}
			break;

			/* Setup usual body priority */
			case RageHitgroup::Chest:
			case RageHitgroup::Stomach:
			case RageHitgroup::Pelvis:
			case RageHitgroup::Arms:
			case RageHitgroup::Legs:
			{
				/* Another code for legs */
				if ( Point.m_nHitGroup == RageHitgroup::Legs )
				{
					/* Average damage in legs! */
					int iLegsAvgDamage = ( GetHitboxAvgDmg( HITBOX_LEFT_THIGH ) + GetHitboxAvgDmg( HITBOX_LEFT_FOOT ) ) / 2;
					
					/* With safepoints for legs it must have VELOCITY FIXED to shoot there */
					if ( !g_SettingsManager->B[ _S( "ragebot.forced_safety" ) ] || ( m_Record->m_flAnimationVelocity > 0.0f || m_Record->m_vecVelocity.Length2D( ) < m_Record->m_flMaxSpeed * 0.34f ) )
					{
						/* Increment for lethal targets */
						if ( iLegsAvgDamage > Player->m_iHealth( ) )
							++Point.m_nPriority;

						/* Increment with fast weapons and doubletap active */
						if ( g_Globals->m_Packet.m_bDoubleTap )
						{
							/* Check weapon speed must be lower or equals then 16 ticks */
							if ( bIsFastWeapon )
								Point.m_nPriority += 2;
						}
					}
					else if ( m_Record->m_flAnimationVelocity > 0.0f )
					{
						++Point.m_nPriority;
						
						/* Increment with fast weapons and doubletap active */
						if ( g_Globals->m_Packet.m_bDoubleTap )
						{
							/* Check weapon speed must be lower or equals then 16 ticks */
							if ( bIsFastWeapon )
								++Point.m_nPriority;
						}
					}
				}
				/* Another code for chest, pelvis and stomach */
				else if ( Point.m_nHitGroup == RageHitgroup::Pelvis || Point.m_nHitGroup == RageHitgroup::Stomach )
				{
					/* Select hitbox from hitgroup */
					int nHitBox = -1;
					switch ( Point.m_nHitGroup )
					{
						case RageHitgroup::Stomach: nHitBox = HITBOX_STOMACH; break;
						case RageHitgroup::Pelvis: nHitBox = HITBOX_PELVIS; break;
					}

					/* Additional check against undefined behavior */
					if ( nHitBox > -1 )
					{
						/* Get Hitbox Dmg */
						float fHitboxDmg = GetHitboxAvgDmg( nHitBox );
						if ( fHitboxDmg > 0.0f )
						{
							/* Check for lethal */
							if ( fHitboxDmg > Player->m_iHealth( ) )
								Point.m_nPriority += 2;
							
							/* Additional priority for DT */
							if ( g_Globals->m_Packet.m_bDoubleTap )
								if ( bIsFastWeapon )
									Point.m_nPriority += 2;
						}

						/* AWP/R8 */
						if ( m_pLocalWeapon->m_nItemID( ) == WEAPON_REVOLVER || m_pLocalWeapon->m_nItemID( ) == WEAPON_AWP || m_Settings.m_bSafePriority )
							++Point.m_nPriority;
					}
				}
				else if ( Point.m_nHitGroup == RageHitgroup::Chest ) // пошел нахуй сука ебаная чест
				{
					int nAvgChestDmg = GetHitboxAvgDmg( HITBOX_CHEST );

					/* Additional priority for DT */
					if ( g_Globals->m_Packet.m_bDoubleTap )
						if ( bIsFastWeapon )
							Point.m_nPriority++;

					/* Check lethal */
					if ( nAvgChestDmg > Player->m_iHealth( ) )
						++Point.m_nPriority;
				}
				else
				{
					/* Avg damage in arms */
					int nAvgArmsDmg = ( GetHitboxAvgDmg( HITBOX_LEFT_UPPER_ARM ) + GetHitboxAvgDmg( HITBOX_LEFT_FOREARM ) ) / 2;

					/* Additional priority for DT */
					if ( g_Globals->m_Packet.m_bDoubleTap )
					{
						/* Check weapon speed must be lower or equals then 16 ticks */
						if ( bIsFastWeapon )
							Point.m_nPriority++;
					}

					/* Check lethal */
					if ( nAvgArmsDmg > Player->m_iHealth( ) )
						++Point.m_nPriority;
				}
			}
			break;
		}
	}
}
AimPointData_t C_RageBot::CalculateTargetPoint( C_BasePlayer* Player, const Vector& vecShootPosition, LagRecord_t* m_Record )
{
	/* Get matrix */
	matrix3x4_t* aMatrix = m_Record->m_Matricies[ 0 ].data( );

	/* Get player's relative rotation along localplayer */
	float flTargetYaw = std::roundf( Math::NormalizeAngle( Math::CalcAngle( vecShootPosition, m_Record->m_vecOrigin ).yaw ) );
	float flRotationDiff = std::roundf( Math::AngleNormalize( Math::AngleDiff( flTargetYaw, Math::NormalizeAngle( m_Record->m_angEyeAngles.yaw ) ) + 180.0f ) );

	/* Legs Multiply */
	float flSideMultiply = 1.0f;
	if ( flRotationDiff > 45.0f || flRotationDiff < -130.0f )
		flSideMultiply = -1.0f;

	/* MinDamage */
	float flMinDamage = static_cast < float > ( min( m_Settings.m_nMinDamage, Player->m_iHealth( ) ) );
	if ( flMinDamage >= 100.0f )
		flMinDamage = static_cast < float > ( Player->m_iHealth( ) ) - 100.0f + m_Settings.m_nMinDamage;

	/* Leg safety */
	bool bLegSafety = m_Record->m_vecVelocity.Length2D( ) < 5.0f || m_Record->m_flAnimationVelocity > 0.0f;

	/* total aimbot points */
	std::vector < PointData_t > Points;

	/* Build aimbot points */
	if ( m_Settings.m_Hitboxes[ RageHitbox::Head ] )
	{
		/* get hitbox data */
		HitboxData_t Head = this->GetHitboxData( Player, aMatrix, HITBOX_HEAD );

		/* build point */
		Vector vecCenter;
		Math::VectorTransform( ( Head.m_vecBBMax + Head.m_vecBBMin ) * 0.5f, aMatrix[ Head.m_pHitbox->m_nBone ], vecCenter );

		Vector vecForward;
		Math::AngleVectors( Math::CalcAngle( vecShootPosition, vecCenter ), vecForward );

		Points.push_back( PointData_t( vecCenter + Vector( 0.0f, 0.0f, Head.m_flRadius * 0.75f ), RageHitgroup::Head, HITBOX_HEAD ) );
	}
	if ( m_Settings.m_Hitboxes[ RageHitbox::Chest ] )
	{
		/* get hitbox data */
		HitboxData_t Chest = this->GetHitboxData( Player, aMatrix, HITBOX_CHEST );

		/* build point */
		Vector vecCenter;
		Math::VectorTransform( ( Chest.m_vecBBMax + Chest.m_vecBBMin ) * 0.5f, aMatrix[ Chest.m_pHitbox->m_nBone ], vecCenter );

		Vector vecForward;
		Math::AngleVectors( Math::CalcAngle( vecShootPosition, vecCenter ), vecForward );

		float flScale = Chest.m_flRadius * 0.75f;
		Points.push_back( PointData_t( vecCenter + vecForward.Cross( Vector( 0.0f, 0.0f, flScale ) ), RageHitgroup::Chest, HITBOX_CHEST ) );
		Points.push_back( PointData_t( vecCenter - vecForward.Cross( Vector( 0.0f, 0.0f, flScale ) ), RageHitgroup::Chest, HITBOX_CHEST ) );
	}
	if ( m_Settings.m_Hitboxes[ RageHitbox::Stomach ] )
	{
		/* get hitbox data */
		HitboxData_t Stomach = this->GetHitboxData( Player, aMatrix, HITBOX_STOMACH );

		/* build point */
		Vector vecCenter;
		Math::VectorTransform( ( Stomach.m_vecBBMax + Stomach.m_vecBBMin ) * 0.5f, aMatrix[ Stomach.m_pHitbox->m_nBone ], vecCenter );

		Vector vecForward;
		Math::AngleVectors( Math::CalcAngle( vecShootPosition, vecCenter ), vecForward );

		float flScale = 1.05f * Stomach.m_flRadius * 0.8f;
		Points.push_back( PointData_t( vecCenter + vecForward.Cross( Vector( 0.0f, 0.0f, flScale ) ), RageHitgroup::Stomach, HITBOX_STOMACH ) );
		Points.push_back( PointData_t( vecCenter - vecForward.Cross( Vector( 0.0f, 0.0f, flScale ) ), RageHitgroup::Stomach, HITBOX_STOMACH ) );
	}
	if ( m_Settings.m_Hitboxes[ RageHitbox::LeftThigh ] )
	{
		/* get hitbox data */
		HitboxData_t LeftThigh = this->GetHitboxData( Player, aMatrix, HITBOX_LEFT_THIGH );

		/* build point */
		Vector vecCenter;
		Math::VectorTransform( ( LeftThigh.m_vecBBMax + LeftThigh.m_vecBBMin ) * 0.5f, aMatrix[ LeftThigh.m_pHitbox->m_nBone ], vecCenter );

		Vector vecForward;
		Math::AngleVectors( Math::CalcAngle( vecShootPosition, vecCenter ), vecForward );

		if ( bLegSafety )
			Points.push_back( PointData_t( vecCenter + ( vecForward.Cross( Vector( 0.0f, 0.0f, LeftThigh.m_flRadius * 0.45f ) ) * -flSideMultiply ), RageHitgroup::Legs, HITBOX_LEFT_THIGH ) );
	}
	if ( m_Settings.m_Hitboxes[ RageHitbox::RightThigh ] )
	{
		/* get hitbox data */
		HitboxData_t RightThigh = this->GetHitboxData( Player, aMatrix, HITBOX_RIGHT_THIGH );

		/* build point */
		Vector vecCenter;
		Math::VectorTransform( ( RightThigh.m_vecBBMax + RightThigh.m_vecBBMin ) * 0.5f, aMatrix[ RightThigh.m_pHitbox->m_nBone ], vecCenter );

		Vector vecForward;
		Math::AngleVectors( Math::CalcAngle( vecShootPosition, vecCenter ), vecForward );

		if ( bLegSafety )
			Points.push_back( PointData_t( vecCenter + ( vecForward.Cross( Vector( 0.0f, 0.0f, RightThigh.m_flRadius * 0.45f ) ) * flSideMultiply ), RageHitgroup::Legs, HITBOX_RIGHT_THIGH ) );
	}
	if ( m_Settings.m_Hitboxes[ RageHitbox::LeftLeg ] )
	{
		/* get hitbox data */
		HitboxData_t LeftFoot = this->GetHitboxData( Player, aMatrix, HITBOX_LEFT_FOOT );

		/* build point */
		Vector vecCenter;
		Math::VectorTransform( ( LeftFoot.m_vecBBMax + LeftFoot.m_vecBBMin ) * 0.5f, aMatrix[ LeftFoot.m_pHitbox->m_nBone ], vecCenter );

		Vector vecForward;
		Math::AngleVectors( Math::CalcAngle( vecShootPosition, vecCenter ), vecForward );

		if ( bLegSafety )
			Points.push_back( PointData_t( vecCenter, RageHitgroup::Legs, HITBOX_LEFT_FOOT ) );
	}
	if ( m_Settings.m_Hitboxes[ RageHitbox::RightLeg ] )
	{
		/* get hitbox data */
		HitboxData_t RightFoot = this->GetHitboxData( Player, aMatrix, HITBOX_RIGHT_FOOT );

		/* build point */
		Vector vecCenter;
		Math::VectorTransform( ( RightFoot.m_vecBBMax + RightFoot.m_vecBBMin ) * 0.5f, aMatrix[ RightFoot.m_pHitbox->m_nBone ], vecCenter );

		Vector vecForward;
		Math::AngleVectors( Math::CalcAngle( vecShootPosition, vecCenter ), vecForward );

		if ( bLegSafety )
			Points.push_back( PointData_t( vecCenter, RageHitgroup::Legs, HITBOX_RIGHT_FOOT ) );
	}
	if ( m_Settings.m_Hitboxes[ RageHitbox::LeftForeArm ] )
	{
		/* get hitbox data */
		HitboxData_t LeftForearm = this->GetHitboxData( Player, aMatrix, HITBOX_LEFT_FOREARM );

		/* build point */
		Vector vecCenter;
		Math::VectorTransform( ( LeftForearm.m_vecBBMax + LeftForearm.m_vecBBMin ) * 0.5f, aMatrix[ LeftForearm.m_pHitbox->m_nBone ], vecCenter );

		Vector vecForward;
		Math::AngleVectors( Math::CalcAngle( vecShootPosition, vecCenter ), vecForward );

		Points.push_back( PointData_t( vecCenter, RageHitgroup::Arms, HITBOX_LEFT_FOREARM ) );
	}
	if ( m_Settings.m_Hitboxes[ RageHitbox::RightForeArm ] )
	{
		/* get hitbox data */
		HitboxData_t RightForearm = this->GetHitboxData( Player, aMatrix, HITBOX_RIGHT_FOREARM );

		/* build point */
		Vector vecCenter;
		Math::VectorTransform( ( RightForearm.m_vecBBMax + RightForearm.m_vecBBMin ) * 0.5f, aMatrix[ RightForearm.m_pHitbox->m_nBone ], vecCenter );

		Vector vecForward;
		Math::AngleVectors( Math::CalcAngle( vecShootPosition, vecCenter ), vecForward );

		Points.push_back( PointData_t( vecCenter, RageHitgroup::Arms, HITBOX_RIGHT_FOREARM ) );
	}
	if ( m_Settings.m_Hitboxes[ RageHitbox::LeftUpperArm ] )
	{
		/* get hitbox data */
		HitboxData_t LeftUpperArm = this->GetHitboxData( Player, aMatrix, HITBOX_LEFT_UPPER_ARM );

		/* build point */
		Vector vecCenter;
		Math::VectorTransform( ( LeftUpperArm.m_vecBBMax + LeftUpperArm.m_vecBBMin ) * 0.5f, aMatrix[ LeftUpperArm.m_pHitbox->m_nBone ], vecCenter );

		Vector vecForward;
		Math::AngleVectors( Math::CalcAngle( vecShootPosition, vecCenter ), vecForward );

		Points.push_back( PointData_t( vecCenter, RageHitgroup::Arms, HITBOX_LEFT_UPPER_ARM ) );
	}
	if ( m_Settings.m_Hitboxes[ RageHitbox::RightUpperArm ] )
	{
		/* get hitbox data */
		HitboxData_t RightUpperArm = this->GetHitboxData( Player, aMatrix, HITBOX_RIGHT_UPPER_ARM );

		/* build point */
		Vector vecCenter;
		Math::VectorTransform( ( RightUpperArm.m_vecBBMax + RightUpperArm.m_vecBBMin ) * 0.5f, aMatrix[ RightUpperArm.m_pHitbox->m_nBone ], vecCenter );

		Vector vecForward;
		Math::AngleVectors( Math::CalcAngle( vecShootPosition, vecCenter ), vecForward );

		Points.push_back( PointData_t( vecCenter, RageHitgroup::Arms, HITBOX_RIGHT_UPPER_ARM ) );
	}

	/* Compute points priority */
	g_RageBot->ComputePointsPriority( Player, m_Record, Points );

	/* Multithread data */
	std::vector < PenetrationData_t > m_Data;
	for ( auto& Point : Points )
	{
		if ( g_SettingsManager->B[ _S( "ragebot.forced_safety" ) ] )
		{
			HitboxData_t Data = g_RageBot->GetHitboxData( Player, aMatrix, Point.m_nHitBox );
			if ( !g_RageBot->IsSafePoint( Player, m_Record, vecShootPosition, Point.m_vecPoint, Data ) )
				continue;
		}

		/* Generate job */
		PenetrationData_t* Data = &m_Data.emplace_back( );
		Data->m_flMinDamage = flMinDamage;
		Data->m_vecShootPosition = vecShootPosition;
		Data->m_vecTargetPosition = Point.m_vecPoint;
		Data->m_nPriority = Point.m_nPriority;
		Data->m_Hitbox = Point.m_nHitBox;
		Threading::QueueJobRef( g_AutoWall->ScanPoint, Data );
	}
	Threading::FinishQueue( );

	/* remove invalid data */
	for ( int nIt = 0; nIt < m_Data.size( ); nIt++ )
	{
		if ( nIt >= m_Data.size( ) )
			continue;

		if ( !m_Data[ nIt ].m_bSuccess )
			m_Data.erase( m_Data.begin( ) + nIt-- );
	}

	/* Check empty */
	if ( m_Data.empty( ) )
		return AimPointData_t( );

	/* Filter scanned data by priority */
	auto SortDataByPriority = [ & ] ( const PenetrationData_t& First, const PenetrationData_t& Second ) -> bool
	{
		return First.m_nPriority > Second.m_nPriority || First.m_flCurrentDamage > Second.m_flCurrentDamage;
	};
	std::sort( m_Data.begin( ), m_Data.end( ), SortDataByPriority );
	
	/* return data */
	return AimPointData_t( m_Data.front( ).m_vecTargetPosition, m_Data.front( ).m_flCurrentDamage, m_Data.front( ).m_nPriority, g_RageBot->GetHitboxData( Player, aMatrix, m_Data.front( ).m_Hitbox ) );
}
LagRecord_t C_LagCompensation::FindBestRecord( C_BasePlayer* pPlayer, std::deque < LagRecord_t > m_LagRecords, int& nPriority, const float& flSimTime )
{
	/* set the latest record as default */
	LagRecord_t Record;
	Record.m_bIsInvalid = true;
	if ( g_Globals->m_Packet.m_bAntiExploit )
	{
		/* Time breaker exploit fix is coming soon! */
		return Record;
	}

	/* state */
	bool bState = false;

	/* iterate all records */
	for ( int nRecord = 0; nRecord < m_LagRecords.size( ); nRecord++ )
	{
		/* skip exist record */
		if ( m_LagRecords[ nRecord ].m_flSimulationTime == flSimTime )
			continue;

		/* skip invalid records automatically */
		if ( !g_LagCompensation->IsRecordValid( pPlayer, &m_LagRecords[ nRecord ] ) )
			continue;

		/* check the oldest record */
		if ( !bState )
		{
			/* set record */
			Record = m_LagRecords[ nRecord ];

			/* check record priority */
			if ( !Record.m_bIsInvalid )
			{
				/* get priority */
				nPriority = g_LagCompensation->GetRecordPriority( &Record );

				/* if record has 75% of max points then use it immediately */
				if ( nPriority >= 6 )
					return Record;
			}

			/* set state */
			bState = true;

			/* this record has been checked */
			continue;
		}

		/* check priority */
		int nTempPriority = g_LagCompensation->GetRecordPriority( &m_LagRecords[ nRecord ] );
		if ( nTempPriority <= nPriority )
			continue;

		/* set as the best */
		nPriority = nTempPriority;
		Record = m_LagRecords[ nRecord ];
	}

	/* return result */
	return Record;
}
LagRecord_t C_LagCompensation::FindFirstRecord( C_BasePlayer* pPlayer, std::deque < LagRecord_t > m_LagRecords )
{
	/* define record and make it invalid */
	LagRecord_t LagRecord;
	LagRecord.m_bIsInvalid = true;

	/* iterate all records */
	for ( int nRecord = m_LagRecords.size( ) - 1; nRecord > -1; nRecord-- )
	{
		auto Record = m_LagRecords[ nRecord ];
		if ( g_LagCompensation->IsRecordValid( pPlayer, &Record ) )
			return Record;
	}

	/* return record */
	return LagRecord;
}
HitboxData_t C_RageBot::GetHitboxData( C_BasePlayer* Player, matrix3x4_t* aMatrix, int nHitbox )
{
	studiohdr_t* pStudioHdr = ( studiohdr_t* ) ( SDK::Interfaces::ModelInfo->GetStudiomodel( Player->GetModel( ) ) );
	if ( !pStudioHdr )
		return HitboxData_t( );

	mstudiohitboxset_t* pHitset = pStudioHdr->GetHitboxSet( Player->m_nHitboxSet( ) );
	if ( !pHitset )
		return HitboxData_t( );

	mstudiobbox_t* pHitbox = pHitset->GetHitbox( nHitbox );
	if ( !pHitbox )
		return HitboxData_t( );

	/* Capture hitbox data */
	HitboxData_t Hitbox;
	Hitbox.m_nHitbox = nHitbox;
	Hitbox.m_pHitbox = pHitbox;
	Hitbox.m_vecBBMin = pHitbox->m_vecBBMin;
	Hitbox.m_vecBBMax = pHitbox->m_vecBBMax;
	Hitbox.m_flRadius = pHitbox->m_flRadius;

	/* Compute vecMins/vecMaxs */
	Math::VectorTransform( pHitbox->m_vecBBMin, aMatrix[ pHitbox->m_nBone ], Hitbox.m_vecMins );
	Math::VectorTransform( pHitbox->m_vecBBMax, aMatrix[ pHitbox->m_nBone ], Hitbox.m_vecMaxs );

	/* Return result */
	return Hitbox;
}

#ifdef OVERSEE_DEV
#include "../../../Interface/Renderer/Renderer.hpp"
#include "../../Fonts/FontManager.hpp"
void C_RageBot::DebugDrawData( C_BasePlayer* Player )
{
	return;
	/* get lagcomp data of this player */
	auto Records = g_LagCompensation->GetPlayerRecords( Player );
	if ( Records.empty( ) )
		return;

	const Vector vecShootPosition = g_LocalAnimations->GetShootPosition( );

	auto Matrix = Records.back( ).m_Matricies[ 0 ];

	// get player rotation relative to us
	float flTargetYaw = std::roundf( Math::NormalizeAngle( Math::CalcAngle( vecShootPosition, Records.back( ).m_vecOrigin ).yaw ) );
	float flRotationDiff = std::roundf( Math::AngleNormalize( Math::AngleDiff( flTargetYaw, Math::NormalizeAngle( Records.back( ).m_angEyeAngles.yaw ) ) + 180.0f ) );
	
	/*{
		Vector vec2D;
		if ( !SDK::Interfaces::DebugOverlay->ScreenPosition( Player->GetAbsOrigin( ), vec2D ) )
		{
			g_Renderer->AddText( g_FontManager->GetFontFromHash( FnvHash( "Player ESP" ) ), 14.0f, ImVec2( vec2D.x, vec2D.y ), std::to_string( flRotationDiff ), ImColor( 255, 255, 255, 255 ) );
		}
	}*/

	std::vector < Vector > Points;

	/* HEAD */
	{
		/* get hitbox data */
		HitboxData_t Head = this->GetHitboxData( Player, Matrix.data( ), HITBOX_HEAD );

		/* build point */
		Vector vecCenter;
		Math::VectorTransform( ( Head.m_vecBBMax + Head.m_vecBBMin ) * 0.5f, Matrix[ Head.m_pHitbox->m_nBone ], vecCenter );

		Vector vecForward;
		Math::AngleVectors( Math::CalcAngle( vecShootPosition, vecCenter ), vecForward );

		Points.push_back( vecCenter + Vector( 0.0f, 0.0f, Head.m_flRadius * 0.75f ) );
	}

	/* CHEST */
	{
		/* get hitbox data */
		HitboxData_t Chest = this->GetHitboxData( Player, Matrix.data( ), HITBOX_CHEST );

		/* build point */
		Vector vecCenter;
		Math::VectorTransform( ( Chest.m_vecBBMax + Chest.m_vecBBMin ) * 0.5f, Matrix[ Chest.m_pHitbox->m_nBone ], vecCenter );

		Vector vecForward;
		Math::AngleVectors( Math::CalcAngle( vecShootPosition, vecCenter ), vecForward );

		float flScale = 1.05f * Chest.m_flRadius * 0.825f;
		Points.push_back( vecCenter + vecForward.Cross( Vector( 0.0f, 0.0f, flScale ) ) );
		Points.push_back( vecCenter - vecForward.Cross( Vector( 0.0f, 0.0f, flScale ) ) );
	}

	/* STOMACH */
	{
		/* get hitbox data */
		HitboxData_t Stomach = this->GetHitboxData( Player, Matrix.data( ), HITBOX_STOMACH );

		/* build point */
		Vector vecCenter;
		Math::VectorTransform( ( Stomach.m_vecBBMax + Stomach.m_vecBBMin ) * 0.5f, Matrix[ Stomach.m_pHitbox->m_nBone ], vecCenter );

		Vector vecForward;
		Math::AngleVectors( Math::CalcAngle( vecShootPosition, vecCenter ), vecForward );

		float flScale = 1.05f * Stomach.m_flRadius * 0.85f;
		Points.push_back( vecCenter + vecForward.Cross( Vector( 0.0f, 0.0f, flScale ) ) );
		Points.push_back( vecCenter - vecForward.Cross( Vector( 0.0f, 0.0f, flScale ) ) );
	}

	/* Legs Multiply */
	float flLegsMultiply = 1.0f;
	if ( flRotationDiff > 45.0f || flRotationDiff < -130.0f )
		flLegsMultiply = -1.0f;

	/* Left thigh */
	{
		/* get hitbox data */
		HitboxData_t LeftThigh = this->GetHitboxData( Player, Matrix.data( ), HITBOX_LEFT_THIGH );

		/* build point */
		Vector vecCenter;
		Math::VectorTransform( ( LeftThigh.m_vecBBMax + LeftThigh.m_vecBBMin ) * 0.5f, Matrix[ LeftThigh.m_pHitbox->m_nBone ], vecCenter );

		Vector vecForward;
		Math::AngleVectors( Math::CalcAngle( vecShootPosition, vecCenter ), vecForward );

		Points.push_back( vecCenter + ( vecForward.Cross( Vector( 0.0f, 0.0f, LeftThigh.m_flRadius * 0.6f ) ) * -flLegsMultiply ) );
	}

	/* Right thigh */
	{
		/* get hitbox data */
		HitboxData_t RightThigh = this->GetHitboxData( Player, Matrix.data( ), HITBOX_RIGHT_THIGH );

		/* build point */
		Vector vecCenter;
		Math::VectorTransform( ( RightThigh.m_vecBBMax + RightThigh.m_vecBBMin ) * 0.5f, Matrix[ RightThigh.m_pHitbox->m_nBone ], vecCenter );

		Vector vecForward;
		Math::AngleVectors( Math::CalcAngle( vecShootPosition, vecCenter ), vecForward );

		Points.push_back( vecCenter + ( vecForward.Cross( Vector( 0.0f, 0.0f, RightThigh.m_flRadius * 0.6f ) ) * flLegsMultiply ) );
	}

	/* Left foot */
	{
		/* get hitbox data */
		HitboxData_t LeftFoot = this->GetHitboxData( Player, Matrix.data( ), HITBOX_LEFT_FOOT );

		/* build point */
		Vector vecCenter;
		Math::VectorTransform( ( LeftFoot.m_vecBBMax + LeftFoot.m_vecBBMin ) * 0.5f, Matrix[ LeftFoot.m_pHitbox->m_nBone ], vecCenter );

		Vector vecForward;
		Math::AngleVectors( Math::CalcAngle( vecShootPosition, vecCenter ), vecForward );

		Points.push_back( vecCenter );
	}

	/* Right foot */
	{
		/* get hitbox data */
		HitboxData_t RightFoot = this->GetHitboxData( Player, Matrix.data( ), HITBOX_RIGHT_FOOT );

		/* build point */
		Vector vecCenter;
		Math::VectorTransform( ( RightFoot.m_vecBBMax + RightFoot.m_vecBBMin ) * 0.5f, Matrix[ RightFoot.m_pHitbox->m_nBone ], vecCenter );

		Vector vecForward;
		Math::AngleVectors( Math::CalcAngle( vecShootPosition, vecCenter ), vecForward );

		Points.push_back( vecCenter );
	}

	/* Left fore arm */
	{
		/* get hitbox data */
		HitboxData_t LeftFoot = this->GetHitboxData( Player, Matrix.data( ), HITBOX_LEFT_FOREARM );

		/* build point */
		Vector vecCenter;
		Math::VectorTransform( ( LeftFoot.m_vecBBMax + LeftFoot.m_vecBBMin ) * 0.5f, Matrix[ LeftFoot.m_pHitbox->m_nBone ], vecCenter );

		Vector vecForward;
		Math::AngleVectors( Math::CalcAngle( vecShootPosition, vecCenter ), vecForward );

		Points.push_back( vecCenter );
	}

	/* Right fore arm */
	{
		/* get hitbox data */
		HitboxData_t RightFoot = this->GetHitboxData( Player, Matrix.data( ), HITBOX_RIGHT_FOREARM );

		/* build point */
		Vector vecCenter;
		Math::VectorTransform( ( RightFoot.m_vecBBMax + RightFoot.m_vecBBMin ) * 0.5f, Matrix[ RightFoot.m_pHitbox->m_nBone ], vecCenter );

		Vector vecForward;
		Math::AngleVectors( Math::CalcAngle( vecShootPosition, vecCenter ), vecForward );

		Points.push_back( vecCenter );
	}

	/* Left upper arm */
	{
		/* get hitbox data */
		HitboxData_t LeftFoot = this->GetHitboxData( Player, Matrix.data( ), HITBOX_LEFT_UPPER_ARM );

		/* build point */
		Vector vecCenter;
		Math::VectorTransform( ( LeftFoot.m_vecBBMax + LeftFoot.m_vecBBMin ) * 0.5f, Matrix[ LeftFoot.m_pHitbox->m_nBone ], vecCenter );

		Vector vecForward;
		Math::AngleVectors( Math::CalcAngle( vecShootPosition, vecCenter ), vecForward );

		Points.push_back( vecCenter );
	}

	/* Right upper arm */
	{
		/* get hitbox data */
		HitboxData_t RightFoot = this->GetHitboxData( Player, Matrix.data( ), HITBOX_RIGHT_UPPER_ARM );

		/* build point */
		Vector vecCenter;
		Math::VectorTransform( ( RightFoot.m_vecBBMax + RightFoot.m_vecBBMin ) * 0.5f, Matrix[ RightFoot.m_pHitbox->m_nBone ], vecCenter );

		Vector vecForward;
		Math::AngleVectors( Math::CalcAngle( vecShootPosition, vecCenter ), vecForward );

		Points.push_back( vecCenter );
	}

	/* draw points */
	for ( auto Point : Points )
	{
		Vector vec2D;
		if ( SDK::Interfaces::DebugOverlay->ScreenPosition( Point, vec2D ) )
			continue;

		g_Renderer->AddRectFilled( ImVec2( vec2D.x - 1, vec2D.y - 1 ), ImVec2( vec2D.x + 1, vec2D.y + 1 ), ImColor( 0, 128, 255, 255 ) );
	}
}
#endif

/* ragebot safepoints part */
bool C_RageBot::CollidePoint( const Vector& vecStart, const Vector& vecEnd, mstudiobbox_t* pHitbox, matrix3x4_t* aMatrix )
{
	if ( !pHitbox || !aMatrix )
		return false;

	Ray_t Ray;
	Ray.Init( vecStart, vecEnd );

	CGameTrace Trace;
	Trace.fraction = 1.0f;
	Trace.startsolid = false;

	typedef int( __fastcall* ClipRayToHitbox_t )( const Ray_t&, mstudiobbox_t*, matrix3x4_t&, trace_t& );
	return ( ( ClipRayToHitbox_t ) ( SDK::EngineData::m_AddressList[ CheatAddressList::ClipRayToHitbox ] ) )( Ray, pHitbox, aMatrix[ pHitbox->m_nBone ], Trace ) >= 0;
}
void C_RageBot::ThreadedCollisionFunc( CollisionData_t* m_Collision )
{
	m_Collision->m_bResult = g_RageBot->CollidePoint( m_Collision->m_vecStart, m_Collision->m_vecForward, m_Collision->m_Hitbox.m_pHitbox, m_Collision->aMatrix );
}
bool C_RageBot::IsSafePoint( C_BasePlayer* Player, LagRecord_t* m_Record, const Vector& vecStart, const Vector& vecEnd, HitboxData_t HitboxData )
{
	// alloc data
	CollisionData_t m_Left;
	CollisionData_t m_Center;
	CollisionData_t m_Right;

	// setup base
	CollisionData_t m_Base = CollisionData_t( );
	m_Base.m_vecStart = vecStart;
	m_Base.m_vecForward = vecStart + ( ( vecEnd - vecStart ) * 8192.0f );
	m_Base.m_Hitbox = HitboxData;

	// copy
	std::memcpy( &m_Left, &m_Base, sizeof( CollisionData_t ) );
	std::memcpy( &m_Right, &m_Base, sizeof( CollisionData_t ) );
	std::memcpy( &m_Center, &m_Base, sizeof( CollisionData_t ) );

	// setup matricies
	m_Left.aMatrix = m_Record->m_Matricies[ 1 ].data( );
	m_Right.aMatrix = m_Record->m_Matricies[ 2 ].data( );
	m_Center.aMatrix = m_Record->m_Matricies[ 3 ].data( );
	
	C_RageBot::ThreadedCollisionFunc( &m_Left );
	C_RageBot::ThreadedCollisionFunc( &m_Right );
	C_RageBot::ThreadedCollisionFunc( &m_Center );

	// calc collision count
	int nCollisionCount = 0;
	if ( m_Left.m_bResult ) nCollisionCount++;
	if ( m_Right.m_bResult ) nCollisionCount++;
	if ( m_Center.m_bResult ) nCollisionCount++;

	// return safe status
	return nCollisionCount >= 2;
}

/* accuracy related ragebot part */
float C_RageBot::GetHitchance( )
{
	/* get player's matrix */
	matrix3x4_t* aMatrix = m_ScanData.m_AimTarget.m_Record.m_Matricies[ 0 ].data( );

	/* get weapon */
	C_BaseCombatWeapon* pWeapon = g_Globals->m_LocalPlayer->m_hActiveWeapon( );
	if ( !pWeapon )
		return 0.0f;

	/* check settings */
	if ( m_Settings.m_nHitChance <= 1 )
		return 100.0f;

	/* define weapon data */
	const C_CSWeaponData* pWeaponData = pWeapon->GetWeaponData( );
	const Vector vecShootPosition = g_LocalAnimations->GetShootPosition( );
	const float flWeaponSpread = g_EnginePrediction->GetSpread( );
	const float flWeaponInaccuracy = g_EnginePrediction->GetInaccuracy( );

	/* check range to point */
	if ( vecShootPosition.DistTo( m_ScanData.m_AimTarget.m_Point.m_vecPoint ) > pWeaponData->m_flRange )
		return 0.0f;

	/* in some cases we can skip hitchance */
	if ( g_RageBot->HasMaximumAccuracy( pWeapon, flWeaponInaccuracy ) )
		return 100.0f;

	/* get angles */
	QAngle angScaledAngles = Math::CalcAngle( vecShootPosition, m_ScanData.m_AimTarget.m_Point.m_vecPoint );
	Math::Normalize3( angScaledAngles );

	/* calculate vectors */
	Vector vecForward, vecRight, vecUp;
	Math::AngleVectors( angScaledAngles, vecForward, vecRight, vecUp );

	/* weapon id */
	const int nWeaponId = pWeapon->m_nItemID( );

	/* recoil idx */
	const float flRecoilIndex = pWeapon->m_flRecoilIndex( );

	/* inaccuracy && spread radius */
	float flInAccuracyCurveDensity = m_CachedData.m_Seeds.m_flInaccuracy;

	/* negev wild blast */
	if ( nWeaponId == 28 && flRecoilIndex < 3 )
	{
		for ( int j = 3; j > flRecoilIndex; --j )
			flInAccuracyCurveDensity *= flInAccuracyCurveDensity;
		flInAccuracyCurveDensity = 1.0f - flInAccuracyCurveDensity;
	}

	/* inaccuracy radius */
	float flInaccuracyRadius = flInAccuracyCurveDensity * flWeaponInaccuracy;

	/* loop seeds */
	int nValidSeeds = 0;
	for ( int nSeed = 0; nSeed < 256; nSeed++ )
	{
		float flSpreadCurveDensity = m_CachedData.m_Seeds.m_Spread[ nSeed ];

		/* negev wild blast */
		if ( nWeaponId == 28 && flRecoilIndex < 3 )
		{
			for ( int j = 3; j > flRecoilIndex; --j )
				flSpreadCurveDensity *= flSpreadCurveDensity;

			flInAccuracyCurveDensity = 1.0f - flInAccuracyCurveDensity;
		}

		/* calculate radius */
		float flSpreadRadius = flSpreadCurveDensity * flWeaponSpread;

		/* spread view */
		Vector vecSpreadView
		(
			( flInaccuracyRadius * cosf( m_CachedData.m_Seeds.m_Theta0[ nSeed ] ) ) + ( flSpreadRadius * cosf( m_CachedData.m_Seeds.m_Theta1[ nSeed ] ) ),
			( flInaccuracyRadius * sinf( m_CachedData.m_Seeds.m_Theta0[ nSeed ] ) ) + ( flSpreadRadius * sinf( m_CachedData.m_Seeds.m_Theta1[ nSeed ] ) ),
			0.0f
		);

		/* calculate direction view */
		Vector vecDirection = Vector( 0.0f, 0.0f, 0.0f );
		vecDirection.x = vecForward.x + ( vecSpreadView.x * vecRight.x ) + ( vecSpreadView.y * vecUp.x );
		vecDirection.y = vecForward.y + ( vecSpreadView.x * vecRight.y ) + ( vecSpreadView.y * vecUp.y );
		vecDirection.z = vecForward.z + ( vecSpreadView.x * vecRight.z ) + ( vecSpreadView.y * vecUp.z );
		vecDirection.Normalized( );

		/* calculate spread view and normalize it */
		QAngle angSpreadView;
		Math::VectorAngles( vecDirection, vecUp, angSpreadView );
		Math::Normalize3( angSpreadView );

		/* calculate forward view and normalize it */
		Vector vecForwardView;
		Math::AngleVectors( angSpreadView, vecForwardView );
		vecForwardView.NormalizeInPlace( );

		/* adjust forward view */
		vecForwardView = vecShootPosition + ( vecForwardView * pWeaponData->m_flRange );

		/* check point collision */
		if ( g_RageBot->CollidePoint( vecShootPosition, vecForwardView, m_ScanData.m_AimTarget.m_Point.m_HitboxData.m_pHitbox, aMatrix ) )
			nValidSeeds++;
	}

	/* return result */
	return ( float ) ( nValidSeeds ) / 2.56f;
}
float C_RageBot::GetAccuracy( )
{
	/* temporarily unused code */
	return 0.0f;
	/* get player's matrix */
	matrix3x4_t* aMatrix = m_ScanData.m_AimTarget.m_Record.m_Matricies[ 0 ].data( );

	/* get weapon */
	C_BaseCombatWeapon* pWeapon = g_Globals->m_LocalPlayer->m_hActiveWeapon( );
	if ( !pWeapon )
		return 0.0f;

	/* check settings */
	if ( m_Settings.m_nAccuracyBoost <= 1 )
		return 100.0f;

	/* define weapon data */
	const C_CSWeaponData* pWeaponData = pWeapon->GetWeaponData( );
	const Vector vecShootPosition = g_LocalAnimations->GetShootPosition( );
	const float flWeaponSpread = g_EnginePrediction->GetSpread( );
	const float flWeaponInaccuracy = g_EnginePrediction->GetInaccuracy( );
	const float flWeaponRange = pWeaponData->m_flRange;

	/* in some cases we can skip hitchance */
	if ( g_RageBot->HasMaximumAccuracy( pWeapon, flWeaponInaccuracy ) )
		return 100.0f;

	/* define player's hitbox */
	const auto lpHitbox = m_ScanData.m_AimTarget.m_Point.m_HitboxData.m_pHitbox;
	if ( !lpHitbox )
		return 0.0f;

	/* define hitbox's data */
	const float flSpreadCone = flWeaponInaccuracy + flWeaponSpread;

	/* get angles */
	QAngle angScaledAngles = Math::CalcAngle( vecShootPosition, m_ScanData.m_AimTarget.m_Point.m_vecPoint );
	Math::Normalize3( angScaledAngles );

	/* calculate vectors */
	Vector vecForward, vecRight, vecUp;
	Math::AngleVectors( angScaledAngles, vecForward, vecRight, vecUp );

	/* mindamage */
	float flMinDamage = fmin( ( float ) ( m_ScanData.m_AimTarget.m_Player->m_iHealth( ) ), m_Settings.m_nMinDamage );

	/* loop result */
	int nHitsAmount = 0;

	/* start loop */
	for ( int i = 1; i <= 6; i++ )
	{
		for ( int j = 0; j < 8; ++j )
		{
			/* spread+accuracy */
			float flCalculatedSpread = flSpreadCone * float( float( i ) / 6.f );

			/* direction cos/sin */
			float flDirCos, flDirSin;
			flDirCos = m_CachedData.m_AccuracyBoost.m_Iterations[ j ].first;
			flDirSin = m_CachedData.m_AccuracyBoost.m_Iterations[ j ].second;

			/* calculate spread */
			Vector2D vecSpread;
			vecSpread.x = flDirCos * flCalculatedSpread;
			vecSpread.y = flDirSin * flCalculatedSpread;

			/* calculate direction */
			Vector vecDirection;
			vecDirection.x = ( vecForward.x + vecSpread.x * vecRight.x + vecSpread.y * vecUp.x );
			vecDirection.y = ( vecForward.y + vecSpread.x * vecRight.y + vecSpread.y * vecUp.y );
			vecDirection.z = ( vecForward.z + vecSpread.x * vecRight.z + vecSpread.y * vecUp.z );

			/* calculate end point */
			Vector vecEnd = vecShootPosition + vecDirection * flWeaponRange;
			if ( g_RageBot->CollidePoint( vecShootPosition, vecEnd, lpHitbox, aMatrix ) )
			{
				/* setup autowall data */
				PenetrationData_t Data;
				Data.m_vecShootPosition = vecShootPosition;
				Data.m_vecDirection = vecDirection;
				Data.m_flMinDamage = flMinDamage;
				Data.m_flCurrentDistance = 0.0f;
				Data.m_PenetrationCount = 4;
				Data.m_flDamageModifier = 0.5f;
				Data.m_flPenetrationModifier = 1.0f;
				Data.m_flCurrentDamage = ( float ) ( pWeaponData->m_iDamage );

				/* run autowall */
				g_AutoWall->SimulateFireBullet( &Data );

				/* check success */
				if ( Data.m_bSuccess )
					nHitsAmount++;
			}
		}
	}

	/* return result */
	return ( ( float ) ( nHitsAmount ) / 48.0f ) * 100.0f;
}
void C_RageBot::OnVelocityModifierProxy( float flVelocityModifier )
{
	m_VelMod.m_bForceAccuracy = false;
	if ( flVelocityModifier < m_VelMod.m_flVelocityModifier )
		m_VelMod.m_bForceAccuracy = true;

	m_VelMod.m_flVelocityModifier = flVelocityModifier;
}
bool C_RageBot::HasMaximumAccuracy( C_BaseCombatWeapon* pWeapon, float flWeaponInAccuracy )
{
	/* skip everything if we have no inaccuracy at all */
	if ( SDK::EngineData::m_ConvarList[ CheatConvarList::WeaponAccuracyNoSpread ]->GetBool( ) )
		return true;

	/* we got forced accuracy because our velocity modifier has failed last time and we have wrong punch angles and smth */
	if ( m_VelMod.m_bForceAccuracy )
		return false;

	/* do not let two shots in a row without hitchance and accuracy checks */
	if ( pWeapon->m_flNextPrimaryAttack( ) == SDK::Interfaces::GlobalVars->m_flCurTime )
		return false;

	/* skip hitchance in air for scout and revolver if we have too low accuracy */
	if ( pWeapon->m_nItemID( ) == WEAPON_SSG08 || pWeapon->m_iItemIDHigh( ) == WEAPON_REVOLVER )
		if ( !( g_Globals->m_LocalPlayer->m_fFlags( ) & FL_ONGROUND ) )
			if ( flWeaponInAccuracy <= 0.009f )
				return true;

	/* and finally skip hitchance if we almost have no inaccuracy */
	if ( fabsf( flWeaponInAccuracy - m_CachedData.m_flDefaultAccuracy ) <= 0.0001f )
		return true;

	/* we cannot skip hitchance/accuracy boost */
	return false;
}
bool C_RageBot::ShouldRunAutoStop( C_BaseCombatWeapon* pWeapon )
{
	/* check velocity */
	if ( g_Globals->m_LocalPlayer->m_vecVelocity( ).Length2D( ) < g_EnginePrediction->GetMaxSpeed( ) * 0.34f )
		return false;

	/* we may run autostop only if weapon is valid */
	if ( !pWeapon->IsGun( ) && pWeapon->m_nItemID( ) != WEAPON_TASER )
		return false;

	/* check time */
	if ( pWeapon->GetWeaponData( )->m_flCycleTimeAlt > 0.2f )
		if ( !g_Globals->m_Packet.m_bCanFire )
			return false;

	/* check settings */
	if ( !m_Settings.m_bAutoStop )
		return false;

	/* do not run in air */
	if ( !( g_Globals->m_LocalPlayer->m_fFlags( ) & FL_ONGROUND ) )
		return false;

	/* allow */
	return true;
}