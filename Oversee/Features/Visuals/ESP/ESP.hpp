#pragma once
#include <unordered_map>
#include "../../../SDK/SDK.hpp"
#include "../../../Interface/GUI/Raw/SegouUI.hpp"
#include "../../Visuals/Model/Model.hpp"

constexpr float SPEED_FREQ = 255 / 4.0f;

struct ESPPlayerData_t
{
	RECT m_aBBox;

	// anims
	float m_flPrevHealth = 0.0f;
	float m_flPrevArmor = 0.0f;
	float m_flPrevAmmo = 0.0f;
	float m_flPlayerPercentage = 0.0f;

	// box width && height
	int m_iWidth = 0;
	int m_iHeight = 0;

	int m_iDownOffset = 0;
	int m_iUpOffset = 0;

	int m_iLeftDownOffset = 0;
	int m_iLeftOffset = 0;

	int m_iRightDownOffset = 0;
	int m_iRightOffset = 0;

	PlayerESPSettings_t m_Options;
};
class C_PlayerESP
{
public:
	virtual void Instance();
private:
	virtual void AddBar(DraggableItemCondiction pos, float& percentage, float max, ImColor color, ImColor color1, ImColor color2, ESPPlayerData_t* m_Data);
	virtual void AddText( std::string text, int size, DraggableItemCondiction pos, ImColor color, ESPPlayerData_t* m_Data );

	virtual void OutOfView(C_BasePlayer* pPlayer);
	virtual void RenderBox(int iIndexPlayer, ESPPlayerData_t* m_Data);

	virtual bool ForceDormant(C_BasePlayer* pPlayer);
	virtual std::string GetWeaponIcon(C_BasePlayer* pPlayer, C_BaseCombatWeapon* pWeapon);

protected:
	std::array < ESPPlayerData_t, 64 > m_PlayerData;
};

inline C_PlayerESP* g_PlayerESP = new C_PlayerESP();