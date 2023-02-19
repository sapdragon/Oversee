#include "Revolver.hpp"
#include "../Prediction/EnginePrediction.hpp"
#include "../../../SDK/Utils/Utils.hpp"

void C_Revolver::OnRunCommand( )
{
	C_BaseCombatWeapon* pCombatWeapon = g_Globals->m_LocalPlayer->m_hActiveWeapon( );
	if ( pCombatWeapon )
	{
		if ( pCombatWeapon->m_nItemID( ) == WEAPON_REVOLVER )
		{
			int nActivity = pCombatWeapon->m_Activity( );
			if ( m_Data.m_nActivity != nActivity )
				if ( nActivity == 208 )
					m_Data.m_nTickBase = g_Globals->m_LocalPlayer->m_nTickBase( ) + 1;

			if ( nActivity == 208 )
				if ( m_Data.m_nTickBase == g_Globals->m_LocalPlayer->m_nTickBase( ) - 1 )
					pCombatWeapon->m_flPostponeFireReadyTime( ) = g_Utils->TicksToTime( m_Data.m_nTickBase ) + 0.2f;

			m_Data.m_nActivity = nActivity;
		}
	}
}
void C_Revolver::OnCreateMove( )
{
	if ( g_Globals->m_LocalPlayer->IsFrozen( ) )
		return;

	C_BaseCombatWeapon* pCombatWeapon = g_Globals->m_LocalPlayer->m_hActiveWeapon( ).Get( );
	if ( !pCombatWeapon || !g_SettingsManager->B[ _S( "ragebot.enable" ) ] || pCombatWeapon->m_nItemID( ) != WEAPON_REVOLVER )
		return;

	g_Globals->m_Packet.m_pCmd->m_nButtons &= ~IN_ATTACK2;
	if ( !g_EnginePrediction->CanAttack( ) || ( g_Globals->m_Packet.m_pCmd->m_nButtons & IN_ATTACK ) )
		return;

	if ( pCombatWeapon->m_flPostponeFireReadyTime( ) > g_Utils->TicksToTime( g_Globals->m_LocalPlayer->m_nTickBase( ) ) )
		g_Globals->m_Packet.m_pCmd->m_nButtons |= IN_ATTACK;
}