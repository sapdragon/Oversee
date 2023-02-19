#include <charconv>
#include <random>

#include "../Hooks.hpp"
#include "../imgui_impl_dx9.h"
#include "../../Features/Game/RageBot/RageBot.hpp"
#include "../../Features/Game/Prediction/EnginePrediction.hpp"
#include "../../Features/Game/Networking/Networking.hpp"
#include "../../Features/Game/Exploits/Exploits.hpp"
#include "../../Features/Game/Dormant/DormantSystem.hpp"
#include "../../Interface/Renderer/Renderer.hpp"
#include "../../Features/Fonts/FontManager.hpp"
#include "../../Features/Inventory Changer/InventoryChanger.hpp"
#include "../../Features/Inventory Changer/ItemsManager.hpp"
#include "../../Features/Misc/Voice Communication/VoiceCommunication.hpp"
#include "../../Features/Misc/Events/Events.hpp"
#include "../../Features/Visuals/World/HitMarker.hpp"
#include "../../Features/Visuals/Chams/Chams.hpp"
#include "../../Features/Game/RageBot/LocalAnimations.hpp"
#include "../../Features/Game/RageBot/AutoStop.hpp"
#include "../../SDK/Utils/Utils.hpp"

void C_Hooks::hkStudioDrawModelExecute( LPVOID pEcx, uint32_t, LPVOID pResults, DrawModelInfo_t* pInfo, matrix3x4_t* pBoneToWorld, float_t* flpFlexWeights, float_t* flpFlexDelayedWeights, Vector& vrModelOrigin, int32_t iFlags )
{
	/* call original */
	return Hooks::o_StudioDrawModelExecute( SDK::Interfaces::StudioRender, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags );
}
int C_Hooks::hkSendDatagram( LPVOID pEcx, uint32_t, void* lpData )
{
	int nSequence = Hooks::o_SendDatagram( pEcx, lpData );
	if ( reinterpret_cast < DWORD > ( _ReturnAddress( ) ) == reinterpret_cast < DWORD > ( SDK::EngineData::m_AddressList[ CheatAddressList::CL_Move_Return ] ) )
		g_Networking->OnSendDatagram( nSequence );

	return nSequence;
}
void C_Hooks::hkCL_Move( float flFrameTime, bool bIsFinalTick )
{	
	/* networking part: data collection and ping reducer */
	g_Networking->StartNetwork( );
	g_Networking->PingReducer( );

	/* recharging */
	if ( g_Exploits->IsRecharging( ) )
		return;

	/* call CL_Move */
	Hooks::o_CL_Move( flFrameTime, bIsFinalTick );

	/* finish network */
	g_Networking->FinishNetwork( );

	/* run exploits */
	return g_Exploits->OnMovePacket( flFrameTime );
}
void C_Hooks::hkDrawSetColor( LPVOID pEcx, uint32_t, int Red, int Green, int Blue, int Alpha )
{
	DWORD dwReturnAddress = ( DWORD ) ( _ReturnAddress( ) );
	if ( !g_Globals->m_LocalPlayer || !g_SettingsManager->I[ _S( "misc.no_scope_mode" ) ] )
		return Hooks::o_DrawSetColor( pEcx, Red, Green, Blue, Alpha );

	// return addresses
	DWORD dwScopeArcReturn = ( DWORD ) ( SDK::EngineData::m_AddressList[ CheatAddressList::ScopeArc_Return ] );
	DWORD dwScopeLensReturn = ( DWORD ) ( SDK::EngineData::m_AddressList[ CheatAddressList::ScopeLens_Return ] );
	DWORD dwScopeLinesBlurryReturn = ( DWORD ) ( SDK::EngineData::m_AddressList[ CheatAddressList::ScopeLinesBlurry_Return ] );
	DWORD dwScopeLinesClearReturn = ( DWORD ) ( SDK::EngineData::m_AddressList[ CheatAddressList::ScopeLinesClear_Return ] );

	if ( dwReturnAddress == dwScopeArcReturn || dwReturnAddress == dwScopeLensReturn )
		return Hooks::o_DrawSetColor( pEcx, Red, Green, Blue, 0 );

	if ( ( dwReturnAddress != dwScopeLinesBlurryReturn && dwReturnAddress != dwScopeLinesClearReturn ) || g_SettingsManager->I[ _S( "misc.no_scope_mode" ) ] == 2 )
		return Hooks::o_DrawSetColor( pEcx, Red, Green, Blue, Alpha );

	// call original
	Hooks::o_DrawSetColor( pEcx, 0, 0, 0, 255 );

	// draw scope
	int iScreenSizeX = g_Globals->m_ScreenData.m_nScreenSizeX, iScreenSizeY = g_Globals->m_ScreenData.m_nScreenSizeY;
	SDK::Interfaces::Surface->DrawLine( 0, iScreenSizeY / 2, iScreenSizeX, iScreenSizeY / 2 );
	SDK::Interfaces::Surface->DrawLine( iScreenSizeX / 2, 0, iScreenSizeX / 2, iScreenSizeY );

	// remove scope lines
	return Hooks::o_DrawSetColor( pEcx, 0, 0, 0, 0 );
}
void C_Hooks::hkProcessMovement( LPVOID pEcx, uint32_t, C_BasePlayer* pPlayer, C_MoveData* m_MoveData )
{
	m_MoveData->m_bGameCodeMovedPlayer = false;
	return Hooks::o_ProcessMovement( pEcx, pPlayer, m_MoveData );
}
void C_Hooks::hkPerformScreenOverlay( LPVOID pEcx, uint32_t, int x, int y, int iWidth, int iHeight )
{
	if ( !g_SettingsManager->B[ _S( "misc.ad_block" ) ] || ( *( SDK::Interfaces::GameRules ) && ( *SDK::Interfaces::GameRules )->IsValveDS( ) ) )
		return Hooks::o_PerformScreenOverlay( pEcx, x, y, iWidth, iHeight );
}
void C_Hooks::hkEmitSound( LPVOID pEcx, uint32_t, IRecipientFilter& pFilter, int iEntIndex, int iChannel, const char* pSoundEntry, unsigned int nSoundEntryHash, const char* szSoundName, float flVolume, int nSeed, float flAttenuation, int iFlags, int iPitch, const Vector* pOrigin, const Vector* pDirection, void* pUtlVecOrigins, bool bUpdatePositions, float flSoundTime, int nSpeakerEntity, int unk )
{
	if ( strstr( pSoundEntry, _S( "WalkJump" ) ) || strstr( pSoundEntry, _S( "SuitLand" ) ) )
		if ( g_Globals->m_Packet.m_bInCreateMove )
			flVolume = 0.0f;

	if ( strstr( pSoundEntry, _S( "UI.PlayerPing" ) ) )
		if ( g_SettingsManager->B[ _S( "misc.removals.item_11" ) ] )
			flVolume = 0.0f;

	return Hooks::o_EmitSound( pEcx, pFilter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, szSoundName, flVolume, nSeed, flAttenuation, iFlags, iPitch, pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, flSoundTime, nSpeakerEntity, unk );
}
void C_Hooks::hkPlaySound( LPVOID pEcx, uint32_t, const char* szSoundEntry )
{
	return Hooks::o_PlaySound( pEcx, szSoundEntry );
}
void C_Hooks::CL_LagCompensation_Callback( IConVar* pConVar, const char* szOldValue, float flOldValue )
{
	g_Globals->m_Packet.m_bAntiExploit = flOldValue == 1.0f;
}
void C_Hooks::hkOnScreenSizeChanged( LPVOID pEcx, uint32_t, int nOldWidth, int nOldHeigth )
{
	Hooks::o_OnScreenSizeChanged( pEcx, nOldWidth, nOldHeigth );

	// get screen size
	SDK::Interfaces::EngineClient->GetScreenSize( g_Globals->m_ScreenData.m_nScreenSizeX, g_Globals->m_ScreenData.m_nScreenSizeY );

	// destroy fonts
	ImGui_DestroyFonts( );

	// create fonts
	g_FontManager->CreateFonts( );
}
void C_Hooks::hkPlayerMove( LPVOID pEcx, uint32_t )
{
	Hooks::o_PlayerMove( pEcx );
	return g_EnginePrediction->OnPlayerMove( );
}
void C_Hooks::hkCL_ReadPackets( bool bFrameFinished )
{
	if ( g_Networking->ReadPackets( ) )
		return Hooks::o_CL_ReadPackets( bFrameFinished );
}
bool C_Hooks::hkDispatchUserMessage( LPVOID pEcx, uint32_t, int iMessageType, int iArgument, int iSecondArgument, LPVOID pData )
{
	if ( iMessageType == CS_UM_TextMsg || iMessageType == CS_UM_HudMsg || iMessageType == CS_UM_SayText )
		if ( g_SettingsManager->B[ _S( "misc.ad_block" ) ] && ( !( *( SDK::Interfaces::GameRules ) ) || !( *( SDK::Interfaces::GameRules ) )->IsValveDS( ) ) )
			return true;

	return Hooks::o_DispatchUserMessage( pEcx, iMessageType, iArgument, iSecondArgument, pData );
}
bool C_Hooks::hkSVCMsg_VoiceData( LPVOID pEcx, uint32_t, C_SVCMsg_VoiceData* Message )
{
	g_VoiceCommunication->OnVoiceDataReceived( Message );
	return Hooks::o_SVCMsg_VoiceData( pEcx, Message );
}
bool C_Hooks::hkSVCMsg_GameEvent( LPVOID pEcx, uint32_t, C_SVCMsg_GameEvent* Message )
{
	/* unserialize event */
	IGameEvent* m_GameEvent = SDK::Interfaces::EventManager->UnserializeMessage( Message );
	if ( !m_GameEvent )
		return true;

	/* let the cheat process event */
	g_CheatEvents->HandleGameEvent( m_GameEvent );

	/* let the game process event */
	SDK::Interfaces::EventManager->FireEventClientSide( m_GameEvent );

	/* finish */
	return true;
}
bool C_Hooks::hkMsgFunc_ReportHit( LPVOID pEcx, uint32_t, CCSUsrMsg_ReportHit* Message )
{
	g_HitMarkers->OnReportHit( Vector( Message->m_flPosX, Message->m_flPosY, Message->m_flPosZ ) );
	return Hooks::o_MsgFunc_ReportHit( pEcx, Message );
}
bool C_Hooks::hkMsgFunc_ProcessSpottedEntityUpdate( LPVOID pEcx, uint32_t, CCSUsrMsg_ProcessSpottedEntityUpdate_SpottedEntityUpdate* Message )
{
	g_DormantSystem->OnRadarDataReceived( Message );
	return Hooks::o_MsgFunc_ProcessSpottedEntityUpdate( pEcx, Message );
}
void C_Hooks::hkSoUpdated( LPVOID pEcx, uint32_t pEdx, Soid_t owner, C_SharedObject* object, int32_t event )
{
	g_InventoryChanger->OnSoUpdated( object, event );
	return Hooks::o_SoUpdated( pEcx, pEdx, owner, object, event );
}
bool C_Hooks::hkEquipItemInLoadout( LPVOID pEcx, uint32_t pEdx, int32_t team, int32_t slot, uint64_t item_id, bool swap )
{
	g_InventoryChanger->OnEquipItemInLoadOut( team, slot, item_id );
	return Hooks::o_EquipItemInLoadout( pEcx, pEdx, team, slot, item_id, swap );
}
bool C_Hooks::hkSetItemBackpackPosition( LPVOID pEcx, int32_t pEdx, C_EconItemView* pItem, uint32_t position, bool force_unequip, bool allow_overflow )
{
	if ( pItem->GetInventory( ) == 0 ) {
		pItem->GetInventory( ) = 1;
		pItem->GetSocData( )->m_nInventory = pItem->GetInventory( );
		SDK::Interfaces::InventoryManager->GetLocalPlayerInventory( )->SoUpdated( SDK::Interfaces::InventoryManager->GetLocalPlayerInventory( )->GetOwnerID( ), ( C_SharedObject* ) pItem->GetSocData( ), 4 );
	}

	return Hooks::o_SetItemBackpackPosition( pEcx, pEdx, pItem, position, force_unequip, allow_overflow );
}
const char* C_Hooks::hkParamsGetArgAsString( LPVOID pEcx, int32_t pEdx, int32_t params, int32_t index )
{
	return Hooks::o_ParamsGetArgsAtString( pEcx, pEdx, params, index );
}
void C_Hooks::hkDeleteItem( LPVOID, int32_t, uint64_t item_id, bool recycle )
{
	IPlayerInventory* m_pInventory = SDK::Interfaces::InventoryManager->GetLocalPlayerInventory( );
	if ( !m_pInventory )
		return;

	C_EconItemView* pEconItemView = m_pInventory->GetInventoryItemByItemID( item_id );
	if ( !pEconItemView )
		return;

	m_pInventory->RemoveItem( pEconItemView->GetSocData( ) );
	g_SettingsManager->m_aInventoryItemList.erase( item_id );
}
void C_Hooks::hkPrecacheCustomMaterials( LPVOID, int32_t, uint64_t item_id, int32_t a2, int32_t a3 )
{
	return;
}
void C_Hooks::hkUpdateGeneratedMaterial( LPVOID pEcx, int32_t pEdx, int32_t a1, int32_t a2, int32_t a3 )
{
	C_EconItemView* pEconItemView = static_cast < C_EconItemView* >( pEcx );
	if ( !pEconItemView )
		return Hooks::o_UpdateGeneratedMaterial( pEcx, pEdx, a1, a2, a3 );

	C_EconItem* pSocData = pEconItemView->GetSocData( );
	if ( !pSocData )
		return;

	auto nInventoryItemIT = g_SettingsManager->m_aInventoryItemList.find( pSocData->m_nItemID );
	if ( nInventoryItemIT == g_SettingsManager->m_aInventoryItemList.end( ) )
		return;

	InventoryItem_t& InventoryItem = nInventoryItemIT->second;

	const auto paint_kit = g_ItemManager.PaintKits[ InventoryItem.m_iPaintKit ];
	if ( !paint_kit )
		return;

	auto o_pearlescent = paint_kit->m_flPearcelent;
	auto o_col0 = paint_kit->m_cColor[ 0 ], o_col1 = paint_kit->m_cColor[ 1 ], o_col2 = paint_kit->m_cColor[ 2 ], o_col3 = paint_kit->m_cColor[ 3 ];

	paint_kit->m_flPearcelent = InventoryItem.m_flPearlescent;
	for ( int32_t i = 0; i < 4; ++i )
		paint_kit->m_cColor[ i ] = { InventoryItem.color[ i ][ 0 ], InventoryItem.color[ i ][ 1 ], InventoryItem.color[ i ][ 2 ] };

	Hooks::o_UpdateGeneratedMaterial( pEcx, pEdx, a1, a2, a3 );

	paint_kit->m_flPearcelent = o_pearlescent;
	paint_kit->m_cColor[ 0 ] = o_col0, paint_kit->m_cColor[ 1 ] = o_col1, paint_kit->m_cColor[ 2 ] = o_col2, paint_kit->m_cColor[ 3 ] = o_col3;
}
bool C_Hooks::hkSendNetMessage( LPVOID pEcx, uint32_t, INetMessage& Message, bool bReliable, bool bVoice )
{
	if ( Message.GetGroup( ) == 9 )
		bVoice = true;

	return Hooks::o_SendNetMessage( pEcx, Message, bReliable, bVoice );
}
LPVOID C_Hooks::hkGetClientModelRenderable( LPVOID pEcx, uint32_t )
{
	return nullptr;
}
int C_Hooks::hkListLeavesInBox( LPVOID pEcx, uint32_t, Vector& vecMin, Vector& vecMax, unsigned short* pList, int iMax )
{
	if ( _rotr( _byteswap_ulong( *( uint32_t* ) ( _ReturnAddress( ) ) ), 32 ) != _rotr( _byteswap_ulong( 0x14244489 ), 32 ) )
		return Hooks::o_ListLeavesInBox( pEcx, vecMax, vecMax, pList, iMax );

	RenderableInfo_t* pRenderableInfo = *( RenderableInfo_t** ) ( ( uintptr_t ) _AddressOfReturnAddress( ) + 0x14 );
	if ( !pRenderableInfo || !pRenderableInfo->m_pRenderable )
		return Hooks::o_ListLeavesInBox( pEcx, vecMin, vecMax, pList, iMax );

	C_BaseEntity* pBaseEntity = pRenderableInfo->m_pRenderable->GetIClientUnknown( )->GetBaseEntity( );
	if ( !pBaseEntity || !pBaseEntity->IsPlayer( ) )
		return Hooks::o_ListLeavesInBox( pEcx, vecMin, vecMax, pList, iMax );

	pRenderableInfo->m_Flags &= ~0x100;
	pRenderableInfo->m_Flags2 |= 0xC0;

	return Hooks::o_ListLeavesInBox( pEcx, Vector( -16384.0f, -16384.0f, -16384.0f ), Vector( 16384.0f, 16384.0f, 16384.0f ), pList, iMax );
}
void C_Hooks::hkProcessInterpolatedList( )
{
	**( bool** ) ( SDK::EngineData::m_AddressList[ CheatAddressList::Extrapolate ] ) = false;
	return Hooks::o_ProcessInterpolatedList( );
}
void C_Hooks::hkShouldDrawFOG( )
{
	if ( !g_SettingsManager->B[ _S( "misc.removals.item_2" ) ] || g_SettingsManager->B[ _S( "misc.fog.enable" ) ] )
		return Hooks::o_ShouldDrawFog( );
}

void __cdecl C_Hooks::hkRecvFlashDurationProxy( CRecvProxyData* Proxy, void* pStruct, void* pOut )
{
	Hooks::m_FlashDurationProxy->GetOriginal( )( Proxy, pStruct, pOut );
	if ( g_SettingsManager->B[ _S( "misc.removals.item_9" ) ] )
		*( float* ) ( pOut ) = 0.0f;
}
void __cdecl C_Hooks::hkRecvVelocityModifierProxy( CRecvProxyData* Proxy, void* pStruct, void* pOut )
{
	Hooks::m_VelocityModifierProxy->GetOriginal( )( Proxy, pStruct, pOut );
	if ( pStruct != g_Globals->m_LocalPlayer )
		return;

	g_EnginePrediction->OnVelocityModifierProxy( Proxy->m_Value.m_Float );
	if ( g_SettingsManager->B[ _S( "ragebot.enable" ) ] )
		g_RageBot->OnVelocityModifierProxy( Proxy->m_Value.m_Float );
}
void __cdecl C_Hooks::hkSimulationTime( CRecvProxyData* Proxy, void* pStruct, void* pOut )
{
	/* Get entity */
	C_BaseEntity* pEntity = static_cast < C_BaseEntity* > ( pStruct );
	if ( !pEntity || !pEntity->IsPlayer( ) )
		return Hooks::m_SimulationTimeProxy->GetOriginal( )( Proxy, pStruct, pOut );

	/* Decode player's tickbase */
	int nPlayerTickBase = Proxy->m_Value.m_Int + SDK::Interfaces::GlobalVars->GetNetworkBase( SDK::Interfaces::GlobalVars->m_iTickCount, pEntity->EntIndex( ) );
	while ( nPlayerTickBase < SDK::Interfaces::GlobalVars->m_iTickCount + 127 )
		nPlayerTickBase += 256;
	while ( nPlayerTickBase > SDK::Interfaces::GlobalVars->m_iTickCount + 127 )
		nPlayerTickBase -= 256;

	/* Change the time and handle time changing */
	float flPlayerSimulationTime = g_Utils->TicksToTime( nPlayerTickBase );
	if ( flPlayerSimulationTime != pEntity->m_flSimulationTime( ) )
	{
		pEntity->OnSimulationTimeChanging( pEntity->m_flSimulationTime( ), flPlayerSimulationTime );
		pEntity->m_flSimulationTime( ) = flPlayerSimulationTime;
	}
}
void __cdecl C_Hooks::hkLifeStateProxy( CRecvProxyData* Proxy, void* pStruct, void* pOut )
{
	/* return original */
	Hooks::m_LifeStateProxy->GetOriginal( )( Proxy, pStruct, pOut );

	/* detect player respawn */
	if ( Proxy->m_Value.m_Int == LifeState::LIFE_ALIVE )
	{
		static_cast < C_BasePlayer* > ( pStruct )->GetAnimationState( )->Reset( );
		if ( pStruct == g_Globals->m_LocalPlayer )
		{
			/* reset death notices */
			g_Globals->m_RoundInfo.m_bShouldClearDeathNotices = true;

			/* reset data */
			g_DormantSystem->ResetData( );
			g_LocalAnimations->ResetData( );
			g_AutoStop->ResetData( );
			g_RageBot->ResetData( );
			
			/* fix player's vision */
			( static_cast < C_BasePlayer* > ( pStruct ) )->SetAbsOrigin( ( static_cast < C_BasePlayer* >( pStruct ) )->m_vecOrigin( ) );
		}
		else
			g_LagCompensation->GetPlayerData( ( static_cast < C_BasePlayer* >( pStruct ) ) ).m_bLeftDormancy = true;
	}
}
void __cdecl C_Hooks::hkSequenceProxy( CRecvProxyData* Proxy, void* pStruct, void* pOut )
{
	g_InventoryChanger->SequenceRemapping( Proxy, static_cast< C_BaseViewmodel* >( pStruct ) );
	Hooks::m_SequenceProxy->GetOriginal( )( Proxy, pStruct, pOut );
}
void __cdecl C_Hooks::hkRecvTickbaseProxy( CRecvProxyData* Proxy, void* pStruct, void* pOut )
{
	Hooks::m_TickbaseProxy->GetOriginal( )( Proxy, pStruct, pOut );
	return g_Exploits->OnTickBaseProxy( Proxy->m_Value.m_Int );
}