#pragma once

#include <array>
#include "SDK/Utils/String/XorStr.hpp"
#include "SDK/Color/Color.hpp"
#include "Libraries/ImGui/imgui.h"
#include <unordered_map>
#include <d3dx9.h>

enum class RageHitgroup : int
{
	Head = 0,
	Chest,
	Stomach,
	Pelvis,
	Arms,
	Legs
};
enum RageHitbox
{
	Head = 0,
	Chest,
	LeftForeArm,
	RightForeArm,
	LeftUpperArm,
	RightUpperArm,
	Pelvis,
	Stomach,
	LeftThigh,
	RightThigh,
	LeftLeg,
	RightLeg
};

struct RageSettings_t
{
	bool m_bAutoStop = false;
	bool m_bAutoScope = false;
	bool m_bEarlyAutoStop = false;
	bool m_bSafePriority = false;
	bool m_bDynamicPoints = false;
	bool m_bExtendedTeleport = false;

	int m_nMinDamage = 80;
	int m_nHitChance = 80;
	int m_nAccuracyBoost = 80;
	int m_nHeadScale = 50;
	int m_nBodyScale = 80;

	std::array < bool, 12 > m_Hitboxes = { true, true, true, true, true, true, true, true, true, true }; //  0 Head, 1  Chest, 2 Left arm, 3 Right arm, 4 Pelvis, 5 Stomach, 6 Left leg, 7 Right leg 
};
struct GlowSettings_t
{
	bool m_bEnable = false;
	int m_nGlowStyle = 0;
	ImColor m_Color;
};
struct ChamsSettings_t
{
	bool m_bDrawChams = false;
	Color m_Color = Color( 0, 128, 255, 255 );
	int m_iFirstChamsLayerStyle = 0;
	std::array < bool, 5 > m_aMaterialParams = { false, false, false, false, false };
	Color m_aMaterialsColor[ 5 ] = { Color( 0, 128, 255, 255 ), Color( 0, 128, 255, 255 ), Color( 0, 128, 255, 255 ), Color( 0, 128, 255, 255 ), Color( 0, 128, 255, 255 ) };
};
struct PlayerESPSettings_t
{
	bool m_bEnable = true;
	bool m_bOnlyVisible = false;

	int m_bBox = 0;
	bool m_bBoxOutlineUpper = false;
	bool m_bBoxOutlineInner = false;
	bool m_bSkeleton = false;
	bool m_bMoney = false;
	bool m_bDistance = false;

	ImColor m_cHealthColorBottom = ImColor( 103, 235, 52, 190 );
	ImColor m_cHealthColorOutlineUpper = ImColor( 45, 45, 45, 70 );
	ImColor m_cHealthColorOutlineInner = ImColor( 45, 45, 45, 180 );

	ImColor m_cArmorColor = ImColor( 0, 221, 255, 255 );
	ImColor m_cArmorColorOutlineUpper = ImColor( 45, 45, 45, 70 );
	ImColor m_cArmorColorOutlineInner = ImColor( 45, 45, 45, 180 );

	ImColor m_cAmmoColor = ImColor( 219, 97, 26, 255 );
	ImColor m_cAmmoColorOutlineUpper = ImColor( 45, 45, 45, 70 );
	ImColor m_cAmmoColorOutlineInner = ImColor( 45, 45, 45, 180 );

	ImColor m_cBoxColor = ImColor( 240, 240, 240, 190 );
	ImColor m_cBoxColorOutlineUpper = ImColor( 240, 240, 240, 190 );
	ImColor m_cBoxColorOutlineInner = ImColor( 240, 240, 240, 190 );

	ImColor m_cPingColor = ImColor( 240, 240, 240, 255 );
	ImColor m_cScopeColor = ImColor( 240, 240, 240, 255 );
	ImColor m_cDefusingColor = ImColor( 240, 240, 240, 255 );
	ImColor m_cFlashedColor = ImColor( 240, 240, 240, 255 );
	ImColor m_cTaserColor = ImColor( 240, 240, 240, 255 );
	ImColor m_cLCColor = ImColor( 240, 240, 240, 255 );
	ImColor m_cDuckingcColor = ImColor( 240, 240, 240, 255 );
	ImColor m_cWeaponColor = ImColor( 240, 240, 240, 255 );
	ImColor m_cNameColor = ImColor( 240, 240, 240, 255 );

	std::array<int, 9> m_aFontSizes = { 14, 14, 14, 14, 14, 14, 14, 14, 14 };

	bool m_bAmmoAmount = true;
};
struct InventoryItem_t {
	int16_t m_nItemID = 0;
	int32_t m_iPaintKit = 0;
	int32_t m_flPearlescent = 0.f;
	float_t m_flWear = 0.f;
	float_t color[ 4 ][ 3 ] = { { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f } };

	struct Stattrack_t {
		bool enabled = false;
		int32_t counter = 0;
	} m_StatTrack;

	struct Stickers_t {
		int32_t m_nID = 0;
		float_t m_flWear = 0.0f;
	};

	std::array<Stickers_t, 5> m_aStickers;

	int32_t ct_team, t_team, none_team;
	int16_t m_iRarity;
	std::string SkinName;
	IDirect3DTexture9* m_tTexture;
};
enum EventLogs : int
{
	ItemPurchase,
	PlayerHurt,
	PlayerHarm,
	Bomb,
	Misses
};
class C_SettingsManager {
public:
	virtual void SetupData( );

	virtual RageSettings_t& GetRagebotSettings( std::string subname );
	virtual GlowSettings_t GetGlowSettings( std::string subname );
	virtual ChamsSettings_t GetChamsSettings( std::string subname );
	virtual PlayerESPSettings_t GetEspSettings( std::string subname );

	std::unordered_map<std::string, bool> B;
	std::unordered_map<std::string, int> I;
	std::unordered_map<std::string, float> F;
	std::unordered_map<std::string, ImColor> IC;
	std::unordered_map<std::string, Color> C;

	std::unordered_map<uint64_t, InventoryItem_t> m_aInventoryItemList;
};

inline C_SettingsManager* g_SettingsManager = new C_SettingsManager( );