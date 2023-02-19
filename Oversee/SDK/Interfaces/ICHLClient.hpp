#pragma once
#include "../Misc/ClientClass.hpp"

class ICHLClient
{
public:
	VFunc( GetAllClasses( ), 8, ClientClass*( __thiscall* )( void* ) );
};