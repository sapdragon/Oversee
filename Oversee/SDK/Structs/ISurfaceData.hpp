#pragma once

class C_SurfaceData
{
public:
	float& GetGroundFactor( )
	{
		return *( float* ) ( ( DWORD ) ( this ) + 0x54 );
	}
	
	float& GetMaxSpeedFactor( )
	{
		return *( float* )( ( DWORD )( this ) + 0x50 );
	}
};