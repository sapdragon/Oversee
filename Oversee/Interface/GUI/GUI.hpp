#pragma once

#include <array>
#include <functional>
#include "Menu.hpp"
#include "../../Libraries/Ethernet API/API/CloudConfigs.hpp"
#include "../../Libraries/Ethernet API/API/CloudScripts.hpp"
#include "MenuChars.hpp"
#include "../../Features/Misc/Keybind V2/KeyManager.hpp"
#include "../../Features/Inventory Changer/ItemsManager.hpp"

typedef int OverallElementsFlag;
enum OverallElementsFlags_ {
	OverallFlags_NotBindable = 0,
	OverallFlags_None = 1 << 2,
	OverallFlags_AutoScale = 1 << 3,
};

class C_UIElements
{
public:
	virtual void KeyBind(const char* szLabelName, int& m_KeyBind);
	virtual void Checkbox(const char* szLabelName, std::string value, std::function<void()> addictional_settings, float height, OverallElementsFlag flags = NULL);
	virtual void Checkbox(const char* szLabelName, std::string value, OverallElementsFlag flags = NULL);
	virtual void MasterSwitch(const char* szLabelName, std::string value);
	virtual void Hitbox(const char* szLabelName, std::string value);
	virtual bool Combo(const char* label, std::string value, std::vector<std::string> items, OverallElementsFlag flags = NULL);
	virtual bool BeginCombo(const char* label, const char* preview_value, ImGuiComboFlags flags = NULL, float width = 270);
	virtual void MultiCombo(const char* name, std::vector < std::string > variable, std::vector < std::string > labels);
	virtual bool SliderFloat(const char* label, std::string value, float v_min, float v_max, const char* format = NULL, ImGuiSliderFlags flag = NULL, OverallElementsFlag flags = NULL);
	virtual bool SliderInt(const char* label, std::string value, int v_min, int v_max, const char* format = NULL, ImGuiSliderFlags flag = NULL, OverallElementsFlag flags = NULL);

	virtual void BeginChild(const char* str_id, const ImVec2 size_arg, bool border = false, ImGuiWindowFlags extra_flags = NULL);
	virtual void EndChild();
	virtual void Tab(const char* szLabelName, const char* szFullName, int& tab, int index, bool wide = false);
	virtual void Subtab(const char* szLabelName, int& tab, int index);
	virtual void SameLineWithoutPadding(float offset_from_start_x = 0, float spacing_w = 0);

	virtual void Configuration(CloudConfig_t config);
	virtual void Script(CloudScript_t config);
	virtual bool Spinner(const char* label, float radius, int thickness, const ImU32& color, ImVec2 pos);
	virtual void AddCircleImageFilled(ImTextureID user_texture_id, const ImVec2& centre, float radius, ImU32 col, int num_segments);
	virtual void PlayerList(const char* name, int index, const ImVec2& size_arg);
	virtual int KeybindVisibleElement(OnKeyAction_t key, int index);
	virtual bool InventoryItem(InventoryItem_t Item);
	virtual bool InventoryItemDefault(DefaultItem_t* Item);
	virtual bool InventoryItemAgent(AgentsItem_t* Item);
	virtual bool InventoryItemMusic(MusicItem_t* Item);
	virtual bool InventoryItemGlove(GloveItem_t* Item);
	virtual bool InventoryItem(WeaponItem_t* Item);
	virtual void WeaponSelect(const char* szLabelName, const char* icon, std::string& value);
	virtual void DashboardItem(const char* szLabelName, bool& v);
	virtual void Animate();
	float swapAnimation = 0.f;
private:
	int m_nItemsPadding = 10;
};

inline C_UIElements* g_UIElements = new C_UIElements();