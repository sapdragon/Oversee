#pragma once
#include "../../../SDK/SDK.hpp"

class C_Revolver
{
public:
	virtual void OnRunCommand( );
	virtual void OnCreateMove( );
	virtual void ResetData( )
	{
		m_Data.m_nTickBase = 0;
		m_Data.m_nActivity = 0;
	}
private:
	struct
	{
		int m_nTickBase = 0;
		int m_nActivity = 0;
	} m_Data;
};

inline C_Revolver* g_Revolver = new C_Revolver( );