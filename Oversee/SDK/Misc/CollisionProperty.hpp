#pragma once

class C_CollisionProperty
{
public:
	void SetCollisionBounds( Vector* vecMins, Vector* vecMaxs )
	{
		return ( ( void( __thiscall* )( void*, Vector*, Vector* ) )( g_Globals->m_AddressList.m_pSetCollisionBounds ) )( this, vecMins, vecMaxs );
	}
	
	Vector& m_vecMins( )
	{
		return *( Vector* )( ( DWORD )( this ) + 0x8 );
	}
	Vector& m_vecMaxs( )
	{
		return *( Vector* )( ( DWORD )( this ) + 0x14 );
	}
};