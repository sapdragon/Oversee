#include "Events.hpp"
#include "../Fakelag/Fakelag.hpp"
#include "../../Game/RageBot/RageBot.hpp"
#include "../../Game/RageBot/AutoStop.hpp"
#include "../../Game/RageBot/Shots.hpp"
#include "../../Game/RageBot/LocalAnimations.hpp"
#include "../../Game/RageBot/LagCompensation.hpp"
#include "../../Game/Revolver/Revolver.hpp"
#include "../../Game/Networking/Networking.hpp"
#include "../../Game/Prediction/EnginePrediction.hpp"
#include "../../Game/Dormant/DormantSystem.hpp"
#include "../../Misc//Buy Bot/BuyBot.hpp"
#include "../../Visuals/Log Manager/LogManager.hpp"
#include "../../Visuals/World/World.hpp"
#include "../../../Data/HitSound.hpp"
#include "../../Inventory Changer/InventoryChanger.hpp"

void C_CheatEvents::HandleGameEvent( IGameEvent* m_GameEvent )
{
	/* check localplayer */
	if ( !g_Globals->m_LocalPlayer )
		return;

	/* functions to get name of hitgroup and get hitbox name from hitgroup */
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

	/* Inventory changer */
	g_InventoryChanger->OnEvent( m_GameEvent );

	/* handle events */
	uint32_t uEventHashed = FNV1A( m_GameEvent->GetName( ) );
	switch ( uEventHashed )
	{
		/* reset data on spawn */
		case FNV1A( "player_death" ):
		{
			/* get player */
			C_BasePlayer* Player = C_BasePlayer::GetPlayerByIndex( SDK::Interfaces::EngineClient->GetPlayerForUserID( m_GameEvent->GetInt( _S( "userid" ) ) ) );
			if ( !Player )
				return;

			/* clean LC data of this player */
			g_LagCompensation->CleanPlayer( Player );
		};
		break;

		/* reset data on spawn */
		case FNV1A( "round_start" ):
		{
			/* Run BuyBot */
			g_BuyBot->OnRoundStart( );

			/* clear death notices */
			g_Globals->m_RoundInfo.m_bShouldClearDeathNotices = true;

			/* reset data */
			g_DormantSystem->ResetData( );
			g_LocalAnimations->ResetData( );
			g_AutoStop->ResetData( );
			g_RageBot->ResetData( );

			/* clean out rage shots */
			g_ShotSystem->m_RageShots.clear( );
		};
		break;

		/* draw traces and run part of the ragebot */
		case FNV1A( "bullet_impact" ):
		{
			/* Check localplayer */
			if ( SDK::Interfaces::EngineClient->GetPlayerForUserID( m_GameEvent->GetInt( _S( "userid" ) ) ) != g_Globals->m_LocalPlayer->EntIndex( ) )
				return;

			/* Handle bullet impacts */
			g_ShotSystem->OnBulletImpact( m_GameEvent );

			/* draw bullet impacts */
			Vector vecImpactPosition = Vector( m_GameEvent->GetInt( _S( "x" ) ), m_GameEvent->GetInt( _S( "y" ) ), m_GameEvent->GetInt( _S( "z" ) ) );
			if ( g_SettingsManager->B[ _S( "misc.server_impacts" ) ] )
				SDK::Interfaces::DebugOverlay->AddBoxOverlay
				(
					vecImpactPosition,
					Vector( -2.0f, -2.0f, -2.0f ),
					Vector( 2.0f, 2.0f, 2.0f ),
					QAngle( 0.0f, 0.0f, 0.0f ),
					g_SettingsManager->IC[ _S( "misc.server_impacts" ) ].Value.x * 255.0f,
					g_SettingsManager->IC[ _S( "misc.server_impacts" ) ].Value.y * 255.0f,
					g_SettingsManager->IC[ _S( "misc.server_impacts" ) ].Value.z * 255.0f,
					g_SettingsManager->IC[ _S( "misc.server_impacts" ) ].Value.w * 255.0f,
					g_SettingsManager->F[ _S( "misc.server_impacts_duration" ) ]
				);
		};
		break;

		/* draw/print logs and run part of the ragebot */
		case FNV1A( "player_hurt" ):
		{
			/* Check localplayer */
			if ( SDK::Interfaces::EngineClient->GetPlayerForUserID( m_GameEvent->GetInt( _S( "attacker" ) ) ) == g_Globals->m_LocalPlayer->EntIndex( ) )
			{
				/* Check localplayer #2 */
				if ( SDK::Interfaces::EngineClient->GetPlayerForUserID( m_GameEvent->GetInt( _S( "userid" ) ) ) != g_Globals->m_LocalPlayer->EntIndex( ) )
				{
					/* Handle player hurt */
					g_ShotSystem->OnPlayerHurt( m_GameEvent );

					/* Hitsound */
					if ( g_SettingsManager->I[ _S( "misc.hit_sound" ) ] )
					{
						/* ModifyVolume function */
						auto ModifyVolume = [ & ] ( BYTE* bytes, int nSize ) {
							int offset = 0;
							for ( int i = 0; i < nSize / 2; i++ ) {
								if ( bytes[ i ] == 'd' && bytes[ i + 1 ] == 'a'
									&& bytes[ i + 2 ] == 't' && bytes[ i + 3 ] == 'a' )
								{
									offset = i;
									break;
								}
							}

							if ( !offset )
								return;

							BYTE* pDataOffset = ( bytes + offset );
							DWORD dwNumSampleBytes = *( DWORD* ) ( pDataOffset + 4 );
							DWORD dwNumSamples = dwNumSampleBytes / 2;

							SHORT* pSample = ( SHORT* ) ( pDataOffset + 8 );
							for ( DWORD dwIndex = 0; dwIndex < dwNumSamples; dwIndex++ )
							{
								SHORT shSample = *pSample;
								shSample = ( SHORT ) ( shSample * ( ( float ) ( g_SettingsManager->I[ _S( "misc.hit_sound_volume" ) ] ) / 100.f ) );
								*pSample = shSample;
								pSample++;
								if ( ( ( BYTE* ) pSample ) >= ( bytes + nSize - 1 ) )
									break;
							}
						};

						/* Sound data */
						BYTE* aRawHitsound = nullptr;
						DWORD dwSoundSize = 0;

						/* Select array */
						switch ( g_SettingsManager->I[ _S( "misc.hit_sound" ) ] )
						{
							case 1: /* arena */
							{
								aRawHitsound = rawArena;
								dwSoundSize = sizeof( rawArena );
							}
							break;

							case 2: /* bubble */
							{
								aRawHitsound = rawBubble;
								dwSoundSize = sizeof( rawBubble );
							}
							break;

							case 3: /* bell */
							{
								aRawHitsound = rawBell;
								dwSoundSize = sizeof( rawBell );
							}
							break;
						}

						static int LastVolume = 100;
						if ( LastVolume != g_SettingsManager->I[ _S( "misc.hit_sound_volume" ) ] )
						{
							ModifyVolume( aRawHitsound, dwSoundSize );
							LastVolume = g_SettingsManager->I[ _S( "misc.hit_sound_volume" ) ];
						}

						/* Play hitsound */
						if ( aRawHitsound )
						{
							/* PlaySound */
							PlaySoundA( ( LPCSTR ) ( aRawHitsound ), NULL, SND_ASYNC | SND_MEMORY );
						}
					}

					/* Event logs */
					if ( g_SettingsManager->B[ _S( "misc.logs.item_2" ) ] )
					{
						/* get player info */
						PlayerInfo_t Info;
						SDK::Interfaces::EngineClient->GetPlayerInfo( SDK::Interfaces::EngineClient->GetPlayerForUserID( m_GameEvent->GetInt( _S( "userid" ) ) ), &Info );

						/* build msg */
						std::string strHurtMessage = _S( "Hurt " );
						strHurtMessage += Info.m_strName;
						strHurtMessage += _S( " in the " );
						strHurtMessage += GetHitboxNameFromHitgroup( m_GameEvent->GetInt( _S( "hitgroup" ) ) );
						strHurtMessage += _S( " for " );
						strHurtMessage += std::to_string( m_GameEvent->GetInt( "dmg_health" ) );
						strHurtMessage += _S( "hp (" );
						strHurtMessage += std::to_string( m_GameEvent->GetInt( "health" ) );
						strHurtMessage += _S( "hp remain)" );

						/* push event log */
						g_LogManager->PushLog( strHurtMessage, EventIcons::CheckMark );
					}
				}
				else
				{
					/* Event logs */
					if ( g_SettingsManager->B[ _S( "misc.logs.item_3" ) ] )
					{
						/* get player info */
						PlayerInfo_t Info;
						SDK::Interfaces::EngineClient->GetPlayerInfo( SDK::Interfaces::EngineClient->GetPlayerForUserID( m_GameEvent->GetInt( _S( "attacker" ) ) ), &Info );

						/* build msg */
						std::string strHurtMessage = _S( "Harmed for " );
						strHurtMessage += std::to_string( m_GameEvent->GetInt( _S( "dmg_health" ) ) ) + _S( " damage from " ) + Info.m_strName + _S( " in the " );
						strHurtMessage += GetHitboxNameFromHitgroup( m_GameEvent->GetInt( _S( "hitgroup" ) ) );

						/* push log */
						g_LogManager->PushLog( strHurtMessage, EventIcons::Cross );
					}
				}
			}
			else
			{

			}
		};
		break;

		/* bomb plant log */
		case FNV1A( "bomb_beginplant" ):
		{
			/* get player info */
			PlayerInfo_t PlayerInfo;
			SDK::Interfaces::EngineClient->GetPlayerInfo( SDK::Interfaces::EngineClient->GetPlayerForUserID( m_GameEvent->GetInt( _S( "userid" ) ) ), &PlayerInfo );

			/* Event logs */
			if ( g_SettingsManager->B[ _S( "misc.logs.item_4" ) ] )
				g_LogManager->PushLog( std::string( PlayerInfo.m_strName ) + _S( " started planting the bomb" ), EventIcons::Bomb );
		}
		break;

		/* bomb defuse log */
		case FNV1A( "bomb_begindefuse" ):
		{
			/* get player info */
			PlayerInfo_t PlayerInfo;
			SDK::Interfaces::EngineClient->GetPlayerInfo( SDK::Interfaces::EngineClient->GetPlayerForUserID( m_GameEvent->GetInt( _S( "userid" ) ) ), &PlayerInfo );

			/* event logs */
			if ( g_SettingsManager->B[ _S( "misc.logs.item_4" ) ] )
				g_LogManager->PushLog( std::string( PlayerInfo.m_strName ) + _S( " started defusing the bomb" ), EventIcons::Bomb );
		}
		break;

		case FNV1A( "item_purchase" ):
		{
			C_BasePlayer* pPlayer = C_BasePlayer::GetPlayerByIndex( SDK::Interfaces::EngineClient->GetPlayerForUserID( m_GameEvent->GetInt( _S( "userid" ) ) ) );
			if ( !g_SettingsManager->B[ _S( "misc.logs.item_1" ) ] || !pPlayer || pPlayer->m_iTeamNum( ) == g_Globals->m_LocalPlayer->m_iTeamNum( ) )
				return;

			PlayerInfo_t Info;
			SDK::Interfaces::EngineClient->GetPlayerInfo( SDK::Interfaces::EngineClient->GetPlayerForUserID( m_GameEvent->GetInt( _S( "userid" ) ) ), &Info );

			std::string strWeaponName = m_GameEvent->GetString( _S( "weapon" ) );
			if ( strstr( strWeaponName.c_str( ), _S( "unknown" ) ) || strstr( strWeaponName.c_str( ), _S( "assaultsuit" ) ) || strstr( strWeaponName.c_str( ), _S( "kevlar" ) ) )
				return;

			if ( strstr( strWeaponName.c_str( ), _S( "weapon_" ) ) )
				strWeaponName.erase( strWeaponName.begin( ), strWeaponName.begin( ) + 7 );
			else if ( strstr( strWeaponName.c_str( ), _S( "item_" ) ) )
				strWeaponName.erase( strWeaponName.begin( ), strWeaponName.begin( ) + 4 );

			if ( strstr( strWeaponName.c_str( ), _S( "_defuser" ) ) )
				strWeaponName = _S( "defuser" );

			std::string strMessage = Info.m_strName;
			strMessage += _S( " bought " );
			strMessage += strWeaponName;
			strMessage += _S( "." );

			g_LogManager->PushLog( strMessage, EventIcons::Basket );
		}
		break;
	}
}
void C_CheatEvents::ResetCheatData( )
{
	/* reset cheat data */
	g_LagCompensation->ResetData( );
	g_DormantSystem->ResetData( );
	g_LocalAnimations->ResetData( );
	g_AutoStop->ResetData( );
	g_RageBot->ResetData( );
	g_EnginePrediction->ResetData( );
	g_Revolver->ResetData( );
	g_World->ResetData( );
	g_Networking->ResetData( );

	/* clean out rage shots */
	g_ShotSystem->m_RageShots.clear( );
}