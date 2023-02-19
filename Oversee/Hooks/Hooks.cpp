#include "Hooks.hpp"
#include "../SDK/Utils/Utils.hpp"

#ifndef OVERSEE_DEV
#define CreateHook( Pointer, Function, Original ) if ( MH_CreateHook( ( LPVOID )( Pointer ), ( LPVOID )( Function ), ( LPVOID* )( &Original ) ) != MH_OK ) { return; }
#endif
#ifdef OVERSEE_DEV
#define CreateHook( Pointer, Function, Original ) \
Status = MH_CreateHook( ( LPVOID )( Pointer ), ( LPVOID )( Function ), ( LPVOID* )( &Original ) );\
if ( Status != MH_OK ) \
{ \
	MessageBoxA( NULL, #Function, std::to_string( Status ).c_str( ), MB_ICONERROR ); \
	return;\
}
#endif

void __declspec( noinline ) CreateProxyHooks( )
{
	auto CreateProxyHook = [ & ] ( const std::string& szTable, const std::string& szPropName, const RecvVarProxyFn Function ) -> C_RecvHook*
	{
		/* get netvar prop */
		RecvProp* m_Prop = NetVars::GetNetvarProp( szTable, szPropName );
		if ( !m_Prop )
		{
		#ifdef OVERSEE_DEV
			MessageBoxA( NULL, ( szPropName + ( std::string ) ( _S( "->" ) ) + szPropName ).c_str( ), _S( "Failed to hook" ), MB_ICONERROR );
		#endif

			return nullptr;
		}

		/* the hook has been correctly created */
		return new C_RecvHook( m_Prop, Function );
	};

	/* setup hooks */
	Hooks::m_FlashDurationProxy = CreateProxyHook( _S( "DT_CSPlayer" ), _S( "m_flFlashDuration" ), C_Hooks::hkRecvFlashDurationProxy );
	Hooks::m_VelocityModifierProxy = CreateProxyHook( _S( "DT_CSPlayer" ), _S( "m_flVelocityModifier" ), C_Hooks::hkRecvVelocityModifierProxy );
	Hooks::m_SequenceProxy = CreateProxyHook( _S( "DT_BaseViewModel" ), _S( "m_nSequence" ), C_Hooks::hkSequenceProxy );
	Hooks::m_TickbaseProxy = CreateProxyHook( _S( "DT_BasePlayer" ), _S( "m_nTickBase" ), C_Hooks::hkRecvTickbaseProxy );
	Hooks::m_LifeStateProxy = CreateProxyHook( _S( "DT_BasePlayer" ), _S( "m_lifeState" ), C_Hooks::hkLifeStateProxy );
	Hooks::m_SimulationTimeProxy = CreateProxyHook( _S( "DT_BaseEntity" ), _S( "m_flSimulationTime" ), C_Hooks::hkSimulationTime );
}
#define GetPointerFromVirtual( Pointer, Virtual ) ( void* ) ( ( *( uintptr_t** ) ( Pointer ) )[ Virtual ] )
void C_Hooks::Initialize( )
{
	MH_STATUS Status = MH_Initialize( );
	if ( Status != MH_OK )
		return;

	void* pEndScene = GetPointerFromVirtual( SDK::Interfaces::DirectDevice, 42 );
	void* pReset = GetPointerFromVirtual( SDK::Interfaces::DirectDevice, 16 );
	void* pLockCursor = GetPointerFromVirtual( SDK::Interfaces::Surface, 67 );
	void* pPaintTraverse = GetPointerFromVirtual( SDK::Interfaces::Panel, 41 );
	void* pDrawModelExecute = GetPointerFromVirtual( SDK::Interfaces::ModelRender, 21 );
	void* pStudioDrawModelExecute = GetPointerFromVirtual( SDK::Interfaces::StudioRender, 29 );
	void* pLevelInitPreEntity = GetPointerFromVirtual( SDK::Interfaces::CHLClient, 5 );
	void* pFrameStageNotify = GetPointerFromVirtual( SDK::Interfaces::CHLClient, 37 );
	void* pDispatchUserMessage = GetPointerFromVirtual( SDK::Interfaces::CHLClient, 38 );
	void* pCreateMove = GetPointerFromVirtual( SDK::Interfaces::CHLClient, 22 );
	void* pWriteUsercmdDeltaToBuffer = GetPointerFromVirtual( SDK::Interfaces::CHLClient, 24 );
	void* pOverrideView = GetPointerFromVirtual( SDK::Interfaces::ClientMode, 18 );
	void* pGetViewmodelFOV = GetPointerFromVirtual( SDK::Interfaces::ClientMode, 35 );
	void* pInPrediction = GetPointerFromVirtual( SDK::Interfaces::Prediction, 14 );
	void* pRunCommand = GetPointerFromVirtual( SDK::Interfaces::Prediction, 19 );
	void* pIsEnginePaused = GetPointerFromVirtual( SDK::Interfaces::EngineClient, 90 );
	void* pGetScreenSizeAspectRatio = GetPointerFromVirtual( SDK::Interfaces::EngineClient, 101 );
	void* pIsConnected = GetPointerFromVirtual( SDK::Interfaces::EngineClient, 27 );
	void* pListLeavesInBox = GetPointerFromVirtual( SDK::Interfaces::EngineClient->GetBSPTreeQuery( ), 6 );
	void* pProcessMovement = GetPointerFromVirtual( SDK::Interfaces::GameMovement, 1 );
	void* pEmitSound = GetPointerFromVirtual( SDK::Interfaces::EngineSound, 5 );
	void* pOnScreenSizeChanged = GetPointerFromVirtual( SDK::Interfaces::Surface, 116 );
	void* pDrawSetColor = GetPointerFromVirtual( SDK::Interfaces::Surface, 15 );
	void* pEquipItemInLoadout = GetPointerFromVirtual( SDK::Interfaces::InventoryManager, 20 );
	void* pSetItemInBackpack = GetPointerFromVirtual( SDK::Interfaces::InventoryManager, 26 );
	void* pParamsGetArgAsString = GetPointerFromVirtual( SDK::Interfaces::PanoramaHelper, 7 );
	void* pSoUpdated = GetPointerFromVirtual( SDK::Interfaces::InventoryManager->GetLocalPlayerInventory( ), 1 );
	void* pSvCheats_GetBool = GetPointerFromVirtual( SDK::Interfaces::CVar->FindVar( _S( "sv_cheats" ) ), 13 );
	void* pClDoResetLatch_GetBool = GetPointerFromVirtual( SDK::Interfaces::CVar->FindVar( _S( "cl_pred_doresetlatch" ) ), 13 );
	void* pDeleteItem = SDK::EngineData::m_AddressList[ CheatAddressList::DeleteItem ];
	void* pUpdateGeneratedMaterial = SDK::EngineData::m_AddressList[ CheatAddressList::UpdateGeneratedMaterial ];
	void* pPreCacheCustomMaterials = SDK::EngineData::m_AddressList[ CheatAddressList::PreCacheCustomMaterials ];
	void* pPacketStart = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "engine.dll" ), _S( "56 8B F1 E8 ? ? ? ? 8B 8E ? ? ? ? 3B 8E ? ? ? ?" ) ) ) - 0x20 );
	void* pPacketEnd = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "engine.dll" ), _S( "56 8B F1 E8 ? ? ? ? 8B 8E ? ? ? ? 3B 8E ? ? ? ?" ) ) ) );
	void* pCL_Move = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "engine.dll" ), _S( "55 8B EC 81 EC 64 01 00 00 53 56 8A F9" ) ) ) );
	void* pProcessTempEntities = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "engine.dll" ), _S( "55 8B EC 83 E4 F8 83 EC 4C A1 ? ? ? ? 80 B8" ) ) ) );
	void* pIsUsingDebugStaticProps = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "engine.dll" ), _S( "8B 0D ? ? ? ? 81 F9 ? ? ? ? 75 ? A1 ? ? ? ? 35 ? ? ? ? EB ? 8B 01 FF 50 ? 83 F8 ? 0F 85 ? ? ? ? 8B 0D" ) ) ) );
	void* pSendNetMessage = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "engine.dll" ), _S( "55 8B EC 83 EC 08 56 8B F1 8B 4D 04 E8 ? ? ? ? 8B" ) ) ) );
	void* pClampBonesInBBOX = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 E4 F8 83 EC 70 56 57 8B F9 89 7C 24 38 83" ) ) ) );
	void* pModifyEyePosition = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 E4 F8 83 EC 70 56 57 8B F9 89 7C 24 14 83 7F 60" ) ) ) );
	void* pDoExtraBoneProcessing = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 8B F1 57 89 74 24 1C" ) ) ) );
	void* pCalcViewmodelBob = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC A1 ? ? ? ? 83 EC 10 56 8B F1 B9" ) ) ) );
	void* pPerformScreenOverlay = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 51 A1 ? ? ? ? 53 56 8B D9" ) ) ) );
	void* pShouldDrawFog = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 8B 0D ? ? ? ? 83 EC 0C 8B 01 53 56 57 FF" ) ) ) );
	void* pCalcView = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 EC 14 53 56 57 FF 75 18" ) ) ) );
	void* pGetClientModelRenderable = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "56 8B F1 80 BE FC 26" ) ) ) );
	void* pSetupBones = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 E4 F0 B8 D8" ) ) ) );
	void* pUpdateClientSideAnimation = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74" ) ) ) );
	void* pDoPostScreenEffects = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 8B 49 18 56 8B" ) ) ) );
	void* pPlayerMove = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 EC 08 56 8B F1 8B 8E 54 0E 00 00 E8" ) ) ) );
	void* pGetAlphaModulation = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "materialsystem.dll" ), _S( "56 8B F1 8A 46 20 C0 E8 02 A8 01 75 0B 6A 00 6A 00 6A 00 E8 ? ? ? ? 80 7E 22 05 76 0E" ) ) ) );
	void* pGetColorModulation = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "materialsystem.dll" ), _S( "55 8B EC 83 EC ? 56 8B F1 8A 46" ) ) ) );
	void* pMsgFunc_ProcessSpottedEntityUpdate = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 EC 18 8B 45 08 53 56 57 80 78 18 00" ) ) ) );
	void* pMsgFunc_ReportHit = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 8B 55 08 83 EC 1C F6 42 1C 01" ) ) ) );
	void* pProcessPacket = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "engine.dll" ), _S( "55 8B EC 83 E4 C0 81 EC ? ? ? ? 53 56 57 8B 7D 08 8B D9" ) ) ) );
	void* pSetupAliveLoop = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 51 56 8B 71 60 83 BE 9C 29 00 00 00 0F 84 93 00 00 00 8B" ) ) ) );
	void* pSVCMsg_VoiceData = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "engine.dll" ), _S( "55 8B EC 83 E4 F8 A1 ? ? ? ? 81 EC 84 01 00" ) ) ) );
	void* pSVCMsg_GameEvent = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "engine.dll" ), _S( "55 8B EC A1 ? ? ? ? B9 ? ? ? ? 56 57 8B" ) ) ) );
	void* pSendDatagram = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "engine.dll" ), _S( "55 8B EC 83 E4 F0 B8 38 01 10 00 E8" ) ) ) );
	void* pCL_ReadPackets = SDK::EngineData::m_AddressList[ CheatAddressList::CL_ReadPackets ];
	void* pInterpolateServerEntities = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 EC 1C 8B 0D ? ? ? ? 53 56 57" ) ) ) );
	void* pProcessInterpolatedList = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "0F B7 05 ? ? ? ? 3D FF FF 00 00 74 3F 56 57" ) ) ) );
	void* pInitNewParticlesScalar = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 EC 18 56 8B F1 C7 45" ) ) ) );
	void* pNotePredictionError = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 EC 0C 56 8B F1 8B 06 8B 80 70 02" ) ) ) );

	/* save createmove ptr */
	SDK::EngineData::m_AddressList[ CheatAddressList::CreateMove ] = pCreateMove;

	// setup default MH hooks
	CreateHook( pEndScene,								C_Hooks::hkEndScene								, Hooks::o_EndScene								);
	CreateHook( pReset,									C_Hooks::hkReset								, Hooks::o_Reset								);
	CreateHook( pLockCursor,							C_Hooks::hkLockCursor							, Hooks::o_LockCursor							);
	CreateHook( pPaintTraverse,							C_Hooks::hkPaintTraverse						, Hooks::o_PaintTraverse						);
	CreateHook( pDrawModelExecute,						C_Hooks::hkDrawModelExecute						, Hooks::o_DrawModelExecute						);
	CreateHook( pStudioDrawModelExecute,				C_Hooks::hkStudioDrawModelExecute				, Hooks::o_StudioDrawModelExecute				);
	CreateHook( pFrameStageNotify,						C_Hooks::hkFrameStageNotify						, Hooks::o_FrameStageNotify						);
	CreateHook( pCreateMove,							C_Hooks::hkCreateMove_Handler					, Hooks::o_CreateMove							);
	CreateHook( pWriteUsercmdDeltaToBuffer,				C_Hooks::hkWriteUsercmdDeltaToBuffer			, Hooks::o_WriteUsercmdDeltaToBuffer			);
	CreateHook( pLevelInitPreEntity,					C_Hooks::hkLevelInitPreEntity					, Hooks::o_LevelInitPreEntity					);
	CreateHook( pGetViewmodelFOV,						C_Hooks::hkGetViewmodelFOV						, Hooks::o_GetViewmodelFOV						);
	CreateHook( pDoPostScreenEffects,					C_Hooks::hkDoPostScreenEffects					, Hooks::o_DoPostScreenEffects					);
	CreateHook( pOverrideView,							C_Hooks::hkOverrideView							, Hooks::o_OverrideView							);
	CreateHook( pInPrediction,							C_Hooks::hkInPrediction							, Hooks::o_InPrediction_GetBool					);
	CreateHook( pRunCommand,							C_Hooks::hkRunCommand							, Hooks::o_RunCommand							);
	CreateHook( pGetScreenSizeAspectRatio,				C_Hooks::hkGetScreenSizeAspectRatio				, Hooks::o_GetScreenSizeAspectRatio				);
	CreateHook( pSvCheats_GetBool,						C_Hooks::hkSvCheats_GetBool						, Hooks::o_SvCheats_GetBool						);
	CreateHook( pClDoResetLatch_GetBool,				C_Hooks::hkClPredDoResetLatch					, Hooks::o_ClPredDoResetLatch_GetBool			);
	CreateHook( pPacketStart,							C_Hooks::hkPacketStart							, Hooks::o_PacketStart							);
	CreateHook( pPacketEnd,								C_Hooks::hkPacketEnd							, Hooks::o_PacketEnd							);
	CreateHook( pNotePredictionError,					C_Hooks::hkNotePredictionError					, Hooks::o_NotePredictionError					);
	CreateHook( pSetupAliveLoop,						C_Hooks::hkSetupAliveLoop						, Hooks::o_SetupAliveLoop						);
	CreateHook( pProcessTempEntities,					C_Hooks::hkProcessTempEntities					, Hooks::o_ProcessTempEntities					);
	CreateHook( pProcessMovement,						C_Hooks::hkProcessMovement						, Hooks::o_ProcessMovement						);
	CreateHook( pDoExtraBoneProcessing,					C_Hooks::hkDoExtraBoneProcessing				, Hooks::o_DoExtraBoneProcessing				);
	CreateHook( pCalcViewmodelBob,						C_Hooks::hkCalcViewmodelBob						, Hooks::o_CalcViewmodelBob						);
	CreateHook( pDrawSetColor,							C_Hooks::hkDrawSetColor							, Hooks::o_DrawSetColor							);
	CreateHook( pCalcView,								C_Hooks::hkCalcView								, Hooks::o_CalcView								);
	CreateHook( pUpdateClientSideAnimation,				C_Hooks::hkUpdateClientSideAnimation			, Hooks::o_UpdateClientSideAnimation			);
	CreateHook( pSetupBones,							C_Hooks::hkSetupBones							, Hooks::o_SetupBones							);
	CreateHook( pClampBonesInBBOX,						C_Hooks::hkClampBonesInBBOX						, Hooks::o_ClampBonesInBBOX						);
	CreateHook( pGetAlphaModulation,					C_Hooks::hkGetAlphaModulation					, Hooks::o_GetAlphaModulation					);
	CreateHook( pGetColorModulation,					C_Hooks::hkGetColorModulation					, Hooks::o_GetColorModulation					);
	CreateHook( pIsUsingDebugStaticProps,				C_Hooks::hkIsUsingDebugStaticProps				, Hooks::m_IsUsingDebugStaticProps				);
	CreateHook( pOnScreenSizeChanged,					C_Hooks::hkOnScreenSizeChanged					, Hooks::o_OnScreenSizeChanged					);
	CreateHook( pModifyEyePosition,						C_Hooks::hkModifyEyePosition					, Hooks::o_ModifyEyePosition					);
	CreateHook( pDispatchUserMessage,					C_Hooks::hkDispatchUserMessage					, Hooks::o_DispatchUserMessage					);
	CreateHook( pPerformScreenOverlay,					C_Hooks::hkPerformScreenOverlay					, Hooks::o_PerformScreenOverlay					);
	CreateHook( pListLeavesInBox,						C_Hooks::hkListLeavesInBox						, Hooks::o_ListLeavesInBox						);
	CreateHook( pShouldDrawFog,							C_Hooks::hkShouldDrawFOG						, Hooks::o_ShouldDrawFog						);
	CreateHook( pGetClientModelRenderable,				C_Hooks::hkGetClientModelRenderable				, Hooks::o_DoExtraBoneProcessing				);
	CreateHook( pCL_Move,								C_Hooks::hkCL_Move								, Hooks::o_CL_Move								);
	CreateHook( pSendNetMessage,						C_Hooks::hkSendNetMessage						, Hooks::o_SendNetMessage						);
	CreateHook( pSendDatagram,							C_Hooks::hkSendDatagram							, Hooks::o_SendDatagram							);
	CreateHook( pEmitSound,								C_Hooks::hkEmitSound							, Hooks::o_EmitSound							);
	CreateHook( pPlayerMove,							C_Hooks::hkPlayerMove							, Hooks::o_PlayerMove							);
	CreateHook( pMsgFunc_ProcessSpottedEntityUpdate,	C_Hooks::hkMsgFunc_ProcessSpottedEntityUpdate	, Hooks::o_MsgFunc_ProcessSpottedEntityUpdate	);
	CreateHook( pMsgFunc_ReportHit,						C_Hooks::hkMsgFunc_ReportHit					, Hooks::o_MsgFunc_ReportHit					);
	CreateHook( pInterpolateServerEntities,				C_Hooks::hkInterpolateServerEntities			, Hooks::o_InterpolateServerEntities			);
	CreateHook( pSVCMsg_VoiceData,						C_Hooks::hkSVCMsg_VoiceData						, Hooks::o_SVCMsg_VoiceData						);
	CreateHook( pSVCMsg_GameEvent,						C_Hooks::hkSVCMsg_GameEvent						, Hooks::o_SVCMsg_GameEvent						);
	CreateHook( pProcessInterpolatedList,				C_Hooks::hkProcessInterpolatedList				, Hooks::o_ProcessInterpolatedList				);
	CreateHook( pEquipItemInLoadout,					C_Hooks::hkEquipItemInLoadout					, Hooks::o_EquipItemInLoadout					);
	CreateHook( pSoUpdated,								C_Hooks::hkSoUpdated							, Hooks::o_SoUpdated							);
	CreateHook( pSetItemInBackpack,						C_Hooks::hkSetItemBackpackPosition				, Hooks::o_SetItemBackpackPosition				);
	CreateHook( pParamsGetArgAsString,					C_Hooks::hkParamsGetArgAsString					, Hooks::o_ParamsGetArgsAtString				);
	CreateHook( pDeleteItem,							C_Hooks::hkDeleteItem							, Hooks::o_DeleteItem							);
	CreateHook( pUpdateGeneratedMaterial,				C_Hooks::hkUpdateGeneratedMaterial				, Hooks::o_UpdateGeneratedMaterial				);
	CreateHook( pPreCacheCustomMaterials,				C_Hooks::hkPrecacheCustomMaterials				, Hooks::o_DoExtraBoneProcessing				);
	CreateHook( pIsConnected,							C_Hooks::hkIsConnected							, Hooks::o_IsConnected							);
	CreateHook( pCL_ReadPackets,						C_Hooks::hkCL_ReadPackets						, Hooks::o_CL_ReadPackets						);

	// init proxy
	CreateProxyHooks( );

	// enable MH hooks
	MH_EnableHook( MH_ALL_HOOKS );
}