#include "../Hooks.hpp"
#include "../../Features/Fonts/FontManager.hpp"
#include "../../Features/Visuals/ESP/ESP.hpp"
#include "../../Interface/Renderer/Renderer.hpp"
#include "../../Features/Visuals/HUD/HUD.hpp"
#include "../../Features/Visuals/Log Manager/LogManager.hpp"
#include "../../Features/Visuals/Grenades/Warning.hpp"
#include "../../Features/Visuals/World/World.hpp"
#include "../../Features/Misc/LuaAPI/LuaHooks.hpp"
#include "../../Interface/GUI/Notifies.h"
#include "../../Features/Game/Exploits/Exploits.hpp"
#include "../../Interface/GUI/GUI.hpp"
#include "../../Features/Visuals/World/Radar.hpp"
#include "../../Features/Visuals/Model/Model.hpp"
#include "../../Features/Misc/Movement/Movement.hpp"
#include "../../Features/Visuals/World/HitMarker.hpp"
HRESULT WINAPI C_Hooks::hkEndScene( IDirect3DDevice9* pDevice )
{
	IDirect3DVertexDeclaration9* pVertDec;
	if ( pDevice->GetVertexDeclaration( &pVertDec ) )
		return Hooks::o_EndScene( pDevice );

	IDirect3DVertexShader9* pVertShader;
	if ( pDevice->GetVertexShader( &pVertShader ) )
		return Hooks::o_EndScene( pDevice );

	DWORD dwOldD3DRSColorWriteEnable = NULL;
	DWORD dwColorWrite = 0;
	DWORD dwSRGBWrite = 0;

	pDevice->GetRenderState( D3DRS_COLORWRITEENABLE, &dwColorWrite );
	pDevice->GetRenderState( D3DRS_SRGBWRITEENABLE, &dwSRGBWrite );

	pDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );
	pDevice->SetRenderState( D3DRS_SRGBWRITEENABLE, false );
	pDevice->GetRenderState( D3DRS_COLORWRITEENABLE, &dwOldD3DRSColorWriteEnable );
	pDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );
	pDevice->SetRenderState( D3DRS_SRGBWRITEENABLE, false );
	pDevice->SetSamplerState( NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	pDevice->SetSamplerState( NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
	pDevice->SetSamplerState( NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP );
	pDevice->SetSamplerState( NULL, D3DSAMP_SRGBTEXTURE, NULL );

	// new frame
	ImGui_ImplDX9_NewFrame( );
	ImGui_ImplWin32_NewFrame( );
	ImGui::NewFrame( );

	g_Renderer->Instance( );
	g_World->RunClanTagChanger( );

	// cheat ui
	g_CustomHUD->Instance( );
	g_Menu->Instance( );
	g_Radar->Instance( );
	notifies::handle( ImGui::GetOverlayDrawList( ) );

	ImGui::Render( );
	ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData( ) );

	pDevice->SetRenderState( D3DRS_COLORWRITEENABLE, dwColorWrite );
	pDevice->SetRenderState( D3DRS_SRGBWRITEENABLE, dwSRGBWrite );
	pDevice->SetRenderState( D3DRS_COLORWRITEENABLE, dwOldD3DRSColorWriteEnable );
	pDevice->SetRenderState( D3DRS_SRGBWRITEENABLE, true );

	pDevice->SetVertexShader( pVertShader );
	pDevice->SetVertexDeclaration( pVertDec );

	// return original end scene
	return Hooks::o_EndScene( pDevice );
}

HRESULT WINAPI C_Hooks::hkReset( IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pDeviceParameters )
{
	ImGui_ImplDX9_InvalidateDeviceObjects( );

	HRESULT lResult = Hooks::o_Reset( pDevice, pDeviceParameters );
	if ( SUCCEEDED( lResult ) )
		ImGui_ImplDX9_CreateDeviceObjects( );

	return lResult;
}

void FASTCALL C_Hooks::hkLockCursor( LPVOID pEcx, uint32_t )
{
	if ( !g_Menu->IsVisible( ) )
		return Hooks::o_LockCursor( pEcx );

	return SDK::Interfaces::Surface->UnlockCursor( );
}

void FASTCALL C_Hooks::hkPaintTraverse( LPVOID pEcx, uint32_t, VGUI::VPANEL pPanel, bool bForceRepaint, bool bAllowForce )
{
	Hooks::o_PaintTraverse( pEcx, pPanel, bForceRepaint, bAllowForce );

	static VGUI::VPANEL pPanelID = NULL;
	if ( !pPanelID )
	{
		if ( !strcmp( SDK::Interfaces::Panel->GetName( pPanel ), _S( "FocusOverlayPanel" ) ) )
			pPanelID = pPanel;
	}
	else if ( pPanelID == pPanel )
	{
		g_Renderer->ClearDrawData( );

		g_World->PenetrationCrosshair( );
		g_World->PreserveKillfeed( );
		g_DrawModel->Instance( );
		g_World->RunVisualFeatures( );
		g_LogManager->Instance( );
		g_PlayerESP->Instance( );
		g_HitMarkers->Instance( );
		g_World->GrenadeWarning( );

#ifdef OVERSEE_DEV
		g_Exploits->DebugDrawData( );
#endif

		for ( auto pCurrent : g_LuaHookManager->GetHooksList( _S( "Draw" ) ) )
			pCurrent.m_pFunc( );

		g_Renderer->SwapDrawData( );
	}
}
