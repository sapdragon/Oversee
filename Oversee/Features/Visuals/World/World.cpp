#include "World.hpp"
#include "../Grenades/Warning.hpp"
#include "../../Game/Networking/Networking.hpp"
#include "../../Game/RageBot/Autowall.hpp"
#include "../../../SDK/Utils/Utils.hpp"
#include "../../../Interface/Renderer/Renderer.hpp"
#include "../../Misc/Movement/Movement.hpp"

void C_World::OnFrameStageNotify( ClientFrameStage_t Frame )
{
	/* check localplayer and check frame */
	if ( !g_Globals->m_LocalPlayer || Frame != ClientFrameStage_t::FRAME_RENDER_START )
		return;

	/* Destroy smoke and hand shaking */
	g_World->DestroySmokeParticles( );
	g_World->DestroySmokeRenderer( );
	g_World->DestroyHandShaking( );
	g_World->FogChanger( );

	/* Modify shadows */
	if ( m_Statements.m_bLastShadowState != g_SettingsManager->B[ _S( "misc.removals.item_5" ) ] )
	{
		/* set parameters */
		SDK::EngineData::m_ConvarList[ CheatConvarList::FootContactShadows ]->SetValue( m_Statements.m_bLastShadowState );
		SDK::EngineData::m_ConvarList[ CheatConvarList::CsmStaticPropShadows ]->SetValue( m_Statements.m_bLastShadowState );
		SDK::EngineData::m_ConvarList[ CheatConvarList::CsmWorldShadows ]->SetValue( m_Statements.m_bLastShadowState );
		SDK::EngineData::m_ConvarList[ CheatConvarList::CsmShadows ]->SetValue( m_Statements.m_bLastShadowState );
		SDK::EngineData::m_ConvarList[ CheatConvarList::CsmViewmodelShadows ]->SetValue( m_Statements.m_bLastShadowState );
		SDK::EngineData::m_ConvarList[ CheatConvarList::CsmSpriteShadows ]->SetValue( m_Statements.m_bLastShadowState );
		SDK::EngineData::m_ConvarList[ CheatConvarList::CsmRopeShadows ]->SetValue( m_Statements.m_bLastShadowState );

		/* save the latest statement */
		m_Statements.m_bLastShadowState = g_SettingsManager->B[ _S( "misc.removals.item_5" ) ];
	}

	/* Remove blood */
	if ( g_SettingsManager->B[ _S( "misc.removals.item_10" ) ] != m_Statements.m_bLastBloodState )
	{
		/* clear decals on value switch */
		SDK::Interfaces::EngineClient->ExecuteClientCmd( _S( "r_cleardecals" ) );
		SDK::EngineData::m_ConvarList[ CheatConvarList::ViolenceHBlood ]->SetValue( m_Statements.m_bLastBloodState );
		SDK::EngineData::m_ConvarList[ CheatConvarList::ViolenceABlood ]->SetValue( m_Statements.m_bLastBloodState );

		/* save last state */
		m_Statements.m_bLastBloodState = g_SettingsManager->B[ _S( "misc.removals.item_10" ) ];
	}

	if ( g_SettingsManager->B[ _S( "misc.client_impacts" ) ] )
	{
		if ( !m_Impacts.m_nLastDrawn )
			m_Impacts.m_flTurnTime = SDK::Interfaces::GlobalVars->m_flRealTime;

		/* no fucking other way to do this */
		auto& aClientImpactList = *( CUtlVector < ClientImpact_t >* )( ( uintptr_t ) ( g_Globals->m_LocalPlayer ) + 0x11C50 );
		for ( auto Impact = aClientImpactList.Count( ); Impact > m_Impacts.m_nLastDrawn; --Impact )
		{
			SDK::Interfaces::DebugOverlay->AddBoxOverlay
			(
				aClientImpactList[ Impact - 1 ].m_vecPosition,
				Vector( -2.0f, -2.0f, -2.0f ),
				Vector( 2.0f, 2.0f, 2.0f ),
				QAngle( 0.0f, 0.0f, 0.0f ),
				g_SettingsManager->IC[ _S( "misc.client_impacts" ) ].Value.x * 255.0f,
				g_SettingsManager->IC[ _S( "misc.client_impacts" ) ].Value.y * 255.0f,
				g_SettingsManager->IC[ _S( "misc.client_impacts" ) ].Value.z * 255.0f,
				g_SettingsManager->IC[ _S( "misc.client_impacts" ) ].Value.w * 255.0f,
				g_SettingsManager->F[ _S( "misc.client_impacts_duration" ) ]
			);
		}

		if ( aClientImpactList.Count( ) != m_Impacts.m_nLastDrawn )
			m_Impacts.m_nLastDrawn = aClientImpactList.Count( );
	}
	else
		m_Impacts.m_nLastDrawn = 0;

	/* set postprocessing state */
	**( bool** ) ( ( DWORD ) ( SDK::EngineData::m_AddressList[ CheatAddressList::PostProcessing ] ) ) = g_SettingsManager->B[ _S( "misc.removals.item_1" ) ];

	/* run skybox changer */
	g_World->PerformSkyBoxTexture( );
}
void C_World::RunFilters( )
{
	/* unlock convars */
	if ( g_SettingsManager->B[ _S( "misc.hidden_convars" ) ] )
	{
		/* do it once */
		if ( !m_Statements.m_bUnlockedConVars )
		{
			/* iterate convars */
			auto pIterator = **reinterpret_cast< ConCommandBase*** >( reinterpret_cast< DWORD >( SDK::Interfaces::CVar ) + 0x34 );
			for ( auto c = pIterator->m_pNext; c != nullptr; c = c->m_pNext )
			{
				c->m_nFlags &= ~FCVAR_DEVELOPMENTONLY;
				c->m_nFlags &= ~FCVAR_HIDDEN;
			}

			/* set state */
			m_Statements.m_bUnlockedConVars = true;
		}
	}

	/* filter console */
	if ( m_Statements.m_bFilteredConsole != g_SettingsManager->B[ _S( "misc.filter_console" ) ] )
	{
		/* set variables */
		SDK::Interfaces::CVar->FindVar( _S( "con_filter_text" ) )->SetValue( _S( "oversee.one the best cheat ever" ) );
		SDK::Interfaces::CVar->FindVar( _S( "con_filter_enable" ) )->SetValue( g_SettingsManager->B[ _S( "misc.filter_console" ) ] );

		/* clean console */
		SDK::Interfaces::EngineClient->ExecuteClientCmd( _S( "clear" ) );

		/* set state */
		m_Statements.m_bFilteredConsole = g_SettingsManager->B[ _S( "misc.filter_console" ) ];
	}
}
void C_World::DestroyHandShaking( )
{
	/* set sway amount once */
	if ( m_Statements.m_bLastShakeState != g_SettingsManager->B[ _S( "misc.removals.item_7" ) ] )
	{
		/* set sway amount */
		SDK::EngineData::m_ConvarList[ CheatConvarList::CsmWpnSwayAmount ]->SetValue( !m_Statements.m_bLastShakeState ? 0.0f : 1.6f );

		/* save the latest state */
		m_Statements.m_bLastShakeState = g_SettingsManager->B[ _S( "misc.removals.item_7" ) ];
	}
}
void C_World::PenetrationCrosshair( )
{
	/* check localplayer and settings */
	if ( !g_Globals->m_LocalPlayer || !g_Globals->m_LocalPlayer->IsAlive( ) || !g_SettingsManager->B[ _S( "misc.penetration_crosshire" ) ] )
		return;

	/* check weapon */
	C_BaseCombatWeapon* pWeapon = g_Globals->m_LocalPlayer->m_hActiveWeapon( ).Get( );
	if ( !pWeapon || !pWeapon->IsGun( ) )
		return;

	/* check weapon data */
	C_CSWeaponData* pWeaponData = pWeapon->GetWeaponData( );
	if ( !pWeaponData )
		return;

	/* get local angles */
	QAngle angLocalAngles;
	SDK::Interfaces::EngineClient->GetViewAngles( &angLocalAngles );

	/* get direction */
	Vector vecDirection;
	Math::AngleVectors( angLocalAngles, vecDirection );

	/* get shootposition */
	const Vector vecShootPosition = g_Globals->m_LocalPlayer->GetAbsOrigin( ) + g_Globals->m_LocalPlayer->m_vecViewOffset( );

	/* Select color from pointer penetration statement */
	ImColor aColor = ImColor( 255, 0, 0, 255 );
	if ( g_AutoWall->IsPenetrablePoint( vecShootPosition, vecShootPosition + ( vecDirection * pWeaponData->m_flRange ) ) )
		aColor = ImColor( 0, 128, 0, 255 );

	/* Draw colored filled rect at the center of the screen */
	return g_Renderer->AddRectFilled( ImVec2( ( g_Globals->m_ScreenData.m_nScreenSizeX / 2 ) - 2, ( g_Globals->m_ScreenData.m_nScreenSizeY / 2 ) - 3 ), ImVec2( ( g_Globals->m_ScreenData.m_nScreenSizeX / 2 ) + 2, ( g_Globals->m_ScreenData.m_nScreenSizeY / 2 ) + 2 ), aColor );
}
void C_World::PreserveKillfeed( )
{
	/* define death notice */
	static PDWORD pHudDeathNotice = NULL;

	/* check localplayer and reset pointer */
	if ( !g_Globals->m_LocalPlayer || !g_Globals->m_LocalPlayer->IsAlive( ) )
	{
		pHudDeathNotice = NULL;
		return;
	}

	/* check death notice pointer and find it */
	if ( !pHudDeathNotice )
	{
		pHudDeathNotice = g_Utils->FindHudElement( _S( "CCSGO_HudDeathNotice" ) );
		return;
	}

	/* set expire time for death notices */
	PFLOAT pNoticeExpireTime = ( PFLOAT ) ( ( DWORD ) ( pHudDeathNotice ) +0x50 );
	if ( pNoticeExpireTime )
		*pNoticeExpireTime = g_SettingsManager->B[ _S( "misc.preserve_killfeed" ) ] ? 200.0f : 1.5f;

	/* clean death notices */
	if ( g_Globals->m_RoundInfo.m_bShouldClearDeathNotices )
		( ( void( __thiscall* )( DWORD ) )( SDK::EngineData::m_AddressList[ CheatAddressList::ClearDeathList ] ) )( ( DWORD ) ( pHudDeathNotice ) -0x14 );

	/* We did clean death notices */
	g_Globals->m_RoundInfo.m_bShouldClearDeathNotices = false;
}
void C_World::DestroySmokeRenderer( )
{
	/* remove smoke count */
	if ( g_SettingsManager->B[ _S( "misc.removals.item_8" ) ] )
		*( int* ) ( *reinterpret_cast < uint32_t** >( ( uint32_t ) ( SDK::EngineData::m_AddressList[ CheatAddressList::SmokeCount ] ) ) ) = 0;
}
void C_World::DestroySmokeParticles( )
{
	/* remove smoke particles */
	if ( m_Statements.m_bLastSmokeState == g_SettingsManager->B[ _S( "misc.removals.item_8" ) ] )
		return;

	/* smoke particles array */
	std::array < std::string, 4 > m_SmokeMaterials
		=
	{
		_S( "particle/vistasmokev1/vistasmokev1_emods" ),
		_S( "particle/vistasmokev1/vistasmokev1_emods_impactdust" ),
		_S( "particle/vistasmokev1/vistasmokev1_fire" ),
		_S( "particle/vistasmokev1/vistasmokev1_smokegrenade" )
	};

	/* iterate materials */
	for ( auto& szMaterialName : m_SmokeMaterials )
	{
		IMaterial* Material = SDK::Interfaces::MaterialSystem->FindMaterial( szMaterialName.c_str( ), _S( TEXTURE_GROUP_OTHER ) );
		if ( !Material )
			continue;

		Material->SetMaterialVarFlag( MaterialVarFlags_t::MATERIAL_VAR_NO_DRAW, !m_Statements.m_bLastSmokeState );
	}

	/* save the last smoke state */
	m_Statements.m_bLastSmokeState = g_SettingsManager->B[ _S( "misc.removals.item_8" ) ];
}

void C_World::RunVisualFeatures( )
{
	int nEntityCount = SDK::Interfaces::EntityList->GetHighestEntityIndex( );
	for ( int nEntIndex = 0; nEntIndex < nEntityCount; nEntIndex++ )
	{
		/* get entity */
		C_BaseEntity* m_Entity = static_cast < C_BaseEntity* > ( SDK::Interfaces::EntityList->GetClientEntity( nEntIndex ) );

		/* check entity */
		if ( !m_Entity || m_Entity->IsDormant( ) )
			continue;

		if ( !m_Entity || m_Entity->IsPlayer( ) )
			continue;

		if ( m_Entity == ( C_BaseEntity* ) 0xE || m_Entity == ( C_BaseEntity* ) 0xB )
			continue;

		const model_t* pModel = m_Entity->GetModel( );

		if ( !pModel )
			continue;

		const char* pModelName = SDK::Interfaces::ModelInfo->GetModelName( pModel );

		if ( !pModelName )
			continue;

		if ( ( strstr( pModelName, _S( "_dropped.mdl" ) ) && !strstr( pModelName, _S( "models/weapons/w_ied" ) ) ) || strstr( pModelName, _S( "smokegrenade_thrown.mdl" ) ) )
		{
			auto pos = Math::GetBBox( m_Entity );

			if ( g_SettingsManager->B[ _S( "world.show_world_grenades" ) ] )
			{
				if ( strstr( pModelName, _S( "fraggrenade" ) ) )
					g_Renderer->AddText( g_Menu->m_sFonts.WeapIcons, 20, ImVec2( pos.left, pos.top ), "j", ImColor( g_SettingsManager->IC[ _S( "world.grenades_hegrenade" ) ] ), true );
				if ( strstr( pModelName, _S( "molotov" ) ) )
					g_Renderer->AddText( g_Menu->m_sFonts.WeapIcons, 20, ImVec2( pos.left, pos.top ), "l", ImColor( g_SettingsManager->IC[ _S( "world.grenades_molotov" ) ] ), true );
				if ( strstr( pModelName, _S( "incendiarygrenade" ) ) )
					g_Renderer->AddText( g_Menu->m_sFonts.WeapIcons, 20, ImVec2( pos.left, pos.top ), "n", ImColor( g_SettingsManager->IC[ _S( "world.grenades_inc" ) ] ), true );
				if ( strstr( pModelName, _S( "smokegrenade_thrown.mdl" ) ) )
					g_Renderer->AddText( g_Menu->m_sFonts.WeapIcons, 20, ImVec2( pos.left, pos.top ), "k", ImColor( g_SettingsManager->IC[ _S( "world.grenades_smoke" ) ] ), true );
				if ( strstr( pModelName, _S( "decoy" ) ) )
					g_Renderer->AddText( g_Menu->m_sFonts.WeapIcons, 20, ImVec2( pos.left, pos.top ), "m", ImColor( g_SettingsManager->IC[ _S( "world.grenades_decoy" ) ] ), true );
				if ( strstr( pModelName, _S( "flashbang" ) ) )
					g_Renderer->AddText( g_Menu->m_sFonts.WeapIcons, 20, ImVec2( pos.left, pos.top ), "i", ImColor( g_SettingsManager->IC[ _S( "world.grenades_flashbang" ) ] ), true );
			}
		}
	}
}
void C_World::PerformSkyBoxTexture( )
{
	std::string strSkyBox = SDK::Interfaces::CVar->FindVar( _S( "sv_skyname" ) )->GetString( );
	switch ( g_SettingsManager->I[ _S( "misc.skybox" ) ] )
	{
		case 1:
		{
			strSkyBox = _S( "cs_tibet" );
		}
		break;

		case 2:
		{
			strSkyBox = _S( "cs_baggage_skybox_" );
		}
		break;

		case 3:
		{
			strSkyBox = _S( "italy" );
		}
		break;

		case 4:
		{
			strSkyBox = _S( "jungle" );
		}
		break;

		case 5:
		{
			strSkyBox = _S( "office" );
		}
		break;

		case 6:
		{
			strSkyBox = _S( "sky_cs15_daylight01_hdr" );
		}
		break;

		case 7:
		{
			strSkyBox = _S( "sky_cs15_daylight02_hdr" );
		}
		break;

		case 8:
		{
			strSkyBox = _S( "vertigoblue_hdr" );
		}
		break;

		case 9:
		{
			strSkyBox = _S( "vertigo" );
		}
		break;

		case 10:
		{
			strSkyBox = _S( "sky_day02_05_hdr" );
		}
		break;

		case 11:
		{
			strSkyBox = _S( "nukeblank" );
		}
		break;

		case 12:
		{
			strSkyBox = _S( "sky_venice" );
		}
		break;

		case 13:
		{
			strSkyBox = _S( "sky_cs15_daylight03_hdr" );
		}
		break;

		case 14:
		{
			strSkyBox = _S( "sky_cs15_daylight04_hdr" );
		}
		break;

		case 15:
		{
			strSkyBox = _S( "sky_csgo_cloudy01" );
		}
		break;

		case 16:
		{
			strSkyBox = _S( "sky_csgo_night02" );
		}
		break;

		case 17:
		{
			strSkyBox = _S( "sky_csgo_night02b" );
		}
		break;

		case 18:
		{
			strSkyBox = _S( "sky_csgo_night_flat" );
		}
		break;

		case 19:
		{
			strSkyBox = _S( "sky_dust" );
		}
		break;

		case 20:
		{
			strSkyBox = _S( "vietnam" );
		}
		break;
	}

	return g_Utils->SetSkybox( strSkyBox.c_str( ) );
}
void C_World::RunClanTagChanger( )
{

}
void C_World::FogChanger( )
{
	if ( !SDK::Interfaces::EngineClient->IsInGame( ) || !SDK::Interfaces::EngineClient->IsConnected( ) )
		return;

	int nMaxEnts = SDK::Interfaces::EntityList->GetHighestEntityIndex( );
	for ( int i = 1; i <= nMaxEnts; i++ )
	{
		C_FogController* pFogController = static_cast < C_FogController* > ( SDK::Interfaces::EntityList->GetClientEntity( i ) );
		if ( !pFogController )
			continue;

		if ( pFogController->GetClientClass( )->m_ClassID != ClassId_CFogController )
			continue;

		pFogController->m_bEnabled( ) = g_SettingsManager->B[ _S( "misc.fog.enable" ) ];
		pFogController->m_flStartDistance( ) = g_SettingsManager->I[ _S( "misc.fog.min_distance" ) ];
		pFogController->m_flEndDistance( ) = g_SettingsManager->I[ _S( "misc.fog.max_distance" ) ];
		pFogController->m_flMaxDensity( ) = 1.f;
		pFogController->m_cMainColor( ) = g_SettingsManager->C[ _S( "misc.fog.color" ) ];
	}
}
void C_World::GrenadeWarning( )
{
	if ( !g_Globals->m_LocalPlayer )
		return;

	static int nOldTick = g_Networking->GetServerTick( );
	if ( nOldTick != g_Networking->GetServerTick( ) )
		g_GrenadePrediction->get_list( ).clear( );
	nOldTick = g_Networking->GetServerTick( );

	int nMaxEnts = SDK::Interfaces::EntityList->GetHighestEntityIndex( );
	for ( int i = 1; i < nMaxEnts; i++ )
	{
		C_BaseEntity* pBaseEntity = static_cast< C_BaseEntity* >( SDK::Interfaces::EntityList->GetClientEntity( i ) );
		if ( !pBaseEntity || pBaseEntity->IsDormant( ) )
			continue;

		int m_ClassID = pBaseEntity->GetClientClass( )->m_ClassID;
		if ( m_ClassID != 114 && m_ClassID != 9 && m_ClassID != 175 )
			continue;

		if ( m_ClassID == ClassId_CBaseCSGrenadeProjectile )
		{
			if ( !strstr( SDK::Interfaces::ModelInfo->GetStudiomodel( pBaseEntity->GetModel( ) )->szName, _S( "fraggrenade" ) ) )
				continue;
		}

		if ( pBaseEntity->m_nExplodeEffectTickBegin( ) )
		{
			g_GrenadePrediction->get_list( ).erase( pBaseEntity->GetRefEHandle( ).ToLong( ) );
			continue;
		}

		int m_GrenadeType = -1;
		if ( m_ClassID == 157 )
			m_GrenadeType = WEAPON_SMOKEGRENADE;
		else if ( m_ClassID == 9 )
			m_GrenadeType = WEAPON_HEGRENADE;
		else if ( m_ClassID == 114 )
			m_GrenadeType == WEAPON_MOLOTOV;

		if ( g_GrenadePrediction->get_list( ).find( pBaseEntity->GetRefEHandle( ).ToLong( ) ) == g_GrenadePrediction->get_list( ).end( ) ) {
			g_GrenadePrediction->get_list( )[ pBaseEntity->GetRefEHandle( ).ToLong( ) ]
				=
				C_GrenadePrediction::data_t
				(
					static_cast< C_BaseCombatWeapon* >( pBaseEntity )->m_hThrower( ).Get( ),
					m_GrenadeType,
					pBaseEntity->m_vecOrigin( ),
					static_cast< C_BasePlayer* >( pBaseEntity )->m_vecVelocity( ),
					pBaseEntity->m_flCreationTime( ),
					TIME_TO_TICKS( pBaseEntity->m_flSimulationTime( ) - pBaseEntity->m_flCreationTime( ) )
				);
		}

		if ( g_GrenadePrediction->get_list( ).at( pBaseEntity->GetRefEHandle( ).ToLong( ) ).draw( ) )
			continue;

		g_GrenadePrediction->get_list( ).erase( pBaseEntity->GetRefEHandle( ).ToLong( ) );
	}

	g_GrenadePrediction->get_local_data( ).draw( );
}