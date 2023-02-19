#include "../Hooks.hpp"
#include "../../Features/Game/RageBot/LocalAnimations.hpp"
#include "../../Features/Game/Prediction/EnginePrediction.hpp"
#include "../../Features/Game/Exploits/Exploits.hpp"
#include "../../Features/Game/Networking/Networking.hpp"
#include "../../Features/Game/Dormant/DormantSystem.hpp"

void C_Hooks::hkPacketStart( LPVOID pEcx, uint32_t, int nSequence, int nCommand )
{
	if ( g_Networking->PacketStart( nCommand ) )
		return Hooks::o_PacketStart( pEcx, nSequence, nCommand );
}
bool C_Hooks::hkProcessTempEntities( LPVOID pEcx, uint32_t, void* pMsg )
{
	int nMaxClients = SDK::Interfaces::ClientState->m_nMaxClients;

	/* fix events interp */
	SDK::Interfaces::ClientState->m_nMaxClients = 1;
	bool bResult = Hooks::o_ProcessTempEntities( pEcx, pMsg );
	SDK::Interfaces::ClientState->m_nMaxClients = nMaxClients;

	return bResult;
}
void C_Hooks::hkPacketEnd( LPVOID pEcx, uint32_t )
{
	/* No Commands Acknowledged */
	int nCommandsAcknowledged = SDK::Interfaces::ClientState->m_nCommandAck - SDK::Interfaces::ClientState->m_nLastCommandAck;
	if ( nCommandsAcknowledged <= 0 )
		return;

	/* Network Data Received */
	if ( SDK::Interfaces::ClientState->m_nDeltaTick == SDK::Interfaces::ClientState->m_ClockDriftMgr.m_nServerTick )
	{
		g_EnginePrediction->OnPostNetworkDataReceived( );
		g_DormantSystem->OnPostNetworkDataReceived( );
	}

	/* PacketEnd */
	Hooks::o_PacketEnd( pEcx );
}