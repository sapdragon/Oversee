#pragma once
#include "../Misc/VFunc.hpp"
#include "../Misc/BaseHandle.hpp"
#include "../Structs/IClientEntity.hpp"

class IClientEntityList
{
public:
    VFunc( GetClientEntity( int nEntIndex ), 3, IClientEntity*( __thiscall* )( void*, int ), nEntIndex ); 
    VFunc( GetClientEntityFromHandle( const CBaseHandle hEnt ), 4, IClientEntity*( __thiscall* )( void*, const CBaseHandle ), hEnt );
    VFunc( NumberOfEntities( bool bIncludeNonNetworkable ), 5, int(__thiscall*)(void*, bool), bIncludeNonNetworkable);
    VFunc( GetHighestEntityIndex( ) , 6, int(__thiscall*)(void*) );
};