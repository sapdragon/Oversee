#pragma once
#include "../../../SDK/SDK.hpp"
class C_Radar
{
public:
	virtual void Instance();
	virtual bool   OnMapUpdate(std::string szMapName);
private:

	virtual Vector WorldToRadar(Vector& vecWorldPos);
	virtual std::string ParseString(const std::string& szBefore, const std::string& szSource);


private:
	bool m_bLoaded = false;
	IDirect3DTexture9* m_pRadarTexture = nullptr;

	Vector m_vecMapOrigin = Vector ( );
	double m_flMapScale = 0;
	Vector m_vecPosBombSiteA = Vector ( );
	Vector m_vecPosBombSiteB = Vector ( );

	bool m_bDrawPlantA = false;
	bool m_bDrawPlantB = false;
};

inline C_Radar* g_Radar = new C_Radar( );