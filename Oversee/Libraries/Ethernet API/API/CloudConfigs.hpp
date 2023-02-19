#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <thread>

#include "../Dependecies/curl/curl.h"
#pragma comment(lib, "LibCurl.lib")

enum class CloudActions_t : int {
	Initialization = -1,
	None = 0,
	GetConfigList = 1,
	GetConfigData = 2,
	CreateConfig = 3,
	SaveConfig = 4
};

struct CloudConfig_t
{
	int ID;
	int UpdatedAt;
	std::string Name;
	std::string Owner;
	std::string Author;
	std::string SecureID;
	bool Market;
};

class C_CloudConfigs
{
public:
	// constructor
	C_CloudConfigs( );

	// configs
	char aConfigName[ 64 ] = "";
	char aConfigID[ 64 ] = "";

	// config vector
	std::vector< CloudConfig_t > m_vConfigs;

	// functions
	virtual void GetConfigList( );
	virtual void GetConfigData( );
	virtual void CreateConfig( );
	virtual void SaveConfig( );
	virtual void Auth( );

	CloudActions_t CloudAction = CloudActions_t::Initialization;
private:
	std::string VideoCardID;
	std::string ProcessorID;
	std::string HardDriveID;
	std::string WindowsUser;
	std::string WindowsVersion;
};

inline C_CloudConfigs* g_CloudConfigs = new C_CloudConfigs( );
static VOID WINAPI HandleConfigs( )
{
#ifndef OVERSEE_DEV
	std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );
#endif
	g_CloudConfigs->GetConfigList( );

	while ( true )
	{
		std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );
		if ( g_CloudConfigs->CloudAction == CloudActions_t::GetConfigList )
			g_CloudConfigs->GetConfigList( );

		if ( g_CloudConfigs->CloudAction == CloudActions_t::GetConfigData )
			g_CloudConfigs->GetConfigData( );

		if ( g_CloudConfigs->CloudAction == CloudActions_t::SaveConfig )
			g_CloudConfigs->SaveConfig( );

		if ( g_CloudConfigs->CloudAction == CloudActions_t::CreateConfig )
			g_CloudConfigs->CreateConfig( );
	}
}