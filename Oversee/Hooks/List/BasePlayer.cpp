#include "../Hooks.hpp"
#include "../../Features/Game/Interpolation/Interpolation.hpp"
#include "../../Features/Game/RageBot/Animations.hpp"
#include "../../Features/Game/RageBot/LocalAnimations.hpp"
#include "../../Features/Game/Exploits/Exploits.hpp"
#include "../../SDK/Utils/Utils.hpp"

void C_Hooks::hkOnClientWeaponChange( LPVOID pEcx, uint32_t, LPVOID pWeapon )
{
	return;
}
void C_Hooks::hkNotifyOnLayerChangeSequence( LPVOID pEcx, uint32_t, const C_AnimationLayer* pLayer, const int nNewSequence )
{
	return;
}
void C_Hooks::hkNotePredictionError( LPVOID pEcx, uint32_t, Vector& vOffset )
{
	return;
}
void C_Hooks::hkDoExtraBoneProcessing( LPVOID pEcx, uint32_t, C_StudioHDR* pHdr, Vector* vecVector, Quaternion* pSomething, matrix3x4_t* aMatrix, LPVOID aList, LPVOID pContext )
{
	return;
}
void C_Hooks::hkClampBonesInBBOX( LPVOID pEcx, uint32_t, int a1, int a2 )
{
	/* Awesome HvH plugin support */
	if ( SDK::EngineData::m_ConvarList[ CheatConvarList::SvLegacyDesync ]->GetBool( ) )
		return;

	float flCurTime = SDK::Interfaces::GlobalVars->m_flCurTime;

	SDK::Interfaces::GlobalVars->m_flCurTime = ( static_cast < C_BasePlayer* > ( pEcx ) )->m_flSimulationTime( );
	if ( pEcx == g_Globals->m_LocalPlayer )
		SDK::Interfaces::GlobalVars->m_flCurTime = g_Utils->TicksToTime( SDK::Interfaces::GlobalVars->m_iTickCount );

	Hooks::o_ClampBonesInBBOX( pEcx, a1, a2 );
	SDK::Interfaces::GlobalVars->m_flCurTime = flCurTime;
}
void C_Hooks::hkSetCollisionBounds( LPVOID pEcx, uint32_t, Vector* vecMins, Vector* vecMaxs )
{
	return Hooks::o_SetCollisionBounds( pEcx, vecMins, vecMaxs );
}
void C_Hooks::hkUpdateClientSideAnimation( LPVOID pEcx, uint32_t )
{
	C_BasePlayer* Player = ( C_BasePlayer* ) ( pEcx );
	if ( !Player || !Player->IsPlayer( ) || !Player->IsAlive( ) || Player->IsFriend( ) )
		return Hooks::o_UpdateClientSideAnimation( pEcx );
	else if ( g_Globals->m_Animations.m_bUpdateAnims )
		Hooks::o_UpdateClientSideAnimation( pEcx );
}
bool C_Hooks::hkSetupBones( LPVOID pEcx, uint32_t, matrix3x4_t* aMatrix, int nBoneCount, int nMask, float flTime )
{
	C_BasePlayer* Player = ( C_BasePlayer* ) ( ( DWORD ) ( pEcx ) -0x4 );
	if ( !Player || !Player->IsPlayer( ) || !Player->IsAlive( ) || Player->IsFriend( ) )
		return Hooks::o_SetupBones( pEcx, aMatrix, nBoneCount, nMask, flTime );

	bool bResult = true;
	if ( g_Globals->m_Animations.m_bSetupBones )
		bResult = Hooks::o_SetupBones( pEcx, aMatrix, nBoneCount, nMask, flTime );
	else if ( aMatrix )
	{
		if ( Player == g_Globals->m_LocalPlayer )
			bResult = g_LocalAnimations->CopyCachedMatrix( aMatrix, nBoneCount );
		else
			bResult = g_PlayerAnimations->CopyCachedMatrix( Player, aMatrix, nBoneCount );
	}

	return bResult;
}
void C_Hooks::hkCalcView( LPVOID pEcx, uint32_t, Vector& vecEyeOrigin, QAngle& angEyeAngles, float& zNear, float& zFar, float& flFov )
{
	if ( !g_Globals->m_LocalPlayer || !g_Globals->m_LocalPlayer->IsAlive( ) || pEcx != g_Globals->m_LocalPlayer )
		return Hooks::o_CalcView( pEcx, vecEyeOrigin, angEyeAngles, zNear, zFar, flFov );

	QAngle angAimPunchAngle = g_Globals->m_LocalPlayer->m_aimPunchAngle( );
	QAngle angViewPunchAngle = g_Globals->m_LocalPlayer->m_viewPunchAngle( );

	if ( g_SettingsManager->B[ _S( "misc.removals.item_3" ) ] )
		g_Globals->m_LocalPlayer->m_aimPunchAngle( ) = QAngle( 0, 0, 0 );

	if ( g_SettingsManager->B[ _S( "misc.removals.item_4" ) ] )
		g_Globals->m_LocalPlayer->m_viewPunchAngle( ) = QAngle( 0, 0, 0 );

	Hooks::o_CalcView( pEcx, vecEyeOrigin, angEyeAngles, zNear, zFar, flFov );
	if ( g_Globals->m_Packet.m_bVisualFakeDucking )
		vecEyeOrigin.z = g_Globals->m_LocalPlayer->GetAbsOrigin( ).z + 64.0f;

	if ( g_SettingsManager->B[ _S( "misc.removals.item_3" ) ] )
		g_Globals->m_LocalPlayer->m_aimPunchAngle( ) = angAimPunchAngle;

	if ( g_SettingsManager->B[ _S( "misc.removals.item_4" ) ] )
		g_Globals->m_LocalPlayer->m_viewPunchAngle( ) = angViewPunchAngle;
}
void C_Hooks::hkCalcViewmodelBob( LPVOID pEcx, uint32_t, Vector& vecViewBob )
{
	if ( pEcx != g_Globals->m_LocalPlayer || !g_SettingsManager->B[ _S( "misc.removals.item_6" ) ] )
		return Hooks::o_CalcViewmodelBob( pEcx, vecViewBob );
}
void C_Hooks::hkInterpolateServerEntities( LPVOID pEcx, uint32_t )
{
	if ( !g_Globals->m_LocalPlayer )
		return Hooks::o_InterpolateServerEntities( pEcx );

	g_LagCompensation->InterpolatePlayers( );
	Hooks::o_InterpolateServerEntities( pEcx );
	return g_PlayerAnimations->InterpolateMatricies( );
}