#pragma once
#include <vector>
#include "../../../SDK/SDK.hpp"

struct HitMarker_t
{
	Vector m_vecOrigin = Vector( 0, 0, 0 );
	float m_flTime = 0.0f;
};

class C_HitMarker
{
public:
	virtual void OnReportHit( Vector vecOrigin );
	virtual void Instance( );
private:
	std::vector < HitMarker_t > m_aHitMarkers = { };
};

inline C_HitMarker* g_HitMarkers = new C_HitMarker( );