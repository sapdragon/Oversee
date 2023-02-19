#pragma once
#include "../Hooks.hpp"
#include "../../Features/Visuals/Glow/Glow.hpp"
#include "../../Features/Visuals/ThirdPerson/ThirdPerson.hpp"
#include "../../Features/Misc/LuaAPI/LuaHooks.hpp"
#include "../../Features/Visuals/Chams/ShotChams.hpp"
#include "../../Features/Game/Viewmodel/Viewmodel.hpp"

float FASTCALL C_Hooks::hkGetViewmodelFOV( LPVOID pEcx, uint32_t )
{
	return g_SettingsManager->I[ _S( "misc.viewmodel.fov" ) ];
}

void FASTCALL C_Hooks::hkOverrideView( LPVOID pEcx, uint32_t, C_ViewSetup* pViewSetup )
{
	/* Custom viewmodel offsets */
	g_ViewmodelAnimation->ModifyOrigin( );

	/* run ThirdPerson */
	g_ThirdPerson->Instance( pViewSetup );

	/* Run LUA */
	if ( g_Globals->m_Lua.m_bScriptLoaded )
		for ( auto pCurrent : g_LuaHookManager->GetHooksList( _S( "OverrideView" ) ) )
			pCurrent.m_pFunc( pViewSetup );

	return Hooks::o_OverrideView( pEcx, pViewSetup );
}

int FASTCALL C_Hooks::hkDoPostScreenEffects( LPVOID pEcx, uint32_t )
{
	/* run visual features */
	if ( g_Globals->m_LocalPlayer )
	{
		g_Glow->Instance( );
		g_ShotChams->Instance( );
	}

	return Hooks::o_DoPostScreenEffects( pEcx );
}