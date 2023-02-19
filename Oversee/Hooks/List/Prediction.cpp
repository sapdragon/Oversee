#include "../Hooks.hpp"
#include "../../Features/Game/Prediction/EnginePrediction.hpp"
#include "../../Features/Game/Revolver/Revolver.hpp"
#include "../../Features/Game/Viewmodel/Viewmodel.hpp"
#include "../../Features/Game/Exploits/Exploits.hpp"
#include "../../Features/Game/Networking/Networking.hpp"

void FASTCALL C_Hooks::hkRunCommand( LPVOID pEcx, uint32_t, C_BasePlayer* pPlayer, C_UserCmd* pCmd, IMoveHelper* pMoveHelper )
{
	// run only for localplayer
	if ( !pPlayer || pPlayer != g_Globals->m_LocalPlayer || !pPlayer->IsAlive( ) )
		return Hooks::o_RunCommand( pEcx, pPlayer, pCmd, pMoveHelper );

	if ( pCmd->m_nTickCount > SDK::Interfaces::GlobalVars->m_iTickCount + g_Networking->GetTickRate( ) )
	{
		pCmd->m_bHasBeenPredicted = true;
		return;
	}

	/* force predicted tickbase */
	TickbaseRecord_t* Record = g_EnginePrediction->GetTickbaseRecord( pCmd->m_nCommand );
	if ( Record->m_bIsValidRecord )
	{
		/* set tickbase */
		pPlayer->m_nTickBase( ) = Record->m_nTickBase - 1;
		
		/* reset record */
		Record->m_nTickBase = -1;
		Record->m_bIsValidRecord = false;
	}

	/* call original function */
	Hooks::o_RunCommand( pEcx, pPlayer, pCmd, pMoveHelper );

	/* store predicted netvars */
	g_EnginePrediction->OnRunCommand( pCmd->m_nCommand );

	/* fix-up revolver time */
	g_Revolver->OnRunCommand( );
}
bool FASTCALL C_Hooks::hkInPrediction( LPVOID pEcx, uint32_t )
{
	DWORD dwReturnAddress = reinterpret_cast < DWORD > ( _ReturnAddress( ) );
	if ( dwReturnAddress == reinterpret_cast < DWORD > ( SDK::EngineData::m_AddressList[ CheatAddressList::SetupBones_Return ] ) )
		return false;

	return Hooks::o_InPrediction_GetBool( pEcx );
}