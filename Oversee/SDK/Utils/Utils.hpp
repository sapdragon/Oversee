#pragma once
#include "../../Includes.hpp"

class C_Utils
{
public:
	// cheat engine tools
	virtual void* PatternScan( const char* szModule, const char* szPattern );
	virtual void* GetPointerFromVirtual( void* pBasePointer, int nVirtual );
	
	// source engine tools
	virtual DWORD* FindHudElement( const char* szHudName );
	virtual float TicksToTime( int nTickCount );
	virtual int TimeToTicks( float flTimeAmount );
	virtual void SetSkybox( std::string szSkyBox );
	virtual bool IsChatRaised( );

	// random tools
	virtual int RandomInt( int iMin, int iMax );
	virtual float RandomFloat( float flMin, float flMax );
	virtual void RandomSeed( int iSeed );
	virtual void SetClantag( const char* szClanTag, void* lpAddress );
};

inline C_Utils* g_Utils = new C_Utils( );