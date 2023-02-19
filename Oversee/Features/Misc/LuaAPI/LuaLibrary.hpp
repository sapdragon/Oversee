#pragma once
#include "../../../SDK/SDK.hpp"
#include "../../../Libraries/ImGui/imgui.h"
#include "../../../Interface/Input/Input.hpp"
#include "../../../Interface/Renderer/Renderer.hpp"
#include "../../../SDK/Utils/Utils.hpp"
#include "../../Fonts/FontManager.hpp"
#include "../Fakelag/Fakelag.hpp"
#include "LuaHooks.hpp"
#include "../../Visuals/Chams/Chams.hpp"


namespace Render
{
	void AddLine( Vector2D min, Vector2D max, Color col, float thickness )
	{
		return g_Renderer->AddLine( ImVec2( min.x, min.y ), ImVec2( max.x, max.y ), ImColor( col.r( ), col.g( ), col.b( ) ), thickness );
	}
	void AddRect( ImVec2 min, ImVec2 max, Color col, float thickness, float rounding, ImDrawCornerFlags corners )
	{
		return g_Renderer->AddRect( min, max, ImColor( col.r( ), col.g( ), col.b( ) ), thickness, rounding, corners );
	}
	void AddRectFilled( ImVec2 min, ImVec2 max, Color col, float rounding, ImDrawCornerFlags corners )
	{
		return g_Renderer->AddRectFilled( min, max, ImColor( col.r( ), col.g( ), col.b( ) ), rounding, corners );
	}
	void AddText( ImFont* font, float font_size, ImVec2 pos, std::string text, Color col )
	{
		return g_Renderer->AddText( font, font_size, pos, text, ImColor( col.r( ), col.g( ), col.b( ) ) );
	}

	Vector2D CalcTextSize( std::string szText )
	{
		return Vector2D( ImGui::CalcTextSize( szText.c_str( ) ).x, ImGui::CalcTextSize( szText.c_str( ) ).y );
	}
}

namespace DebugOverlay
{
	void AddBoxOverlay( Vector vOrigin, Vector vMins, Vector vMax, QAngle value, Color cColor, float flDuration )
	{
		return SDK::Interfaces::DebugOverlay->AddBoxOverlay( vOrigin, vMins, vMax, value, cColor.r( ), cColor.g( ), cColor.b( ), cColor.a( ), flDuration );
	}

	void AddSphereOverlay( Vector vOrigin, float flRadius, int nTheta, int nPhi, Color cColor, float flDuration )
	{
		return SDK::Interfaces::DebugOverlay->AddSphereOverlay( vOrigin, flRadius, nTheta, nPhi, cColor.r( ), cColor.g( ), cColor.b( ), cColor.a( ), flDuration );
	}

	void AddTriangleOverlay( Vector vP1, Vector vP2, Vector vP3, Color cColor, bool bnoDepthTest, float flDuration )
	{
		return SDK::Interfaces::DebugOverlay->AddTriangleOverlay( vP1, vP2, vP3, cColor.r( ), cColor.g( ), cColor.b( ), cColor.a( ), bnoDepthTest, flDuration );
	}

	void AddLineOverlay( Vector vOrigin, Vector vDest, Color cColor, bool bNoDepthTest, float flDuration )
	{
		return SDK::Interfaces::DebugOverlay->AddLineOverlay( vOrigin, vDest, cColor.r( ), cColor.g( ), cColor.b( ), bNoDepthTest, flDuration );
	}

}

namespace EngineClient
{
	void ExecuteClientCmd( std::string szMessage )
	{
		return SDK::Interfaces::EngineClient->ExecuteClientCmd( szMessage.c_str( ) );
	}

	PlayerInfo_t* GetPlayerInfo( int iEntityIndex )
	{
		PlayerInfo_t pTempInfo;
		SDK::Interfaces::EngineClient->GetPlayerInfo( iEntityIndex, &pTempInfo );
		return &pTempInfo;
	}

	std::string GetLevelName( )
	{
		return SDK::Interfaces::EngineClient->GetLevelName( );
	}

	std::string GetLevelNameShort( )
	{
		return SDK::Interfaces::EngineClient->GetLevelNameShort( );
	}

	int GetLocalPlayer( )
	{
		return SDK::Interfaces::EngineClient->GetLocalPlayer( );
	}

	INetChannel* GetNetChannel( )
	{
		return SDK::Interfaces::EngineClient->GetNetChannel( );
	}

	Vector2D GetScreenSize( )
	{
		return Vector2D( ImGui::GetIO( ).DisplaySize.x, ImGui::GetIO( ).DisplaySize.y );
	}

	QAngle GetViewAngles( )
	{
		QAngle TempAng;
		SDK::Interfaces::EngineClient->GetViewAngles( &TempAng );

		return TempAng;
	}

	bool IsConnected( )
	{
		return SDK::Interfaces::EngineClient->IsConnected( );
	}

	bool IsInGame( )
	{
		return SDK::Interfaces::EngineClient->IsInGame( );
	}

	int GetLastTimestamp( )
	{
		return SDK::Interfaces::EngineClient->GetLastTimeStamp( );
	}


}
namespace EntityList
{
	int NumberOfEntities( bool inclNonNetworkable )
	{
		return SDK::Interfaces::EntityList->NumberOfEntities( inclNonNetworkable );
	}

	int GetHighestEntityIndex( )
	{
		return SDK::Interfaces::EntityList->GetHighestEntityIndex( );
	}

	C_BaseEntity* GetClientEntityFromHandle( unsigned long hHandle )
	{
		return  ( C_BaseEntity* ) SDK::Interfaces::EntityList->GetClientEntityFromHandle( hHandle );
	}

	C_BasePlayer* GetLocalPlayer( )
	{
		return g_Globals->m_LocalPlayer;
	}

	C_BaseCombatWeapon* GetWeapon( int iEntityIndex )
	{
		return ( C_BaseCombatWeapon* ) SDK::Interfaces::EntityList->GetClientEntity( iEntityIndex );
	}

	C_BasePlayer* GetPlayerFromHandle( unsigned long hHandle )
	{
		return ( C_BasePlayer* ) SDK::Interfaces::EntityList->GetClientEntityFromHandle( hHandle );
	}

	C_BaseCombatWeapon* GetWeaponFromHandle( unsigned long hHandle )
	{
		return ( C_BaseCombatWeapon* ) SDK::Interfaces::EntityList->GetClientEntityFromHandle( hHandle );
	}

	C_BasePlayer* GetPlayerForUserID( int iPlayerID )
	{
		return ( C_BasePlayer* ) SDK::Interfaces::EntityList->GetClientEntity( SDK::Interfaces::EngineClient->GetPlayerForUserID( iPlayerID ) );
	}


}
namespace GlobalVars
{
	int GetRealTime( )
	{
		return SDK::Interfaces::GlobalVars->m_flRealTime;
	}

	int GetFrameCount( )
	{
		return SDK::Interfaces::GlobalVars->m_iFrameCount;
	}

	float GetAbsoluteFrameTime( )
	{
		return SDK::Interfaces::GlobalVars->m_flAbsFrameTime;
	}

	float GetCurTime( )
	{
		return SDK::Interfaces::GlobalVars->m_flCurTime;
	}

	float GetFrameTime( )
	{
		return SDK::Interfaces::GlobalVars->m_flFrameTime;
	}

	int GetMaxClients( )
	{
		return SDK::Interfaces::GlobalVars->m_iMaxClients;
	}

	int GetTickCount( )
	{
		return SDK::Interfaces::GlobalVars->m_iTickCount;
	}

	float GetIntervalPerTick( )
	{
		return SDK::Interfaces::GlobalVars->m_flIntervalPerTick;
	}

	float GetInterpolationAmount( )
	{
		return SDK::Interfaces::GlobalVars->m_flInterpolationAmount;
	}

	int  GetSimTicksForThisFrame( )
	{
		return SDK::Interfaces::GlobalVars->m_iSimTicksThisFrame;
	}

	int GetNetworkProtocol( )
	{
		return SDK::Interfaces::GlobalVars->m_iNetworkProtocol;
	}

	void* GetSaveData( )
	{
		return SDK::Interfaces::GlobalVars->m_pSaveData;
	}

	bool GetClient( )
	{
		return SDK::Interfaces::GlobalVars->m_bClient;
	}

	bool GetRemoteClient( )
	{
		return SDK::Interfaces::GlobalVars->m_bRemoteClient;
	}
}
namespace ClientState
{
	int GetLastOutgoingCommand( )
	{
		return SDK::Interfaces::ClientState->m_nLastOutgoingCommand;
	}

	int GetChokedCommands( )
	{
		return SDK::Interfaces::ClientState->m_nChokedCommands;
	}

	int GetLastCommandAck( )
	{
		return SDK::Interfaces::ClientState->m_nLastCommandAck;
	}

	int GetCommandAck( )
	{
		return SDK::Interfaces::ClientState->m_nCommandAck;
	}


}
namespace Utils
{
	void* PatternScan( std::string szModule, std::string szPattern )
	{
		return g_Utils->PatternScan( szModule.c_str( ), szPattern.c_str( ) );
	}

	int GetRandomInt( int iMin, int iMax )
	{
		return g_Utils->RandomInt( iMin, iMax );
	}

	float GetRandomFloat( float flMin, float flMax )
	{
		return g_Utils->RandomFloat( flMin, flMax );
	}

	void RandomSeed( int iSeed )
	{
		return g_Utils->RandomSeed( iSeed );
	}

	int GetUnixTime( )
	{
		std::time_t iTemp = std::time( nullptr );
		std::asctime( std::localtime( &iTemp ) );
		return iTemp;
	}

	void* CreateInterface( std::string szModule, std::string szInterface )
	{
		return SDK::GetInterface < void* >( GetModuleHandle( szModule.c_str( ) ), szInterface.c_str( ) );
	}

	bool IsKeyHeld( int iKey )
	{
		return g_InputSystem->IsKeyHeld( iKey );
	}

	bool IsKeyToggled( int iKey )
	{
		return g_InputSystem->IsToggled( iKey );
	}
}
namespace CheatVars
{
	Vector2D GetMousePosition( )
	{
		return Vector2D( ImGui::GetIO( ).MousePos.x, ImGui::GetIO( ).MousePos.y );
	}

	void AddCallBack( sol::this_state s, int ScriptID, std::string szHookName, sol::protected_function pFunc )
	{
		g_LuaHookManager->RegisterHook( szHookName, ScriptID, pFunc );
	}

	void SetBool( std::string param, bool value ) {
		g_SettingsManager->B[ param ] = value;
	}

	void SetInt( std::string param, int value ) {
		g_SettingsManager->I[ param ] = value;
	}

	void SetFloat( std::string param, float value ) {
		g_SettingsManager->F[ param ] = value;
	}

	void SetColor( std::string param, Color value ) {
		g_SettingsManager->C[ param ] = value;
	}

	bool GetBool( std::string param ) {
		return g_SettingsManager->B[ param ];
	}

	int GetInt( std::string param ) {
		return g_SettingsManager->I[ param ];
	}

	float GetFloat( std::string param ) {
		return g_SettingsManager->F[ param ];
	}

	Color GetColor( std::string param ) {
		return g_SettingsManager->C[ param ];
	}
}

namespace CvarManager
{
	ConVar* FindVar( std::string szName )
	{
		return SDK::Interfaces::CVar->FindVar( szName.c_str( ) );
	}
}

namespace FakeLags
{
	void SetPacketMode( int nMode )
	{
		g_FakeLag->SetPacketMode( nMode );
	}
	void SetChokeAmount( int nChokeAmount )
	{
		g_FakeLag->SetChokeAmount( nChokeAmount );
	}
	int GetChokedCommands( )
	{
		return g_FakeLag->GetChokedPackets( );
	}
}

namespace FontManager
{
	void PushFontFromFile( std::string szName, std::string szFontName, int iPixelSize )
	{
		g_FontManager->PushFont( szFontName.c_str( ), FNV1A( szName.c_str( ) ), iPixelSize );
	}

	void PushFontFromData( std::string szName, void* pData, int iPixelSize )
	{
		g_FontManager->PushFont( pData, sizeof( pData ), FNV1A( szName.c_str( ) ), iPixelSize );
	}

	ImFont* GetFont( std::string szName )
	{
		g_FontManager->GetFontFromHash( FNV1A( szName.c_str( ) ) );
	}

	void UpdateFonts( )
	{
		g_FontManager->CreateFonts( );
	}
}

namespace MaterialSystem
{
	IMaterial* CreateMaterial( std::string szName, std::string szContent )
	{
		return g_ChamsSystem->CreateMaterial( szName.c_str( ), szContent.c_str( ) );
	}

	IMaterial* FindMaterial( std::string szName )
	{
		return SDK::Interfaces::MaterialSystem->FindMaterial( szName.c_str( ) );
	}

	unsigned short FirstMaterial( )
	{
		return SDK::Interfaces::MaterialSystem->FirstMaterial( );
	}

	unsigned short NextMaterial( unsigned short hMaterial )
	{
		return SDK::Interfaces::MaterialSystem->NextMaterial( hMaterial );
	}

	IMaterial* GetMaterial( unsigned short hMaterial )
	{
		return SDK::Interfaces::MaterialSystem->GetMaterial( hMaterial );
	}
}