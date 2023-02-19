#pragma once
#include "Enums.hpp"

class ClientClass;
class IClientNetworkable;
class RecvTable;

typedef IClientNetworkable* (*CreateClientClassFn)(int entnum, int serialNum);
typedef IClientNetworkable* (*CreateEventFn)();

class ClientClass
{
public:
    CreateClientClassFn     m_pCreateFn;
    CreateEventFn           m_pCreateEventFn;
    char*                   m_strNetworkName;
    RecvTable*              m_pRecvTable;
    ClientClass*            m_pNext;
    ClassId                 m_ClassID;
};