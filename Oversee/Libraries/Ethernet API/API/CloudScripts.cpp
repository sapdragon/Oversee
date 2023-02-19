#include "CloudScripts.hpp"
#include "../../../SDK/Utils/String/XorStr.hpp"

#include "../Dependecies/json.hpp"
#include "../Dependecies/MD5/MD5.hpp"
#include "../Configurations/Configurations.hpp"

#include "Writer.hpp"

C_CloudScripts::C_CloudScripts( )
{

}

enum State { ESCAPED, UNESCAPED };

std::string unescapeJSO2( const std::string& input )
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

void C_CloudScripts::GetScriptList( )
{
	std::string szRequest = _S( "https://oversee.one/api/shared/scripts/list/" ) + g_Globals->m_szHWID;
	std::string szResponse;

	curl_global_init( CURL_GLOBAL_ALL );
	auto inited_curl = curl_easy_init( );

	curl_easy_setopt( inited_curl, CURLOPT_URL, szRequest.c_str( ) );
	curl_easy_setopt( inited_curl, CURLOPT_POST, 1L );
	curl_easy_setopt( inited_curl, CURLOPT_WRITEFUNCTION, writer );
	curl_easy_setopt( inited_curl, CURLOPT_WRITEDATA, &szResponse );

	curl_easy_perform( inited_curl );

	if ( szResponse.empty( ) || md5( szResponse ) == md5( _S( "NULL" ) ) || md5( szResponse ) == md5( _S( "Something went wrong" ) ) ) {
		g_CloudScripts->CloudAction = CloudScriptAction_t::None;
		return;
	}

	g_LuaAPI->UnloadAll( );

	m_vScripts.clear( );

	g_LuaAPI->InEdit.clear( );
	g_LuaAPI->InEditMD5.clear( );
	g_LuaAPI->InEditFile.clear( );
	g_LuaAPI->Loaded.clear( );
	g_LuaAPI->Scripts.clear( );

	nlohmann::json jConfigResponse = nlohmann::json::parse( szResponse );
	for ( int nConfig = 0; nConfig < jConfigResponse.size( ); nConfig++ )
	{
		CloudScript_t configuration
		{
			jConfigResponse[ nConfig ][ _S( "ID" ) ].get < int >( ),
			jConfigResponse[ nConfig ][ _S( "UpdatedAt" ) ].get < int >( ),
			jConfigResponse[ nConfig ][ _S( "Name" ) ].get < std::string >( ),
			jConfigResponse[ nConfig ][ _S( "Owner" ) ].get < std::string >( ),
			jConfigResponse[ nConfig ][ _S( "Author" ) ].get < std::string >( ),
			jConfigResponse[ nConfig ][ _S( "SecureID" ) ].get < std::string >( ),
			jConfigResponse[ nConfig ][ _S( "MD5" ) ].get < std::string >( ),
			unescapeJSO2( jConfigResponse[ nConfig ][ _S( "Data" ) ].get < std::string >( ) ),
			jConfigResponse[ nConfig ][ _S( "Market" ) ].get < bool >( ),
		};

		g_LuaAPI->InEdit.insert( { jConfigResponse[ nConfig ][ _S( "ID" ) ].get < int >( ), false } );
		g_LuaAPI->Loaded.insert( { jConfigResponse[ nConfig ][ _S( "ID" ) ].get < int >( ), false } );
		g_LuaAPI->Scripts.insert( { jConfigResponse[ nConfig ][ _S( "ID" ) ].get < int >( ), jConfigResponse[ nConfig ][ _S( "Data" ) ].get <  std::string >( ) } );
		g_LuaAPI->InEditMD5.insert( { jConfigResponse[ nConfig ][ _S( "ID" ) ].get < int >( ), jConfigResponse[ nConfig ][ _S( "MD5" ) ].get <  std::string >( ) } );
		g_LuaAPI->InEditFile.insert( { jConfigResponse[ nConfig ][ _S( "ID" ) ].get < int >( ), jConfigResponse[ nConfig ][ _S( "SecureID" ) ].get <  std::string >( ) + ".lua" } );


		m_vScripts.emplace_back( configuration );
	}

	curl_easy_cleanup( inited_curl );

	g_CloudScripts->CloudAction = CloudScriptAction_t::None;
}

void C_CloudScripts::CreateScript( )
{
	curl_global_init( CURL_GLOBAL_ALL );
	auto inited_curl = curl_easy_init( );

	std::string szRequest = _S( "https://oversee.one/api/shared/scripts/create/" ) + ( std::string ) ( aConfigName ) +_S( "/" ) + g_Globals->m_szHWID;
	std::string szResponse;

	std::string szPost = "";

	curl_easy_setopt( inited_curl, CURLOPT_URL, szRequest.c_str( ) );
	curl_easy_setopt( inited_curl, CURLOPT_POST, 1L );
	curl_easy_setopt( inited_curl, CURLOPT_POSTFIELDS, szPost.c_str( ) );
	curl_easy_setopt( inited_curl, CURLOPT_POSTFIELDSIZE, szPost.size( ) );
	curl_easy_setopt( inited_curl, CURLOPT_WRITEFUNCTION, writer );
	curl_easy_setopt( inited_curl, CURLOPT_WRITEDATA, &szResponse );

	curl_easy_perform( inited_curl );

	if ( szResponse.empty( ) || md5( szResponse ) == md5( _S( "NULL" ) ) || md5( szResponse ) == md5( _S( "Something went wrong" ) ) ) {
		g_CloudScripts->CloudAction = CloudScriptAction_t::None;
		return;
	}

	curl_easy_cleanup( inited_curl );

	g_CloudScripts->CloudAction = CloudScriptAction_t::GetScriptList;
}

void C_CloudScripts::SaveScript( )
{
	curl_global_init( CURL_GLOBAL_ALL );
	auto inited_curl = curl_easy_init( );

	std::string szRequest = _S( "https://oversee.one/api/shared/scripts/save/" ) + ( std::string ) ( aConfigID ) +_S( "/" ) + g_Globals->m_szHWID;
	std::string szResponse;

	std::ifstream t( g_LuaAPI->InEditFile.at( iScriptFileIndex ) );
	std::stringstream buffer;
	buffer << t.rdbuf( );

	std::string szPost = _S( "data=" ) + buffer.str( );

	curl_easy_setopt( inited_curl, CURLOPT_URL, szRequest.c_str( ) );
	curl_easy_setopt( inited_curl, CURLOPT_POST, 1L );
	curl_easy_setopt( inited_curl, CURLOPT_POSTFIELDS, szPost.c_str( ) );
	curl_easy_setopt( inited_curl, CURLOPT_POSTFIELDSIZE, szPost.size( ) );
	curl_easy_setopt( inited_curl, CURLOPT_WRITEFUNCTION, writer );
	curl_easy_setopt( inited_curl, CURLOPT_WRITEDATA, &szResponse );

	curl_easy_perform( inited_curl );

	if ( szResponse.empty( ) || md5( szResponse ) == md5( _S( "NULL" ) ) || md5( szResponse ) == md5( _S( "Something went wrong" ) ) ) {
		g_CloudScripts->CloudAction = CloudScriptAction_t::None;
		return;
	}

	curl_easy_cleanup( inited_curl );

	remove( g_LuaAPI->InEditFile.at( iScriptFileIndex ).c_str( ) );
	g_CloudScripts->CloudAction = CloudScriptAction_t::GetScriptList;
}