#pragma once

#include <vector>
#include <ShlObj_core.h>


#include "../../../SDK/SDK.hpp"
#include "../../../Features/Inventory Changer/InventoryChanger.hpp"
#include "../Dependecies/json.hpp"

#include "../../../Features/Visuals/Model/Model.hpp"

class C_ConfigManager {
public:
	virtual void Action(bool save, nlohmann::json& m_jsonConfiguration);

private:
	virtual void ESPAction(nlohmann::json& j, bool action);
	virtual void InventoryChanger(nlohmann::json& j, bool action);
	virtual void BindsSaver(nlohmann::json& j, bool action);

	virtual void MakeAction(nlohmann::json& j, float v_default, bool action, std::string key);
	virtual void MakeAction(nlohmann::json& j, int v_default, bool action, std::string key);
	virtual void MakeAction(nlohmann::json& j, Color v_default, bool action, std::string key);
	void MakeAction(nlohmann::json& j, ImColor v_default, bool action, std::string key);
	virtual void MakeAction(nlohmann::json& j, bool v_default, bool action, std::string key);
};

inline C_ConfigManager* g_ConfigManager = new C_ConfigManager();