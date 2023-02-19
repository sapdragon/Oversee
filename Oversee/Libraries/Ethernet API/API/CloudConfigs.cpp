#include "CloudConfigs.hpp"
#include "../../../SDK/Utils/String/XorStr.hpp"

#include "Writer.hpp"

#include "../../../Interface/GUI/Notifies.h"
#include "../Dependecies/json.hpp"
#include "../Dependecies/MD5/MD5.hpp"
#include "../Configurations/Configurations.hpp"

C_CloudConfigs::C_CloudConfigs( )
{

}

void C_CloudConfigs::GetConfigList( )
{
	std::string szRequest = _S( "https://oversee.one/api/shared/configs/list/" ) + g_Globals->m_szHWID;
	std::string szResponse;

	curl_global_init( CURL_GLOBAL_ALL );
	auto inited_curl = curl_easy_init( );

	auto headers = curl_slist_append( NULL, _S( "Expect:" ) );
	curl_easy_setopt( inited_curl, CURLOPT_HTTPHEADER, headers );
	curl_easy_setopt( inited_curl, CURLOPT_URL, szRequest.c_str( ) );
	curl_easy_setopt( inited_curl, CURLOPT_POST, 1L );
	curl_easy_setopt( inited_curl, CURLOPT_WRITEFUNCTION, writer );
	curl_easy_setopt( inited_curl, CURLOPT_WRITEDATA, &szResponse );
	curl_easy_setopt( inited_curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4 );

	curl_easy_perform( inited_curl );

	if ( szResponse.empty( ) || md5( szResponse ) == md5( _S( "NULL" ) ) || md5( szResponse ) == md5( _S( "Something went wrong" ) ) ) {
		g_CloudConfigs->CloudAction = CloudActions_t::None;
		return;
	}

	m_vConfigs.clear( );

	nlohmann::json jConfigResponse = nlohmann::json::parse( szResponse );
	for ( int nConfig = 0; nConfig < jConfigResponse.size( ); nConfig++ )
	{
		CloudConfig_t configuration
		{
			jConfigResponse[ nConfig ][ _S( "ID" ) ].get < int >( ),
			jConfigResponse[ nConfig ][ _S( "UpdatedAt" ) ].get < int >( ),
			jConfigResponse[ nConfig ][ _S( "Name" ) ].get < std::string >( ),
			jConfigResponse[ nConfig ][ _S( "Owner" ) ].get < std::string >( ),
			jConfigResponse[ nConfig ][ _S( "Author" ) ].get < std::string >( ),
			jConfigResponse[ nConfig ][ _S( "SecureID" ) ].get < std::string >( ),
			jConfigResponse[ nConfig ][ _S( "Market" ) ].get < bool >( ),
		};

		m_vConfigs.emplace_back( configuration );
	}

	notifies::push( _S( "List of configurations successfully received" ) );

	curl_easy_cleanup( inited_curl );
	curl_slist_free_all( headers );

	g_CloudConfigs->CloudAction = CloudActions_t::None;
}

enum State { ESCAPED, UNESCAPED };

std::string unescapeJSON( const std::string& input )
{
	State s = UNESCAPED;
	std::string output;
	output.reserve( input.length( ) );

	for ( std::string::size_type i = 0; i < input.length( ); ++i )
	{
		switch ( s )
		{
			case ESCAPED:
			{
				switch ( input[ i ] )
				{
					case '"':
						output += '\"';
						break;
					case '/':
						output += '/';
						break;
					case 'b':
						output += '\b';
						break;
					case 'f':
						output += '\f';
						break;
					case 'n':
						output += '\n';
						break;
					case 'r':
						output += '\r';
						break;
					case 't':
						output += '\t';
						break;
					case '\\':
						output += '\\';
						break;
					default:
						output += input[ i ];
						break;
				}

				s = UNESCAPED;
				break;
			}
			case UNESCAPED:
			{
				switch ( input[ i ] )
				{
					case '\\':
						s = ESCAPED;
						break;
					default:
						output += input[ i ];
						break;
				}
			}
		}
	}
	return output;
}

void C_CloudConfigs::GetConfigData( )
{
	curl_global_init( CURL_GLOBAL_ALL );
	auto inited_curl = curl_easy_init( );

	std::string szRequest = _S( "https://oversee.one/api/shared/configs/data/" ) + std::string( aConfigID ) + _S( "/" ) + g_Globals->m_szHWID;
	std::string szResponse;

	auto headers = curl_slist_append( NULL, _S( "Expect:" ) );
	curl_easy_setopt( inited_curl, CURLOPT_HTTPHEADER, headers );
	curl_easy_setopt( inited_curl, CURLOPT_URL, szRequest.c_str( ) );
	curl_easy_setopt( inited_curl, CURLOPT_POST, 1L );
	curl_easy_setopt( inited_curl, CURLOPT_WRITEFUNCTION, writer );
	curl_easy_setopt( inited_curl, CURLOPT_WRITEDATA, &szResponse );
	curl_easy_setopt( inited_curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4 );
	curl_easy_perform( inited_curl );

	if ( szResponse.empty( ) || md5( szResponse ) == md5( _S( "NULL" ) ) || md5( szResponse ) == md5( _S( "Something went wrong" ) ) ) {
		g_CloudConfigs->CloudAction = CloudActions_t::None;
		return;
	}

	nlohmann::json jResponse = nlohmann::json::parse( szResponse );
	nlohmann::json jData = nlohmann::json::parse( unescapeJSON( jResponse[ _S( "Data" ) ].get<std::string>( ) ) );

	g_ConfigManager->Action( false, jData );

	curl_easy_cleanup( inited_curl );
	curl_slist_free_all( headers );

	notifies::push( _S( "Configuration successfully loaded" ) );

	g_CloudConfigs->CloudAction = CloudActions_t::None;
}
void C_CloudConfigs::CreateConfig( )
{
	curl_global_init( CURL_GLOBAL_ALL );
	auto inited_curl = curl_easy_init( );

	std::string szRequest = _S( "https://oversee.one/api/shared/configs/create/" ) + ( std::string ) ( aConfigName ) +_S( "/" ) + g_Globals->m_szHWID;
	std::string szResponse;

	nlohmann::json jJsonData;
	g_ConfigManager->Action( true, jJsonData );
	std::string szPost = _S( "data=" ) + jJsonData.dump( );

	auto headers = curl_slist_append( NULL, _S( "Expect:" ) );
	curl_easy_setopt( inited_curl, CURLOPT_HTTPHEADER, headers );
	curl_easy_setopt( inited_curl, CURLOPT_URL, szRequest.c_str( ) );
	curl_easy_setopt( inited_curl, CURLOPT_POST, 1L );
	curl_easy_setopt( inited_curl, CURLOPT_POSTFIELDS, szPost.c_str( ) );
	curl_easy_setopt( inited_curl, CURLOPT_POSTFIELDSIZE, szPost.size( ) );
	curl_easy_setopt( inited_curl, CURLOPT_WRITEFUNCTION, writer );
	curl_easy_setopt( inited_curl, CURLOPT_WRITEDATA, &szResponse );
	curl_easy_setopt( inited_curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4 );

	curl_easy_perform( inited_curl );

	if ( md5( szResponse ) == md5( _S( "NULL" ) ) || md5( szResponse ) == md5( _S( "Something went wrong" ) ) ) {
		g_CloudConfigs->CloudAction = CloudActions_t::None;
		return;
	}

	curl_easy_cleanup( inited_curl );
	curl_slist_free_all( headers );

	notifies::push( _S( "Configuration successfully created" ) );

	g_CloudConfigs->CloudAction = CloudActions_t::GetConfigList;
}
void C_CloudConfigs::SaveConfig( )
{
	curl_global_init( CURL_GLOBAL_ALL );
	auto inited_curl = curl_easy_init( );

	std::string szRequest = _S( "https://oversee.one/api/shared/configs/save/" ) + ( std::string ) ( aConfigID ) +_S( "/" ) + g_Globals->m_szHWID;
	std::string szResponse;

	nlohmann::json jJsonData;
	g_ConfigManager->Action( true, jJsonData );
	std::string szPost = _S( "data=" ) + jJsonData.dump( );

	auto headers = curl_slist_append( NULL, "Expect:" );
	curl_easy_setopt( inited_curl, CURLOPT_HTTPHEADER, headers );
	curl_easy_setopt( inited_curl, CURLOPT_URL, szRequest.c_str( ) );
	curl_easy_setopt( inited_curl, CURLOPT_POST, 1L );
	curl_easy_setopt( inited_curl, CURLOPT_POSTFIELDS, szPost.c_str( ) );
	curl_easy_setopt( inited_curl, CURLOPT_POSTFIELDSIZE, szPost.size( ) );
	curl_easy_setopt( inited_curl, CURLOPT_WRITEFUNCTION, writer );
	curl_easy_setopt( inited_curl, CURLOPT_WRITEDATA, &szResponse );
	curl_easy_setopt( inited_curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4 );

	curl_easy_perform( inited_curl );

	if ( md5( szResponse ) == md5( _S( "NULL" ) ) || md5( szResponse ) == md5( _S( "Something went wrong" ) ) ) {
		g_CloudConfigs->CloudAction = CloudActions_t::None;
		return;
	}

	curl_easy_cleanup( inited_curl );
	curl_slist_free_all( headers );

	notifies::push( _S( "Configuration successfully saved" ) );

	g_CloudConfigs->CloudAction = CloudActions_t::GetConfigList;
}

void C_CloudConfigs::Auth( )
{
	curl_global_init( CURL_GLOBAL_ALL );
	auto inited_curl = curl_easy_init( );

	std::string szRequest = _S( "https://oversee.one/api/information/" ) + g_Globals->m_szHWID;
	std::string szResponse;

	auto headers = curl_slist_append( NULL, _S( "Expect:" ) );
	curl_easy_setopt( inited_curl, CURLOPT_HTTPHEADER, headers );
	curl_easy_setopt( inited_curl, CURLOPT_URL, szRequest.c_str( ) );
	curl_easy_setopt( inited_curl, CURLOPT_POST, 1L );
	curl_easy_setopt( inited_curl, CURLOPT_WRITEFUNCTION, writer );
	curl_easy_setopt( inited_curl, CURLOPT_WRITEDATA, &szResponse );
	curl_easy_setopt( inited_curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4 );
	curl_easy_perform( inited_curl );

	curl_easy_cleanup( inited_curl );
	curl_slist_free_all( headers );

	nlohmann::json jResponse = nlohmann::json::parse( szResponse );
	g_Globals->m_szName = jResponse[ _S( "Username" ) ].get < std::string > ( );
}