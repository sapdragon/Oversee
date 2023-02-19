#pragma once
#include "../Misc/VFunc.hpp"

class IMoveHelper
{
public:
    VFunc( SetHost( void* pEntity ), 1, void( __thiscall* )( void*, void* ), pEntity );
};