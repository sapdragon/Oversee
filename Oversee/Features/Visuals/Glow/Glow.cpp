#include "Glow.hpp"

void C_Glow::Instance( )
{
	const int nCount = SDK::Interfaces::GlowObjManager->m_GlowObjectDefinitions.Count( );
	for ( int iGlowIndex = 0; iGlowIndex < nCount; iGlowIndex++ )
	{
		C_BaseEntity* pEntity = ( C_BaseEntity* ) SDK::Interfaces::GlowObjManager->m_GlowObjectDefinitions[ iGlowIndex ].m_pEntity;
		if ( !pEntity || pEntity->IsDormant( ) )
			continue;

		GlowSettings_t pGlowOptions;
		if ( pEntity->IsPlayer( ) )
		{
			if ( pEntity == g_Globals->m_LocalPlayer )
				pGlowOptions = g_SettingsManager->GetGlowSettings( "local" );
			else if ( pEntity->m_iTeamNum( ) == g_Globals->m_LocalPlayer->m_iTeamNum( ) )
				pGlowOptions = g_SettingsManager->GetGlowSettings( "team" );
			else
				pGlowOptions = g_SettingsManager->GetGlowSettings( "enemy" );
		}
		else if ( pEntity->IsWeapon( ) )
		{
			auto pWeapon = ( C_BaseCombatWeapon* ) pEntity;
			if ( pWeapon->m_nOwnerEntity( ).IsValid( ) )
				continue;

			pGlowOptions = g_SettingsManager->GetGlowSettings( "weapon" );
		}

		auto& m_GlowObject = SDK::Interfaces::GlowObjManager->m_GlowObjectDefinitions[ iGlowIndex ];
		if ( m_GlowObject.IsUnused( ) || !pGlowOptions.m_bEnable )
			continue;

		m_GlowObject.m_flRed = pGlowOptions.m_Color.Value.x;
		m_GlowObject.m_flGreen = pGlowOptions.m_Color.Value.y;
		m_GlowObject.m_flBlue = pGlowOptions.m_Color.Value.z;
		m_GlowObject.m_flAlpha = pGlowOptions.m_Color.Value.w;

		m_GlowObject.m_bRenderWhenOccluded = true;
		m_GlowObject.m_bRenderWhenUnoccluded = false;

		m_GlowObject.m_nGlowStyle = pGlowOptions.m_nGlowStyle;
	}
}