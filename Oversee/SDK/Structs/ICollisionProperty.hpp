#pragma once

class ICollisionProperty
{
public:
	Vector& m_vecMins( )
	{
		return *( Vector* )( ( DWORD )( this ) + 0x8 );
	}
	Vector& m_vecMaxs( )
	{
		return *( Vector* )( ( DWORD )( this ) + 0x14 );
	}
};