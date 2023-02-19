#pragma once

#include <vector>
#include <algorithm>

#include "../../../SDK/SDK.hpp"
#include "../../../Libraries/ImGui/imgui.h"
#include "../../../Libraries/ImGui/imgui_internal.h"

struct HUDPlayerData_t {
	int m_Health = -1;
	int m_Armor = -1;
};

struct HUDChatMessages_t {

};

struct KillMessage_t {
	unsigned __int64 m_EndTime = 0;
	int m_UserIDDied = 0;
	int m_UserIDAttacker = 0;
	int m_UserIDAssister = 0;
	std::string m_WeaponName = "";
	bool m_Headshot = false;
	bool m_NoScope = false;
	bool m_AttackerFlashed = false;
	bool m_FlashAssist = false;
};

class C_CustomHUD {
public:
	virtual void Instance( );
	virtual void EventHandler( IGameEvent* m_pEvent );
	virtual void RenderHUD( );

	virtual void HealthArmorHandler( ImDrawList* draw, ImVec2 pos, ImVec2 size );
	virtual void KillListHandler( ImDrawList* draw, ImVec2 pos, ImVec2 size );
	virtual void WeaponsHandler( ImDrawList* draw, ImVec2 pos, ImVec2 size );
	virtual void CrosshireHandler( ImDrawList* draw, ImVec2 pos, ImVec2 size );

	virtual void UpdateHUDStruct( C_BasePlayer* m_Player );
	virtual std::tuple<std::string, float> C_CustomHUD::GetWeaponIcon( std::string weapon );

private:
	HUDPlayerData_t m_tHUDData;

	int m_iKillPosition = 0;
	std::vector<KillMessage_t> m_tKillList;
};

inline C_CustomHUD* g_CustomHUD = new C_CustomHUD( );