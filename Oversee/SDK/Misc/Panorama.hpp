#pragma once
#include "../SDK.hpp"
class C_Panorama {
public:
	void RunScript(const char* code) {

		if (!menu_root) {

			auto _root = GetRoot();

			if (!_root) return;

			menu_root = _root;
		}
		SDK::Interfaces::PanoramaEngine->AccessUIEngine()->RunScript(menu_root, code, _S("panorama/layout/base_mainmenu.xml"), 8, 10, false, false);
	}
private:
	IUIPanel* GetRoot() {
		auto panel = SDK::Interfaces::PanoramaEngine->AccessUIEngine()->GetLastDispatchedEventTargetPanel();
		IUIPanel* ret = nullptr;

		while (panel && SDK::Interfaces::PanoramaEngine->AccessUIEngine()->IsValidPanelPointer(panel)) {

			if (FNV1A(panel->GetID()) == FNV1A("CSGOHud")) {
				ret = panel;
				break;
			}
			else if (FNV1A(panel->GetID()) == FNV1A("CSGOMainMenu")) {
				ret = panel;
				break;
			}

			panel = panel->GetParent();
		}
		return ret;
	}

	IUIPanel* menu_root = nullptr;
};

inline C_Panorama g_Panorama;