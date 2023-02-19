#include "../Hooks.hpp"
#include "../../Features/Visuals/World/World.hpp"
#include "../../Features/Visuals/World/Radar.hpp"
#include "../../Features/Visuals/Grenades/Warning.hpp"
#include "../../Features/Misc/Movement/Movement.hpp"
#include "../../Features/Misc/LuaAPI/LuaHooks.hpp"
#include "../../Features/Misc/Fakelag/Fakelag.hpp"
#include "../../Features/Game/Prediction/EnginePrediction.hpp"
#include "../../Features/Game/Dormant/DormantSystem.hpp"
#include "../../Features/Game/Networking/Networking.hpp"
#include "../../Features/Game/Exploits/Exploits.hpp"
#include "../../Features/Game/RageBot/RageBot.hpp"
#include "../../Features/Game/RageBot/Autowall.hpp"
#include "../../Features/Game/RageBot/Antiaim.hpp"
#include "../../Features/Game/RageBot/Shots.hpp"
#include "../../Features/Game/RageBot/AutoStop.hpp"
#include "../../Features/Game/RageBot/LagCompensation.hpp"
#include "../../Features/Game/RageBot/LocalAnimations.hpp"
#include "../../Features/Game/RageBot/Animations.hpp"
#include "../../SDK/Utils/Utils.hpp"
#include "../../Libraries/Ethernet API/Dependecies/json.hpp"
#include "../../Features/Game/Viewmodel/Viewmodel.hpp"
#include "../../Features/Game/Revolver/Revolver.hpp"
#include "../../Features/Game/Interpolation/Interpolation.hpp"
#include "../../Features/Inventory Changer/InventoryChanger.hpp"
#include "../../Features/Misc/Voice Communication/VoiceCommunication.hpp"
#include "../../Features/Misc/Buy Bot/BuyBot.hpp"
#include "../../Features/Misc/Events/Events.hpp"

#ifdef OVERSEE_DEV
void __declspec( noinline ) DrawHitboxes( uintptr_t pPlayer, float fDuration, void* sig )
{
	__asm
	{
		pushad
		movss xmm1, fDuration
		push 1
		mov ecx, pPlayer
		call sig
		popad
	}
}
uintptr_t _declspec( noinline ) GetPlayerByIndex( int nIndex )
{
	static void* sig2 = reinterpret_cast< void* >( g_Utils->PatternScan( "server.dll", "85 C9 7E 32 A1 ? ? ? ?" ) );
	typedef uintptr_t( __fastcall* util_player_by_index_t ) ( unsigned int );
	util_player_by_index_t util_player_by_index = reinterpret_cast< util_player_by_index_t >( sig2 );
	return util_player_by_index( nIndex );
}
void __declspec( noinline ) DrawServerModel( )
{
	static void* sig = reinterpret_cast< void* >( g_Utils->PatternScan( "server.dll", "55 8B EC 81 EC ? ? ? ? 53 56 8B 35 ? ? ? ? 8B D9 57 8B CE" ) );
	for ( int nPlayerID = 1; nPlayerID <= SDK::Interfaces::GlobalVars->m_iMaxClients; nPlayerID++ )
	{
		uintptr_t player = GetPlayerByIndex( nPlayerID );
		if ( !player )
			return;

		DrawHitboxes( player, -1.0f, sig );
	}
}
#endif

void FASTCALL C_Hooks::hkFrameStageNotify( LPVOID pEcx, uint32_t, ClientFrameStage_t Stage )
{
	/* get screen parameters */
	if ( !g_Globals->m_ScreenData.m_nScreenSizeX || !g_Globals->m_ScreenData.m_nScreenSizeY )
		SDK::Interfaces::EngineClient->GetScreenSize( g_Globals->m_ScreenData.m_nScreenSizeX, g_Globals->m_ScreenData.m_nScreenSizeY );

	/* run filters */
	g_World->RunFilters( );

	/* get localplayer */
	g_Globals->m_LocalPlayer = C_BasePlayer::GetPlayerByIndex( SDK::Interfaces::EngineClient->GetLocalPlayer( ) );
	if ( !g_Globals->m_LocalPlayer )
		return Hooks::o_FrameStageNotify( pEcx, Stage );

#ifdef OVERSEE_DEV
	if ( Stage == FRAME_NET_UPDATE_END )
	{
		if ( g_Globals->m_LocalPlayer->IsAlive( ) )
		{
			uintptr_t Local = GetPlayerByIndex( SDK::Interfaces::EngineClient->GetLocalPlayer( ) );
			if ( Local )
				g_Globals->m_Packet.m_nTicksAllowed = *( int* ) ( Local + 0x126C );

			DrawServerModel( );
		}
	}
#endif

	/* run LUA */
	if ( g_Globals->m_Lua.m_bScriptLoaded )
		for ( auto pCurrent : g_LuaHookManager->GetHooksList( _S( "FrameStageNotify" ) ) )
			pCurrent.m_pFunc( Stage );

	/* run inventory changer */
	g_InventoryChanger->OnFrameStageNotify( Stage );

	/* ragebot part */
	g_LagCompensation->Instance( Stage );
	g_ShotSystem->OnFrameStageNotify( Stage );
	g_EnginePrediction->ModifyDatamap( );

	/* modulate the world */
	g_World->OnFrameStageNotify( Stage );

	/* run local interp fix */
	if ( Stage == FRAME_RENDER_START )
		g_LocalInterpolation->SetupInterpolation( false );

	// call orig
	Hooks::o_FrameStageNotify( pEcx, Stage );

	/* run local interp fix */
	if ( Stage == FRAME_RENDER_START )
		g_LocalInterpolation->SetupInterpolation( true );

	/* voice communication feature */
	if ( Stage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START )
	{
		g_VoiceCommunication->RunSharedESP( );
		g_VoiceCommunication->RunSharedChamsData( );
	}
	
	/* force viewmodel data from prediction */
	if ( Stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START )
		g_ViewmodelAnimation->ForceViewmodelData( );
	
	return g_EnginePrediction->OnFrameStageNotify( Stage );
}
bool FASTCALL C_Hooks::hkWriteUsercmdDeltaToBuffer( LPVOID pEcx, uint32_t, int nSlot, bf_write* lpBuffer, int nFrom, int nTo, bool bIsNew )
{
	return g_Exploits->WriteUsercmdDeltaToBuffer( lpBuffer, nSlot, nFrom, nTo, bIsNew );
}
void STDCALL C_Hooks::hkCreateMove( int iSequence, float flFrametime, bool bIsActive, bool& bSendPacket )
{
	Hooks::o_CreateMove( iSequence, flFrametime, bIsActive );
	if ( !g_Globals->m_LocalPlayer || !g_Globals->m_LocalPlayer->IsAlive( ) || SDK::Interfaces::ClientState->m_nSignonState < 6 )
	{
		Hooks::o_CreateMove( iSequence, flFrametime, bIsActive );
		return;
	}

	/* it is required for several fixes */
	g_Globals->m_Packet.m_bInCreateMove = true;

	/* Reset Globals Data */
	g_Globals->m_Packet.m_bIsValidPacket = true;
	g_Globals->m_Packet.m_bSkipMatrix = false;

	/* reset DT state */
	g_Globals->m_Packet.m_bDoubleTap = false;

	/* Force packet */
	if ( g_Globals->m_Packet.m_bForcePacket )
		g_Globals->m_Packet.m_bIsValidPacket = false;
	g_Globals->m_Packet.m_bForcePacket = false;

	/* get sequence */
	g_Globals->m_Packet.m_nSequence = iSequence;

	/* update prediction */
	g_EnginePrediction->UpdatePrediction( );

	/* get cmd */
	g_Globals->m_Packet.m_pCmd = SDK::Interfaces::Input->GetUserCmd( g_Globals->m_Packet.m_nSequence );
	if ( !g_Globals->m_Packet.m_pCmd || !g_Globals->m_Packet.m_pCmd->m_nCommand )
	{
		Hooks::o_CreateMove( g_Globals->m_Packet.m_nSequence, flFrametime, bIsActive );
		return;
	}

	/* infinty duck */
	if ( g_SettingsManager->B[ _S( "misc.infinity_duck" ) ] )
		g_Globals->m_Packet.m_pCmd->m_nButtons |= IN_BULLRUSH;

	/* remove attack buttons in menu */
	static bool bWasVisible = false;
	if ( g_Menu->IsVisible( ) )
	{
		g_Globals->m_Packet.m_pCmd->m_nButtons &= ~( IN_ATTACK | IN_ATTACK2 );
		bWasVisible = true;
	}
	else if ( bWasVisible )
	{
		g_Globals->m_Packet.m_pCmd->m_nButtons &= ~( IN_ATTACK | IN_ATTACK2 );
		bWasVisible = false;
	}

	/* setup ragebot weapon && cache weapondata in autowall */
	g_AutoWall->CacheWeaponData( );
	g_RageBot->SetupConfiguration( );

	/* run alternative createmove here */
	g_Exploits->SetupExploits( );
	if ( g_Exploits->RunExploits( ) )
	{
		g_Globals->m_Packet.m_bInCreateMove = false;
		return;
	}

	/* save sendpacket pointer */
	g_Globals->m_Packet.m_bSendPacket = &bSendPacket;

	/* setup data for createmove features */
	{
		// backup players data
		g_LagCompensation->StartLagCompensation( );

		// reset fakelag
		g_FakeLag->GetData( )->m_bFakeLag = g_SettingsManager->B[ _S( "misc.fake_lag.enable" ) ];
		g_FakeLag->GetData( )->m_nChokeAmount = g_SettingsManager->I[ _S( "misc.fake_lag.amount" ) ];
		g_FakeLag->GetData( )->m_nPacketMode = EPacketMode::Defualt;
		g_FakeLag->GetData( )->m_bIsChoking = false;
		g_FakeLag->GetData( )->m_nCustomChoke = -1;
	}

	/* run movement features */
	{
		/* default movement features */
		g_Movement->BunnyHop( );
		g_Movement->FastAutoStop( );
		g_Movement->MouseCorrections( );
		g_Movement->AutoStrafe( );

		/* rage-related movement features */
		g_AntiAim->SlowWalk( );
		g_AntiAim->MicroMovement( );
		g_AntiAim->JitterMove( );
		g_AntiAim->LegMovement( );
	}

	/* run fakeduck and required packet fix for it*/
	{
		g_FakeLag->FakeDuck( );
		g_Networking->OnFakeDuck( );
		g_Exploits->OnFakeDuck( );
	}

	/* run LUA scripts */
	if ( g_Globals->m_Lua.m_bScriptLoaded )
		for ( auto pCurrent : g_LuaHookManager->GetHooksList( _S( "CreateMove_Start" ) ) )
			pCurrent.m_pFunc( g_Globals->m_Packet.m_pCmd );

	/* run main cheat features here */
	g_EnginePrediction->StartPrediction( );
	{
		/* setup shoot position */
		g_LocalAnimations->CopyPlayerAnimationData( false );
		g_LocalAnimations->SetupShootPosition( );

		/* run auto-stop */
		{
			g_AutoStop->Initialize( );
			g_AutoStop->RunEarlyAutoStop( );
			g_AutoStop->RunMovementPrediction( );
		}

		/* run fakelag */
		g_FakeLag->Instance( );

		/* run grenade prediction */
		g_GrenadePrediction->OnCreateMove( g_Globals->m_Packet.m_pCmd );

		/* Run exploits before rage */
		g_Exploits->BeforeRageBot( );

		/* Run RageBot things */
		g_RageBot->Instance( );

		/* Run exploits post rage */
		g_Exploits->PostRageBot( );

		/* run anti-aim */
		g_AntiAim->OnCreateMove( );
	};
	g_EnginePrediction->FinishPrediction( );

	/* finish createmove */
	g_AntiAim->FinishCreateMove( );

	/* run buybot */
	g_BuyBot->OnCreateMove( );

	/* run LUA scripts */
	if ( g_Globals->m_Lua.m_bScriptLoaded )
		for ( auto pCurrent : g_LuaHookManager->GetHooksList( _S( "CreateMove_End" ) ) )
			pCurrent.m_pFunc( g_Globals->m_Packet.m_pCmd );

	// finish packet ( Clamp choke, run local animfix, restore data, etc... "
	return g_Networking->FinishPacket( iSequence );
}
void FASTCALL C_Hooks::hkLevelInitPreEntity( LPVOID pEcx, uint32_t, const char* szNewMapName )
{
	/* force update */
	g_Globals->m_Inventory.m_bNeedFullUpdate = true;
	
	/* parse radar image */
	g_Radar->OnMapUpdate( szNewMapName );
	
	/* reset cheat data*/
	g_CheatEvents->ResetCheatData( );

	return Hooks::o_LevelInitPreEntity( pEcx, szNewMapName );
}
__declspec( naked ) void __stdcall C_Hooks::hkCreateMove_Handler( int iSequence, float flFrameTime, bool bIsActive )
{
	__asm
	{
		push ebx
		push esp
		push dword ptr[ esp + 8 + 8 + 4 ]
		push dword ptr[ esp + 12 + 8 ]
		push[ esp + 16 + 4 ]
		call hkCreateMove
		pop ebx
		retn 12
	}
}