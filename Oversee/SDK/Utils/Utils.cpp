#include "Utils.hpp"
#include "../../SDK/SDK.hpp"

// cheat engine tools
void* C_Utils::PatternScan( const char* szModuleName, const char* szPattern )
{
    HMODULE module = GetModuleHandleA( szModuleName );
    if ( !module )
        return nullptr;

    static auto pattern_to_byte = [ ] ( const char* pattern ) {
        auto bytes = std::vector<int> {};
        auto start = const_cast< char* >( pattern );
        auto end = const_cast< char* >( pattern ) + strlen( pattern );

        for ( auto current = start; current < end; ++current )
        {
            if ( *current == '?' )
            {
                ++current;
                if ( *current == '?' )
                    ++current;
                bytes.push_back( -1 );
            }
            else
            {
                bytes.push_back( strtoul( current, &current, 16 ) );
            }
        }
        return bytes;
    };

    auto dosHeader = ( PIMAGE_DOS_HEADER ) module;
    auto ntHeaders = ( PIMAGE_NT_HEADERS ) ( ( std::uint8_t* ) module + dosHeader->e_lfanew );

    auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
    auto patternBytes = pattern_to_byte( szPattern );
    auto scanBytes = reinterpret_cast< std::uint8_t* >( module );

    auto s = patternBytes.size( );
    auto d = patternBytes.data( );

    for ( auto i = 0ul; i < sizeOfImage - s; ++i )
    {
        bool found = true;
        for ( auto j = 0ul; j < s; ++j )
        {
            if ( scanBytes[ i + j ] != d[ j ] && d[ j ] != -1 )
            {
                found = false;
                break;
            }
        }
        if ( found )
        {
            return &scanBytes[ i ];
        }
    }
    return nullptr;
}
void* C_Utils::GetPointerFromVirtual( void* pBaseAddress, int nVirtual )
{
    return ( void* ) ( ( *( uintptr_t** ) ( pBaseAddress ) )[ nVirtual ] );
}

// source engine tools
DWORD* C_Utils::FindHudElement( const char* szHudName )
{
    return ( DWORD* ) ( ( ( DWORD( __thiscall* )( LPVOID, const char* ) )( SDK::EngineData::m_AddressList[ CheatAddressList::FindHudElement ] ) )( *reinterpret_cast < PDWORD* >( SDK::EngineData::m_AddressList[ CheatAddressList::FindHudElementPtr ] ), szHudName ) );
}
float C_Utils::TicksToTime( int nTicks )
{
    return ( float ) ( ( float ) ( nTicks ) * SDK::Interfaces::GlobalVars->m_flIntervalPerTick );
}
int C_Utils::TimeToTicks( float flTime )
{
    return ( int ) ( ( float ) ( flTime / SDK::Interfaces::GlobalVars->m_flIntervalPerTick ) + 0.5f );
}
void C_Utils::SetSkybox( std::string szSkybox )
{
    return ( ( void( __fastcall* )( const char* ) )( SDK::EngineData::m_AddressList[ CheatAddressList::LoadSkybox ] ) )( szSkybox.c_str( ) );
}
bool C_Utils::IsChatRaised( )
{
    PDWORD dwHudChat = this->FindHudElement( _S( "CCSGO_HudChat" ) );
    if ( !dwHudChat )
        return false;

    return *( bool* ) ( ( DWORD ) ( dwHudChat ) + 0x58 );
}

// random tools
int C_Utils::RandomInt( int iMin, int iMax )
{
    return ( ( int( * )( int, int ) )( GetProcAddress( GetModuleHandleA( _S( "vstdlib.dll" ) ), _S( "RandomInt" ) ) ) )( iMin, iMax );
}
float C_Utils::RandomFloat( float flMin, float flMax )
{
    return ( ( float( * )( float, float ) )( GetProcAddress( GetModuleHandleA( _S( "vstdlib.dll" ) ), _S( "RandomFloat" ) ) ) )( flMin, flMax );
}
void C_Utils::RandomSeed( int iSeed )
{
    ( reinterpret_cast< void( * )( int ) >( GetProcAddress( GetModuleHandleA( _S( "vstdlib.dll" ) ), _S( "RandomSeed" ) ) ) )( iSeed );
}
void C_Utils::SetClantag( const char* szClanTag, void* lpAddress )
{
    reinterpret_cast< void( __fastcall* )( const char*, const char* ) >( lpAddress )( szClanTag, szClanTag );
}