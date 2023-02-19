#pragma once

class C_FogController : public C_BaseEntity
{
public:
	NETVAR(m_bEnabled, bool, "DT_FogController", "m_fog.enable");
	NETVAR(m_flStartDistance, float, "DT_FogController", "m_fog.start");
	NETVAR(m_flEndDistance, float, "DT_FogController", "m_fog.end");
	NETVAR(m_flMaxDensity, float, "DT_FogController", "m_fog.maxdensity");
	NETVAR(m_cMainColor, Color, "DT_FogController", "m_fog.colorPrimary");
};