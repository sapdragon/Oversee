#pragma once

#include <cstdint>
#include "../Misc/VFunc.hpp"
#include "../Misc/GameEventListener.hpp"

class IGameEventManager
{
public:
    VFunc( FireEventClientSide( IGameEvent* GameEvent ), 9, void ( __thiscall* ) ( void*, IGameEvent* ), GameEvent );
    VFunc( UnserializeMessage( void* Msg ), 13, IGameEvent* ( __thiscall* ) ( void*, void* ), Msg );
    VFunc( AddListener( IGameEventListener* pListener, const char* szName, bool bServerSide ), 3, bool( __thiscall* )( void*, IGameEventListener*, const char*, bool ), pListener, szName, bServerSide );
};