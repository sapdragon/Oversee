#pragma once
#include "../Misc/VFunc.hpp"

class IPanel
{
public:
    VFunc( GetName( unsigned int nVGUIPanel ), 36, const char*( __thiscall* )( void*, unsigned int ), nVGUIPanel );
    VFunc( GetClassName( unsigned int nVGUIPanel ), 37, const char*( __thiscall* )( void*, unsigned int ), nVGUIPanel );
};
