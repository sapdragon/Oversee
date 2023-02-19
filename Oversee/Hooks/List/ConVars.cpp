#include <sstream>
#include "../Hooks.hpp"
#include "../../Features/Game/Networking/Networking.hpp"

bool FASTCALL C_Hooks::hkClPredDoResetLatch( LPVOID pEcx, uint32_t )
{
	DWORD dwReturnAddress = reinterpret_cast < DWORD > ( _ReturnAddress( ) );
	if ( dwReturnAddress == reinterpret_cast < DWORD > ( SDK::EngineData::m_AddressList[ CheatAddressList::ClPredDoResetLatch ] ) )
		return false;

	return Hooks::o_ClPredDoResetLatch_GetBool( pEcx );
}
bool FASTCALL C_Hooks::hkSvCheats_GetBool( LPVOID pEcx, uint32_t )
{
	DWORD dwReturnAddress = reinterpret_cast < DWORD > ( _ReturnAddress( ) );
	if ( dwReturnAddress == reinterpret_cast < DWORD > ( SDK::EngineData::m_AddressList[ CheatAddressList::CamThink_Return ] ) )
		return true;

	//g_Networking->OnNetShowFragments( reinterpret_cast < DWORD >( _ReturnAddress( ) ) );
	return Hooks::o_SvCheats_GetBool( pEcx );
}