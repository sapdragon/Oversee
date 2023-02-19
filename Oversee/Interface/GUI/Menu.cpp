#include "Menu.hpp"
#include "../../SDK/SDK.hpp"
#include "../GUI/GUI.hpp"
#include "../Renderer/Renderer.hpp"
#include "../../Features/Visuals/Model/Model.hpp"
#include "../../Libraries/ImGui/imgui_tricks.hpp"
#include "../../Libraries/Ethernet API/API/CloudConfigs.hpp"
#include "../../Libraries/Ethernet API/API/CloudScripts.hpp"
#include "../../Features/Inventory Changer/InventoryChanger.hpp"
#include "../../Features/Inventory Changer/ItemsManager.hpp"
#include "../../SDK/Utils/String/FNV1A.hpp"
#include "SteamManager.hpp"
#include "../../Features/Misc/Keybind V2/KeyManager.hpp"

inline bool ColorEdit4( const char* label, Color* v, bool show_alpha = true )
{
	auto clr = ImVec4 {
		v->r( ) / 255.0f,
		v->g( ) / 255.0f,
		v->b( ) / 255.0f,
		v->a( ) / 255.0f
	};

	if ( ImGui::ColorPicker4( label, &clr.x, show_alpha | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoSidePreview ) )
	{
		v->SetColor( clr.x, clr.y, clr.z, clr.w );
		return true;
	}
	return false;
}

inline bool ColorPicker4( const char* label, Color* v, bool show_alpha = true )
{
	auto clr = ImVec4 {
		v->r( ) / 255.0f,
		v->g( ) / 255.0f,
		v->b( ) / 255.0f,
		v->a( ) / 255.0f
	};

	if ( ImGui::ColorEdit4( label, &clr.x, show_alpha | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoInputs ) )
	{
		v->SetColor( clr.x, clr.y, clr.z, clr.w );
		return true;
	}
	return false;
}

std::string to_string( wchar_t const* wcstr ) {
	auto s = std::mbstate_t( );
	auto const target_char_count = std::wcsrtombs( nullptr, &wcstr, 0, &s );
	auto str = std::string( target_char_count, '\0' );
	std::wcsrtombs( str.data( ), &wcstr, str.size( ) + 1, &s );
	return str;
}

inline bool ColorEdit3( const char* label, Color* v )
{
	return ColorEdit4( label, v, false );
}

const char* tabs[ 10 ] = { "Dashboard", "Ragebot", "Legitbot", "Players", "World", "Inventory Changer", "Miscellaneous", "Configurations", "Scripts", "About" };

void C_Menu::RenderWatermark( )
{
	auto GenerateText = [ ] ( ) {
#ifdef OVERSEE_DEV
		static int nAverageFPS = ( int ) ImGui::GetIO( ).Framerate;
		static float flLastChecked = SDK::Interfaces::GlobalVars->m_flRealTime;
		if ( fabs( flLastChecked - SDK::Interfaces::GlobalVars->m_flRealTime ) > 2.0f )
		{
			nAverageFPS = ( int ) ImGui::GetIO( ).Framerate;
			flLastChecked = SDK::Interfaces::GlobalVars->m_flRealTime;
		}

		return std::string( _S( "oversee.one | " ) ) + ( std::string ) ( _S( " | FPS: " ) ) + std::to_string( nAverageFPS );
#endif
#ifndef OVERSEE_DEV
		return std::string( _S( "oversee.one | " ) ) + g_Globals->m_szName + ( std::string ) ( _S( " | FPS: " ) ) + std::to_string( ( int ) ImGui::GetIO( ).Framerate );
#endif
	};

	auto GenerateTempText = [ ] ( ) {
		return std::string( _S( "oversee.one | " ) ) + g_Globals->m_szName + std::string( _S( " | FPS: 200" ) );
	};

	int nWindowSize = ImGui::CalcTextSize( GenerateTempText( ).c_str( ) ).x + 12;
	int nWindowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;

	ImGui::Begin( _S( "Watermark" ), NULL, nWindowFlags );
	{
		ImGui::SetWindowSize( ImVec2( nWindowSize, 25 ) );
		ImGui::SetWindowPos( ImVec2( ImGui::GetIO( ).DisplaySize.x - nWindowSize - 10, 10 ) );

		auto draw = ImGui::GetWindowDrawList( );
		auto pos = ImGui::GetWindowPos( );
		auto size = ImGui::GetWindowSize( );

		draw->AddRectFilled( pos, pos + size, ImColor( 45, 45, 45, 100 ), 4, ImDrawCornerFlags_Top );
		draw->AddRectFilledMultiColor( pos + ImVec2( 0, 23 ), pos + size, ImColor( 0, 153, 255, 100 ), ImColor( 0, 153, 255, 0 ), ImColor( 0, 153, 255, 0 ), ImColor( 0, 153, 255, 100 ) );
		draw->AddText( pos + ImVec2( 6, 4 ), ImColor( 240, 240, 240 ), GenerateText( ).c_str( ) );
	}
	ImGui::End( );
}

std::string GetCurrentWeapon( ) {
	C_BaseCombatWeapon* pCombatWeapon = g_Globals->m_LocalPlayer->m_hActiveWeapon( );
	if ( !pCombatWeapon )
		return "";

	switch ( pCombatWeapon->m_nItemID( ) )
	{
		case WEAPON_AK47:
		case WEAPON_M4A1:
		case WEAPON_M4A1_SILENCER:
		case WEAPON_FAMAS:
		case WEAPON_SG553:
		case WEAPON_GALILAR:
		return "Rifle | ";
		case WEAPON_MAG7:
		case WEAPON_NOVA:
		case WEAPON_XM1014:
		case WEAPON_SAWEDOFF:
		return "Shotgun | ";
		case WEAPON_MP7:
		case WEAPON_MP9:
		case WEAPON_P90:
		case WEAPON_M249:
		case WEAPON_NEGEV:
		case WEAPON_UMP45:
		return "SMG | ";
		case WEAPON_SCAR20:
		case WEAPON_G3SG1:
		return "Auto | ";
		case WEAPON_GLOCK:
		case WEAPON_HKP2000:
		case WEAPON_USP_SILENCER:
		case WEAPON_CZ75A:
		case WEAPON_TEC9:
		case WEAPON_ELITE:
		case WEAPON_FIVESEVEN:
		case WEAPON_P250:
		return "Pistol | ";
		case WEAPON_SSG08:
		return "Scout | ";
		case WEAPON_AWP:
		return "AWP | ";
		case WEAPON_DEAGLE:
		return "Deagle | ";
		case WEAPON_REVOLVER:
		return "Revolver | ";
	}
}

void C_Menu::RenderKeybinds( ) {

	if ( !g_Globals->m_LocalPlayer )
		return;

	int nWindowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar;
	int nWindowSizeAppend = 0;

	ImGui::Begin( _S( "Keybinds" ), NULL, nWindowFlags );
	{
		ImGui::SetWindowSize( ImVec2( 225, 26 + nWindowSizeAppend ) );

		auto draw = ImGui::GetWindowDrawList( );
		auto pos = ImGui::GetWindowPos( );
		auto size = ImGui::GetWindowSize( );

		draw->AddRectFilled( pos, pos + size, ImColor( 45, 45, 45, 100 ), 4 );
		draw->AddRectFilled( pos, pos + ImVec2( 225, 22 ), ImColor( 45, 45, 45, 100 ), 4, ImDrawCornerFlags_Top );
		draw->AddRectFilledMultiColor( pos + ImVec2( 0, 23 ), pos + ImVec2( 225, 22 ), ImColor( 0, 153, 255, 100 ), ImColor( 0, 153, 255, 0 ), ImColor( 0, 153, 255, 0 ), ImColor( 0, 153, 255, 100 ) );
		draw->AddText( pos + ImVec2( 8, 3 ), ImColor( 240, 240, 240 ), _S( "Keybinds" ) );

		ImGui::SetCursorPos( { 8, 26 } );
		ImGui::BeginGroup( );

		for ( auto a = 0; a < g_KeyManager->m_vKeyActions.size( ); a++ ) 
		{
			if ( g_KeyManager->m_vKeyActions[ a ].m_iStatus == 0)
				continue;

			if ( g_KeyManager->m_vKeyActions[ a ].m_sVariableMapName.find( "ragebot." ) != std::string::npos ) {
				if ( g_KeyManager->m_vKeyActions[ a ].m_sVariableMapName != "ragebot.hide_shots" && g_KeyManager->m_vKeyActions[ a ].m_sVariableMapName != "ragebot.double_tap" ) {
					if ( !strstr( g_KeyManager->m_vKeyActions[ a ].m_sRenderableName.c_str( ), GetCurrentWeapon( ).c_str( ) ) ) {
						continue;
					}
				}
			}

			ImGui::Text(  g_KeyManager->m_vKeyActions[ a ].m_sRenderableName.c_str() );

			if ( g_KeyManager->m_vKeyActions[ a ].m_tVariableType == INT_KEY_TYPE ) {
				ImGui::SameLine( 210 - ImGui::CalcTextSize( std::to_string( g_SettingsManager->I[ g_KeyManager->m_vKeyActions[ a ].m_sVariableMapName ]).c_str() ).x );
				ImGui::Text( std::to_string( g_SettingsManager->I[ g_KeyManager->m_vKeyActions[ a ].m_sVariableMapName ] ).c_str( ) );
			}

			if ( g_KeyManager->m_vKeyActions[ a ].m_tVariableType == FLOAT_KEY_TYPE ) {
				ImGui::SameLine( 210 - ImGui::CalcTextSize( std::to_string( g_SettingsManager->F[ g_KeyManager->m_vKeyActions[ a ].m_sVariableMapName ] ).c_str( ) ).x );
				ImGui::Text( std::to_string( g_SettingsManager->F[ g_KeyManager->m_vKeyActions[ a ].m_sVariableMapName ] ).c_str( ) );
			}

			if ( g_KeyManager->m_vKeyActions[ a ].m_tVariableType == BOOL_KEY_TYPE ) {
				ImGui::SameLine( 210 - ImGui::CalcTextSize( "On" ).x );
				ImGui::Text("On" );
			}


			nWindowSizeAppend += 19;
			ImGui::SetWindowSize( ImVec2( 225, 26 + nWindowSizeAppend ) );
		}

		ImGui::EndGroup( );
	}
	ImGui::End( );

	nWindowSizeAppend = 0;
}

void C_Menu::RenderItemsControllers( ) {

	auto nWindowFlags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;

	ImGui::Begin( _S( "Item Controller" ), NULL, nWindowFlags );
	{
		ImGui::SetWindowPos( { ImGui::GetIO( ).DisplaySize.x / 2 - 70, 0 } );
		ImGui::SetWindowSize( ImVec2( 140, 36 ) );

		auto draw = ImGui::GetWindowDrawList( );
		auto pos = ImGui::GetWindowPos( );
		auto size = ImGui::GetWindowSize( );

		draw->AddRectFilled( pos, pos + size, ImColor( 45, 45, 45, 220 ), 5 );
		draw->AddText( pos + ImVec2( 60 - ImGui::CalcTextSize( _S( "Dashboard" ) ).x / 2, 36 / 2 - ImGui::CalcTextSize( _S( "Dashboard" ) ).y / 2 ), ImColor( 220, 220, 220, 220 ), _S( "Dashboard" ) );

		if ( ImGui::BeginPopup( _S( "Items" ), ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground ) )
		{
			auto draw2 = ImGui::GetWindowDrawList( );
			auto pos2 = ImGui::GetWindowPos( );

			draw2->AddRectFilled( pos2, pos2 + ImVec2( 140, 90 ), ImColor( 45, 45, 45, 220 ), 5 );

			ImGui::SetWindowPos( pos + ImVec2( 0, 40 ) );

			ImGui::SetCursorPos( { 10, 10 } );
			ImGui::BeginChild( _S( "ItemsChild" ), { 120, 90 } );
			{
				g_UIElements->DashboardItem( _S( "Watermark" ), g_SettingsManager->B[ _S( "ui.watermark" ) ] );
				g_UIElements->DashboardItem( _S( "Keybinds" ), g_SettingsManager->B[ _S( "ui.keybindlist" ) ] );
			}
			ImGui::EndChild( );
			ImGui::Dummy( { 140, 7 } );

			ImGui::EndPopup( );
		}

		ImGui::SetCursorPos( { 140 - 31, 5 } );
		ImGui::PushFont( m_sFonts.MenuIcons );

		if ( ImGui::Button( "h", { 26, 26 } ) )
			ImGui::OpenPopup( _S( "Items" ) );

		ImGui::PopFont( );
	}
	ImGui::End( );
}

bool HasOnlySpaces( const std::string& str )
{
	for ( std::string::const_iterator it = str.begin( ); it != str.end( ); ++it )
	{
		if ( *it != ' ' ) return false;
	}
	return true;
}

void C_Menu::RenderChat( ) {
}

std::string ToUtf88( std::wstring_view wstr ) {

	if ( wstr.empty( ) )
		return std::string( );

	auto size_needed = WideCharToMultiByte( CP_UTF8, 0, &wstr[ 0 ], static_cast< int32_t >( wstr.size( ) ), NULL, 0, NULL, NULL );

	std::string out( size_needed, 0 );
	WideCharToMultiByte( CP_UTF8, 0, &wstr[ 0 ], static_cast< int >( wstr.size( ) ), &out[ 0 ], size_needed, NULL, NULL );

	return out;
}

std::string LocalizeTex2( const char* in ) {

	const auto wide_name = SDK::Interfaces::Localize->FindSafe( in );

	if ( !wcslen( wide_name ) )
		return "";

	return ToUtf88( wide_name );
}

void C_Menu::Instance( )
{
	int nWindowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	static float nToggleValue = 0.f;

	ImVec2 pos = { };
	ImVec2 size = { };

	if ( g_SettingsManager->B[ _S( "ui.watermark" ) ] ) RenderWatermark( );
	if ( g_SettingsManager->B[ _S( "ui.keybindlist" ) ] ) RenderKeybinds( );

	if ( m_tUserAvatar == nullptr ) {
		m_tUserAvatar = SteamManager::LocalSteamImage( );
	}

#ifndef OVERSEE_DEV
	if ( g_CloudConfigs->CloudAction == CloudActions_t::Initialization )
	{
		ImGui::Begin( "Loading Screen", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground );
		{
			ImGui::SetWindowSize( ImGui::GetIO( ).DisplaySize );
			ImGui::SetWindowPos( { 0, 0 } );

			auto Animation = ImTricks::Animations::FastFloatLerp( ImGui::GetID( "Load Anim" ), true, 0.f, 1.f, 0.015f );
			ImGui::GetWindowDrawList( )->AddRectFilled( ImGui::GetWindowPos( ), ImGui::GetWindowPos( ) + ImGui::GetIO( ).DisplaySize, ImColor( 60, 60, 60, int( Animation * 180 ) ) );
			ImGui::GetWindowDrawList( )->AddText( g_Menu->m_sFonts.Default24, 20, ImGui::GetWindowPos( ) + ImGui::GetIO( ).DisplaySize / 2 - ImVec2( ImGui::CalcTextSize( "OVERSEE.ONE" ).x / 2, 100 ), ImColor( 255, 255, 255 ), "OVERSEE.ONE" );
			g_UIElements->Spinner( "Loading", 50, 5, ImColor( 255, 255, 255, int( Animation * 255 ) ), ImGui::GetIO( ).DisplaySize / 2 - ImVec2( 35, 50 ) );
		}
		ImGui::End( );
	}
#endif
	if ( !this->IsVisible( ) )
		return;

#ifndef OVERSEE_DEV
	if ( !( g_CloudConfigs->CloudAction == CloudActions_t::Initialization ) )
	{
#endif
		RenderItemsControllers( );

		ImGui::Begin( _S( "Fuck the world" ), NULL, nWindowFlags );
		{
			g_UIElements->Animate( );
			ImGui::SetWindowSize( ImVec2( 650, 600 ) );

			auto draw = ImGui::GetWindowDrawList( );
			pos = ImGui::GetWindowPos( );
			size = ImGui::GetWindowSize( );

			draw->AddRectFilled( pos, pos + ImVec2( 40, size.y ), ImColor( 25, 25, 25 ), 8, ImDrawCornerFlags_Left );

			draw->AddRectFilled( pos + ImVec2( 40, size.y - 20 ), pos + size, ImColor( 15, 15, 15 ), 8, ImDrawCornerFlags_BotRight );
			draw->AddText( pos + ImVec2( 45, size.y - 19 ), ImColor( 160, 160, 160 ), _S( "oversee.one" ) );

			draw->AddRectFilled( pos + ImVec2( 40, 0 ), pos + ImVec2( size.x, 35 ), ImColor( 38, 38, 38 ), 8, ImDrawCornerFlags_TopRight );
			draw->AddRectFilled( pos + ImVec2( size.x - 75, 0 ), pos + ImVec2( size.x, 35 ), ImColor( 88, 88, 88 ), 8, ImDrawCornerFlags_TopRight | ImDrawCornerFlags_BotLeft );
			draw->AddRectFilled( pos + ImVec2( size.x - 150, 0 ), pos + ImVec2( size.x - 60, 35 ), ImColor( 88, 88, 88 ), 12, ImDrawCornerFlags_TopRight | ImDrawCornerFlags_BotLeft );
			draw->AddText( pos + ImVec2( size.x - 110, 9 ), ImColor( 240, 240, 240 ), "pers0na2" );
			draw->AddText( m_sFonts.MenuIcons, 14, pos + ImVec2( 50, 9 ), ImColor( 120, 120, 120 ), _S( "u" ) );
			g_UIElements->AddCircleImageFilled( m_tUserAvatar, pos + ImVec2( size.x - 130, 17 ), 11, ImColor( 255, 255, 255 ), 32 );

			ImGui::SetCursorPos( { 68, 7 } );
			ImGui::InputTextWithHint( _S( "##Searchbar" ), _S( "Type your search query here..." ), m_cSearch, 64 );

			draw->AddText( m_sFonts.Default24, 24, pos + ImVec2( 55, 50 ), ImColor( 190, 190, 190 ), tabs[ m_iSelectedTab ] );

			ImGui::SetCursorPos( { 0, 5 } );
			ImGui::BeginGroup( );
			{
				for ( auto i = 0; i < 9; i++ )
					if( i != 8 )
					g_UIElements->Tab( std::to_string( i ).c_str( ), tabs[ i ], m_iSelectedTab, i );

				ImGui::SetCursorPos( { 0, 555 } );
				g_UIElements->Tab( std::to_string( 9 ).c_str( ), "About", m_iSelectedTab, 9 );
			}
			ImGui::EndGroup( );

			draw->AddRectFilled( pos + ImVec2( 40, 90 ), pos + ImVec2( size.x, 125 ), ImColor( 30, 30, 30 ) );
			draw->AddRectFilled( pos + ImVec2( 40, 125 ), pos + ImVec2( size.x, size.y - 20 ), ImColor( 50, 50, 50 ) );

			std::vector <std::string > aChamsStyle = { _S( "Flat" ), _S( "Regular" ), _S( "Sky" ) };
			std::vector <std::string > aChamsStyleAdded = { _S( "Gold" ), _S( "Glow" ), _S( "Ghost" ), _S( "Glass" ), _S( "Pulse" ) };

			if ( m_iSelectedTab == 0 )
			{
				ImGui::SetCursorPos( { 40, 90 } );
				ImGui::BeginGroup( );
				{
					g_UIElements->Subtab( _S( "My Keybinds" ), m_iSelectedSubTab[ 0 ], 0 );
					//g_UIElements->SameLineWithoutPadding( );
					//g_UIElements->Subtab( _S( "Player List" ), m_iSelectedSubTab[ 0 ], 1 );
				}
				ImGui::EndGroup( );

				if ( m_iSelectedSubTab[ 0 ] == 0 )
				{
					draw->AddRectFilled( pos + ImVec2( size.x - 10, 125 ), pos + ImVec2( size.x, size.y - 20 ), ImColor( 100, 100, 100 ) );
					draw->AddRectFilled( pos + ImVec2( 40, 125 ), pos + ImVec2( size.x, 155 ), ImColor( 26, 26, 26 ) );
					draw->AddText( pos + ImVec2( 55, 125 + 8 ), ImColor( 255, 255, 255 ), "Variable name" );
					draw->AddText( pos + ImVec2( 240, 125 + 8 ), ImColor( 255, 255, 255 ), "Type" );
					draw->AddText( pos + ImVec2( 340, 125 + 8 ), ImColor( 255, 255, 255 ), "Key" );

					ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, { 0, 0 } );
					{
						ImGui::SetCursorPos( { 40, 155 } );
						ImGui::BeginChild( _S( "##mainframess" ), { size.x - 40, 420 } );
						{
							for ( auto a = 0; a < g_KeyManager->m_vKeyActions.size( ); a++ ) {
								if ( g_UIElements->KeybindVisibleElement( g_KeyManager->m_vKeyActions[ a ], a + 1 ) == 1 )
									g_KeyManager->m_vKeyActions.erase( g_KeyManager->m_vKeyActions.begin( ) + a );
							}
						}
						ImGui::EndChild( );
					}
					ImGui::PopStyleVar( );
				}

				//if ( m_iSelectedSubTab[ 0 ] == 1 )
				//{
				//	ImGui::SetCursorPos( { 40, 125 } );
				//	ImGui::BeginChild( _S( "##mainframess" ), { size.x - 50, 455 } );
				//	{
				//		std::vector <std::string> spectators_name;

				//		for ( auto iPlayerIndex = 1; iPlayerIndex < SDK::Interfaces::GlobalVars->m_iMaxClients; iPlayerIndex++ )
				//		{
				//			C_BasePlayer* pPlayer = C_BasePlayer::GetPlayerByIndex( iPlayerIndex );
				//			if ( !pPlayer || !pPlayer->IsPlayer( ) )
				//				continue;

				//			PlayerInfo_t player_info;
				//			SDK::Interfaces::EngineClient->GetPlayerInfo( iPlayerIndex, &player_info );

				//			spectators_name.push_back( player_info.m_strName );
				//			if ( m_tAvatars[ iPlayerIndex - 1 ] == nullptr )
				//				m_tAvatars[ iPlayerIndex - 1 ] = SteamManager::SteamImageBySteamID( ( uint64 ) player_info.m_iSteamID64 );
				//		}
				//		for ( auto i = 0; i < spectators_name.size( ); i++ )
				//		{
				//			g_UIElements->PlayerList( spectators_name.at( i ).c_str( ), i, ImVec2( 180, 25 ) );
				//		}
				//	}
				//	ImGui::EndChild( );
				//}
			}
			else if ( m_iSelectedTab == 1 )
			{
				ImGui::SetCursorPos( { 600, 54 } );
				g_UIElements->MasterSwitch( "Ragebot Master", "ragebot.enable" );
				draw->AddText( pos + ImVec2( 510, 35 + 20 ), ImColor( 255, 255, 255 ), "Master Switch" );

				ImGui::SetCursorPos( { 40, 90 } );
				ImGui::BeginGroup( );
				{
					g_UIElements->Subtab( _S( "Primary" ), m_iSelectedSubTab[ 1 ], 0 );
					g_UIElements->SameLineWithoutPadding( );
					g_UIElements->Subtab( _S( "Anti-Aims" ), m_iSelectedSubTab[ 1 ], 1 );
				}
				ImGui::EndGroup( );

				static std::string weapon_group = "auto";


				if ( m_iSelectedSubTab[ 1 ] == 0 )
				{
					draw->PushClipRect( pos + ImVec2( 40, 600 - 55 ), pos + ImVec2( 650, 600 - 20 ), false );
					{
						draw->AddRectFilled(
							pos + ImVec2( 40, 600 - 20 - ( 35 * g_UIElements->swapAnimation ) ),
							pos + ImVec2( 650, 600 - ( 20 * g_UIElements->swapAnimation ) ),
							ImColor( 65, 65, 65 )
						);
						ImGui::SetCursorPos( { 40, 600 - 20 - ( 35 * g_UIElements->swapAnimation ) } );
						ImGui::BeginGroup( );
						{
							g_UIElements->WeaponSelect( _S( "auto" ), "Y", weapon_group );
							g_UIElements->SameLineWithoutPadding( );
							g_UIElements->WeaponSelect( _S( "scout" ), "a", weapon_group );
							g_UIElements->SameLineWithoutPadding( );
							g_UIElements->WeaponSelect( _S( "awp" ), "Z", weapon_group );
							g_UIElements->SameLineWithoutPadding( );
							g_UIElements->WeaponSelect( _S( "deagle" ), "A", weapon_group );
							g_UIElements->SameLineWithoutPadding( );
							g_UIElements->WeaponSelect( _S( "revolver" ), "J", weapon_group );
							g_UIElements->SameLineWithoutPadding( );
							g_UIElements->WeaponSelect( _S( "pistol" ), "G", weapon_group );
							g_UIElements->SameLineWithoutPadding( );
							g_UIElements->WeaponSelect( _S( "rifle" ), "T", weapon_group );
							g_UIElements->SameLineWithoutPadding( );
							g_UIElements->WeaponSelect( _S( "shotgun" ), "e", weapon_group );
							g_UIElements->SameLineWithoutPadding( );
							g_UIElements->WeaponSelect( _S( "smg" ), "K", weapon_group );
						}
						ImGui::EndGroup( );
					}
					draw->PopClipRect( );
				}

				ImGui::SetCursorPos( { 50, 135 } );
				ImGui::BeginChild( _S( "##mainframess" ), { size.x - 50, 420 + ( m_iSelectedSubTab[ 1 ] == 1 ? 35.f : 0.f ) } );
				{
					if ( m_iSelectedSubTab[ 1 ] == 0 )
					{

						ImGui::BeginChild( _S( "Section main2" ), { 300, 392 } );
						{
							g_UIElements->BeginChild( _S( "1" ), { 290, 40 } );
							{
								g_UIElements->Checkbox( _S( "Auto Scope" ), "ragebot." + weapon_group + ".auto_scope" );
							}
							g_UIElements->EndChild( );

							g_UIElements->BeginChild( _S( "2" ), { 290, 65 } );
							{
								g_UIElements->Checkbox( _S( "Auto Stop" ), "ragebot." + weapon_group + ".auto_stop" );
								g_UIElements->Checkbox( _S( "Early Auto Stop" ), "ragebot." + weapon_group + ".early_auto_stop" );
							}
							g_UIElements->EndChild( );

							g_UIElements->BeginChild( _S( "3" ), { 290, 119 } );
							{
								g_UIElements->Checkbox( _S( "Double tap" ), _S( "ragebot.double_tap" ) );
								g_UIElements->Checkbox( _S( "Extended teleport" ), _S( "ragebot." ) + weapon_group + _S( ".extended_teleport" ) );
								g_UIElements->Checkbox( _S( "Hide shots" ), _S( "ragebot.hide_shots" ) );
							}
							g_UIElements->EndChild( );

							g_UIElements->BeginChild( _S( "Accura4cy" ), { 290, 185 } );
							{
								g_UIElements->SliderInt( _S( "Hit Chance" ), _S( "ragebot." ) + weapon_group + _S( ".hit_chance" ), 0, 100, _S( "%i" ), NULL, OverallFlags_AutoScale );
								g_UIElements->SliderInt( _S( "Min Damage" ), _S( "ragebot." ) + weapon_group + _S( ".min_damage" ), 0, 130, _S( "%i" ), NULL, OverallFlags_AutoScale );
								g_UIElements->SliderInt( _S( "Head Scale" ), _S( "ragebot." ) + weapon_group + _S( ".head_scale" ), 0, 100, _S( "Already configured" ), NULL, OverallFlags_AutoScale );
								g_UIElements->SliderInt( _S( "Body Scale" ), _S( "ragebot." ) + weapon_group + _S( ".body_scale" ), 0, 100, _S( "Already configured" ), NULL, OverallFlags_AutoScale );
							}
							g_UIElements->EndChild( );

							g_UIElements->BeginChild( _S( "4" ), { 290, 65 } );
							{
								g_UIElements->Checkbox( _S( "Force safepoint ( Temporarily unswitchable )" ), _S( "ragebot.forced_safety" ) );
								g_SettingsManager->B[ _S( "ragebot.forced_safety" ) ] = true;
							}
							g_UIElements->EndChild( );
						}
						ImGui::EndChild( );
						ImGui::SameLine( 305 );
						ImGui::BeginGroup( );
						{
							g_UIElements->BeginChild( _S( "Points" ), { 285, 392 } );
							{
								if ( g_DrawModel->GetTexture( ) )
								{
									ImGui::GetWindowDrawList( )->AddImage( g_DrawModel->GetTexture( )->pTextureHandles[ 0 ]->lpRawTexture,
																		   ImGui::GetWindowPos( ) - ImVec2( 10, 90 ),
																		   ImGui::GetWindowPos( ) + ImVec2( g_DrawModel->GetTexture( )->GetActualWidth( ) * 0.8f,
																			   g_DrawModel->GetTexture( )->GetActualHeight( ) * 0.8f ) - ImVec2( 10, 90 ),
																		   ImVec2( 0, 0 ), ImVec2( 1, 1 ),
																		   ImColor( 1.0f, 1.0f, 1.0f, 1.0f ) );
								}

								ImGui::SetCursorPos( { 120, 40 } );
								g_UIElements->Hitbox( _S( "head" ), "ragebot." + weapon_group + ".hit_box_1" );

								ImGui::SetCursorPos( { 125, 105 } );
								g_UIElements->Hitbox( _S( "chest" ), "ragebot." + weapon_group + ".hit_box_2" );

								ImGui::SetCursorPos( { 125, 140 } );
								g_UIElements->Hitbox( _S( "stomach" ), "ragebot." + weapon_group + ".hit_box_6" );

								ImGui::SetCursorPos( { 125, 175 } );
								g_UIElements->Hitbox( _S( "pelvis" ), "ragebot." + weapon_group + ".hit_box_5" );

								ImGui::SetCursorPos( { 75, 155 } );
								g_UIElements->Hitbox( _S( "LeftForeArm" ), "ragebot." + weapon_group + ".hit_box_3" );

								ImGui::SetCursorPos( { 173, 155 } );
								g_UIElements->Hitbox( _S( "RightForeArm" ), "ragebot." + weapon_group + ".hit_box_3" );

								ImGui::SetCursorPos( { 85, 90 } );
								g_UIElements->Hitbox( _S( "LeftUpperArm" ), "ragebot." + weapon_group + ".hit_box_4" );

								ImGui::SetCursorPos( { 155, 85 } );
								g_UIElements->Hitbox( _S( "RightUpperArm" ), "ragebot." + weapon_group + ".hit_box_4" );

								ImGui::SetCursorPos( { 100, 215 } );
								g_UIElements->Hitbox( _S( "LeftThigh" ), "ragebot." + weapon_group + ".hit_box_8" );

								ImGui::SetCursorPos( { 152, 215 } );
								g_UIElements->Hitbox( _S( "RightThigh" ), "ragebot." + weapon_group + ".hit_box_8" );

								ImGui::SetCursorPos( { 100, 280 } );
								g_UIElements->Hitbox( _S( "LeftLeg" ), "ragebot." + weapon_group + ".hit_box_9" );

								ImGui::SetCursorPos( { 160, 280 } );
								g_UIElements->Hitbox( _S( "RightLeg" ), "ragebot." + weapon_group + ".hit_box_9" );
							}
							g_UIElements->EndChild( );
						}
						ImGui::EndGroup( );
					}
					if ( m_iSelectedSubTab[ 1 ] == 1 )
					{
						ImGui::BeginGroup( );
						{
							g_UIElements->BeginChild( _S( "Section 123" ), { 290, 335 } );
							{
								g_UIElements->Checkbox( _S( "Enable" ), _S( "antiaims.enable" ) );
								g_UIElements->Checkbox( _S( "With knife" ), _S( "antiaims.at_knife" ) );
								g_UIElements->Checkbox( _S( "At targets" ), _S( "antiaims.at_targets" ) );
								g_UIElements->Checkbox( _S( "Invert side" ), _S( "antiaims.invert_side" ) );
								g_UIElements->Checkbox( _S( "Avoid overlap" ), _S( "antiaims.avoid_overlap" ) );
								g_UIElements->SliderInt( _S( "Yaw Add offset" ), _S( "antiaims.yaw_offset" ), -180, 180 );
								g_UIElements->SliderInt( _S( "Left desync limit" ), _S( "antiaims.left_limit" ), 0, 58 );
								g_UIElements->SliderInt( _S( "Right desync limit" ), _S( "antiaims.right_limit" ), 0, 58 );
								g_UIElements->SliderInt( _S( "Jitter limit" ), _S( "antiaims.jitter_offset" ), -90, 90 );
								g_UIElements->SliderInt( _S( "Roll antiaim" ), _S( "antiaims.roll_antiaim" ), -50, 50 );
							}
							g_UIElements->EndChild( );
						}
						ImGui::EndGroup( );
						ImGui::SameLine( );
						ImGui::BeginGroup( );
						{
							g_UIElements->BeginChild( _S( "Section 1231" ), { 290, 230 } );
							{
								g_UIElements->Checkbox( _S( "Slow Walk" ), _S( "antiaims.slow_walk" ) );
								g_UIElements->Checkbox( _S( "Jitter move" ), _S( "antiaims.jitter_move" ) );
								g_UIElements->Checkbox( _S( "Jitter sides" ), _S( "antiaims.jitter_sides" ) );

								std::vector <std::string > vMovmentType = { _S( "Default" ), _S( "Slide" ) };
								g_UIElements->Combo( _S( "Movement Type" ), _S( "antiaims.leg_mode" ), vMovmentType );
							}
							g_UIElements->EndChild( );

							g_UIElements->BeginChild( _S( "Section 5" ), { 290, 120 } );
							{
								g_UIElements->Checkbox( _S( "Fakelag" ), _S( "misc.fake_lag.enable" ) );
								g_UIElements->Checkbox( _S( "Randomization" ), _S( "misc.fake_lag.randomization" ) );
								g_UIElements->SliderInt( _S( "Limit" ), _S( "misc.fake_lag.amount" ), 1, 16 );
							}
							g_UIElements->EndChild( );
						}
						ImGui::EndGroup( );
					}
				}
				ImGui::EndChild( );
			}
			else if ( m_iSelectedTab == 3 )
			{
				ImGui::SetCursorPos( { 40, 90 } );
				ImGui::BeginGroup( );
				{
					g_UIElements->Subtab( _S( "Enemy" ), m_iSelectedSubTab[ 3 ], 0 );
					g_UIElements->SameLineWithoutPadding( );
					g_UIElements->Subtab( _S( "Local" ), m_iSelectedSubTab[ 3 ], 2 );
					g_UIElements->SameLineWithoutPadding( );
					g_UIElements->Subtab( _S( "Teammates" ), m_iSelectedSubTab[ 3 ], 1 );
				}
				ImGui::EndGroup( );

				ImGui::SetCursorPos( { 50, 135 } );
				ImGui::BeginChild( _S( "##mainframess" ), { size.x - 50, 455 } );
				{
					if ( m_iSelectedSubTab[ 3 ] == 0 )
					{
						static ImVec2 CurrentPos = ImGui::GetCursorPos( );
						g_UIElements->BeginChild( _S( "Section 1234" ), { 290, 80 } );
						{
							g_UIElements->Checkbox( _S( "Enable" ), _S( "esp.enemy.enabled" ) );
						}
						g_UIElements->EndChild( );

						g_UIElements->BeginChild( _S( "Section 2231" ), { 290, 97 } );
						{
							g_UIElements->Checkbox( _S( "Glow enable" ), _S( "glow.enemy.enabled" ), [ ] ( ) {
								ImGui::ColorEdit4( _S( "Color" ), &g_SettingsManager->IC[ _S( "glow.enemy.color" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
							}, 50 );
							std::vector <std::string > aGlowStyle = { _S( "Standart" ), _S( "Pulse" ), _S( "Inner" ) };
							g_UIElements->Combo( _S( "Glow style" ), _S( "glow.enemy.style" ), aGlowStyle );

						}
						g_UIElements->EndChild( );

						g_UIElements->BeginChild( _S( "Section 2323" ), { 290, 140 } );
						{
							g_UIElements->Checkbox( _S( "On Shot chams enable" ), _S( "chams.on_shot.draw_chams" ), [ ] ( ) {
								ColorPicker4( _S( "Main Style Color" ), &g_SettingsManager->C[ _S( "chams.on_shot.color" ) ], true );
								ColorPicker4( _S( "Modificator 1 Color" ), &g_SettingsManager->C[ _S( "chams.on_shot.color1" ) ], true );
								ColorPicker4( _S( "Glow color" ), &g_SettingsManager->C[ _S( "chams.on_shot.color2" ) ], true );
								ColorPicker4( _S( "Modificator 3 Color" ), &g_SettingsManager->C[ _S( "chams.on_shot.color3" ) ], true );
								ColorPicker4( _S( "Modificator 4 Color" ), &g_SettingsManager->C[ _S( "chams.on_shot.color4" ) ], true );
								ColorPicker4( _S( "Modificator 5 Color" ), &g_SettingsManager->C[ _S( "chams.on_shot.color5" ) ], true );
							}, 185 );
							g_UIElements->Combo( _S( "On Shot main chams style" ), _S( "chams.on_shot.main_style" ), aChamsStyle );
							std::vector<std::string> ChamsVars = {
								_S( "chams.on_shot.mat_param_1" ), _S( "chams.on_shot.mat_param_2" ), _S( "chams.on_shot.mat_param_3" ),
								_S( "chams.on_shot.mat_param_4" ), _S( "chams.on_shot.mat_param_5" ),
							};
							g_UIElements->MultiCombo( _S( "On Shot Modificators" ), ChamsVars, aChamsStyleAdded );
						}
						g_UIElements->EndChild( );

						ImGui::SetCursorPos( ImVec2( CurrentPos.x + 300, CurrentPos.y ) );

						g_UIElements->BeginChild( _S( "Section 31231" ), { 290, 290 + 130 } );
						{
							g_UIElements->Checkbox( _S( "Visible chams enable" ), _S( "chams.enemy_visible.draw_chams" ), [ ] ( ) {
								ColorPicker4( _S( "Main Style Color" ), &g_SettingsManager->C[ _S( "chams.enemy_visible.color" ) ], true );
								ColorPicker4( _S( "Modificator 1 Color" ), &g_SettingsManager->C[ _S( "chams.enemy_visible.color1" ) ], true );
								ColorPicker4( _S( "Glow color" ), &g_SettingsManager->C[ _S( "chams.enemy_visible.color2" ) ], true );
								ColorPicker4( _S( "Modificator 3 Color" ), &g_SettingsManager->C[ _S( "chams.enemy_visible.color3" ) ], true );
								ColorPicker4( _S( "Modificator 4 Color" ), &g_SettingsManager->C[ _S( "chams.enemy_visible.color4" ) ], true );
								ColorPicker4( _S( "Modificator 5 Color" ), &g_SettingsManager->C[ _S( "chams.enemy_visible.color5" ) ], true );
							}, 185 );
							g_UIElements->Combo( _S( "Visible main chams style" ), _S( "chams.enemy_visible.main_style" ), aChamsStyle );
							std::vector<std::string> ChamsVars = {
								_S( "chams.enemy_visible.mat_param_1" ), _S( "chams.enemy_visible.mat_param_2" ), _S( "chams.enemy_visible.mat_param_3" ),
								_S( "chams.enemy_visible.mat_param_4" ), _S( "chams.enemy_visible.mat_param_5" ),
							};
							g_UIElements->MultiCombo( _S( "Visible Modificators" ), ChamsVars, aChamsStyleAdded );

							ImGui::Spacing( );

							g_UIElements->Checkbox( _S( "Invisible chams enable" ), _S( "chams.enemy_invisible.draw_chams" ), [ ] ( ) {
								ColorPicker4( _S( "Main Style Color" ), &g_SettingsManager->C[ _S( "chams.enemy_invisible.color" ) ], true );
								ColorPicker4( _S( "Modificator 1 Color" ), &g_SettingsManager->C[ _S( "chams.enemy_invisible.color1" ) ], true );
								ColorPicker4( _S( "Glow color" ), &g_SettingsManager->C[ _S( "chams.enemy_invisible.color2" ) ], true );
								ColorPicker4( _S( "Modificator 3 Color" ), &g_SettingsManager->C[ _S( "chams.enemy_invisible.color3" ) ], true );
								ColorPicker4( _S( "Modificator 4 Color" ), &g_SettingsManager->C[ _S( "chams.enemy_invisible.color4" ) ], true );
								ColorPicker4( _S( "Modificator 5 Color" ), &g_SettingsManager->C[ _S( "chams.enemy_invisible.color5" ) ], true );
							}, 185 );
							g_UIElements->Combo( _S( "Invisible main chams style" ), _S( "chams.enemy_invisible.main_style" ), aChamsStyle );
							std::vector<std::string> ChamsVars2 = {
								_S( "chams.enemy_invisible.mat_param_1" ), _S( "chams.enemy_invisible.mat_param_2" ), _S( "chams.enemy_invisible.mat_param_3" ),
								_S( "chams.enemy_invisible.mat_param_4" ), _S( "chams.enemy_invisible.mat_param_5" ),
							};
							g_UIElements->MultiCombo( _S( "Invisible Modificators" ), ChamsVars2, aChamsStyleAdded );

							ImGui::Spacing( );

							g_UIElements->Checkbox( _S( "Backtrack chams enable" ), _S( "chams.enemy_backtrack.draw_chams" ), [ ] ( ) {
								ColorPicker4( _S( "Main Style Color" ), &g_SettingsManager->C[ _S( "chams.enemy_backtrack.color" ) ], true );
								ColorPicker4( _S( "Modificator 1 Color" ), &g_SettingsManager->C[ _S( "chams.enemy_backtrack.color1" ) ], true );
								ColorPicker4( _S( "Glow color" ), &g_SettingsManager->C[ _S( "chams.enemy_backtrack.color2" ) ], true );
								ColorPicker4( _S( "Modificator 3 Color" ), &g_SettingsManager->C[ _S( "chams.enemy_backtrack.color3" ) ], true );
								ColorPicker4( _S( "Modificator 4 Color" ), &g_SettingsManager->C[ _S( "chams.enemy_backtrack.color4" ) ], true );
								ColorPicker4( _S( "Modificator 5 Color" ), &g_SettingsManager->C[ _S( "chams.enemy_backtrack.color5" ) ], true );
							}, 185 );
							g_UIElements->Combo( _S( "Backtrack main chams style" ), _S( "chams.enemy_backtrack.main_style" ), aChamsStyle );
							std::vector<std::string> ChamsVars3 = {
								_S( "chams.enemy_backtrack.mat_param_1" ), _S( "chams.enemy_backtrack.mat_param_2" ), _S( "chams.enemy_backtrack.mat_param_3" ),
								_S( "chams.enemy_backtrack.mat_param_4" ), _S( "chams.enemy_backtrack.mat_param_5" ),
							};
							g_UIElements->MultiCombo( _S( "Backtrack Modificators" ), ChamsVars3, aChamsStyleAdded );
						}
						g_UIElements->EndChild( );
					}
					else if ( m_iSelectedSubTab[ 3 ] == 1 )
					{
						static ImVec2 CurrentPos = ImGui::GetCursorPos( );

						g_UIElements->BeginChild( _S( "Section 1" ), { 290, 40 } );
						{
							g_UIElements->Checkbox( _S( "Enable" ), _S( "esp.team.enabled" ) );
						}
						g_UIElements->EndChild( );

						g_UIElements->BeginChild( _S( "Section 2" ), { 290, 97 } );
						{
							g_UIElements->Checkbox( _S( "Glow enable" ), _S( "glow.team.enabled" ), [ ] ( ) {
								ImGui::ColorEdit4( _S( "Color" ), &g_SettingsManager->IC[ _S( "glow.team.color" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
							}, 50 );
							std::vector <std::string > aGlowStyle = { _S( "Standart" ), _S( "Pulse" ), _S( "Inner" ) };
							g_UIElements->Combo( _S( "Glow style" ), _S( "glow.team.style" ), aGlowStyle );

						}
						g_UIElements->EndChild( );
						ImGui::SetCursorPos( ImVec2( CurrentPos.x + 300, CurrentPos.y ) );

						g_UIElements->BeginChild( _S( "Section 3" ), { 290, 285 } );
						{
							g_UIElements->Checkbox( _S( "Visible chams enable" ), _S( "chams.team_visible.draw_chams" ), [ ] ( ) {
								ColorPicker4( _S( "Main Style Color" ), &g_SettingsManager->C[ _S( "chams.team_visible.color" ) ], true );
								ColorPicker4( _S( "Modificator 1 Color" ), &g_SettingsManager->C[ _S( "chams.team_visible.color1" ) ], true );
								ColorPicker4( _S( "Glow color" ), &g_SettingsManager->C[ _S( "chams.team_visible.color2" ) ], true );
								ColorPicker4( _S( "Modificator 3 Color" ), &g_SettingsManager->C[ _S( "chams.team_visible.color3" ) ], true );
								ColorPicker4( _S( "Modificator 4 Color" ), &g_SettingsManager->C[ _S( "chams.team_visible.color4" ) ], true );
								ColorPicker4( _S( "Modificator 5 Color" ), &g_SettingsManager->C[ _S( "chams.team_visible.color5" ) ], true );
							}, 185 );
							g_UIElements->Combo( _S( "Visible main chams style" ), _S( "chams.team_visible.main_style" ), aChamsStyle );
							std::vector<std::string> ChamsVars = {
								_S( "chams.team_visible.mat_param_1" ), _S( "chams.team_visible.mat_param_2" ), _S( "chams.team_visible.mat_param_3" ),
								_S( "chams.team_visible.mat_param_4" ), _S( "chams.team_visible.mat_param_5" ),
							};
							g_UIElements->MultiCombo( _S( "Visible Modificators" ), ChamsVars, aChamsStyleAdded );

							ImGui::Spacing( );

							g_UIElements->Checkbox( _S( "Invisible chams enable" ), _S( "chams.team_invisible.draw_chams" ), [ ] ( ) {
								ColorPicker4( _S( "Main Style Color" ), &g_SettingsManager->C[ _S( "chams.team_invisible.color" ) ], true );
								ColorPicker4( _S( "Modificator 1 Color" ), &g_SettingsManager->C[ _S( "chams.team_invisible.color1" ) ], true );
								ColorPicker4( _S( "Glow color" ), &g_SettingsManager->C[ _S( "chams.team_invisible.color2" ) ], true );
								ColorPicker4( _S( "Modificator 3 Color" ), &g_SettingsManager->C[ _S( "chams.team_invisible.color3" ) ], true );
								ColorPicker4( _S( "Modificator 4 Color" ), &g_SettingsManager->C[ _S( "chams.team_invisible.color4" ) ], true );
								ColorPicker4( _S( "Modificator 5 Color" ), &g_SettingsManager->C[ _S( "chams.team_invisible.color5" ) ], true );
							}, 185 );
							g_UIElements->Combo( _S( "Invisible main chams style" ), _S( "chams.team_invisible.main_style" ), aChamsStyle );
							std::vector<std::string> ChamsVars2 = {
								_S( "chams.team_invisible.mat_param_1" ), _S( "chams.team_invisible.mat_param_2" ), _S( "chams.team_invisible.mat_param_3" ),
								_S( "chams.team_invisible.mat_param_4" ), _S( "chams.team_invisible.mat_param_5" ),
							};
							g_UIElements->MultiCombo( _S( "Invisible Modificators" ), ChamsVars2, aChamsStyleAdded );

						}
						g_UIElements->EndChild( );
					}
					else if ( m_iSelectedSubTab[ 3 ] == 2 )
					{
						static ImVec2 CurrentPos = ImGui::GetCursorPos( );

						ImGui::BeginGroup( );
						{
							g_UIElements->BeginChild( _S( "Section 2" ), { 290, 95 } );
							{
								g_UIElements->Checkbox( _S( "Glow enable" ), _S( "glow.local.enabled" ), [ ] ( ) {
									ImGui::ColorEdit4( _S( "Color" ), &g_SettingsManager->IC[ _S( "glow.local.color" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
								}, 50 );
								std::vector <std::string > aGlowStyle = { _S( "Standart" ), _S( "Pulse" ), _S( "Inner" ) };
								g_UIElements->Combo( _S( "Glow style" ), _S( "glow.local.style" ), aGlowStyle );

							}
							g_UIElements->EndChild( );

							g_UIElements->BeginChild( _S( "Section 5" ), { 290, 285 } );
							{
								g_UIElements->SliderInt( _S( "Scoped transparency" ), _S( "misc.transparent_in_tperson" ), 20, 100 );
								g_UIElements->Checkbox( _S( "Arms chams enable" ), _S( "chams.arms.draw_chams" ), [ ] ( ) {
									ColorPicker4( _S( "Main Style Color" ), &g_SettingsManager->C[ _S( "chams.arms.color" ) ], true );
									ColorPicker4( _S( "Modificator 1 Color" ), &g_SettingsManager->C[ _S( "chams.arms.color1" ) ], true );
									ColorPicker4( _S( "Glow color" ), &g_SettingsManager->C[ _S( "chams.arms.color2" ) ], true );
									ColorPicker4( _S( "Modificator 3 Color" ), &g_SettingsManager->C[ _S( "chams.arms.color3" ) ], true );
									ColorPicker4( _S( "Modificator 4 Color" ), &g_SettingsManager->C[ _S( "chams.arms.color4" ) ], true );
									ColorPicker4( _S( "Modificator 5 Color" ), &g_SettingsManager->C[ _S( "chams.arms.color5" ) ], true );
								}, 185 );
								g_UIElements->Combo( _S( "Arms main chams style" ), _S( "chams.arms.main_style" ), aChamsStyle );
								std::vector<std::string> ChamsVars = {
									_S( "chams.arms.mat_param_1" ), _S( "chams.arms.mat_param_2" ), _S( "chams.arms.mat_param_3" ),
									_S( "chams.arms.mat_param_4" ), _S( "chams.arms.mat_param_5" ),
								};
								g_UIElements->MultiCombo( _S( "Arms Modificators" ), ChamsVars, aChamsStyleAdded );

								ImGui::Spacing( );

								g_UIElements->Checkbox( _S( "Local Weapon chams enable" ), _S( "chams.local_weapon.draw_chams" ), [ ] ( ) {
									ColorPicker4( _S( "Main Style Color" ), &g_SettingsManager->C[ _S( "chams.local_weapon.color" ) ], true );
									ColorPicker4( _S( "Modificator 1 Color" ), &g_SettingsManager->C[ _S( "chams.local_weapon.color1" ) ], true );
									ColorPicker4( _S( "Glow color" ), &g_SettingsManager->C[ _S( "chams.local_weapon.color2" ) ], true );
									ColorPicker4( _S( "Modificator 3 Color" ), &g_SettingsManager->C[ _S( "chams.local_weapon.color3" ) ], true );
									ColorPicker4( _S( "Modificator 4 Color" ), &g_SettingsManager->C[ _S( "chams.local_weapon.color4" ) ], true );
									ColorPicker4( _S( "Modificator 5 Color" ), &g_SettingsManager->C[ _S( "chams.local_weapon.color5" ) ], true );
								}, 185 );
								g_UIElements->Combo( _S( "Weapon main chams style" ), _S( "chams.local_weapon.main_style" ), aChamsStyle );
								std::vector<std::string> ChamsVar2s = {
									_S( "chams.local_weapon.mat_param_1" ), _S( "chams.local_weapon.mat_param_2" ), _S( "chams.local_weapon.mat_param_3" ),
									_S( "chams.local_weapon.mat_param_4" ), _S( "chams.local_weapon.mat_param_5" ),
								};
								g_UIElements->MultiCombo( _S( "Weapon Modificators" ), ChamsVar2s, aChamsStyleAdded );
							}
							g_UIElements->EndChild( );
						}
						ImGui::EndGroup( );

						ImGui::SetCursorPos( ImVec2( CurrentPos.x + 300, CurrentPos.y ) );
						g_UIElements->BeginChild( _S( "Section 3" ), { 290, 285 } );
						{
							g_UIElements->Checkbox( _S( "Real chams enable" ), _S( "chams.real.draw_chams" ), [ ] ( ) {
								ColorPicker4( _S( "Main Style Color" ), &g_SettingsManager->C[ _S( "chams.real.color" ) ], true );
								ColorPicker4( _S( "Modificator 1 Color" ), &g_SettingsManager->C[ _S( "chams.real.color1" ) ], true );
								ColorPicker4( _S( "Glow color" ), &g_SettingsManager->C[ _S( "chams.real.color2" ) ], true );
								ColorPicker4( _S( "Modificator 3 Color" ), &g_SettingsManager->C[ _S( "chams.real.color3" ) ], true );
								ColorPicker4( _S( "Modificator 4 Color" ), &g_SettingsManager->C[ _S( "chams.real.color4" ) ], true );
								ColorPicker4( _S( "Modificator 5 Color" ), &g_SettingsManager->C[ _S( "chams.real.color5" ) ], true );
							}, 185 );
							g_UIElements->Combo( _S( "Real main chams style" ), _S( "chams.real.main_style" ), aChamsStyle );
							std::vector<std::string> ChamsVar2s = {
								_S( "chams.real.mat_param_1" ), _S( "chams.real.mat_param_2" ), _S( "chams.real.mat_param_3" ),
								_S( "chams.real.mat_param_4" ), _S( "chams.real.mat_param_5" ),
							};
							g_UIElements->MultiCombo( _S( "Real Modificators" ), ChamsVar2s, aChamsStyleAdded );

							ImGui::Spacing( );

							g_UIElements->Checkbox( _S( "Desync chams enable" ), _S( "chams.desync.draw_chams" ), [ ] ( ) {
								ColorPicker4( _S( "Main Style Color" ), &g_SettingsManager->C[ _S( "chams.desync.color" ) ], true );
								ColorPicker4( _S( "Modificator 1 Color" ), &g_SettingsManager->C[ _S( "chams.desync.color1" ) ], true );
								ColorPicker4( _S( "Glow color" ), &g_SettingsManager->C[ _S( "chams.desync.color2" ) ], true );
								ColorPicker4( _S( "Modificator 3 Color" ), &g_SettingsManager->C[ _S( "chams.desync.color3" ) ], true );
								ColorPicker4( _S( "Modificator 4 Color" ), &g_SettingsManager->C[ _S( "chams.desync.color4" ) ], true );
								ColorPicker4( _S( "Modificator 5 Color" ), &g_SettingsManager->C[ _S( "chams.desync.color5" ) ], true );
							}, 185 );
							g_UIElements->Combo( _S( "Desync main chams style" ), _S( "chams.desync.main_style" ), aChamsStyle );
							std::vector<std::string> ChamsVar23s = {
								_S( "chams.desync.mat_param_1" ), _S( "chams.desync.mat_param_2" ), _S( "chams.desync.mat_param_3" ),
								_S( "chams.desync.mat_param_4" ), _S( "chams.desync.mat_param_5" ),
							};
							g_UIElements->MultiCombo( _S( "Desync Modificators" ), ChamsVar23s, aChamsStyleAdded );
						}
						g_UIElements->EndChild( );
					}
				}
				ImGui::EndChild( );
			}
			else if ( m_iSelectedTab == 4 )
			{
				ImGui::SetCursorPos( { 40, 90 } );
				ImGui::BeginGroup( );
				{
					g_UIElements->Subtab( _S( "Modulations  /  Removals" ), m_iSelectedSubTab[ 4 ], 0 );
					g_UIElements->SameLineWithoutPadding( );
					g_UIElements->Subtab( _S( "Information" ), m_iSelectedSubTab[ 4 ], 1 );
				}
				ImGui::EndGroup( );

				ImGui::SetCursorPos( { 50, 135 } );
				ImGui::BeginChild( _S( "##mainframess" ), { size.x - 50, 455 } );
				{
					if ( m_iSelectedSubTab[ 4 ] == 0 )
					{
						ImGui::BeginGroup( );
						{
							g_UIElements->BeginChild( _S( "Section main" ), { 290, 142 } );
							{
								g_UIElements->Checkbox( _S( "Fog Enabled" ), _S( "misc.fog.enable" ) );
								g_UIElements->SliderInt( _S( "Fog Minimal Distance" ), _S( "misc.fog.min_distance" ), 0, 5000 );
								g_UIElements->SliderInt( _S( "Fog Max Distance" ), _S( "misc.fog.max_distance" ), 0, 5000 );
								ColorPicker4( _S( "Fog Color" ), &g_SettingsManager->C[ _S( "misc.fog.color" ) ], true );
							}
							g_UIElements->EndChild( );

							g_UIElements->BeginChild( _S( "Section 2" ), { 290, 95 } );
							{
								ColorPicker4( _S( "World Modulation" ), &g_SettingsManager->C[ _S( "misc.world_modulation" ) ], true );
								ColorPicker4( _S( "Prop Modulation" ), &g_SettingsManager->C[ _S( "misc.prop_modulation" ) ], true );
								ColorPicker4( _S( "Sky Modulation" ), &g_SettingsManager->C[ _S( "misc.sky_modulation" ) ], true );
							}
							g_UIElements->EndChild( );
						}
						ImGui::EndGroup( );
						ImGui::SameLine( );
						ImGui::BeginGroup( );
						{
							g_UIElements->BeginChild( _S( "Section 4" ), { 290, 168 } );
							{
								std::vector<std::string> aRemovals =
								{
									_S( "Postprocess" ), _S( "Fog" ), _S( "Recoil" ),
									_S( "Punch" ), _S( "Shadows" ), _S( "Landing bob" ),
									_S( "Hands sway" ), _S( "Smoke" ), _S( "Flash" ),
									_S( "Blood" ), _S( "Ping sound" )
								};

								std::vector<std::string> aRemovalsVars =
								{
									_S( "misc.removals.item_1" ), _S( "misc.removals.item_2" ), _S( "misc.removals.item_3" ),
									_S( "misc.removals.item_4" ), _S( "misc.removals.item_5" ), _S( "misc.removals.item_6" ),
									_S( "misc.removals.item_7" ), _S( "misc.removals.item_8" ), _S( "misc.removals.item_9" ),
									_S( "misc.removals.item_10" ), _S( "misc.removals.item_11" )
								};

								g_UIElements->MultiCombo( _S( "Removals" ), aRemovalsVars, aRemovals );

								std::vector<std::string> aEventList =
								{
									_S( "Purchase" ),
									_S( "Hurt" ),
									_S( "Harm" ),
									_S( "Bomb" ),
									_S( "Miss" )
								};

								std::vector<std::string> aEventListsVars =
								{
									_S( "misc.logs.item_1" ), _S( "misc.logs.item_2" ), _S( "misc.logs.item_3" ),
									_S( "misc.logs.item_4" ), _S( "misc.logs.item_5" ),
								};

								g_UIElements->MultiCombo( _S( "Event list" ), aEventListsVars, aEventList );

								std::vector < std::string > aRemoveScope
									=
								{
									_S( "Default" ),
									_S( "Static" ),
									_S( "Dynamic" )
								};
								g_UIElements->Combo( _S( "No-Scope mode" ), _S( "misc.no_scope_mode" ), aRemoveScope );
							}
							g_UIElements->EndChild( );
						}
						ImGui::EndGroup( );
					}
					if ( m_iSelectedSubTab[ 4 ] == 1 )
					{
						ImGui::BeginGroup( );
						{
							g_UIElements->BeginChild( _S( "Section 55" ), { 290, 200 } );
							{
								g_UIElements->Checkbox( _S( "World Grenades" ), _S( "world.show_world_grenades" ) );
								ImGui::ColorEdit4( _S( "Flashbang Color" ), &g_SettingsManager->IC[ _S( "world.grenades_flashbang" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
								ImGui::ColorEdit4( _S( "HE Grenade Color" ), &g_SettingsManager->IC[ _S( "world.grenades_hegrenade" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
								ImGui::ColorEdit4( _S( "Molotov Color" ), &g_SettingsManager->IC[ _S( "world.grenades_molotov" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
								ImGui::ColorEdit4( _S( "Inc Color" ), &g_SettingsManager->IC[ _S( "world.grenades_inc" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
								ImGui::ColorEdit4( _S( "Decoy Color" ), &g_SettingsManager->IC[ _S( "world.grenades_decoy" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
								ImGui::ColorEdit4( _S( "Smoke Color" ), &g_SettingsManager->IC[ _S( "world.grenades_smoke" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
							}
							g_UIElements->EndChild( );

							g_UIElements->BeginChild( _S( "Section12" ), { 290, 40 } );
							{
								g_UIElements->Checkbox( _S( "Grenade Warning" ), _S( "misc.grenade_warning" ) );
							}
							g_UIElements->EndChild( );
						}
						ImGui::EndGroup( );
						ImGui::SameLine( );
						ImGui::BeginGroup( );
						{
							g_UIElements->BeginChild( _S( "Section 4" ), { 290, 40 } );
							{
								g_UIElements->Checkbox( _S( "Penetration Crosshair" ), _S( "misc.penetration_crosshire" ) );
							}
							g_UIElements->EndChild( );

							g_UIElements->BeginChild( _S( "Section 51235" ), { 290, 70 } );
							{
								g_UIElements->Checkbox( _S( "Out of view Arrows" ), _S( "misc.out_of_view" ) );
								ImGui::ColorEdit4( _S( "Color" ), &g_SettingsManager->IC[ _S( "misc.out_of_view" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
							}
							g_UIElements->EndChild( );
						}
						ImGui::EndGroup( );
					}
				}
				ImGui::EndChild( );
			}
			else if ( m_iSelectedTab == 6 )
			{
				ImGui::SetCursorPos( { 40, 90 } );
				ImGui::BeginGroup( );
				{
					g_UIElements->Subtab( _S( "Main" ), m_iSelectedSubTab[ 6 ], 0 );
					g_UIElements->SameLineWithoutPadding( );
					g_UIElements->Subtab( _S( "Events" ), m_iSelectedSubTab[ 6 ], 3 );
					g_UIElements->SameLineWithoutPadding( );
					g_UIElements->Subtab( _S( "Visual" ), m_iSelectedSubTab[ 6 ], 1 );
					g_UIElements->SameLineWithoutPadding( );
					g_UIElements->Subtab( _S( "Head-Up Display" ), m_iSelectedSubTab[ 6 ], 2 );
				}
				ImGui::EndGroup( );

				ImGui::SetCursorPos( { 50, 135 } );
				ImGui::BeginChild( _S( "##mainframe" ), { size.x - 50, 455 } );
				{
					if ( m_iSelectedSubTab[ 6 ] == 0 )
					{
						ImGui::BeginGroup( );
						{
							g_UIElements->BeginChild( _S( "Section 1" ), { 290, 121 } );
							{
								g_UIElements->Checkbox( _S( "Auto-Jump" ), _S( "misc.auto_jump" ) );
								g_UIElements->Checkbox( _S( "Auto-Strafe" ), _S( "misc.auto_strafe" ) );
								g_UIElements->Checkbox( _S( "Fast stop" ), _S( "misc.quick_stop" ) );
								g_UIElements->Checkbox( _S( "Edge-Jump" ), _S( "misc.edge_jump" ) );
							}
							g_UIElements->EndChild( );

							g_UIElements->BeginChild( _S( "Section 5" ), { 290, 92 } );
							{
								g_UIElements->Checkbox( _S( "Auto-Peek" ), _S( "misc.auto_pick" ) );

								std::vector<std::string> Vars = {
									_S( "misc.auto_pick_button" ), _S( "misc.auto_pick_on_shot" ),
								};

								std::vector<std::string> Names = {
									_S( "Retreat on WASD down" ), _S( "Retread on shot" ),
								};

								g_UIElements->MultiCombo( _S( "Auto Peek Modes" ), Vars, Names );
							}
							g_UIElements->EndChild( );
						}
						ImGui::EndGroup( );
						ImGui::SameLine( );
						ImGui::BeginGroup( );
						{
							g_UIElements->BeginChild( _S( "Section 4" ), { 290, 65 } );
							{
								g_UIElements->Checkbox( _S( "Fake Duck" ), _S( "misc.fake_duck" ) );
								g_UIElements->Checkbox( _S( "Infinity Duck" ), _S( "misc.infinity_duck" ) );
							}
							g_UIElements->EndChild( );

							g_UIElements->BeginChild( _S( "Section 3" ), { 290, 92 } );
							{
								g_UIElements->Checkbox( _S( "Unlock hidden convars" ), _S( "misc.hidden_convars" ) );
								g_UIElements->Checkbox( _S( "Unlock inventory access" ), _S( "misc.unlock_inventory_access" ) );
								g_UIElements->Checkbox( _S( "Filter console" ), _S( "misc.filter_console" ) );
							}
							g_UIElements->EndChild( );
						}
						ImGui::EndGroup( );
					}
					if ( m_iSelectedSubTab[ 6 ] == 1 )
					{
						ImGui::BeginGroup( );
						{
							g_UIElements->BeginChild( _S( "Section 3" ), { 290, 80 } );
							{
								g_UIElements->Checkbox( _S( "Thirdperson" ), _S( "misc.third_person" ) );
								g_UIElements->SliderInt( _S( "Thirdperson Distance" ), _S( "misc.third_person_distance" ), 100, 250 );
							}
							g_UIElements->EndChild( );

							g_UIElements->BeginChild( _S( "Section 5" ), { 290, 65 } );
							{
								g_UIElements->Checkbox( _S( "Adblock" ), _S( "misc.ad_block" ) );
								g_UIElements->Checkbox( _S( "Preserve Killfeed" ), _S( "misc.preserve_killfeed" ) );

							}
							g_UIElements->EndChild( );

							g_UIElements->BeginChild( _S( "Section 6" ), { 290, 50 } );
							{
								g_UIElements->SliderFloat( _S( "Aspect Ratio" ), _S( "misc.aspect_ratio" ), 0.f, 2.5f, "%0.2f" );
							}
							g_UIElements->EndChild( );

							g_UIElements->BeginChild( _S( "Section 9" ), { 290, 200 } );
							{
								g_UIElements->Checkbox( _S( "Draw Client Impacts" ), _S( "misc.client_impacts" ) );
								ImGui::ColorEdit4( _S( "Client impacts" ), &g_SettingsManager->IC[ _S( "misc.client_impacts" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
								g_UIElements->Checkbox( _S( "Draw Server Impacts" ), _S( "misc.server_impacts" ) );
								ImGui::ColorEdit4( _S( "Server impacts" ), &g_SettingsManager->IC[ _S( "misc.server_impacts" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
								g_UIElements->SliderFloat( _S( "Client Impacts Duration" ), _S( "misc.client_impacts_duration" ), 0.0f, 7.0f, "%0.1f" );
								g_UIElements->SliderFloat( _S( "Server Impacts Duration" ), _S( "misc.server_impacts_duration" ), 0.0f, 7.0f, "%0.1f" );
							}
							g_UIElements->EndChild( );
						}
						ImGui::EndGroup( );
						ImGui::SameLine( );
						ImGui::BeginGroup( );
						{
							g_UIElements->BeginChild( _S( "Section 2" ), { 290, 110 } );
							{
								g_UIElements->Checkbox( _S( "Visual interpolation" ), _S( "misc.visual_interpolation" ) );
								g_UIElements->Checkbox( _S( "Visual interpolation dead only" ), _S( "misc.visual_interpolation_dead_only" ) );
								g_UIElements->SliderInt( _S( "Interpolation value" ), _S( "misc.visual_interpolation_amount" ), 1, 14 );
							}
							g_UIElements->EndChild( );

							g_UIElements->BeginChild( _S( "Section 22" ), { 290, 160 } );
							{
								g_UIElements->SliderInt( _S( "Viewmodel FOV" ), _S( "misc.viewmodel.fov" ), 60, 120 );
								g_UIElements->SliderFloat( _S( "Viewmodel X" ), _S( "misc.viewmodel.x" ), -10.0f, 10.0f, "%0.1f" );
								g_UIElements->SliderFloat( _S( "Viewmodel Y" ), _S( "misc.viewmodel.y" ), -10.0f, 10.0f, "%0.1f" );
								g_UIElements->SliderFloat( _S( "Viewmodel Z" ), _S( "misc.viewmodel.z" ), -10.0f, 10.0f, "%0.1f" );
							}
							g_UIElements->EndChild( );

							g_UIElements->BeginChild( _S( "Section 7" ), { 290, 80 } );
							{
								g_UIElements->SliderInt( _S( "Camera FOV" ), _S( "misc.camerafov" ), 90, 140 );
								g_UIElements->Checkbox( _S( "Override while scoped" ), _S( "misc.change_fov_while_scoped" ) );
							}
							g_UIElements->EndChild( );

							g_UIElements->BeginChild( _S( "Section 65" ), { 290, 65 } );
							{
								g_UIElements->Checkbox( _S( "Shared ESP" ), _S( "misc.shared_esp" ) );
								g_UIElements->Checkbox( _S( "Shared Chams" ), _S( "misc.shared_chams" ) );
							}
							g_UIElements->EndChild( );
						}
						ImGui::EndGroup( );
					}
					if ( m_iSelectedSubTab[ 6 ] == 2 )
					{
						ImGui::BeginGroup( );
						{
							g_UIElements->BeginChild( _S( "Section 1" ), { 290, 40 } );
							{
								g_UIElements->Checkbox( _S( "Enable" ), _S( "ui.custom_hud" ) );
							}
							g_UIElements->EndChild( );

							g_UIElements->BeginChild( _S( "Section 4" ), { 290, 40 } );
							{
								g_UIElements->Checkbox( _S( "Radar" ), _S( "misc.radar.enable" ) );
							}
							g_UIElements->EndChild( );

							g_UIElements->BeginChild( _S( "Section 3" ), { 290, 150 } );
							{
								ImGui::ColorEdit3( _S( "Crosshire Color" ), &g_SettingsManager->IC[ _S( "misc.hud.crosshire_color" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
								g_UIElements->SliderInt( _S( "Line Size" ), _S( "misc.hud.crosshire_line_size" ), 1, 3 );
								g_UIElements->SliderInt( _S( "Gap Size" ), _S( "misc.hud.crosshire_gap_size" ), 0, 20 );
								g_UIElements->SliderInt( _S( "Width Size" ), _S( "misc.hud.crosshire_width_size" ), 0, 25 );
							}
							g_UIElements->EndChild( );
						}
						ImGui::EndGroup( );
						ImGui::SameLine( );
						ImGui::BeginGroup( );
						{
							g_UIElements->BeginChild( _S( "Section 22" ), { 290, 210 } );
							{
								g_UIElements->Checkbox( _S( "Clear kill feed on round start" ), _S( "misc.hud.clear_kill_feed_on_round" ) );

								g_UIElements->SliderInt( _S( "Kill feed time" ), _S( "misc.hud.kill_list_time" ), 3000, 15000 );
								g_UIElements->Checkbox( _S( "Colorize by team" ), _S( "misc.hud.kill_list_colorize" ) );

								ImGui::ColorEdit3( _S( "Terrorist Color" ), &g_SettingsManager->IC[ _S( "misc.hud.kill_terrorist" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
								ImGui::ColorEdit3( _S( "Counter-Terrorist Color" ), &g_SettingsManager->IC[ _S( "misc.hud.kill_counter_terrorist" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
								ImGui::ColorEdit3( _S( "Text Color" ), &g_SettingsManager->IC[ _S( "misc.hud.kill_list_color" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
								ImGui::ColorEdit3( _S( "Headshot Icon Color" ), &g_SettingsManager->IC[ _S( "misc.hud.kill_list_headshot" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
							}
							g_UIElements->EndChild( );
						}
						ImGui::EndGroup( );
					}
					if ( m_iSelectedSubTab[ 6 ] == 3 )
					{
						ImGui::BeginGroup( );
						{
							std::vector < std::string > aPrimaryWeapon
								=
							{
								_S( "None" ),
								_S( "SCAR20/G3SG1" ),
								_S( "SSG08" ),
								_S( "AWP" ),
								_S( "AK-47/M4A1(-S)" ),
								_S( "P250" )
							};

							std::vector < std::string >  aSecondaryWeapon
								=
							{
								_S( "None" ),
								_S( "FN57/TEC9" ),
								_S( "Dual Elites" ),
								_S( "Deagle/Revolver" ),
								_S( "USP-S/Glock-18" ),
								_S( "P250" ),
							};


							g_UIElements->BeginChild( "Section 3", { 290, 225 } );
							{
								g_UIElements->Checkbox( _S( "Buy Bot" ), _S( "misc.buybot.enable" ) );
								g_UIElements->Combo( _S( "Primary Weapon" ), _S( "misc.buybot.primary" ), aPrimaryWeapon );
								g_UIElements->Checkbox( _S( "Override AWP" ), _S( "misc.buybot.keep_awp" ) );
								g_UIElements->Combo( _S( "Secondary Weapon" ), _S( "misc.buybot.secondary" ), aSecondaryWeapon );

								std::vector < std::string > aEquipment = {
									_S( "Fire grenade/Molotov" ), _S( "Smoke grenade" ),
									_S( "Explosive grenade" ), _S( "Taser" ),
									_S( "Heavy armor" ), _S( "Helmet" ),
									_S( "Defuser" )
								};

								std::vector < std::string > aEquipmentVar = {
									_S( "misc.buybot.molotov" ), _S( "misc.buybot.smokegrenade" ),
									_S( "misc.buybot.hegrenade" ), _S( "misc.buybot.taser" ),
									_S( "misc.buybot.heavyarmor" ), _S( "misc.buybot.vesthelm" ),
									_S( "misc.buybot.defuser" )
								};

								g_UIElements->MultiCombo( _S( "Equipment" ), aEquipmentVar, aEquipment );
							}
							g_UIElements->EndChild( );
						}
						ImGui::EndGroup( );
						ImGui::SameLine( );
						ImGui::BeginGroup( );
						{
							g_UIElements->BeginChild( "Section53", { 290, 180 } );
							{
								std::vector < std::string > aHitMarkers = {
									_S( "World" ),
									_S( "Crosshair" )
								};

								std::vector < std::string > aHitMarkersVar = {
									_S( "misc.hitmarker_world" ),
									_S( "misc.hitmarker_crosshair" )
								};

								std::vector < std::string > aHitSounds = {
									_S( "Off" ),
									_S( "Arena" ),
									_S( "Bubble" ),
									_S( "Bell" )
								};

								g_UIElements->MultiCombo( _S( "Hitmarker" ), aHitMarkersVar, aHitMarkers );
								g_UIElements->Combo( _S( "Hitsound" ), _S( "misc.hit_sound" ), aHitSounds );
								g_UIElements->SliderInt( _S( "Hitsound volume" ), _S( "misc.hit_sound_volume" ), 1, 100 );
							}
							g_UIElements->EndChild( );
						}
						ImGui::EndGroup( );
					}
				}
				ImGui::EndChild( );
			}
			else if ( m_iSelectedTab == 7 )
			{
				ImGui::SetCursorPos( { 40, 90 } );
				ImGui::BeginGroup( );
				{
					g_UIElements->Subtab( _S( "My Configurations" ), m_iSelectedSubTab[ 7 ], 0 );
					g_UIElements->SameLineWithoutPadding( );
					g_UIElements->Subtab( _S( "Marketplace Configurations" ), m_iSelectedSubTab[ 7 ], 1 );
				}
				ImGui::EndGroup( );

				ImGui::SetCursorPos( { 50, 135 } );
				ImGui::BeginChild( _S( "##mainframe" ), { size.x - 50, size.y - 200 } );
				{
					if ( g_CloudConfigs->CloudAction == CloudActions_t::None ) {
						if ( m_iSelectedSubTab[ 7 ] == 0 ) {
							for ( auto config : g_CloudConfigs->m_vConfigs )
								if ( config.Market == false )
									if ( config.Name.find( std::string( g_Menu->m_cSearch ) ) != std::string::npos )
										g_UIElements->Configuration( config );
						}
						else if ( m_iSelectedSubTab[ 7 ] == 1 ) {
							for ( auto config : g_CloudConfigs->m_vConfigs )
								if ( config.Market == true )
									if ( config.Name.find( std::string( g_Menu->m_cSearch ) ) != std::string::npos )
										g_UIElements->Configuration( config );
						}
					}
					else {
						g_UIElements->Spinner( _S( "Fetching Data" ), 40, 4, ImColor( 255, 255, 255 ), pos + ImVec2 { 310, 295 } );
					}
				}
				ImGui::EndChild( );

				draw->AddRectFilled( pos + ImVec2( 40, size.y - 55 ), pos + ImVec2( size.x, size.y - 20 ), ImColor( 38, 38, 38 ) );

				draw->AddRectFilled( pos + ImVec2( size.x - 60, size.y - 55 ), pos + ImVec2( size.x, size.y - 20 ), ImColor( 0, 153, 255 ) );
				draw->AddText( pos + ImVec2( size.x - 60 + ( 30 - ImGui::CalcTextSize( _S( "Create" ) ).x / 2 ), size.y - 55 + ( 17 - ImGui::CalcTextSize( _S( "Create" ) ).y / 2 ) ), ImColor( 255, 255, 255 ), _S( "Create" ) );

				ImGui::SetCursorPos( { size.x - 60, size.y - 55 } );
				if ( ImGui::InvisibleButton( _S( "Create" ), { 60, 35 } ) )
					g_CloudConfigs->CloudAction = CloudActions_t::CreateConfig;

				ImGui::PushItemWidth( size.x - 48 - 120 );
				ImGui::SetCursorPos( { 48, size.y - 55 + 7 } );
				ImGui::InputTextWithHint( _S( "##configbar" ), _S( "Type your config name or share code here..." ), g_CloudConfigs->aConfigName, 64 );
			}
			/*else if ( m_iSelectedTab == 8 )
			{
				ImGui::SetCursorPos( { 40, 90 } );
				ImGui::BeginGroup( );
				{
					g_UIElements->Subtab( _S( "My Scripts" ), m_iSelectedSubTab[ 8 ], 0 );
					g_UIElements->SameLineWithoutPadding( );
					g_UIElements->Subtab( _S( "Marketplace Scripts" ), m_iSelectedSubTab[ 8 ], 1 );
					g_UIElements->SameLineWithoutPadding( );
					g_UIElements->Subtab( _S( "Editor" ), m_iSelectedSubTab[ 8 ], 2 );
				}
				ImGui::EndGroup( );

				ImGui::SetCursorPos( { 50, 135 } );
				ImGui::BeginChild( _S( "##mainframe" ), { size.x - 50, size.y - 200 } );
				{
					if ( g_CloudScripts->CloudAction == CloudScriptAction_t::None ) {
						if ( m_iSelectedSubTab[ 8 ] == 0 ) {
							for ( auto script : g_CloudScripts->m_vScripts )
								if ( script.Market == false )
									if ( script.Name.find( std::string( g_Menu->m_cSearch ) ) != std::string::npos )
										g_UIElements->Script( script );
						}
						else if ( m_iSelectedSubTab[ 8 ] == 1 ) {
							for ( auto script : g_CloudScripts->m_vScripts )
								if ( script.Market == true )
									if ( script.Name.find( std::string( g_Menu->m_cSearch ) ) != std::string::npos )
										g_UIElements->Script( script );
						}
					}
					else {
						g_UIElements->Spinner( _S( "Fetching Data" ), 40, 4, ImColor( 255, 255, 255 ), pos + ImVec2 { 310, 295 } );
					}
				}
				ImGui::EndChild( );

				draw->AddRectFilled( pos + ImVec2( 40, size.y - 55 ), pos + ImVec2( size.x, size.y - 20 ), ImColor( 38, 38, 38 ) );

				draw->AddRectFilled( pos + ImVec2( size.x - 60, size.y - 55 ), pos + ImVec2( size.x, size.y - 20 ), ImColor( 0, 153, 255 ) );
				draw->AddText( pos + ImVec2( size.x - 60 + ( 30 - ImGui::CalcTextSize( _S( "Create" ) ).x / 2 ), size.y - 55 + ( 17 - ImGui::CalcTextSize( _S( "Create" ) ).y / 2 ) ), ImColor( 255, 255, 255 ), _S( "Create" ) );

				ImGui::SetCursorPos( { size.x - 60, size.y - 55 } );
				if ( ImGui::InvisibleButton( _S( "Create" ), { 60, 35 } ) )
					g_CloudScripts->CloudAction = CloudScriptAction_t::CreateScript;

				ImGui::PushItemWidth( size.x - 48 - 120 );
				ImGui::SetCursorPos( { 48, size.y - 55 + 7 } );
				ImGui::InputTextWithHint( _S( "##configbar" ), _S( "Type your script name here..." ), g_CloudScripts->aConfigName, 64 );
			}*/
			else if ( m_iSelectedTab == 9 )
			{
				ImGui::SetCursorPos( { 40, 90 } );
				ImGui::BeginGroup( );
				{
					g_UIElements->Subtab( _S( "About" ), m_iSelectedSubTab[ 9 ], 0 );
					//g_UIElements->SameLineWithoutPadding( );
					//g_UIElements->Subtab( _S( "Menu Settings" ), m_iSelectedSubTab[ 9 ], 1 );
				}
				ImGui::EndGroup( );

				ImGui::SetCursorPos( { 50, 135 } );
				ImGui::BeginChild( _S( "##mainframe" ), { size.x - 50, size.y - 175 } );
				{
					ImGui::GetWindowDrawList( )->AddText( m_sFonts.Default24, 24, ImGui::GetWindowPos( ) + ImVec2( 15, 15 ), ImColor( 190, 190, 190 ), "Oversee.one" );
					ImGui::GetWindowDrawList( )->AddText( m_sFonts.Default14, 14, ImGui::GetWindowPos( ) + ImVec2( 15, 45 ), ImColor( 190, 190, 190 ), "Modification for Counter-Strike: Global Offensive" );
					ImGui::GetWindowDrawList( )->AddText( m_sFonts.Default14, 14, ImGui::GetWindowPos( ) + ImVec2( 15, 63 ), ImColor( 190, 190, 190 ), "Client is bound to user pers0na2" );

					ImGui::GetWindowDrawList( )->AddText( m_sFonts.Default14, 14, ImGui::GetWindowPos( ) + ImVec2( 15, 100 ), ImColor( 190, 190, 190 ), "All functionality is presented \"as is\".\nAny changes to the binary file are prohibited by the rules of the service.\n\nVersion 1.00.00\n\n2022 Oversee One. All Rights Reserved." );

				}
				ImGui::EndChild( );
			}
			else if ( m_iSelectedTab == 5 )
			{
				static int selectedPage = 0;
				static int selectedAgentTeam = 0;
				static int16_t selectedWeapon = 0;
				static WeaponItem_t* selectedWeaponSkin;

				if(!g_ItemManager.Initialized )
					g_UIElements->Spinner( _S("ItemsParsing"), 5, 2, ImColor(200,200,200), pos + ImVec2(240, 60) );

				ImGui::SetCursorPos( { 40, 90 } );
				ImGui::BeginGroup( );
				{
					g_UIElements->Subtab( _S( "My Inventory" ), m_iSelectedSubTab[ 5 ], 0 );
					if ( selectedPage != 0 ) {
						g_UIElements->SameLineWithoutPadding( );
						g_UIElements->Subtab( _S( "Back" ), m_iSelectedSubTab[ 5 ], 1 );
						if ( ImGui::IsItemClicked( ) ) {
							selectedPage = 0;
							selectedWeapon = 0;
						}
					}
				}
				ImGui::EndGroup( );

				ImGui::SetCursorPos( { 50, 135 } );
				ImGui::BeginChild( _S( "##mainframess" ), { size.x - 50, 430 } );
				{
					static InventoryItem_t inventory_list;

					if ( selectedPage == 0 )
					{
						if ( ImGui::Button( "Add New Item", { 140, 142 } ) )
							selectedPage = 1;
						ImGui::SameLine( 148 );

						int padding = 1;
						for ( const auto& [key, value] : g_SettingsManager->m_aInventoryItemList ) {
							padding = 1 + padding;

							g_UIElements->InventoryItem( value );

							if ( padding * 148 > ImGui::GetWindowSize( ).x - 148 )
								ImGui::NewLine( ), padding = 0;
							else
								ImGui::SameLine( padding * 148 );
						}
					}
					if ( selectedPage == 1 )
					{
						int padding = 0;
						for ( auto i = 0; i < g_ItemManager.m_vAgents.size( ); i++ ) {
							if ( i < 2 ) {
								padding = 1 + padding;
								if ( g_UIElements->InventoryItemAgent( g_ItemManager.m_vAgents[ i ] ) )
									selectedPage = 5, selectedAgentTeam = i;

								if ( padding * 148 > ImGui::GetWindowSize( ).x - 148 )
									ImGui::NewLine( ), padding = 0;
								else
									ImGui::SameLine( padding * 148 );
							}
						}

						ImGui::NewLine( );
						ImGui::NewLine( );
						ImGui::Separator( );
						ImGui::NewLine( );
						int padding2 = 0;

						for ( const auto& weapon : g_ItemManager.m_vDefaults ) {
							padding2 = 1 + padding2;

							if ( g_UIElements->InventoryItemDefault( weapon ) )
								selectedWeapon = weapon->m_nItemID, selectedPage = 2;

							if ( padding2 * 148 > ImGui::GetWindowSize( ).x - 148 )
								ImGui::NewLine( ), padding2 = 0;
							else
								ImGui::SameLine( padding2 * 148 );
						}

						ImGui::NewLine( );
						ImGui::NewLine( );
						ImGui::Separator( );
						ImGui::NewLine( );
						int padding3 = 0;

						for ( auto i = 0; i < g_ItemManager.m_vGloves.size( ); i++ ) {
							padding3 = 1 + padding3;
							if ( g_UIElements->InventoryItemGlove( g_ItemManager.m_vGloves[ i ] ) ) {
								inventory_list.m_nItemID = g_ItemManager.m_vGloves[ i ]->m_nItemID;
								inventory_list.m_iPaintKit = g_ItemManager.m_vGloves[ i ]->m_nKitID;
								inventory_list.SkinName = LocalizeTex2( g_ItemManager.m_vGloves[ i ]->m_szName.c_str( ) );
								g_InventoryChanger->AddItemToInventory( SDK::Interfaces::InventoryManager->GetLocalPlayerInventory( ), inventory_list );
								inventory_list = InventoryItem_t( );
								selectedPage = 0;
							}

							if ( padding3 * 148 > ImGui::GetWindowSize( ).x - 148 )
								ImGui::NewLine( ), padding3 = 0;
							else
								ImGui::SameLine( padding3 * 148 );
						}
					}
					if ( selectedPage == 5 )
					{
						int padding = 0;
						for ( auto i = 2; i < g_ItemManager.m_vAgents.size( ); i++ ) {
							if ( g_ItemManager.m_vAgents[ i ]->m_iTeamID == selectedAgentTeam ) {
								padding = 1 + padding;
								if ( g_UIElements->InventoryItemAgent( g_ItemManager.m_vAgents[ i ] ) ) {
									inventory_list.m_nItemID = g_ItemManager.m_vAgents[ i ]->m_nItemID;
									inventory_list.m_iPaintKit = 0;
									inventory_list.m_iRarity = 0;
									inventory_list.m_flPearlescent = 0;
									inventory_list.ct_team = selectedAgentTeam;
									inventory_list.SkinName = LocalizeTex2( g_ItemManager.m_vAgents[ i ]->m_szName.c_str( ) );
									g_InventoryChanger->AddItemToInventory( SDK::Interfaces::InventoryManager->GetLocalPlayerInventory( ), inventory_list );
									inventory_list = InventoryItem_t( );
									selectedPage = 0;
								}


								if ( padding * 148 > ImGui::GetWindowSize( ).x - 148 )
									ImGui::NewLine( ), padding = 0;
								else
									ImGui::SameLine( padding * 148 );
							}
						}
					}
					if ( selectedPage == 2 )
					{
						int padding = 0;
						for ( const auto& weapon : g_ItemManager.m_vWeapons ) {
							if ( weapon->m_tItem->m_nID != selectedWeapon )
								continue;

							padding = 1 + padding;

							if ( g_UIElements->InventoryItem( weapon ) ) {
								selectedWeaponSkin = weapon;
								selectedPage = 3;
								for ( size_t i = 0; i < 4; i++ )
									inventory_list.color[ i ][ 0 ] = float_t( weapon->m_tPaintKit->m_cColor[ i ].r( ) ) / 255.f, inventory_list.color[ i ][ 1 ] = float_t( weapon->m_tPaintKit->m_cColor[ i ].g( ) ) / 255.f, inventory_list.color[ i ][ 2 ] = float_t( weapon->m_tPaintKit->m_cColor[ i ].b( ) ) / 255.f;
							}

							if ( padding * 148 > ImGui::GetWindowSize( ).x - 148 )
								ImGui::NewLine( ), padding = 0;
							else
								ImGui::SameLine( padding * 148 );
						}
					}
					if ( selectedPage == 3 )
					{
						static ImVec2 CurrentPos = ImGui::GetCursorPos( );

						g_UIElements->BeginChild( _S( "preview page" ), { 290, 167 }, false );
						{
							ImGuiWindow* window { ImGui::GetCurrentWindow( ) };
							window->DrawList->AddImage( selectedWeaponSkin->m_tTexture, { window->Pos.x + 18, window->Pos.y + 15.f }, { window->Pos.x + 245, window->Pos.y + 157 } );
						}
						g_UIElements->EndChild( );
						ImGui::SameLine( );
						g_UIElements->BeginChild( _S( "Section 2" ), { 290, 333 } );
						{
							ImGui::BeginGroup( );
							ImGui::ColorEdit3( _S( "Paint Kit Color First" ), inventory_list.color[ 0 ], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop );
							ImGui::SameLine( 25 );
							ImGui::ColorEdit3( _S( "Paint Kit Color Second" ), inventory_list.color[ 1 ], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop );
							ImGui::SameLine( 50 );
							ImGui::ColorEdit3( _S( "Paint Kit Color Third" ), inventory_list.color[ 2 ], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop );
							ImGui::SameLine( 75 );
							ImGui::ColorEdit3( _S( "Paint Kit Color Fourth" ), inventory_list.color[ 3 ], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop );
							ImGui::EndGroup( );

							if ( ImGui::Button( _S( "Add to inventory" ), ImVec2( 270, 25 ) ) )
							{
								inventory_list.m_nItemID = selectedWeaponSkin->m_tItem->m_nID;
								inventory_list.m_iPaintKit = selectedWeaponSkin->m_tPaintKit->m_nID;
								inventory_list.m_iRarity = selectedWeaponSkin->m_tPaintKit->m_nRarity;
								inventory_list.SkinName = LocalizeTex2( selectedWeaponSkin->m_tPaintKit->m_nDescriptionTag ).c_str( );
								inventory_list.m_flPearlescent = 0;
								g_InventoryChanger->AddItemToInventory( SDK::Interfaces::InventoryManager->GetLocalPlayerInventory( ), inventory_list );
								inventory_list = InventoryItem_t( );
								selectedPage = 0;
							}
						}
						g_UIElements->EndChild( );
					}
				}
				ImGui::EndChild( );
			}
		}
		ImGui::End( );

		if ( m_iSelectedTab == 3 )
		{
			ImGui::PushStyleColor( ImGuiCol_WindowBg, { 0.1f, 0.1f, 0.1f, 0.6f * nToggleValue } );
			ImGui::Begin( _S( "HUD" ), nullptr, nWindowFlags | ImGuiWindowFlags_NoMove );
			{
				ImGui::SetWindowPos( pos + ImVec2( 660, 0 ) );
				ImGui::SetWindowSize( { 375, 600 } );

				auto draw = ImGui::GetWindowDrawList( );
				auto size = ImGui::GetWindowSize( );

				draw->AddRectFilled( pos + ImVec2( 660, 0 ), pos + ImVec2( 660, 0 ) + size, ImColor( 25, 25, 25 ), 8 );

				if ( g_DrawModel->GetTexture( ) != nullptr )
				{
					draw->AddImage(
						g_DrawModel->GetTexture( )->pTextureHandles[ 0 ]->lpRawTexture,
						ImGui::GetWindowPos( ) - ImVec2( 0, 60 ),
						ImGui::GetWindowPos( ) + ImVec2( g_DrawModel->GetTexture( )->GetActualWidth( ), g_DrawModel->GetTexture( )->GetActualHeight( ) ) - ImVec2( 0, 60 ),
						ImVec2( 0, 0 ), ImVec2( 1, 1 ),
						ImColor( 1.0f, 1.0f, 1.0f, 1.0f ) );

					g_DrawModel->RenderPreview( );
				}
			}
			ImGui::End( );
			ImGui::PopStyleColor( );
		}

#ifndef OVERSEE_DEV
	}
#endif
}

#include "Raw/WeaponIcon.h"
#include "../imgui_freetype.h"

void C_Menu::SetupData( )
{
	auto style = &ImGui::GetStyle( );

	style->AntiAliasedFill = 0;
	style->AntiAliasedLines = 0;
	style->AntiAliasedLinesUseTex = 0;

	static const ImWchar ranges[ ] = {
		0x0020, 0x00FF, 0x2000, 0x206F, 0x3000, 0x30FF, 0x31F0, 0x31FF, 0xFF00,
		0xFFEF, 0x4e00, 0x9FAF, 0x0400, 0x052F, 0x2DE0, 0x2DFF, 0xA640, 0xA69F, 0
	};

	m_sFonts.Default14 = ImGui::GetIO( ).Fonts->AddFontFromMemoryCompressedTTF( sfpro_compressed_data, sfpro_compressed_size, 14, nullptr, ranges );
	m_sFonts.Default24 = ImGui::GetIO( ).Fonts->AddFontFromMemoryCompressedTTF( sfpro_compressed_data, sfpro_compressed_size, 24, nullptr, ranges );
	m_sFonts.MenuIcons = ImGui::GetIO( ).Fonts->AddFontFromMemoryCompressedTTF( MenuIcons_compressed_data, MenuIcons_compressed_size, 16, nullptr, nullptr );
	ImFontConfig WeaponIconsConfig;
	WeaponIconsConfig.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_NoAutoHint;
	m_sFonts.WeapIcons = ImGui::GetIO( ).Fonts->AddFontFromMemoryCompressedBase85TTF( WeaponIcons_compressed_data_base85, 20, &WeaponIconsConfig );

	ImGuiFreeType::BuildFontAtlas( ImGui::GetIO( ).Fonts );

	// set default font
	ImGui::GetIO( ).FontDefault = m_sFonts.Default14;


	style->WindowMinSize = { 24, 24 };
	style->WindowPadding = { 0, 0 };
	style->WindowRounding = 8;
	style->WindowBorderSize = 0;

	style->PopupRounding = 4;
	style->PopupBorderSize = 1;

	style->ScrollbarSize = 7;

	style->GrabMinSize = 1;

	style->Colors[ ImGuiCol_ChildBg ] = ImColor { 45, 45, 45, 0 };
	style->Colors[ ImGuiCol_WindowBg ] = ImColor { 45, 45, 45 };
	style->Colors[ ImGuiCol_PopupBg ] = ImColor { 55, 55, 55 };
	style->Colors[ ImGuiCol_ScrollbarBg ] = ImColor { 0, 0, 0, 0 };

	style->Colors[ ImGuiCol_Header ] = ImColor { 85, 85, 85, 85 };
	style->Colors[ ImGuiCol_HeaderActive ] = ImColor { 76, 76, 76 };
	style->Colors[ ImGuiCol_HeaderHovered ] = ImColor { 66, 66, 66 };

	ImTricks::Animations::SetupData( );
}