#include "LuaAPI.hpp"
#include "LuaLibrary.hpp"

void C_LuaAPI::Instance( )
{
	// Initialization of Lua
	this->SetupLuaJIT( );

	// Creating tables of game interfaces and init.
	this->SetupLuaTables( );

	// Creating usertypes and init of this
	this->SetupUserTypes( );
}
std::string C_LuaAPI::GetCurrentScript( sol::this_state s )
{
	sol::state_view lua_state( s );
	sol::table rs = lua_state[ "debug" ][ "getinfo" ]( 2, ( "S" ) );
	std::string source = rs[ "source" ];

	MessageBoxA( 0, source.c_str( ), 0, 0 );

	return source;
}

void C_LuaAPI::Load( int id )
{
	if ( id == -1 )
		return;

	if ( Loaded.at( id ) ) //-V106
		return;

	auto LoadError = false;
	Loaded.at( id ) = true;

	std::string szStringPrepared = Scripts.at( id );

	while ( szStringPrepared.find( "GLOBAL_SCRIPTID" ) != std::string::npos )
		szStringPrepared.replace( szStringPrepared.find( "GLOBAL_SCRIPTID" ), 15, std::to_string( id ) );

	pLuaState.script( szStringPrepared,
		[ &LoadError ] ( lua_State*, sol::protected_function_result result )
	{
		if ( !result.valid( ) )
		{
			sol::error error = result;

			auto log = "Lua error: " + ( std::string ) error.what( );
			SDK::Interfaces::CVar->ConsolePrintf( _S( "[ oversee-lua ] %s\n" ), log.c_str( ) );

			LoadError = true;
		}

		return result;
	}
	);

	if ( LoadError || Loaded.at( id ) == false )
	{
		Loaded.at( id ) = false;
		return;
	}
}
void C_LuaAPI::Unload( int id )
{
	if ( id == -1 )
		return;

	if ( !Loaded.at( id ) )
		return;

	g_LuaHookManager->UnregisterHook( id );
	Loaded.at( id ) = false;
}
void C_LuaAPI::UnloadAll( ) {
	for ( auto const& [key, value] : Scripts ) {
		Unload( key );
	}
}
void C_LuaAPI::InEditManager( int id, bool yeah )
{
	if ( id == -1 )
		return;

	if ( yeah ) {
		InEdit.at( id ) = true;
	}
	else {
		InEdit.at( id ) = false;
	}
}
void lua_panic( sol::optional <std::string> message )
{
	if ( !message )
		return;

	std::string log = _S( "Lua error: " ) + message.value_or( _S( "unknown" ) );
	SDK::Interfaces::CVar->ConsolePrintf( _S( "[ oversee ] %s\n" ), log.c_str( ) );
}
void C_LuaAPI::SetupLuaJIT( )
{
	// Initialization sol lua
	pLuaState = sol::state( sol::c_call<decltype( &lua_panic ), &lua_panic> );
	pLuaState.open_libraries( sol::lib::ffi, sol::lib::bit32, sol::lib::math, sol::lib::string, sol::lib::utf8 );

	// Remove trash functions from lua state
	pLuaState[ _S( "collectgarbage" ) ] = sol::nil;
	pLuaState[ _S( "dofilsse" ) ] = sol::nil;
	pLuaState[ _S( "load" ) ] = sol::nil;
	pLuaState[ _S( "loadfile" ) ] = sol::nil;
	pLuaState[ _S( "pcall" ) ] = sol::nil;
	pLuaState[ _S( "print" ) ] = sol::nil;
	pLuaState[ _S( "xpcall" ) ] = sol::nil;
	pLuaState[ _S( "getmetatable" ) ] = sol::nil;
	pLuaState[ _S( "setmetatable" ) ] = sol::nil;
	pLuaState[ _S( "__nil_callback" ) ] = [ ] ( ) { };
}
void C_LuaAPI::SetupUserTypes( )
{
	pLuaState.new_usertype<Vector>( _S( "Vector" ),
		sol::constructors <Vector( ), Vector( float, float, float ), Vector( int, int, int )>( ),
		( std::string ) _S( "x" ), &Vector::x,
		( std::string ) _S( "y" ), &Vector::y,
		( std::string ) _S( "z" ), &Vector::z,
		( std::string ) _S( "Length" ), &Vector::Length,
		( std::string ) _S( "Length2D" ), &Vector::Length2D,
		( std::string ) _S( "DistTo" ), &Vector::DistTo,
		( std::string ) _S( "Dot" ), &Vector::Dot,
		( std::string ) _S( "Cross" ), &Vector::Cross
		);

	pLuaState.new_usertype<Vector2D>( _S( "Vector2" ),
		sol::constructors <Vector2D( ), Vector2D( int, int ), Vector( float, float ) >( ),
		( std::string ) _S( "x" ), &Vector2D::x,
		( std::string ) _S( "y" ), &Vector2D::y,
		( std::string ) _S( "Length" ), &Vector2D::Length
		);

	pLuaState.new_usertype < QAngle >( _S( "QAngle" ),
		sol::constructors <QAngle( ), QAngle( int, int, int ), QAngle( float, float, float ) >( ),
		( std::string ) _S( "pitch" ), &QAngle::pitch,
		( std::string ) _S( "yaw" ), &QAngle::yaw,
		( std::string ) _S( "roll" ), &QAngle::roll,
		( std::string ) _S( "Normalize" ), &QAngle::Normalize,
		( std::string ) _S( "Length" ), &QAngle::Length
		);

	pLuaState.new_usertype< C_UserCmd >( _S( "CUserCmd" ),
		( std::string ) _S( "m_nCommand" ), &C_UserCmd::m_nCommand,
		( std::string ) _S( "m_nTickCount" ), &C_UserCmd::m_nTickCount,
		( std::string ) _S( "m_angViewAngles" ), &C_UserCmd::m_angViewAngles,
		( std::string ) _S( "m_vecAimDirection" ), &C_UserCmd::m_vecAimDirection,
		( std::string ) _S( "m_flForwardMove" ), &C_UserCmd::m_flForwardMove,
		( std::string ) _S( "m_flSideMove" ), &C_UserCmd::m_flSideMove,
		( std::string ) _S( "m_flUpMove" ), &C_UserCmd::m_flUpMove,
		( std::string ) _S( "m_nButtons" ), &C_UserCmd::m_nButtons,
		( std::string ) _S( "m_nImpulse" ), &C_UserCmd::m_nImpulse,
		( std::string ) _S( "m_iWeaponSelect" ), &C_UserCmd::m_iWeaponSelect,
		( std::string ) _S( "m_iWeaponSubType" ), &C_UserCmd::m_iWeaponSubType,
		( std::string ) _S( "m_iRandomSeed" ), &C_UserCmd::m_iRandomSeed,
		( std::string ) _S( "m_nMouseDirectionX" ), &C_UserCmd::m_nMouseDirectionX,
		( std::string ) _S( "m_nMouseDirectionY" ), &C_UserCmd::m_nMouseDirectionY,
		( std::string ) _S( "m_bHasBeenPredicted" ), &C_UserCmd::m_bHasBeenPredicted
		);

	pLuaState.new_usertype < Color >( _S( "Color" ),
		sol::constructors <Color( ), Color( float, float, float ), Color( float, float, float, float )>( ),
		( std::string ) _S( "red" ), &Color::r,
		( std::string ) _S( "green" ), &Color::g,
		( std::string ) _S( "blue" ), &Color::b,
		( std::string ) _S( "FromHSB" ), &Color::FromHSB

		);

	pLuaState.new_usertype<C_ViewSetup >( _S( "CViewSetup" ),
		( std::string ) _S( "m_nX" ), &C_ViewSetup::iX,
		( std::string ) _S( "m_nY" ), &C_ViewSetup::iY,
		( std::string ) _S( "m_nWidth" ), &C_ViewSetup::iWidth,
		( std::string ) _S( "m_nHeight" ), &C_ViewSetup::iHeight,
		( std::string ) _S( "m_flFOV" ), &C_ViewSetup::flFOV,
		( std::string ) _S( "m_vecOrigin" ), &C_ViewSetup::vecOrigin,
		( std::string ) _S( "m_angView" ), &C_ViewSetup::angView
		);

	pLuaState.new_usertype< PlayerInfo_t >( _S( "PlayerInfo" ),
		( std::string ) _S( "IsFakePlayer" ), &PlayerInfo_t::m_bIsFakePlayer,
		( std::string ) _S( "bFilesDownloaded" ), &PlayerInfo_t::m_bFilesDownloaded,
		( std::string ) _S( "bIsHLTV" ), &PlayerInfo_t::m_bIsHLTV,
		( std::string ) _S( "SteamID" ), &PlayerInfo_t::m_iSteamID,
		( std::string ) _S( "SteamID64" ), &PlayerInfo_t::m_iSteamID64,
		( std::string ) _S( "UserId" ), &PlayerInfo_t::m_iUserId,
		( std::string ) _S( "XuidLow" ), &PlayerInfo_t::m_iXuidLow

		);

	pLuaState.new_usertype < ConVar >( _S( "ConVar" ),
		( std::string ) _S( "GetString" ), &ConVar::GetString,
		( std::string ) _S( "GetFloat" ), &ConVar::GetFloat,
		( std::string ) _S( "GetInt" ), &ConVar::GetInt,
		( std::string ) _S( "SetString" ), &ConVar::SetString,
		( std::string ) _S( "SetFloat" ), &ConVar::SetFloat,
		( std::string ) _S( "SetInt" ), &ConVar::SetInt
		);

	pLuaState.new_usertype< IGameEvent >( _S( "GameEvent" ),
		( std::string ) _S( "GetName" ), &IGameEvent::GetName,
		( std::string ) _S( "GetString" ), &IGameEvent::GetString,
		( std::string ) _S( "GetBool" ), &IGameEvent::GetBool,
		( std::string ) _S( "GetInt" ), &IGameEvent::GetInt,
		( std::string ) _S( "GetFloat" ), &IGameEvent::GetFloat,
		( std::string ) _S( "SetString" ), &IGameEvent::SetString,
		( std::string ) _S( "SetBool" ), &IGameEvent::SetBool,
		( std::string ) _S( "SetInt" ), &IGameEvent::SetInt,
		( std::string ) _S( "SetFloat" ), &IGameEvent::SetFloat
		);

	pLuaState.new_usertype< IMaterial >( _S( "Material" ),
		( std::string ) _S( "ColorModulate" ), &IMaterial::ColorModulate,
		( std::string ) _S( "AlphaModulate" ), &IMaterial::AlphaModulate,
		( std::string ) _S( "GetName" ), &IMaterial::GetName,
		( std::string ) _S( "IsErrorMaterial" ), &IMaterial::IsErrorMaterial,
		( std::string ) _S( "IsErrorMaterial" ), &IMaterial::SetMaterialVarFlag
		);

	pLuaState.new_usertype< INetChannel >( _S( "NetChannel" ),
		( std::string ) _S( "GetName" ), &INetChannel::GetName,
		( std::string ) _S( "GetAddress" ), &INetChannel::GetAddress,
		( std::string ) _S( "IsPlayback" ), &INetChannel::IsPlayback,
		( std::string ) _S( "GetLatency" ), &INetChannel::GetLatency,
		( std::string ) _S( "GetAvgLatency" ), &INetChannel::GetAvgLatency,
		( std::string ) _S( "GetAvgLoss" ), &INetChannel::GetAvgLoss,
		( std::string ) _S( "GetAvgChoke" ), &INetChannel::GetAvgChoke,
		( std::string ) _S( "GetAvgData" ), &INetChannel::GetAvgData,
		( std::string ) _S( "GetAvgPackets" ), &INetChannel::GetAvgPackets,
		( std::string ) _S( "GetTotalData" ), &INetChannel::GetTotalData
		);

}
void C_LuaAPI::SetupLuaTables( )
{
	auto tEngineClient = pLuaState.create_table( );
	tEngineClient[ _S( "ExecuteClientCmd" ) ] = EngineClient::ExecuteClientCmd;
	tEngineClient[ _S( "GetLevelName" ) ] = EngineClient::GetLevelName;
	tEngineClient[ _S( "GetLevelNameShort" ) ] = EngineClient::GetLevelNameShort;
	tEngineClient[ _S( "GetLocalPlayer" ) ] = EngineClient::GetLocalPlayer;
	tEngineClient[ _S( "GetNetChannel" ) ] = EngineClient::GetNetChannel;
	tEngineClient[ _S( "GetScreenSize" ) ] = EngineClient::GetScreenSize;
	tEngineClient[ _S( "GetViewAngles" ) ] = EngineClient::GetViewAngles;
	tEngineClient[ _S( "IsConnected" ) ] = EngineClient::IsConnected;
	tEngineClient[ _S( "IsInGame" ) ] = EngineClient::IsInGame;
	tEngineClient[ _S( "GetLastTimestamp" ) ] = EngineClient::GetLastTimestamp;
	tEngineClient[ _S( "GetPlayerInfo" ) ] = EngineClient::GetPlayerInfo;
	pLuaState[ _S( "g_EngineClient" ) ] = tEngineClient;

	auto tEntityList = pLuaState.create_table( );
	tEntityList[ _S( "NumberOfEntities" ) ] = EntityList::NumberOfEntities;
	tEntityList[ _S( "GetHighestEntityIndex" ) ] = EntityList::GetHighestEntityIndex;
	tEntityList[ _S( "GetClientEntityFromHandle" ) ] = EntityList::GetClientEntityFromHandle;
	tEntityList[ _S( "GetLocalPlayer" ) ] = EntityList::GetLocalPlayer;
	tEntityList[ _S( "GetWeapon" ) ] = EntityList::GetWeapon;
	tEntityList[ _S( "GetPlayerFromHandle" ) ] = EntityList::GetPlayerFromHandle;
	tEntityList[ _S( "GetWeaponFromHandle" ) ] = EntityList::GetWeaponFromHandle;
	tEntityList[ _S( "GetPlayerForUserID" ) ] = EntityList::GetPlayerForUserID;
	tEntityList[ _S( "NumberOfEntities" ) ] = EntityList::NumberOfEntities;
	pLuaState[ _S( "g_EntityList" ) ] = tEntityList;

	auto tGlobalVars = pLuaState.create_table( );
	tGlobalVars[ _S( "GetRealTime" ) ] = GlobalVars::GetRealTime;
	tGlobalVars[ _S( "GetFrameCount" ) ] = GlobalVars::GetFrameCount;
	tGlobalVars[ _S( "GetAbsoluteFrameTime " ) ] = GlobalVars::GetAbsoluteFrameTime;
	tGlobalVars[ _S( "GetCurTime" ) ] = GlobalVars::GetCurTime;
	tGlobalVars[ _S( "GetFrameTime" ) ] = GlobalVars::GetFrameTime;
	tGlobalVars[ _S( "GetMaxClients" ) ] = GlobalVars::GetMaxClients;
	tGlobalVars[ _S( "GetTickCount" ) ] = GlobalVars::GetTickCount;
	tGlobalVars[ _S( "GetIntervalPerTick " ) ] = GlobalVars::GetIntervalPerTick;
	tGlobalVars[ _S( "GetSimTicksForThisFrame" ) ] = GlobalVars::GetSimTicksForThisFrame;
	tGlobalVars[ _S( "GetNetworkProtocol" ) ] = GlobalVars::GetNetworkProtocol;
	tGlobalVars[ _S( "GetSaveData" ) ] = GlobalVars::GetSaveData;
	tGlobalVars[ _S( "GetClient" ) ] = GlobalVars::GetClient;
	tGlobalVars[ _S( "GetRemoteClient" ) ] = GlobalVars::GetRemoteClient;
	pLuaState[ _S( "g_GlobalVars" ) ] = tGlobalVars;

	auto tClientState = pLuaState.create_table( );
	tClientState[ _S( "GetLastOutgoingCommand" ) ] = ClientState::GetLastOutgoingCommand;
	tClientState[ _S( "GetChokedCommands" ) ] = ClientState::GetChokedCommands;
	tClientState[ _S( "GetLastCommandAck" ) ] = ClientState::GetLastCommandAck;
	tClientState[ _S( "GetCommandAck" ) ] = ClientState::GetCommandAck;
	pLuaState[ _S( "g_ClientState" ) ] = tClientState;

	auto tUtils = pLuaState.create_table( );
	tUtils[ _S( "PatternScan" ) ] = Utils::PatternScan;
	tUtils[ _S( "GetRandomInt" ) ] = Utils::GetRandomInt;
	tUtils[ _S( "GetRandomFloat" ) ] = Utils::GetRandomFloat;
	tUtils[ _S( "GetRandomSeed" ) ] = Utils::RandomSeed;
	tUtils[ _S( "GetUnixTime" ) ] = Utils::GetUnixTime;
	tUtils[ _S( "CreateInterface" ) ] = Utils::CreateInterface;
	tUtils[ _S( "IsKeyHold" ) ] = Utils::IsKeyHeld;
	tUtils[ _S( "IsKeyToggled" ) ] = Utils::IsKeyToggled;
	pLuaState[ _S( "g_Utils" ) ] = tUtils;

	auto tCheat = pLuaState.create_table( );
	tCheat[ _S( "GetMousePosition" ) ] = CheatVars::GetMousePosition;
	tCheat[ _S( "AddCallBack" ) ] = CheatVars::AddCallBack;
	tCheat[ _S( "GetBool" ) ] = CheatVars::GetBool;
	tCheat[ _S( "GetInt" ) ] = CheatVars::GetInt;
	tCheat[ _S( "GetFloat" ) ] = CheatVars::GetFloat;
	tCheat[ _S( "GetColor" ) ] = CheatVars::GetColor;
	tCheat[ _S( "SetBool" ) ] = CheatVars::SetBool;
	tCheat[ _S( "SetInt" ) ] = CheatVars::SetInt;
	tCheat[ _S( "SetFloat" ) ] = CheatVars::SetFloat;
	tCheat[ _S( "SetColor" ) ] = CheatVars::SetColor;
	pLuaState[ _S( "g_Cheat" ) ] = tCheat;

	auto tRender = pLuaState.create_table( );
	tRender[ _S( "AddText" ) ] = Render::AddText;
	tRender[ _S( "AddLine" ) ] = Render::AddLine;
	tRender[ _S( "AddRectFilled" ) ] = Render::AddRectFilled;
	tRender[ _S( "AddRect" ) ] = Render::AddRect;
	pLuaState[ _S( "g_Render" ) ] = tRender;

	auto tCvarManager = pLuaState.create_table( );
	tCvarManager[ _S( "FindVar" ) ] = CvarManager::FindVar;
	pLuaState[ _S( "g_CVarManager" ) ] = tCvarManager;

	auto tFakeLag = pLuaState.create_table( );
	tFakeLag[ _S( "SetChokeAmount" ) ] = FakeLags::SetChokeAmount;
	tFakeLag[ _S( "SetPacketMode" ) ] = FakeLags::SetPacketMode;
	tFakeLag[ _S( "GetChokedCommands" ) ] = FakeLags::GetChokedCommands;
	pLuaState[ _S( "g_FakeLag" ) ] = tFakeLag;

	auto tFontManager = pLuaState.create_table( );
	tFontManager[ _S( "PushFontFromFile" ) ] = FontManager::PushFontFromFile;
	tFontManager[ _S( "PushFontFromData" ) ] = FontManager::PushFontFromData;
	tFontManager[ _S( "GetFont" ) ] = FontManager::GetFont;
	tFontManager[ _S( "UpdateFonts" ) ] = FontManager::UpdateFonts;
	pLuaState[ _S( "g_FontManager" ) ] = tFontManager;

	auto tMaterialSystem = pLuaState.create_table( );
	tMaterialSystem[ _S( "CreateMaterial" ) ] = MaterialSystem::CreateMaterial;
	tMaterialSystem[ _S( "FirstMaterial" ) ] = MaterialSystem::FirstMaterial;
	tMaterialSystem[ _S( "NextMaterial" ) ] = MaterialSystem::NextMaterial;
	tMaterialSystem[ _S( "GetMaterial" ) ] = MaterialSystem::GetMaterial;
	tMaterialSystem[ _S( "FindMaterial" ) ] = MaterialSystem::FindMaterial;
}