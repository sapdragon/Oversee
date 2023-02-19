#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <thread>

#include "../Dependecies/MD5/MD5.hpp"
#include "../Dependecies/curl/curl.h"
#pragma comment(lib, "LibCurl.lib")

#include "../../../Features/Misc/LuaAPI/LuaAPI.hpp"
#include "../../../Interface/GUI/Menu.hpp"

enum class CloudScriptAction_t : int {
	Initialization = -1,
	None = 0,
	GetScriptList = 1,
	CreateScript = 3,
	SaveScript = 4
};

struct CloudScript_t
{
	int ID;
	int UpdatedAt;
	std::string Name;
	std::string Owner;
	std::string Author;
	std::string SecureID;
	std::string MD5;
	std::string Data;
	bool Market;
};

class C_CloudScripts
{
public:
	C_CloudScripts( );

	char aConfigName[ 64 ] = "";
	char aConfigID[ 64 ] = "";
	int iScriptFileIndex = 0;

	std::vector< CloudScript_t > m_vScripts;

	virtual void GetScriptList( );
	virtual void CreateScript( );
	virtual void SaveScript( );

	CloudScriptAction_t CloudAction = CloudScriptAction_t::Initialization;
};

inline C_CloudScripts* g_CloudScripts = new C_CloudScripts( );

static DWORD WINAPI HandleScripts( )
{
#ifndef OVERSEE_DEV
	std::this_thread::sleep_for( std::chrono::milliseconds( 6000 ) );
#endif

	g_CloudScripts->GetScriptList( );

	while ( true )
	{
		std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );

		if ( g_CloudScripts->CloudAction == CloudScriptAction_t::GetScriptList )
			g_CloudScripts->GetScriptList( );

		if ( g_CloudScripts->CloudAction == CloudScriptAction_t::SaveScript )
			g_CloudScripts->SaveScript( );

		if ( g_CloudScripts->CloudAction == CloudScriptAction_t::CreateScript )
			g_CloudScripts->CreateScript( );
	}
}

static DWORD WINAPI HandleScriptsChanges( )
{
	while ( true )
	{
		std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );

		for ( const auto& [id, InEdit] : g_LuaAPI->InEdit ) {
			if ( InEdit == false )
				continue;

			std::ifstream t( g_LuaAPI->InEditFile.at( id ) );
			std::stringstream buffer;
			buffer << t.rdbuf( );

			g_LuaAPI->InEditMD5.at( id ) = md5( buffer.str( ) );
		}
	}
}