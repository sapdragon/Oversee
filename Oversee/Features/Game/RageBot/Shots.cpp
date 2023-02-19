#include "Shots.hpp"
#include "Autowall.hpp"
#include "LocalAnimations.hpp"
#include "../Prediction/EnginePrediction.hpp"
#include "../Networking/Networking.hpp"
#include "../../../SDK/Utils/Utils.hpp"
#include "../../Visuals/World/HitMarker.hpp"
#include "../../Visuals/Log Manager/LogManager.hpp"

void C_ShotSystem::OnBulletImpact( IGameEvent* m_GameEvent )
{
	Vector vecImpactPosition = Vector( m_GameEvent->GetInt( _S( "x" ) ), m_GameEvent->GetInt( _S( "y" ) ), m_GameEvent->GetInt( _S( "z" ) ) );
	if ( SDK::Interfaces::EngineClient->GetPlayerForUserID( m_GameEvent->GetInt( _S( "userid" ) ) ) != SDK::Interfaces::EngineClient->GetLocalPlayer( ) || m_RageShots.empty( ) )
		return;

	/* sort algorithm */
	auto SortByTime = [ ] ( const RageShot_t& First, const RageShot_t& Second ) -> bool
	{
		const int nFirstDelta = g_Networking->GetServerTick( ) - First.m_nTickCount;
		const int nSecondDelta = g_Networking->GetServerTick( ) - Second.m_nTickCount;
		return nFirstDelta < nSecondDelta;
	};
	
	/* sort shots */
	std::sort( m_RageShots.begin( ), m_RageShots.end( ), SortByTime );

	/* set data */
	RageShot_t* Shot = &m_RageShots.front( );
	Shot->m_bRegisteredShot = true;
	Shot->m_ImpactList.push_back( vecImpactPosition );
}

void C_ShotSystem::OnPlayerHurt( IGameEvent* m_GameEvent )
{
	if ( SDK::Interfaces::EngineClient->GetPlayerForUserID( m_GameEvent->GetInt( _S( "attacker" ) ) ) != SDK::Interfaces::EngineClient->GetLocalPlayer( ) )
		return;

	if ( SDK::Interfaces::EngineClient->GetPlayerForUserID( m_GameEvent->GetInt( _S( "userid" ) ) ) == SDK::Interfaces::EngineClient->GetLocalPlayer( ) || m_RageShots.empty( ) )
		return;

	auto GetHitboxByHitgroup = [ ] ( int iHitgroup ) -> int
	{
		switch ( iHitgroup )
		{
			case HITGROUP_HEAD:
			return HITBOX_HEAD;
			case HITGROUP_CHEST:
			return HITBOX_CHEST;
			case HITGROUP_STOMACH:
			return HITBOX_STOMACH;
			case HITGROUP_LEFTARM:
			return HITBOX_LEFT_HAND;
			case HITGROUP_RIGHTARM:
			return HITBOX_RIGHT_HAND;
			case HITGROUP_LEFTLEG:
			return HITBOX_RIGHT_CALF;
			case HITGROUP_RIGHTLEG:
			return HITBOX_LEFT_CALF;
			default:
			return HITBOX_PELVIS;
		}
	};
	auto GetHitboxNameFromHitgroup = [ GetHitboxByHitgroup ] ( int iHitgroup ) -> std::string
	{
		switch ( GetHitboxByHitgroup( iHitgroup ) )
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

	/* sort algorithm */
	auto SortByTime = [ ] ( const RageShot_t& First, const RageShot_t& Second ) -> bool
	{
		const int nFirstDelta = g_Networking->GetServerTick( ) - First.m_nTickCount;
		const int nSecondDelta = g_Networking->GetServerTick( ) - Second.m_nTickCount;
		return nFirstDelta < nSecondDelta;
	};

	/* sort shots */
	std::sort( m_RageShots.begin( ), m_RageShots.end( ), SortByTime );

	/* set data */
	RageShot_t* Shot = &m_RageShots.front( );
	if ( Shot->m_bRegisteredShot )
	{
		/* set damage */
		Shot->m_flServerDamage = m_GameEvent->GetInt( _S( "dmg_health" ) );
		Shot->m_szServerHitbox = GetHitboxNameFromHitgroup( m_GameEvent->GetInt( _S( "hitgroup" ) ) );

		/* set as hurted */
		Shot->m_bDidHurtPlayer = true;
	}
}

void C_ShotSystem::OnFrameStageNotify( ClientFrameStage_t Stage )
{
	/* check stage */
	if ( Stage != ClientFrameStage_t::FRAME_NET_UPDATE_END )
		return;

	/* sort algorithm */
	auto SortByTime = [ ] ( const RageShot_t& First, const RageShot_t& Second ) -> bool
	{
		const int nFirstDelta = g_Networking->GetServerTick( ) - First.m_nTickCount;
		const int nSecondDelta = g_Networking->GetServerTick( ) - Second.m_nTickCount;
		return nFirstDelta < nSecondDelta;
	};

	/* sort shots */
	std::sort( m_RageShots.begin( ), m_RageShots.end( ), SortByTime );

	/* iterate all shots */
	for ( int nShot = 0; nShot < m_RageShots.size( ); nShot++ )
	{
		/* check size */
		if ( nShot >= m_RageShots.size( ) )
			break;

		/* get shot from the array */
		RageShot_t* Shot = &m_RageShots[ nShot ];

		/* target must be valid and alive */
		C_BasePlayer* pPlayer = Shot->m_Target.m_Player;
		if ( !pPlayer )
		{
			/* invalid shot */
			m_RageShots.erase( m_RageShots.begin( ) + nShot );
			nShot--;

			/* skip this shot */
			continue;
		}

		/* bullet tracers */
		if ( Shot->m_ImpactList.size( ) > 0 )
			if ( Shot->m_bRegisteredShot )
				SDK::Interfaces::DebugOverlay->AddLineOverlayAlpha( Shot->m_ImpactList.back( ), Shot->m_vecShootPosition, 255, 255, 255, 255, false, 4.0f );

		/* check only alive targets */
		if ( !pPlayer->IsAlive( ) )
		{
			/* invalid shot */
			m_RageShots.erase( m_RageShots.begin( ) + nShot );
			nShot--;

			/* skip this shot */
			continue;
		}

		/* Detect of unregistered shots */
		if ( g_Networking->GetServerTick( ) > Shot->m_nEstimate )
		{
			/* if shot has not been registered for 16 + latency ticks, then it won't be networked at all */
			if ( !Shot->m_bRegisteredShot )
			{
				/* check for logs enabled */
				if ( g_SettingsManager->B[ _S( "misc.logs.item_5" ) ] )
				{
					/* draw log msg */
					if ( g_Globals->m_LocalPlayer->IsAlive( ) )
						g_LogManager->PushLog( _S( "Missed shot due to unregistered shot" ), EventIcons::Cross );
					else
						g_LogManager->PushLog( _S( "Missed shot due to death" ), EventIcons::Cross );
				}
			}
		}
		else if ( Shot->m_bRegisteredShot )
		{
			/* string from status */
			auto GetStringFromStatus = [ ] ( ShotStatus Status ) -> std::string
			{
				std::string szMessage;
				switch ( Status )
				{
					case ShotStatus::Occlusion: szMessage += _S( "occlusion" ); break;
					case ShotStatus::Spread: szMessage += _S( "spread" ); break;
					case ShotStatus::Resolver: szMessage += _S( "animation desync" ); break;
					case ShotStatus::LagCompensation: szMessage += _S( "prediction error" ); break;
					case ShotStatus::NoStatus: szMessage += _S( "unknown reason" ); break;
				}

				return szMessage;
			};
			if ( !Shot->m_bDidHurtPlayer )
			{
				/* shot status */
				ShotStatus Status = ShotStatus::Spread;

				/* calc impacts yaw */
				float flClientYaw = Math::NormalizeAngle( Math::CalcAngle( Shot->m_vecShootPosition, Shot->m_Target.m_Point.m_vecPoint ).yaw );
				float flServerYaw = Math::NormalizeAngle( Math::CalcAngle( Shot->m_vecShootPosition, Shot->m_ImpactList.back( ) ).yaw );

				/* SPREAD CHECKS */
				if ( fabs( flClientYaw - flServerYaw ) < 1.0f )
				{
					/* OCCLUSION CHECKS */
					if ( Shot->m_vecShootPosition.DistTo( Shot->m_ImpactList.back( ) ) < Shot->m_vecShootPosition.DistTo( Shot->m_Target.m_Point.m_vecPoint ) )
					{
						/* check for penetration */
						if ( Shot->m_Target.m_Point.m_vecPoint.DistTo( Shot->m_vecShootPosition ) > 5.0f )
							Status = ShotStatus::Occlusion;
					}

					/* RESOLVER CHECKS */
					if ( Status != ShotStatus::Occlusion )
					{
						/* calculate vectors */
						const Vector vecStart = Shot->m_vecShootPosition;
						const Vector vecEnd = Shot->m_ImpactList.back( );

						/* Get hitbox */
						HitboxData_t Hitbox;
						if ( g_RageBot->CollidePoint( vecStart, vecEnd, Hitbox.m_pHitbox, Shot->m_Target.m_Record.m_Matricies[ 0 ].data( ) ) )
							Status = ShotStatus::Resolver;
					}
				}

				/* print reason */
				if ( g_SettingsManager->B[ _S( "misc.logs.item_5" ) ] )
				{
					/* build msg */
					std::string szMessage = _S( "Missed shot due to " ) + GetStringFromStatus( Status );

					/* draw log */
					g_LogManager->PushLog( szMessage, EventIcons::Cross );
				}
			}
		}
		else
			continue;

		/* erase shot */
		m_RageShots.erase( m_RageShots.begin( ) + nShot );
		nShot--;
	}
}