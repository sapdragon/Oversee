#pragma once
#include "../../../SDK/SDK.hpp"

class C_LocalInterpolation
{
public:
	virtual void SetupInterpolation( bool bPostFrame );
	virtual void SkipInterpolation( );
	virtual void ContinueInterpolation( );
private:
	struct
	{
		bool m_bSkipInterpolation = false;
		float m_flInterpolationAmount = 0.0f;
		int m_nFinalPredictedTick = 0;
	} m_Data;
};

inline C_LocalInterpolation* g_LocalInterpolation = new C_LocalInterpolation( );