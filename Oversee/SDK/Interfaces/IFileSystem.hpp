#pragma once
#include "../Misc/VFunc.hpp"

class IFileSystem
{
public:
    VFunc( Read( void* pOutput, int nFileSize, void* pFile ), 0, int( __thiscall* )( void*, void*, int, void* ), pOutput, nFileSize, pFile );
    VFunc( Open( const char* szFileName, const char* szOptions, const char* szPathID ), 2, void*( __thiscall* )( void*, const char*, const char*, const char* ), szFileName, szOptions, szPathID );
    VFunc( Close( void* pFile ), 3, void( __thiscall* )( void*, void* ), pFile );
    VFunc( Size( void* pFile ), 7, int( __thiscall* )( void*, void* ), pFile );
};