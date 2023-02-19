#include "ESP.hpp"
#include "../../Fonts/FontManager.hpp"
#include "../../Game/Dormant/DormantSystem.hpp"
#include "../../../Interface/Renderer/Renderer.hpp"
#include "../../Game/RageBot/LagCompensation.hpp"

void C_PlayerESP::Instance( )
{
	if ( !g_Globals->m_LocalPlayer || !SDK::Interfaces::EngineClient->IsInGame( ) || !SDK::Interfaces::EngineClient->IsConnected( ) )
		return;

	int iScreenSizeX = 0;
	int iScreenSizeY = 0;

	SDK::Interfaces::EngineClient->GetScreenSize( iScreenSizeX, iScreenSizeY );
	for ( auto iPlayerIndex = 1; iPlayerIndex < SDK::Interfaces::GlobalVars->m_iMaxClients; iPlayerIndex++ )
	{
		C_BasePlayer* pPlayer = C_BasePlayer::GetPlayerByIndex( iPlayerIndex );
		if ( !pPlayer || !pPlayer->IsAlive( ) || !pPlayer->IsPlayer( ) )
		{
			if ( pPlayer && pPlayer->IsPlayer( ) )
				g_DormantSystem->ResetPlayer( pPlayer );

			continue;
		}

		// get player data
		ESPPlayerData_t* m_Data = &m_PlayerData[ iPlayerIndex ];
		if ( !m_Data )
			continue;

		if ( pPlayer->IsDormant( ) )
		{
			m_Data->m_flPlayerPercentage = Math::Interpolate( m_Data->m_flPlayerPercentage, 0.15f, SDK::Interfaces::GlobalVars->m_flFrameTime * 3.0f );
			if ( m_Data->m_flPlayerPercentage <= 0.15f )
				m_Data->m_flPlayerPercentage = 0.15f;

			if ( !ForceDormant( pPlayer ) )
				continue;
		}
		else
		{
			m_Data->m_flPlayerPercentage = Math::Interpolate( m_Data->m_flPlayerPercentage, 1.01f, SDK::Interfaces::GlobalVars->m_flFrameTime * 3.0f );
			g_DormantSystem->ResetPlayer( pPlayer );
		}

		m_Data->m_flPlayerPercentage = fmin( m_Data->m_flPlayerPercentage, 1.00f );
		if ( pPlayer == g_Globals->m_LocalPlayer )
		{
			if ( !SDK::Interfaces::Input->m_bCameraInThirdPerson )
				continue;
		}
		else
		{
			Vector vecToScreen = Vector( 0, 0, 0 );
			if ( SDK::Interfaces::DebugOverlay->ScreenPosition( pPlayer->GetAbsOrigin( ), vecToScreen ) )
			{
				if ( vecToScreen.x <= 0 || vecToScreen.x > iScreenSizeX || vecToScreen.y <= 0 || vecToScreen.y > iScreenSizeY )
				{
					OutOfView( pPlayer );
					continue;
				}
			}
		}

		// setup bbox data
		m_Data->m_aBBox = Math::GetBBox( pPlayer );
		m_Data->m_iWidth = abs( m_Data->m_aBBox.right - m_Data->m_aBBox.left );
		m_Data->m_iHeight = abs( m_Data->m_aBBox.top - m_Data->m_aBBox.bottom );

		PlayerESPSettings_t aOptions;
		if ( pPlayer->m_iTeamNum( ) == g_Globals->m_LocalPlayer->m_iTeamNum( ) && pPlayer )
			aOptions = g_SettingsManager->GetEspSettings( "team" );
		else if ( pPlayer != g_Globals->m_LocalPlayer )
			aOptions = g_SettingsManager->GetEspSettings( "enemy" );

		if ( pPlayer == g_Globals->m_LocalPlayer )
			continue;

		m_Data->m_Options = aOptions;
		if ( !aOptions.m_bEnable )
			continue;

		int nItemID = 0;
		if ( pPlayer->m_hActiveWeapon( ) )
			nItemID = pPlayer->m_hActiveWeapon( )->m_nItemID( );

		int nWeaponType = 0;
		if ( pPlayer->m_hActiveWeapon( ) )
			if ( pPlayer->m_hActiveWeapon( )->GetWeaponData( ) )
				nWeaponType = pPlayer->m_hActiveWeapon( )->GetWeaponData( )->m_iWeaponType;

		int nHealth = pPlayer->m_iHealth( );
		if ( pPlayer->IsDormant( ) )
		{
			int nLastWeaponID = g_DormantSystem->GetLastActiveWeapon( iPlayerIndex );
			if ( pPlayer->m_hActiveWeapon( ) )
			{
				if ( nLastWeaponID > 0 )
					pPlayer->m_hActiveWeapon( )->m_nItemID( ) = nLastWeaponID;

				int nLastWeaponType = g_DormantSystem->GetLastActiveWeaponType( iPlayerIndex );
				if ( pPlayer->m_hActiveWeapon( )->GetWeaponData( ) )
					if ( nLastWeaponType > -1 )
						pPlayer->m_hActiveWeapon( )->GetWeaponData( )->m_iWeaponType = nLastWeaponType;
			}

			int nDormantHealth = g_DormantSystem->GetDormantHealth( pPlayer->EntIndex( ) );
			if ( nDormantHealth != nHealth )
				pPlayer->m_iHealth( ) = nDormantHealth;
		}

		for ( auto a = 0; a < POOL_COND; a++ )
		{
			m_Data->m_iDownOffset = 0;
			m_Data->m_iUpOffset = 0;

			m_Data->m_iLeftDownOffset = 0;
			m_Data->m_iLeftOffset = 0;

			m_Data->m_iRightDownOffset = 0;
			m_Data->m_iRightOffset = 0;

			for ( auto b = 0; b < Pool[ a ].size( ); b++ )
			{
				if ( Pool[ a ][ b ].Type == 0 ) {
					std::string Text;
					int Size =  14;
					ImColor Color;

					if ( Pool[ a ][ b ].ItemName == "Weapon" ) {
						C_BaseCombatWeapon* pWeapon = pPlayer->m_hActiveWeapon( ).Get( );
						if ( !pWeapon )
							continue;

						Text = pWeapon->GetLocalizedName( );
						if ( Text.length( ) <= 0 )
							continue;

						if ( m_Data->m_Options.m_bAmmoAmount ) {
							if ( pWeapon->m_iClip1( ) >= 0 ) {
								std::string ammoAmount = std::to_string( pWeapon->m_iClip1( ) );
								Text = Text + " [ " + ammoAmount + " ]";
							}
						}

						Color = m_Data->m_Options.m_cWeaponColor;
						Size = m_Data->m_Options.m_aFontSizes[ 0 ];
					}
					else if ( Pool[ a ][ b ].ItemName == "Username" ) {
						PlayerInfo_t info;
						SDK::Interfaces::EngineClient->GetPlayerInfo( pPlayer->EntIndex( ), &info );

						Text = info.m_strName;
						if ( Text.length( ) > 32 )
						{
							Text.erase( 32, Text.length( ) - 32 );
							Text.append( _S( "..." ) );
						}
						Size = m_Data->m_Options.m_aFontSizes[ 1 ];
						Color = m_Data->m_Options.m_cNameColor;
					}
					else if ( Pool[ a ][ b ].ItemName == "Scope" ) {
						if ( pPlayer->m_bIsScoped( ) ) {
							Size = m_Data->m_Options.m_aFontSizes[ 3 ];
							Text = "Scoped";
							Color = m_Data->m_Options.m_cScopeColor;
						}
						else
							continue;
					}
					else if ( Pool[ a ][ b ].ItemName == "Defusing" ) {
						if ( pPlayer->m_bIsDefusing( ) ) {
							Text = "Defusing";
							Size = m_Data->m_Options.m_aFontSizes[ 4 ];
							Color = m_Data->m_Options.m_cDefusingColor;
						}
						else
							continue;
					}
					else if ( Pool[ a ][ b ].ItemName == "Flashed" ) {
						if ( pPlayer->m_flFlashDuration( ) > 0.f ) {
							Text = "Flashed";
							Size = m_Data->m_Options.m_aFontSizes[ 5 ];
							Color = m_Data->m_Options.m_cFlashedColor;
						}
						else
							continue;
					}
					else if ( Pool[ a ][ b ].ItemName == "Taser" ) {
						if ( g_Globals->m_LocalPlayer->m_hActiveWeapon( ) && g_Globals->m_LocalPlayer->m_hActiveWeapon( )->m_nItemID( ) == WEAPON_TASER )
						{
							float flDistance = g_Globals->m_LocalPlayer->m_vecOrigin( ).DistTo( pPlayer->m_vecOrigin( ) );
							if ( flDistance < g_Globals->m_LocalPlayer->m_hActiveWeapon( )->GetWeaponData( )->m_flRange )
							{
								Text = "Taser";
								Size = m_Data->m_Options.m_aFontSizes[ 6 ];
								Color = m_Data->m_Options.m_cTaserColor;
							}
							else
								continue;
						}
						else
							continue;
					}
					else if ( Pool[ a ][ b ].ItemName == "LC" ) {
						auto aRecords = g_LagCompensation->GetPlayerRecords( pPlayer );
						if ( aRecords.size( ) <= 1 || aRecords.back( ).m_bHasBrokenLC || aRecords.back( ).m_bIsInvalid ) 
						{
							Text = "LC";
							Size = m_Data->m_Options.m_aFontSizes[ 7 ];
							Color = m_Data->m_Options.m_cLCColor;
						}
						else continue;
					}
					else if ( Pool[ a ][ b ].ItemName == "FD" ) {
						if ( pPlayer->m_flDuckAmount( ) == 0.8f ) {
							Text = "FD";
							Color = m_Data->m_Options.m_cDuckingcColor;
							Size = m_Data->m_Options.m_aFontSizes[ 8 ];
						}
						else continue;
					}

					AddText( Text, Size, ( DraggableItemCondiction ) a, Color, m_Data );
				}	
				if ( Pool[ a ][ b ].Type == 1 ) {
					ImColor Main;
					ImColor Inner;
					ImColor Outer;

					if ( Pool[ a ][ b ].ItemName == "Health" ) {
						auto iHealthValue = std::clamp( pPlayer->m_iHealth( ), 0, 100 );
						auto percentage = iHealthValue / 100.f;
						if ( m_Data->m_flPrevHealth > iHealthValue )
							m_Data->m_flPrevHealth -= SPEED_FREQ * SDK::Interfaces::GlobalVars->m_flFrameTime;
						else
							m_Data->m_flPrevHealth = iHealthValue;

						Main = m_Data->m_Options.m_cHealthColorBottom;
						Outer = m_Data->m_Options.m_cHealthColorOutlineUpper;
						Inner = m_Data->m_Options.m_cHealthColorOutlineInner;

						AddBar( ( DraggableItemCondiction ) a, m_Data->m_flPrevHealth, 100, Main, Inner, Outer, m_Data );
					}

					if ( Pool[ a ][ b ].ItemName == "Armor" ) {
						auto iArmorValue = std::clamp( pPlayer->m_ArmourValue( ), 0, 100 );
						if ( m_Data->m_flPrevArmor > iArmorValue )
							m_Data->m_flPrevArmor -= SPEED_FREQ * SDK::Interfaces::GlobalVars->m_flFrameTime;
						else
							m_Data->m_flPrevArmor = iArmorValue;

						Main = m_Data->m_Options.m_cArmorColor;
						Outer = m_Data->m_Options.m_cArmorColorOutlineUpper;
						Inner = m_Data->m_Options.m_cArmorColorOutlineInner;

						AddBar( ( DraggableItemCondiction ) a, m_Data->m_flPrevArmor, 100, Main, Inner, Outer, m_Data );
					}

					if ( Pool[ a ][ b ].ItemName == "Ammo" ) {
						C_BaseCombatWeapon* pWeapon = pPlayer->m_hActiveWeapon( ).Get( );
						if ( !pWeapon )
							continue;

						auto WeaponData = pWeapon->GetWeaponData( );

						if ( !WeaponData )
							continue;

						auto iArmorValue = std::clamp( pWeapon->m_iClip1( ), 0, WeaponData->m_iMaxClip1 );
						
						if ( pPlayer->GetSequenceActivity( pPlayer->GetAnimationLayers( )[ ANIMATION_LAYER_WEAPON_ACTION ].m_nSequence ) != ACT_CSGO_RELOAD ) {
							if ( m_Data->m_flPrevAmmo > iArmorValue )
								m_Data->m_flPrevAmmo -= SPEED_FREQ * SDK::Interfaces::GlobalVars->m_flFrameTime;
							else
								m_Data->m_flPrevAmmo = iArmorValue;
						}

						if ( pPlayer->GetSequenceActivity( pPlayer->GetAnimationLayers( )[ ANIMATION_LAYER_WEAPON_ACTION ].m_nSequence ) == ACT_CSGO_RELOAD )
							m_Data->m_flPrevAmmo = WeaponData->m_iMaxClip1 * pPlayer->GetAnimationLayers( )[ ANIMATION_LAYER_WEAPON_ACTION ].m_flCycle;

						Main = m_Data->m_Options.m_cAmmoColor;
						Outer = m_Data->m_Options.m_cAmmoColorOutlineUpper;
						Inner = m_Data->m_Options.m_cAmmoColorOutlineInner;

						AddBar( ( DraggableItemCondiction ) a, m_Data->m_flPrevAmmo, WeaponData->m_iMaxClip1, Main, Inner, Outer, m_Data );
					}
				}
				if ( Pool[ a ][ b ].Type == 2 )
					RenderBox( iPlayerIndex, m_Data);
			}
		}

		if ( pPlayer->IsDormant( ) )
		{
			if ( pPlayer->m_hActiveWeapon( ) )
			{
				pPlayer->m_hActiveWeapon( )->m_nItemID( ) = nItemID;
				if ( pPlayer->m_hActiveWeapon( )->GetWeaponData( ) )
					pPlayer->m_hActiveWeapon( )->GetWeaponData( )->m_iWeaponType = nWeaponType;
			}

			pPlayer->m_iHealth( ) = nHealth;
		}
	}
}

void C_PlayerESP::OutOfView( C_BasePlayer* pPlayer )
{
	if ( g_SettingsManager->B[ _S( "misc.out_of_view" ) ] == false )
		return;

	if ( pPlayer->m_iTeamNum( ) == g_Globals->m_LocalPlayer->m_iTeamNum( ) )
		return;

	if ( !g_Globals->m_LocalPlayer->IsAlive( ) )
		return;

	float width = 7.f;
	QAngle viewangles;
	SDK::Interfaces::EngineClient->GetViewAngles( &viewangles );

	auto angle = viewangles.yaw - Math::CalcAngle( g_Globals->m_LocalPlayer->m_vecOrigin( ) + g_Globals->m_LocalPlayer->m_vecViewOffset( ), pPlayer->GetAbsOrigin( ) ).yaw - 90;

	int iScreenSizeX, iScreenSizeY;
	SDK::Interfaces::EngineClient->GetScreenSize( iScreenSizeX, iScreenSizeY );

	g_Renderer->AddArc( ImVec2( iScreenSizeX / 2, iScreenSizeY / 2 ), 256, angle - width, angle + width, g_SettingsManager->IC[ _S( "misc.out_of_view" ) ], 4.f );
	g_Renderer->AddArc( ImVec2( iScreenSizeX / 2, iScreenSizeY / 2 ), 250, angle - width, angle + width, g_SettingsManager->IC[ _S( "misc.out_of_view" ) ], 1.5f );
}

ImVec2 GetPosition( DraggableItemCondiction pos, ESPPlayerData_t* m_Data ) {
	if( pos == RIGHT_COND )
		return ImVec2( m_Data->m_aBBox.right + 2 + m_Data->m_iRightOffset, m_Data->m_aBBox.top + m_Data->m_iRightDownOffset );
	if ( pos == LEFT_COND )
		return ImVec2( m_Data->m_aBBox.left - 2 - m_Data->m_iLeftOffset, m_Data->m_aBBox.top + m_Data->m_iLeftDownOffset );
	if ( pos == BOT_COND )
		return ImVec2( m_Data->m_aBBox.left + ( ( m_Data->m_aBBox.right - m_Data->m_aBBox.left ) * 0.5f ), m_Data->m_aBBox.bottom + 2 + m_Data->m_iDownOffset );
	if ( pos == TOP_COND )
		return ImVec2( m_Data->m_aBBox.left + ( ( m_Data->m_aBBox.right - m_Data->m_aBBox.left ) * 0.5f ), m_Data->m_aBBox.top - m_Data->m_iUpOffset );
}

ImVec2 GetPositionOffsetless( DraggableItemCondiction pos, ESPPlayerData_t* m_Data ) {
	if ( pos == RIGHT_COND )
		return ImVec2( m_Data->m_aBBox.right + 3 + m_Data->m_iRightOffset, m_Data->m_aBBox.top );
	if ( pos == LEFT_COND )
		return ImVec2( m_Data->m_aBBox.left - 3 - m_Data->m_iLeftOffset, m_Data->m_aBBox.top );
	if ( pos == BOT_COND )
		return ImVec2( m_Data->m_aBBox.left, m_Data->m_aBBox.bottom + 2  );
	if ( pos == TOP_COND )
		return ImVec2( m_Data->m_aBBox.left, m_Data->m_aBBox.top - 4 );
}

void C_PlayerESP::AddBar( DraggableItemCondiction pos, float& percentage, float max, ImColor color, ImColor color1, ImColor color2, ESPPlayerData_t* m_Data ) {

	ImVec2 Position = GetPositionOffsetless( pos, m_Data );
	int XOffset, X2Offset;
	int YOffset, Y2Offset;

	auto widthSides = abs( m_Data->m_aBBox.bottom - m_Data->m_aBBox.top ) - ( ( ( abs( m_Data->m_aBBox.bottom - m_Data->m_aBBox.top ) * percentage ) / max ) );
	auto widthUpDown = abs( m_Data->m_aBBox.left - m_Data->m_aBBox.right ) - ( ( ( abs( m_Data->m_aBBox.left - m_Data->m_aBBox.right ) * percentage ) / max ) );

	if ( pos == RIGHT_COND ) {
		XOffset = 1 + m_Data->m_iRightOffset;
		X2Offset = 3 + m_Data->m_iRightOffset;
		YOffset = 1;
		Y2Offset = -1 + m_Data->m_iHeight;
	}

	if ( pos == LEFT_COND ) {
		XOffset = -1 - m_Data->m_iLeftOffset;
		X2Offset = -3 - m_Data->m_iLeftOffset;
		YOffset = 1;
		Y2Offset = -1 + m_Data->m_iHeight;
	}

	if ( pos == BOT_COND || pos == TOP_COND ) {
		XOffset = 1;
		X2Offset = m_Data->m_iWidth - 1;
		YOffset = m_Data->m_iDownOffset;
		Y2Offset = 2 + m_Data->m_iDownOffset;
	}

	g_Renderer->AddRect
	(
		{ ( float ) Position.x + XOffset - 1 + ( pos == LEFT_COND ? 1 : 0), ( float ) Position.y + YOffset - 1 },
		{ ( float ) Position.x + X2Offset + 1 - ( pos == LEFT_COND ? 1 : 0), ( float ) Position.y + Y2Offset + 1 },
		ImColor( color2.Value.x, color2.Value.y, color2.Value.z, fmin( color2.Value.w, m_Data->m_flPlayerPercentage ) )
	);

	g_Renderer->AddRectFilled
	(
		{ ( float ) Position.x + XOffset, ( float ) Position.y + YOffset },
		{ ( float ) Position.x + X2Offset, ( float ) Position.y + Y2Offset },
		ImColor( color1.Value.x, color1.Value.y, color1.Value.z, fmin( color1.Value.w, m_Data->m_flPlayerPercentage ) )
	);

	g_Renderer->AddRectFilled
	(
		{ ( float ) Position.x + XOffset, ( float ) Position.y + YOffset + ( pos == LEFT_COND || pos == RIGHT_COND ? widthSides : 0 ) },
		{ ( float ) Position.x + X2Offset - ( pos == BOT_COND || pos == TOP_COND ? widthUpDown : 0), ( float ) Position.y + Y2Offset },
		ImColor(color.Value.x,color.Value.y,color.Value.z,fmin( color.Value.w, m_Data->m_flPlayerPercentage ))
	);

	if ( pos == RIGHT_COND )
		m_Data->m_iRightOffset = m_Data->m_iRightOffset + 3;
	if ( pos == LEFT_COND )
		m_Data->m_iLeftOffset = m_Data->m_iLeftOffset + 3;
	if ( pos == BOT_COND )
		m_Data->m_iDownOffset = m_Data->m_iDownOffset + 5;
	if ( pos == TOP_COND )
		m_Data->m_iUpOffset = m_Data->m_iUpOffset + 5;
}

void C_PlayerESP::AddText( std::string text, int size, DraggableItemCondiction pos, ImColor color, ESPPlayerData_t* m_Data ) {
	ImFont* m_Font = g_FontManager->GetFontFromHash( FnvHash( "Player ESP" ) );
	if ( !m_Font )
		return;

	ImVec2 ImTextSize = m_Font->CalcTextSizeA( size, FLT_MAX, 0.0f, text.c_str( ) );
	ImVec2 Position = GetPosition( pos, m_Data );

	if ( pos == LEFT_COND )
		Position = Position - ImVec2( ImTextSize.x + 4, 0 );
	if ( pos == RIGHT_COND )
		Position = Position + ImVec2( 4, 0 );
	if ( pos == BOT_COND )
		Position = Position - ImVec2( ImTextSize.x / 2, 0 );
	if ( pos == TOP_COND )
		Position = Position - ImVec2( ImTextSize.x / 2, ImTextSize.y );

	g_Renderer->AddText(
		m_Font, size,
		Position, text,
		ImColor( color.Value.x, color.Value.y, color.Value.z, fmin( color.Value.w, m_Data->m_flPlayerPercentage ))
	);

	if ( pos == RIGHT_COND )
		m_Data->m_iRightDownOffset = m_Data->m_iRightDownOffset + ImTextSize.y;
	if ( pos == LEFT_COND )
		m_Data->m_iLeftDownOffset = m_Data->m_iLeftDownOffset + ImTextSize.y;
	if ( pos == BOT_COND )
		m_Data->m_iDownOffset = m_Data->m_iDownOffset + ImTextSize.y;
	if ( pos == TOP_COND )
		m_Data->m_iUpOffset = m_Data->m_iUpOffset + ImTextSize.y;
}

bool C_PlayerESP::ForceDormant( C_BasePlayer* pPlayer )
{
	float flTimeInDormant = g_DormantSystem->GetTimeInDormant( pPlayer->EntIndex( ) );
	float flSinceLastData = g_DormantSystem->GetTimeSinceLastData( pPlayer->EntIndex( ) );

	if ( flTimeInDormant < flSinceLastData )
	{
		if ( flTimeInDormant < 8.0f )
			pPlayer->SetAbsOrigin( g_DormantSystem->GetLastNetworkOrigin( pPlayer->EntIndex( ) ) );
		else if ( flSinceLastData < 8.0f )
			pPlayer->SetAbsOrigin( g_DormantSystem->GetLastDormantOrigin( pPlayer->EntIndex( ) ) );
	}
	else
	{
		if ( flSinceLastData < 8.0f )
			pPlayer->SetAbsOrigin( g_DormantSystem->GetLastDormantOrigin( pPlayer->EntIndex( ) ) );
		else if ( flTimeInDormant < 8.0f )
			pPlayer->SetAbsOrigin( g_DormantSystem->GetLastNetworkOrigin( pPlayer->EntIndex( ) ) );
	}

	return flTimeInDormant < 8.0f || flSinceLastData < 8.0f;
}

void C_PlayerESP::RenderBox( int iIndexPlayer, ESPPlayerData_t* m_Data )
{
	g_Renderer->AddRect(
		{ ( float ) m_Data->m_aBBox.left, ( float ) m_Data->m_aBBox.top },
		{ ( float ) m_Data->m_aBBox.right, ( float ) m_Data->m_aBBox.bottom },
		m_Data->m_Options.m_cBoxColorOutlineUpper );

	g_Renderer->AddRect(
		{ ( float ) m_Data->m_aBBox.left + 1, ( float ) m_Data->m_aBBox.top + 1 },
		{ ( float ) m_Data->m_aBBox.right - 1, ( float ) m_Data->m_aBBox.bottom - 1 },
		m_Data->m_Options.m_cBoxColor );

	g_Renderer->AddRect(
		{ ( float ) m_Data->m_aBBox.left + 2, ( float ) m_Data->m_aBBox.top + 2},
		{ ( float ) m_Data->m_aBBox.right - 2, ( float ) m_Data->m_aBBox.bottom - 2 },
		m_Data->m_Options.m_cBoxColorOutlineUpper );
}

std::string C_PlayerESP::GetWeaponIcon( C_BasePlayer* pPlayer, C_BaseCombatWeapon* pWeapon )
{
	if ( !pWeapon )
		return "";

	C_CSWeaponData* pWeaponData = pWeapon->GetWeaponData( );
	if ( !pWeaponData )
		return "";

	if ( pWeapon->IsKnife( ) )
	{
		if ( pPlayer->m_iTeamNum( ) == 1 )
			return _S( ")]" );
		else
			return _S( "[" );
	}

	std::string strResult = "";
	switch ( pWeapon->m_nItemID( ) )
	{
		case WEAPON_SCAR20: strResult = _S( "Y" ); break;
		case WEAPON_G3SG1: strResult = _S( "X" ); break;
		case WEAPON_AWP: strResult = _S( "Z" ); break;
		case WEAPON_SSG08: strResult = _S( "a" ); break;
		case WEAPON_DEAGLE: strResult = _S( "A" ); break;
		case WEAPON_REVOLVER: strResult = _S( "J" ); break;
		case WEAPON_HKP2000: strResult = _S( "E" ); break; break;
		case WEAPON_GLOCK: strResult = _S( "D" ); break;
		case WEAPON_USP_SILENCER: strResult = _S( "G" ); break;
		case WEAPON_ELITE: strResult = _S( "B" ); break;
		case WEAPON_C4: strResult = _S( "o" ); break;
		case WEAPON_P250: strResult = _S( "F" ); break;
		case WEAPON_AUG: strResult = _S( "U" ); break;
		case WEAPON_FIVESEVEN: strResult = _S( "C" ); break;
		case WEAPON_AK47: strResult = _S( "W" ); break;
		case WEAPON_GALILAR: strResult = _S( "Q" ); break;
		case WEAPON_CZ75A: strResult = _S( "I" ); break;
		case WEAPON_FAMAS: strResult = _S( "R" ); break;
		case WEAPON_TEC9: strResult = _S( "H" ); break;
		case WEAPON_BIZON: strResult = _S( "M" ); break;
		case WEAPON_M249: strResult = _S( "g" ); break;
		case WEAPON_NEGEV: strResult = _S( "f" ); break;
		case WEAPON_NOVA: strResult = _S( "e" ); break;
		case WEAPON_MAG7: strResult = _S( "d" ); break;
		case WEAPON_TASER: strResult = _S( "h" ); break;
		case WEAPON_HEGRENADE: strResult = _S( "j" ); break;
		case WEAPON_SMOKEGRENADE: strResult = _S( "k" ); break;
		case WEAPON_INCGRENADE: strResult = _S( "n" ); break;
		case WEAPON_MOLOTOV: strResult = _S( "l" ); break;
		case WEAPON_SAWEDOFF: strResult = _S( "c" ); break;
		case WEAPON_DECOY: strResult = _S( "m" ); break;
		case WEAPON_FLASHBANG: strResult = _S( "i" ); break;
		case WEAPON_M4A1: strResult = _S( "S" ); break;
		case WEAPON_M4A1_SILENCER: strResult = _S( "T" ); break;
		case WEAPON_FRAG_GRENADE: strResult = _S( "k" ); break;
		case WEAPON_MAC10: strResult = _S( "K" ); break;
		case WEAPON_UMP45: strResult = _S( "L" ); break;
		case WEAPON_MP7: strResult = _S( "N" ); break;
		case WEAPON_P90: strResult = _S( "P" ); break;
		case WEAPON_MP9: strResult = _S( "N" ); break;
		case WEAPON_SG553: strResult = _S( "V" ); break;
		case WEAPON_XM1014: strResult = _S( "e" ); break;
		case WEAPON_TAGRENADE: strResult = _S( "i" ); break;
	}

	return strResult;
}