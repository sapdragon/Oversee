#include "HitMarker.hpp"

#include "../../../INterface/Renderer/Renderer.hpp"

void C_HitMarker::Instance( )
{
	if ( m_aHitMarkers.empty( ) )
		return;

	float flTime = SDK::Interfaces::GlobalVars->m_flRealTime;
	for ( int i = 0; i < m_aHitMarkers.size( ); i++ )
	{
		auto HitMarker = &m_aHitMarkers[ i ];
		if ( flTime - HitMarker->m_flTime > 4.0f )
		{
			m_aHitMarkers.erase( m_aHitMarkers.begin( ) + i );
			continue;
		}

		const float flGap = 2.0f;
		const float flLength = 6.0f;

		Vector vecScreenPosition = Vector( g_Globals->m_ScreenData.m_nScreenSizeX / 2, g_Globals->m_ScreenData.m_nScreenSizeY / 2, 0 );
		if ( g_SettingsManager->B[ _S( "misc.hitmarker_crosshair" ) ] )
		{
			g_Renderer->AddLine( ImVec2( vecScreenPosition.x - flGap, vecScreenPosition.y - flGap ), ImVec2( vecScreenPosition.x - flLength, vecScreenPosition.y - flLength ), ImColor( 255, 255, 255, 255 ) );
			g_Renderer->AddLine( ImVec2( vecScreenPosition.x + flGap, vecScreenPosition.y + flGap ), ImVec2( vecScreenPosition.x + flLength, vecScreenPosition.y + flLength ), ImColor( 255, 255, 255, 255 ) );
			g_Renderer->AddLine( ImVec2( vecScreenPosition.x + flGap, vecScreenPosition.y - flGap ), ImVec2( vecScreenPosition.x + flLength, vecScreenPosition.y - flLength ), ImColor( 255, 255, 255, 255 ) );
			g_Renderer->AddLine( ImVec2( vecScreenPosition.x - flGap, vecScreenPosition.y + flGap ), ImVec2( vecScreenPosition.x - flLength, vecScreenPosition.y + flLength ), ImColor( 255, 255, 255, 255 ) );
		}
		if ( g_SettingsManager->B[ _S( "misc.hitmarker_world" ) ] )
		{
			if ( SDK::Interfaces::DebugOverlay->ScreenPosition( HitMarker->m_vecOrigin, vecScreenPosition ) )
				continue;

			g_Renderer->AddLine( ImVec2( vecScreenPosition.x - flGap, vecScreenPosition.y - flGap ), ImVec2( vecScreenPosition.x - flLength, vecScreenPosition.y - flLength ), ImColor( 255, 255, 255, 255 ) );
			g_Renderer->AddLine( ImVec2( vecScreenPosition.x + flGap, vecScreenPosition.y + flGap ), ImVec2( vecScreenPosition.x + flLength, vecScreenPosition.y + flLength ), ImColor( 255, 255, 255, 255 ) );
			g_Renderer->AddLine( ImVec2( vecScreenPosition.x + flGap, vecScreenPosition.y - flGap ), ImVec2( vecScreenPosition.x + flLength, vecScreenPosition.y - flLength ), ImColor( 255, 255, 255, 255 ) );
			g_Renderer->AddLine( ImVec2( vecScreenPosition.x - flGap, vecScreenPosition.y + flGap ), ImVec2( vecScreenPosition.x - flLength, vecScreenPosition.y + flLength ), ImColor( 255, 255, 255, 255 ) );
		}
	}
}

void C_HitMarker::OnReportHit( Vector vecOrigin )
{
	HitMarker_t HitMarker;
	HitMarker.m_flTime = SDK::Interfaces::GlobalVars->m_flRealTime;
	HitMarker.m_vecOrigin = vecOrigin;
	m_aHitMarkers.push_back( HitMarker );
}