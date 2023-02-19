/*
* ImGui Tricks v1.02
* ImTricks is a library that simplifies and increases the functionality of ImGui.
* At the moment, Imtrucks adds functions to speed up the creation of animated menus,
* adds a notification system, and also adds support for ImColor in ColorEdit3/ColorEdit4.
*
* Made with love by pers0na2.
*/

#include "../../Includes.hpp"

#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

#ifndef IMGUI_TRICK_ONCE
#define IMGUI_TRICK_ONCE

enum NotifyState : int {
	ImTrickNotify_Success = 0,
	ImTrickNotify_Warning = 1,
	ImTrickNotify_Danger = 2,
	ImTrickNotify_Default = 3
};

namespace ImTricks
{
	static std::unordered_map<ImGuiID, int> bindsMapInt;
	static std::unordered_map<ImGuiID, int> comboMap;
	static std::unordered_map<ImGuiID, std::string> bindsMapString;

	namespace Animations
	{

		static std::unordered_map<ImGuiID, int> valuesMapInt;
		static std::unordered_map<ImGuiID, float> valuesMapFloat;

		static void SetupData( )
		{
			valuesMapInt.reserve( 1024 );
			bindsMapInt.reserve( 1024 );
			valuesMapFloat.reserve( 1024 );
			comboMap.reserve( 1024 );
			bindsMapString.reserve( 1024 );
		};

		extern int FastIntLerp( ImGuiID identifier, bool state, int min, int max, int speed );
		extern float FastFloatLerp( ImGuiID identifier, bool state, float min, float max, float speed );
		extern ImVec2 FastImVec2Lerp( ImGuiID identifier, bool state, ImVec2 min, ImVec2 max, float speed );
		extern ImColor FastColorLerp( ImColor start, ImColor end, float stage );
	}

	namespace NotifyManager {

		struct NotifyStruct {
			const char* message;
			NotifyState state;
			ULONGLONG time;
		};

		extern std::vector<NotifyStruct> notifies;
		extern void AddNotify( const char* message, NotifyState state );
		extern void HandleNotifies( ImDrawList* draw = ImGui::GetOverlayDrawList( ) );
	}
}

#endif