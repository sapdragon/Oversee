#include "Radar.hpp"

#include <d3dx9.h>
#pragma comment( lib, "d3dx9.lib" )

void C_Radar::Instance( )
{
	if ( !g_SettingsManager->B[ _S( "misc.radar.enable" ) ] )
		return;

	if ( !m_pRadarTexture )
		return;

	bool bTrueRadar = SDK::Interfaces::EngineClient->IsInGame( ) && SDK::Interfaces::EngineClient->IsConnected( );
	if ( !bTrueRadar )
		return;

	ImGui::SetNextWindowSize( { 170.f, 170.f }, ImGuiCond_Once );
	ImGui::SetNextWindowPos( { 10.f, 10.f } );
	ImGui::Begin( "##radar", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize );

	// Vars
	int x = ImGui::GetWindowPos( ).x;
	int y = ImGui::GetWindowPos( ).y;
	int size = ImGui::GetWindowSize( ).x;
	int center_size = ( size / 2 );

	// Limit radar bounds
	ImGui::GetWindowDrawList( )->PushClipRect( ImVec2( x, y ), ImVec2( x + size, y + size ), false );

	// Scale map
	float new_size = size;
	float size_diff = new_size - size;
	size = new_size;
	x -= size_diff / 2;
	y -= size_diff / 2;

	// Locate map by our local player
	auto local_player = g_Globals->m_LocalPlayer;


	// Prepare imgui shit

	ImVec2 uv_a( 0, 0 ), uv_c( 1, 1 ), uv_b( uv_c.x, uv_a.y ), uv_d( uv_a.x, uv_c.y );
	ImVec2 a( x, y ), c( x + size, y + size ), b( c.x, a.y ), d( a.x, c.y );


	// Render background first
	ImGui::GetWindowDrawList( )->AddRectFilledMultiColor( ImGui::GetWindowPos( ), ImVec2( ImGui::GetWindowPos( ).x + 170, ImGui::GetWindowPos( ).y + 170 ),
		ImColor( 45, 45, 45, 180 ), ImColor( 45, 45, 45, 180 ),
		ImColor( 45, 45, 45, 50 ), ImColor( 45, 45, 45, 50 ) );

	if ( m_bLoaded ) {
		ImGui::GetWindowDrawList( )->PushTextureID( m_pRadarTexture );
		ImGui::GetWindowDrawList( )->PrimReserve( 6, 4 );
		ImGui::GetWindowDrawList( )->PrimQuadUV( a, b, c, d, uv_a, uv_b, uv_c, uv_d, 0xFFFFFFFF );
		ImGui::GetWindowDrawList( )->PopTextureID( );
	}

	// Remove limit
	ImGui::GetWindowDrawList( )->PopClipRect( );

	if ( local_player )
	{
		const int max = SDK::Interfaces::EntityList->GetHighestEntityIndex( );
		for ( auto i = 1; i <= max; i++ )
		{
			auto e = static_cast< C_BaseEntity* >( SDK::Interfaces::EntityList->GetClientEntity( i ) );

			if ( !e )
				continue;

			if ( e->IsPlayer( ) )
				continue;

			if ( e->IsDormant( ) )
				continue;

			auto client_class = e->GetClientClass( );

			if ( !client_class )
				continue;

			if ( client_class->m_ClassID == ClassId_CC4 && g_SettingsManager->B[ _S( "misc.radar.item_1" ) ] )
			{
				auto owner = ( C_BasePlayer* ) SDK::Interfaces::EntityList->GetClientEntityFromHandle( e->m_nOwnerEntity( ) );

				if ( !owner->IsPlayer( ) )
				{
					Vector bomb_pos;

					bomb_pos = e->GetAbsOrigin( );
					bomb_pos = this->WorldToRadar( bomb_pos );

					ImGui::GetWindowDrawList( )->AddText( ImVec2( bomb_pos.x - 1, bomb_pos.y ), ImColor( 225, 165, 25 ), "C4" );
				}
			}
			else if ( client_class->m_ClassID == ClassId_CC4 && g_SettingsManager->B[ _S( "misc.radar.item_2" ) ] )
			{
				Vector bomb_pos;

				bomb_pos = e->GetAbsOrigin( );
				bomb_pos = this->WorldToRadar( bomb_pos );

				ImGui::GetWindowDrawList( )->AddRectFilled( ImVec2( bomb_pos.x - 2, bomb_pos.y - 2 ), ImVec2( bomb_pos.x + 1, bomb_pos.y + 2 ), ImGui::GetColorU32( ImVec4( 225 / 255.f, 165 / 255.f, 25 / 255.f, 1.f ) ) );
			}
		}

		if ( local_player->IsAlive( ) && g_SettingsManager->B[ _S( "misc.radar.item_3" ) ] )
		{
			Vector world_pos = local_player->GetAbsOrigin( );
			Vector map_pos = this->WorldToRadar( world_pos );

			ImGui::GetWindowDrawList( )->AddCircleFilled( ImVec2( map_pos.x, map_pos.y ), 5, ImColor( 30, 255, 30 ), 25 );
		}

		for ( auto i = 1; i < SDK::Interfaces::GlobalVars->m_iMaxClients; i++ )
		{
			auto e = ( C_BasePlayer* ) SDK::Interfaces::EntityList->GetClientEntity( i );

			if ( !e || !e->IsAlive( ) || !e->IsPlayer( ) || e->IsDormant( ) )
				continue;

			if ( e == local_player )
				continue;

			if ( e->m_iTeamNum( ) == local_player->m_iTeamNum( ) && g_SettingsManager->B[ _S( "misc.radar.item_4" ) ] )
				continue;

			if ( e->m_iTeamNum( ) != local_player->m_iTeamNum( ) && g_SettingsManager->B[ _S( "misc.radar.item_5" ) ] )
				continue;

			auto team_num = e->m_iTeamNum( );
			bool bIsEnemy = local_player->m_iTeamNum( ) != team_num;

			Vector EntityPos;

			//EntityPos = e->IsDormant() ? g_ctx.globals.dormant_origin[i] : e->GetAbsOrigin();
			EntityPos = this->WorldToRadar( EntityPos );

			ImColor clr = bIsEnemy ? ImColor( 255, 50, 50 ) : ( team_num == 3 ? ImColor( 95, 185, 245 ) : ImColor( 245, 205, 65 ) );
			//ImColor clr_dormant = bIsEnemy ? ImColor(255, 50, 50, (int)(playeresp::get().esp_alpha_fade[i] * 255.0f)) : (team_num == 2 ? ImColor(95, 185, 245) : ImColor(245, 205, 65));

			ImGui::GetWindowDrawList( )->AddCircleFilled( ImVec2( EntityPos.x, EntityPos.y ), 5, clr, 30 );

			if ( g_SettingsManager->B[ _S( "misc.radar.item_6" ) ] )
			{
				auto hp = std::clamp( ( float ) e->m_iHealth( ), 0.0f, 100.0f ) / 100.0f;

				if ( hp <= 0.0f )
					continue;

				auto height = std::clamp( ( float ) e->m_iHealth( ) / 20.0f, 2.0f, 5.0f );

				ImColor clr = ImColor( int( ( 1 - hp ) * 255 ), int( hp * 255 ), 0, 255 );
				ImGui::GetWindowDrawList( )->AddRectFilled( ImVec2( EntityPos.x - 4, EntityPos.y - height ), ImVec2( EntityPos.x - 2, EntityPos.y + height ), clr );
			}
		}

		if ( m_bDrawPlantA && !m_vecPosBombSiteA.IsZero( ) )
		{
			Vector plant_pos_A;
			plant_pos_A.x = m_vecPosBombSiteA.x * ImGui::GetWindowSize( ).x;
			plant_pos_A.x += ImGui::GetWindowPos( ).x;
			plant_pos_A.y = m_vecPosBombSiteA.y * ImGui::GetWindowSize( ).y;
			plant_pos_A.y += ImGui::GetWindowPos( ).y;

			ImGui::GetWindowDrawList( )->AddText( ImVec2( plant_pos_A.x, plant_pos_A.y ), ImColor( 225, 225, 25 ), "A" );
		}

		if ( m_bDrawPlantB && !m_vecPosBombSiteB.IsZero( ) )
		{
			Vector plant_pos_B;
			plant_pos_B.x = m_vecPosBombSiteB.x * ImGui::GetWindowSize( ).x;
			plant_pos_B.x += ImGui::GetWindowPos( ).x;
			plant_pos_B.y = m_vecPosBombSiteB.y * ImGui::GetWindowSize( ).y;
			plant_pos_B.y += ImGui::GetWindowPos( ).y;

			ImGui::GetWindowDrawList( )->AddText( ImVec2( plant_pos_B.x, plant_pos_B.y ), ImColor( 225, 225, 25 ), "B" );
		}
	}

	ImGui::End( );

}

Vector C_Radar::WorldToRadar( Vector& vecWorldPos )
{
	const int iMapCenter = 512;

	// World2Map
	Vector vMapPosition;

	vMapPosition.x = ( vecWorldPos.x - m_vecMapOrigin.x ) / m_flMapScale;
	vMapPosition.y = ( vecWorldPos.y - m_vecMapOrigin.y ) / -m_flMapScale;
	vMapPosition.z = 0;

	// Map2Panel
	Vector vOffset;
	vOffset.x = vMapPosition.x - iMapCenter;
	vOffset.y = vMapPosition.y - iMapCenter;
	vOffset.z = 0;

	// (m_fZoom * m_fFullZoom) / OVERVIEW_MAP_SIZE
	float fScale = 1.f / 1024;
	vOffset.x *= fScale;
	vOffset.y *= fScale;

	Vector vFinalPos;
	vFinalPos.x = ( ImGui::GetWindowSize( ).x * 0.5f ) + ( ImGui::GetWindowSize( ).x * vOffset.x );
	vFinalPos.y = ( ImGui::GetWindowSize( ).x * 0.5f ) + ( ImGui::GetWindowSize( ).x * vOffset.y );
	vFinalPos.z = 0;

	vMapPosition.x = vMapPosition.x / ( 1024.f / ImGui::GetWindowSize( ).x );
	vMapPosition.y = vMapPosition.y / ( 1024.f / ImGui::GetWindowSize( ).y );

	vMapPosition.x += ImGui::GetWindowPos( ).x;
	vMapPosition.y += ImGui::GetWindowPos( ).y;

	return vMapPosition;
}

bool C_Radar::OnMapUpdate( std::string szMapName )
{
	// Resetting vars
	m_bDrawPlantA = false;
	m_bDrawPlantB = false;
	m_bLoaded = false;

	char szPath[ MAX_PATH ];
	sprintf( szPath, _S( "csgo\\resource\\overviews\\%s_radar.dds" ), szMapName.c_str( ) );

	std::ifstream file;
	file.open( szPath );

	if ( !file.is_open( ) )
		return false;

	file.close( );

	if ( FAILED( D3DXCreateTextureFromFileA( SDK::Interfaces::DirectDevice, szPath, &m_pRadarTexture ) ) )
		return false;

	sprintf( szPath, _S( ".\\csgo\\resource\\overviews\\%s.txt" ), szMapName.c_str( ) );
	std::ifstream ifs( szPath );

	if ( ifs.bad( ) )
		return false;

	std::string szInfo( ( std::istreambuf_iterator<char>( ifs ) ), ( std::istreambuf_iterator<char>( ) ) );

	if ( szInfo.empty( ) )
		return false;

	m_vecMapOrigin.x = std::stoi( ParseString( _S( "\"pos_x\"" ), szInfo ) );
	m_vecMapOrigin.y = std::stoi( ParseString( _S( "\"pos_y\"" ), szInfo ) );
	m_flMapScale = std::stod( ParseString( _S( "\"scale\"" ), szInfo ) );

	auto bombA_x = ParseString( _S( "\"bombA_x\"" ), szInfo );

	if ( !bombA_x.empty( ) )
	{
		m_vecPosBombSiteA.x = std::stod( ParseString( _S( "\"bombA_x\"" ), szInfo ) );
		m_vecPosBombSiteA.y = std::stod( ParseString( _S( "\"bombA_y\"" ), szInfo ) );
		m_vecPosBombSiteA.z = 0.0f;

		m_bDrawPlantA = true;
	}

	auto bombB_x = ParseString( _S( "\"bombB_x\"" ), szInfo );

	if ( !bombB_x.empty( ) )
	{
		m_vecPosBombSiteB.x = std::stod( ParseString( _S( "\"bombB_x\"" ), szInfo ) );
		m_vecPosBombSiteB.y = std::stod( ParseString( _S( "\"bombB_y\"" ), szInfo ) );
		m_vecPosBombSiteB.z = 0.0f;

		m_bDrawPlantB = true;
	}

	m_bLoaded = true;
	return true;
}

std::string C_Radar::ParseString( const std::string& szBefore, const std::string& szSource )
{
	if ( !szBefore.empty( ) && !szSource.empty( ) && ( szSource.find( szBefore ) != std::string::npos ) )
	{
		std::string t = strstr( szSource.c_str( ), szBefore.c_str( ) ); //-V522
		t.erase( 0, szBefore.length( ) );
		size_t firstLoc = t.find( '\"', 0 );
		size_t secondLoc = t.find( '\"', firstLoc + 1 );
		t = t.substr( firstLoc + 1, secondLoc - 3 );
		return t;
	}
	else
		return _S( "" );
}