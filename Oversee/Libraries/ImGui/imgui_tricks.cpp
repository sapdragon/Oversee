#include "imgui_tricks.hpp"

namespace ImTricks {

	namespace Animations {

		int FastIntLerp(ImGuiID identifier, bool state, int min, int max, int speed) {

			auto value = valuesMapInt.find(identifier);

			if (value == valuesMapInt.end()) {
				valuesMapInt.insert({ identifier,  min });
				value = valuesMapInt.find(identifier);
			}

			const float frameRateSpeed = speed * (1.f - ImGui::GetIO().DeltaTime);

			if (state) {
				if (value->second < max)
					value->second += frameRateSpeed;
			}
			else {
				if (value->second > min)
					value->second -= frameRateSpeed;
			}

			value->second = std::clamp(value->second, min, max);

			return value->second;
		}

		float FastFloatLerp(ImGuiID identifier, bool state, float min, float max, float speed) {

			auto value = valuesMapFloat.find(identifier);

			if (value == valuesMapFloat.end()) {
				valuesMapFloat.insert({ identifier, min });
				value = valuesMapFloat.find(identifier);
			}

			const float frameRateSpeed = speed * (1.f - ImGui::GetIO().DeltaTime);

			if (state) {
				if (value->second < max)
					value->second += frameRateSpeed;
			}
			else {
				if (value->second > min)
					value->second -= frameRateSpeed;
			}

			value->second = std::clamp(value->second, min, max);

			return value->second;
		}

		ImVec2 FastImVec2Lerp(ImGuiID identifier, bool state, ImVec2 min, ImVec2 max, float speed) {

			static std::unordered_map<ImGuiID, ImVec2> valuesMapFloat;
			auto value = valuesMapFloat.find(identifier);

			if (value == valuesMapFloat.end()) {
				valuesMapFloat.insert({ identifier, min });
				value = valuesMapFloat.find(identifier);
			}

			const float frameRateSpeed = speed * (1.f - ImGui::GetIO().DeltaTime);

			if (state) {
				if (value->second.x < max.x)
					value->second.x += frameRateSpeed;

				if (value->second.y < max.y)
					value->second.y += frameRateSpeed;
			}
			else {
				if (value->second.x > min.x)
					value->second.x -= frameRateSpeed;

				if (value->second.y > min.y)
					value->second.y -= frameRateSpeed;
			}

			value->second.x = std::clamp(value->second.x, min.x, max.x);
			value->second.y = std::clamp(value->second.y, min.y, max.y);

			return value->second;
		}

		ImColor FastColorLerp(ImColor start, ImColor end, float stage)
		{
			ImVec4 lerp = ImLerp(
				ImVec4(start.Value.x, start.Value.y, start.Value.z, start.Value.w),
				ImVec4(end.Value.x, end.Value.y, end.Value.z, end.Value.w),
				stage);

			return ImGui::ColorConvertFloat4ToU32(lerp);
		}
	}

	namespace NotifyManager {

		std::vector<NotifyStruct> notifies;

		void AddNotify(const char* message, NotifyState state) {
			notifies.push_back({ message, state, GetTickCount64() + 3000 });
		}

		void HandleNotifies(ImDrawList* draw) {

			if (notifies.empty())
				return;

			const auto ScreenSize = ImGui::GetIO().DisplaySize;
			ImVec2 NotifyPos = ScreenSize - ImVec2(320.f, 50.f);

			auto DrawNotify = [&draw, &NotifyPos](NotifyStruct notify) {

				const auto NotifyEndPos = NotifyPos + ImVec2(300, 30);

				draw->AddRectFilled(NotifyPos, NotifyEndPos, ImGui::GetColorU32(ImGuiCol_PopupBg), ImGui::GetStyle().PopupRounding);

				auto StateColor = ImColor(45, 45, 45);

				switch (notify.state) {
				case ImTrickNotify_Success: StateColor = ImColor(0, 255, 0); break;
				case ImTrickNotify_Warning: StateColor = ImColor(130, 255, 0); break;
				case ImTrickNotify_Danger: StateColor = ImColor(255, 0, 0); break;
				case ImTrickNotify_Default:
				default:
					StateColor = ImColor(45, 45, 45);
					break;
				}

				draw->AddRectFilled(NotifyPos, ImVec2(NotifyPos.x + 5.f, NotifyPos.y + 30), StateColor, 1.f);

				const auto TextSize = ImGui::CalcTextSize(notify.message);
				const auto TextPos = NotifyPos + ImVec2(15.f, 15.f - TextSize.y / 2.f);

				draw->AddText(TextPos, ImGui::GetColorU32(ImGuiCol_Text), notify.message);

				NotifyPos.y -= 40.f;
			};

			for (int i = 0; i < notifies.size(); i++)
			{
				auto current_notify = notifies.at(i);
				if (current_notify.time < GetTickCount64())
				{
					notifies.erase(notifies.begin() + i);
					continue;
				}
				DrawNotify(current_notify);
			}
		}
	}
}