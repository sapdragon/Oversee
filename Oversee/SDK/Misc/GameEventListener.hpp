#pragma once
#include "GameEvent.hpp"

class IGameEventListener
{
public:
    virtual ~IGameEventListener(void) {}

    virtual void FireGameEvent( IGameEvent* m_pEvent) = 0;
    virtual int  GetEventDebugID(void) = 0;
public:
    int m_iDebugId;
};
class ICustomEventListener : public IGameEventListener
{
public:
    virtual void FireGameEvent( IGameEvent* pEvent ) { };
    virtual int  GetEventDebugID( void ) { return 42; };
public:
    int m_iDebugId; //-V703
};