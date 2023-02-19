#include "HUD.hpp"
#include "../../../Interface/GUI/Menu.hpp"

void C_CustomHUD::Instance( )
{
	ConVar* m_HideHud = SDK::EngineData::m_ConvarList[ CheatConvarList::HideHud ];
	if ( g_SettingsManager->B[ _S( "ui.custom_hud" ) ] )
	{
		if ( m_HideHud->GetInt( ) != 0 )
			m_HideHud->SetInt( 0 );
	}
	else
	{
		if ( m_HideHud->GetInt( ) == 0 )
			m_HideHud->SetInt( 1 );
	}

	if ( !g_SettingsManager->B[ _S( "ui.custom_hud" ) ] )
		return;

	if ( !SDK::Interfaces::EngineClient->IsConnected( ) || !SDK::Interfaces::EngineClient->IsInGame( ) )
		return;

	if ( g_Globals->m_LocalPlayer == nullptr )
		return;

	if ( g_Globals->m_LocalPlayer->IsAlive( ) )
		UpdateHUDStruct( g_Globals->m_LocalPlayer );

	RenderHUD( );
}

void C_CustomHUD::EventHandler( IGameEvent* m_pEvent )
{
	if ( strstr( m_pEvent->GetName( ), _S( "player_death" ) ) ) {
		m_tKillList.push_back( KillMessage_t {
			GetTickCount64( ) + g_SettingsManager->I[ _S( "misc.hud.kill_list_time" ) ],
			m_pEvent->GetInt( _S( "userid" ) ),
			m_pEvent->GetInt( _S( "attacker" ) ),
			m_pEvent->GetInt( _S( "assister" ) ),
			m_pEvent->GetString( _S( "weapon" ) ),
			m_pEvent->GetBool( _S( "headshot" ) ),
			m_pEvent->GetBool( _S( "noscope" ) ),
			m_pEvent->GetBool( _S( "attackerblind" ) ),
			m_pEvent->GetBool( _S( "assistedflash" ) ),
			} );
	}
	else if ( strstr( m_pEvent->GetName( ), _S( "round_start" ) ) ) {
		if ( g_SettingsManager->B[ _S( "misc.hud.clear_kill_feed_on_round" ) ] )
			m_tKillList.clear( );
	}
}

void C_CustomHUD::UpdateHUDStruct( C_BasePlayer* m_Player )
{
	if ( m_Player == nullptr )
		m_tHUDData = HUDPlayerData_t { -1, -1 };

	m_tHUDData.m_Armor = m_Player->m_ArmourValue( );
	m_tHUDData.m_Health = m_Player->m_iHealth( );
}

void C_CustomHUD::RenderHUD( )
{
	auto m_WindowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;

	auto m_WindowSize = ImGui::GetIO( ).DisplaySize;
	auto m_WindowPos = ImVec2 { 0, 0 };

	ImGui::Begin( "Head-Up Display", nullptr, m_WindowFlags );
	{
		ImGui::SetWindowPos( m_WindowPos );
		ImGui::SetWindowSize( m_WindowSize );

		auto draw = ImGui::GetWindowDrawList( );
		auto pos = ImGui::GetWindowPos( );

		HealthArmorHandler( draw, pos, m_WindowSize );
		KillListHandler( draw, pos, m_WindowSize );
		CrosshireHandler( draw, pos, m_WindowSize );
		WeaponsHandler( draw, pos, m_WindowSize );
	}
	ImGui::End( );
}

void C_CustomHUD::HealthArmorHandler( ImDrawList* draw, ImVec2 pos, ImVec2 size )
{
	draw->AddRectFilledMultiColor( pos + ImVec2( 10, size.y - 40 ), pos + ImVec2( 120, size.y - 10 ),
		ImColor( 45, 45, 45, 180 ), ImColor( 45, 45, 45, 0 ),
		ImColor( 45, 45, 45, 0 ), ImColor( 45, 45, 45, 180 ) );

	draw->AddRectFilledMultiColor( pos + ImVec2( 10, size.y - 40 - 35 ), pos + ImVec2( 120, size.y - 10 - 35 ),
		ImColor( 45, 45, 45, 180 ), ImColor( 45, 45, 45, 0 ),
		ImColor( 45, 45, 45, 0 ), ImColor( 45, 45, 45, 180 ) );

	draw->AddText( g_Menu->m_sFonts.MenuIcons, 14, pos + ImVec2( 20, size.y - 32 ), ImColor( 240, 240, 240 ), "l" );
	draw->AddText( g_Menu->m_sFonts.Default24, 20, pos + ImVec2( 40, size.y - 36 ), ImColor( 255, 255, 255 ), std::to_string( m_tHUDData.m_Health ).c_str( ) );

	draw->AddText( g_Menu->m_sFonts.MenuIcons, 14, pos + ImVec2( 20, size.y - 32 - 35 ), ImColor( 240, 240, 240 ), "k" );
	draw->AddText( g_Menu->m_sFonts.Default24, 20, pos + ImVec2( 40, size.y - 36 - 35 ), ImColor( 255, 255, 255 ), std::to_string( m_tHUDData.m_Armor ).c_str( ) );
}

void C_CustomHUD::CrosshireHandler( ImDrawList* draw, ImVec2 pos, ImVec2 size )
{
	if ( g_SettingsManager->B[ _S( "misc.hud.crosshire_dot" ) ] ) {
		draw->AddRectFilled(
			pos + ( size / 2 ) - ImVec2( g_SettingsManager->I[ _S( "misc.hud.crosshire_dot_size" ) ], g_SettingsManager->I[ _S( "misc.hud.crosshire_dot_size" ) ] ),
			pos + ( size / 2 ) + ImVec2( g_SettingsManager->I[ _S( "misc.hud.crosshire_dot_size" ) ], g_SettingsManager->I[ _S( "misc.hud.crosshire_dot_size" ) ] ),
			ImColor(
				g_SettingsManager->IC[ _S( "misc.hud.crosshire_color" ) ].Value.x,
				g_SettingsManager->IC[ _S( "misc.hud.crosshire_color" ) ].Value.y,
				g_SettingsManager->IC[ _S( "misc.hud.crosshire_color" ) ].Value.z,
				g_SettingsManager->IC[ _S( "misc.hud.crosshire_color" ) ].Value.w
			), 16
		);
	}

	// Left
	draw->AddLine(
		pos + ( size / 2 ) - ImVec2( g_SettingsManager->I[ _S( "misc.hud.crosshire_width_size" ) ], 0 ),
		pos + ( size / 2 ) - ImVec2( g_SettingsManager->I[ _S( "misc.hud.crosshire_gap_size" ) ], 0 ),
		ImColor(
			g_SettingsManager->IC[ _S( "misc.hud.crosshire_color" ) ].Value.x,
			g_SettingsManager->IC[ _S( "misc.hud.crosshire_color" ) ].Value.y,
			g_SettingsManager->IC[ _S( "misc.hud.crosshire_color" ) ].Value.z,
			g_SettingsManager->IC[ _S( "misc.hud.crosshire_color" ) ].Value.w
		),
		g_SettingsManager->I[ _S( "misc.hud.crosshire_line_size" ) ]
	);

	// Right
	draw->AddLine(
		pos + ( size / 2 ) + ImVec2( g_SettingsManager->I[ _S( "misc.hud.crosshire_width_size" ) ], 0 ),
		pos + ( size / 2 ) + ImVec2( g_SettingsManager->I[ _S( "misc.hud.crosshire_gap_size" ) ], 0 ),
		ImColor(
			g_SettingsManager->IC[ _S( "misc.hud.crosshire_color" ) ].Value.x,
			g_SettingsManager->IC[ _S( "misc.hud.crosshire_color" ) ].Value.y,
			g_SettingsManager->IC[ _S( "misc.hud.crosshire_color" ) ].Value.z,
			g_SettingsManager->IC[ _S( "misc.hud.crosshire_color" ) ].Value.w
		),
		g_SettingsManager->I[ _S( "misc.hud.crosshire_line_size" ) ]
	);

	// Top
	draw->AddLine(
		pos + ( size / 2 ) - ImVec2( 0, g_SettingsManager->I[ _S( "misc.hud.crosshire_width_size" ) ] ),
		pos + ( size / 2 ) - ImVec2( 0, g_SettingsManager->I[ _S( "misc.hud.crosshire_gap_size" ) ] ),
		ImColor(
			g_SettingsManager->IC[ _S( "misc.hud.crosshire_color" ) ].Value.x,
			g_SettingsManager->IC[ _S( "misc.hud.crosshire_color" ) ].Value.y,
			g_SettingsManager->IC[ _S( "misc.hud.crosshire_color" ) ].Value.z,
			g_SettingsManager->IC[ _S( "misc.hud.crosshire_color" ) ].Value.w
		),
		g_SettingsManager->I[ _S( "misc.hud.crosshire_line_size" ) ]
	);

	// Bot
	draw->AddLine(
		pos + ( size / 2 ) + ImVec2( 0, g_SettingsManager->I[ _S( "misc.hud.crosshire_gap_size" ) ] ),
		pos + ( size / 2 ) + ImVec2( 0, g_SettingsManager->I[ _S( "misc.hud.crosshire_width_size" ) ] ),
		ImColor(
			g_SettingsManager->IC[ _S( "misc.hud.crosshire_color" ) ].Value.x,
			g_SettingsManager->IC[ _S( "misc.hud.crosshire_color" ) ].Value.y,
			g_SettingsManager->IC[ _S( "misc.hud.crosshire_color" ) ].Value.z,
			g_SettingsManager->IC[ _S( "misc.hud.crosshire_color" ) ].Value.w
		),
		g_SettingsManager->I[ _S( "misc.hud.crosshire_line_size" ) ]
	);
}

void C_CustomHUD::KillListHandler( ImDrawList* draw, ImVec2 pos, ImVec2 size )
{
	auto DrawNotify = [ this, draw, pos, size ] ( KillMessage_t& message ) {

		C_BasePlayer* pAttacker = C_BasePlayer::GetPlayerByIndex( SDK::Interfaces::EngineClient->GetPlayerForUserID( message.m_UserIDAttacker ) );
		C_BasePlayer* pDied = C_BasePlayer::GetPlayerByIndex( SDK::Interfaces::EngineClient->GetPlayerForUserID( message.m_UserIDDied ) );

		if ( !pAttacker->IsPlayer( ) || !pDied->IsPlayer( ) || !pDied || !pAttacker )
			return;

		float alpha = 1.f;
		if ( message.m_EndTime - 1000 <= GetTickCount64( ) )
		{
			const auto delta = std::clamp<float>( message.m_EndTime - GetTickCount64( ), 0.f, 1000 );
			alpha = std::clamp<float>( delta / 1000, 0.f, 1.f );
		}

		PlayerInfo_t UserDied, UserAttacker;
		SDK::Interfaces::EngineClient->GetPlayerInfo( pAttacker->EntIndex( ), &UserAttacker );
		SDK::Interfaces::EngineClient->GetPlayerInfo( pDied->EntIndex( ), &UserDied );

		auto DiedSize = ImGui::CalcTextSize( UserDied.m_strName );
		auto AttackerSize = ImGui::CalcTextSize( UserAttacker.m_strName );

		float WeaponSize;
		std::string WeaponName;

		std::tie( WeaponName, WeaponSize ) = GetWeaponIcon( message.m_WeaponName );

		auto GetKillWidht = [ DiedSize, AttackerSize, WeaponSize, message ] ( float AddictSpacing = 20, bool Died = true, bool Attacked = true, bool Weapon = true ) {
			return ( Died ? DiedSize.x : 0 ) +
				( Attacked ? AttackerSize.x : 0 ) +
				( message.m_Headshot ? 20 : 0 ) +
				( Weapon ? WeaponSize + 10 : 0 ) + AddictSpacing;
		};

		auto append = ( g_SettingsManager->B[ _S( "ui.watermark" ) ] ? 40 : 10 );

		if ( g_SettingsManager->B[ _S( "misc.hud.kill_list_colorize" ) ] ) {
			auto color = pAttacker->m_iTeamNum( ) == 2 ?
				ImColor( g_SettingsManager->IC[ _S( "misc.hud.kill_terrorist" ) ].Value.x,
					g_SettingsManager->IC[ _S( "misc.hud.kill_terrorist" ) ].Value.y,
					g_SettingsManager->IC[ _S( "misc.hud.kill_terrorist" ) ].Value.z, 0.f ) :
				ImColor( g_SettingsManager->IC[ _S( "misc.hud.kill_counter_terrorist" ) ].Value.x,
					g_SettingsManager->IC[ _S( "misc.hud.kill_counter_terrorist" ) ].Value.y,
					g_SettingsManager->IC[ _S( "misc.hud.kill_counter_terrorist" ) ].Value.z, 0.f );

			auto color_alpha = pAttacker->m_iTeamNum( ) == 2 ?
				ImColor( g_SettingsManager->IC[ _S( "misc.hud.kill_terrorist" ) ].Value.x,
					g_SettingsManager->IC[ _S( "misc.hud.kill_terrorist" ) ].Value.y,
					g_SettingsManager->IC[ _S( "misc.hud.kill_terrorist" ) ].Value.z, alpha * 0.70f ) :
				ImColor( g_SettingsManager->IC[ _S( "misc.hud.kill_counter_terrorist" ) ].Value.x,
					g_SettingsManager->IC[ _S( "misc.hud.kill_counter_terrorist" ) ].Value.y,
					g_SettingsManager->IC[ _S( "misc.hud.kill_counter_terrorist" ) ].Value.z, alpha * 0.70f );

			draw->AddRectFilledMultiColor( pos + ImVec2( size.x - GetKillWidht( ) - 10, append + m_iKillPosition ),
				pos + ImVec2( size.x - 10, append + 22 + m_iKillPosition ),
				color, color_alpha,
				color_alpha, color );
		}
		else {
			draw->AddRectFilledMultiColor( pos + ImVec2( size.x - GetKillWidht( ) - 10, append + m_iKillPosition ),
				pos + ImVec2( size.x - 10, append + 22 + m_iKillPosition ),
				ImColor( 45, 45, 45, 0 ), ImColor( 45, 45, 45, int( alpha * 180 ) ),
				ImColor( 45, 45, 45, int( alpha * 180 ) ), ImColor( 45, 45, 45, 0 ) );
		}

		ImColor text_color = ImColor( g_SettingsManager->IC[ _S( "misc.hud.kill_list_color" ) ].Value.x,
			g_SettingsManager->IC[ _S( "misc.hud.kill_list_color" ) ].Value.y,
			g_SettingsManager->IC[ _S( "misc.hud.kill_list_color" ) ].Value.z, alpha );

		ImColor headshot_color = ImColor( g_SettingsManager->IC[ _S( "misc.hud.kill_list_headshot" ) ].Value.x,
			g_SettingsManager->IC[ _S( "misc.hud.kill_list_headshot" ) ].Value.y,
			g_SettingsManager->IC[ _S( "misc.hud.kill_list_headshot" ) ].Value.z, alpha );

		draw->AddText( pos + ImVec2( size.x - GetKillWidht( ) - 10, append + m_iKillPosition + 3 ), text_color, UserAttacker.m_strName );
		draw->AddText( g_Menu->m_sFonts.WeapIcons, 14, pos + ImVec2( size.x - GetKillWidht( 10, true, false, true ) - 10, append + m_iKillPosition + 4 ), text_color, WeaponName.c_str( ) );
		if ( message.m_Headshot ) draw->AddText( g_Menu->m_sFonts.MenuIcons, 13, pos + ImVec2( size.x - GetKillWidht( 10, true, false, false ) - 10, append + m_iKillPosition + 5 ), headshot_color, "i" );
		draw->AddText( pos + ImVec2( size.x - GetKillWidht( message.m_Headshot ? -10 : 10, true, false, false ) - 10, append + m_iKillPosition + 3 ), text_color, UserDied.m_strName );

		m_iKillPosition += 25;
	};

	std::for_each( m_tKillList.begin( ), m_tKillList.end( ), [ this, DrawNotify ] ( KillMessage_t& message ) {
		if ( message.m_EndTime < GetTickCount64( ) )
			return;

		DrawNotify( message );
	} );

	m_iKillPosition = 0;
}

char asciitolower( char in ) {
	if ( in <= 'Z' && in >= 'A' )
		return in - ( 'Z' - 'z' );
	return in;
}

void C_CustomHUD::WeaponsHandler( ImDrawList* draw, ImVec2 pos, ImVec2 size )
{
	draw->AddRectFilledMultiColor( pos + ImVec2( size.x - 150, size.y - 40 ), pos + ImVec2( size.x - 10, size.y - 10 ),
		ImColor( 45, 45, 45, 0 ), ImColor( 45, 45, 45, 180 ),
		ImColor( 45, 45, 45, 180 ), ImColor( 45, 45, 45, 0 ) );

	C_BaseCombatWeapon* pWeapon = g_Globals->m_LocalPlayer->m_hActiveWeapon( ).Get( );
	if ( !pWeapon )
		return;

	std::string szLocalizedName = pWeapon->GetLocalizedName( );
	if ( szLocalizedName.length( ) <= 0 )
		return;

	std::string clipAmount1 = std::to_string( pWeapon->m_iClip1( ) );
	std::string clipAmount2 = std::to_string( pWeapon->m_iPrimaryReserveAmmoCount( ) );

	std::string ammos = clipAmount1 + " / " + clipAmount2;

	auto a1 = g_Menu->m_sFonts.Default24->CalcTextSizeA( 20, FLT_MAX, 0.0f, ammos.c_str( ) ).x;

	draw->AddText( g_Menu->m_sFonts.WeapIcons, 16, pos + ImVec2( size.x - a1 - 40 - std::get<1>( GetWeaponIcon( szLocalizedName ) ), size.y - 34 ), ImColor( 255, 255, 255 ), std::get<0>( GetWeaponIcon( szLocalizedName ) ).c_str( ) );
	draw->AddText( g_Menu->m_sFonts.Default24, 20, pos + ImVec2( size.x - a1 - 20, size.y - 36 ), ImColor( 255, 255, 255 ), ammos.c_str( ) );
}

std::tuple<std::string, float> C_CustomHUD::GetWeaponIcon( std::string weapon )
{
	if ( weapon == _S( "m249" ) ) return std::make_tuple<std::string, float>( "l", 30 );
	if ( weapon == _S( "mag7" ) ) return std::make_tuple<std::string, float>( "K", 27 );
	if ( weapon == _S( "negev" ) ) return std::make_tuple<std::string, float>( "L", 30 );
	if ( weapon == _S( "nova" ) ) return std::make_tuple<std::string, float>( "S", 38 );
	if ( weapon == _S( "sawedoff" ) ) return std::make_tuple<std::string, float>( "M", 34 );
	if ( weapon == _S( "xm1014" ) ) return std::make_tuple<std::string, float>( "I", 35 );
	if ( weapon == _S( "cz75a" ) ) return std::make_tuple<std::string, float>( "o", 20 );
	if ( weapon == _S( "deagle" ) ) return std::make_tuple<std::string, float>( "1", 22 );
	if ( weapon == _S( "elite" ) ) return std::make_tuple<std::string, float>( "2", 34 );
	if ( weapon == _S( "fiveseven" ) ) return std::make_tuple<std::string, float>( "3", 18 );
	if ( weapon == _S( "glock" ) ) return std::make_tuple<std::string, float>( "4", 20 );
	if ( weapon == _S( "hkp2000" ) ) return std::make_tuple<std::string, float>( "C", 18 );
	if ( weapon == _S( "p250" ) ) return std::make_tuple<std::string, float>( "P", 17 );
	if ( weapon == _S( "revolver" ) ) return std::make_tuple<std::string, float>( "", 20 );
	if ( weapon == _S( "tec9" ) ) return std::make_tuple<std::string, float>( "N", 20 );
	if ( weapon == _S( "usp_silencer" ) ) return std::make_tuple<std::string, float>( "m", 22 );
	if ( weapon == _S( "ak47" ) ) return std::make_tuple<std::string, float>( "7", 36 );
	if ( weapon == _S( "aug" ) ) return std::make_tuple<std::string, float>( "8", 32 );
	if ( weapon == _S( "awp" ) ) return std::make_tuple<std::string, float>( "9", 47 );
	if ( weapon == _S( "famas" ) ) return std::make_tuple<std::string, float>( ":", 32 );
	if ( weapon == _S( "g3sg1" ) ) return std::make_tuple<std::string, float>( ";", 40 );
	if ( weapon == _S( "galilar" ) ) return std::make_tuple<std::string, float>( "=", 43 );
	if ( weapon == _S( "m4a1" ) ) return std::make_tuple<std::string, float>( ">", 36 );
	if ( weapon == _S( "m4a1_silencer" ) ) return std::make_tuple<std::string, float>( "@", 48 );
	if ( weapon == _S( "scar20" ) ) return std::make_tuple<std::string, float>( "V", 40 );
	if ( weapon == _S( "sg556" ) ) return std::make_tuple<std::string, float>( "W", 40 );
	if ( weapon == _S( "ssg08" ) ) return std::make_tuple<std::string, float>( "X", 43 );
	if ( weapon == _S( "bizon" ) ) return std::make_tuple<std::string, float>( "J", 38 );
	if ( weapon == _S( "mac10" ) ) return std::make_tuple<std::string, float>( "A", 19 );
	if ( weapon == _S( "mp5sd" ) ) return std::make_tuple<std::string, float>( "", 20 );
	if ( weapon == _S( "mp7" ) ) return std::make_tuple<std::string, float>( "Q", 28 );
	if ( weapon == _S( "mp9" ) ) return std::make_tuple<std::string, float>( "R", 20 );
	if ( weapon == _S( "p90" ) ) return std::make_tuple<std::string, float>( "T", 28 );
	if ( weapon == _S( "ump45" ) ) return std::make_tuple<std::string, float>( "H", 33 );

	if ( weapon == _S( "M249" ) ) return std::make_tuple<std::string, float>( "l", 30 );
	if ( weapon == _S( "MAG-7" ) ) return std::make_tuple<std::string, float>( "K", 27 );
	if ( weapon == _S( "Negev" ) ) return std::make_tuple<std::string, float>( "L", 30 );
	if ( weapon == _S( "Nova" ) ) return std::make_tuple<std::string, float>( "S", 38 );
	if ( weapon == _S( "Sawed-Off" ) ) return std::make_tuple<std::string, float>( "M", 34 );
	if ( weapon == _S( "XM1014" ) ) return std::make_tuple<std::string, float>( "I", 35 );
	if ( weapon == _S( "CZ75-Auto" ) ) return std::make_tuple<std::string, float>( "o", 20 );
	if ( weapon == _S( "Desert Eagle" ) ) return std::make_tuple<std::string, float>( "1", 22 );
	if ( weapon == _S( "Dual Berettas" ) ) return std::make_tuple<std::string, float>( "2", 34 );
	if ( weapon == _S( "Five-SeveN" ) ) return std::make_tuple<std::string, float>( "3", 18 );
	if ( weapon == _S( "Glock-18" ) ) return std::make_tuple<std::string, float>( "4", 20 );
	if ( weapon == _S( "P2000" ) ) return std::make_tuple<std::string, float>( "C", 18 );
	if ( weapon == _S( "P250" ) ) return std::make_tuple<std::string, float>( "P", 17 );
	if ( weapon == _S( "R8 Revolver" ) ) return std::make_tuple<std::string, float>( "", 20 );
	if ( weapon == _S( "Tec-9" ) ) return std::make_tuple<std::string, float>( "N", 20 );
	if ( weapon == _S( "USP-S" ) ) return std::make_tuple<std::string, float>( "m", 22 );
	if ( weapon == _S( "AK-47" ) ) return std::make_tuple<std::string, float>( "7", 36 );
	if ( weapon == _S( "AUG" ) ) return std::make_tuple<std::string, float>( "8", 32 );
	if ( weapon == _S( "AWP" ) ) return std::make_tuple<std::string, float>( "9", 47 );
	if ( weapon == _S( "FAMAS" ) ) return std::make_tuple<std::string, float>( ":", 32 );
	if ( weapon == _S( "G3SG1" ) ) return std::make_tuple<std::string, float>( ";", 40 );
	if ( weapon == _S( "Galil AR" ) ) return std::make_tuple<std::string, float>( "=", 43 );
	if ( weapon == _S( "M4A4" ) ) return std::make_tuple<std::string, float>( ">", 36 );
	if ( weapon == _S( "M4A1-S" ) ) return std::make_tuple<std::string, float>( "@", 48 );
	if ( weapon == _S( "SCAR-20" ) ) return std::make_tuple<std::string, float>( "V", 40 );
	if ( weapon == _S( "SG 553" ) ) return std::make_tuple<std::string, float>( "W", 40 );
	if ( weapon == _S( "SSG 08)" ) ) return std::make_tuple<std::string, float>( "X", 43 );
	if ( weapon == _S( "PP-Bizon" ) ) return std::make_tuple<std::string, float>( "J", 38 );
	if ( weapon == _S( "MAC-10" ) ) return std::make_tuple<std::string, float>( "A", 19 );
	if ( weapon == _S( "MP5-SD" ) ) return std::make_tuple<std::string, float>( "", 20 );
	if ( weapon == _S( "MP7" ) ) return std::make_tuple<std::string, float>( "Q", 28 );
	if ( weapon == _S( "MP9" ) ) return std::make_tuple<std::string, float>( "R", 20 );
	if ( weapon == _S( "P90" ) ) return std::make_tuple<std::string, float>( "T", 28 );
	if ( weapon == _S( "UMP-45" ) ) return std::make_tuple<std::string, float>( "H", 33 );
}