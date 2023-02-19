#include "LuaHooks.hpp"

void C_LuaHookManager::RegisterHook(std::string szHookName, int iScriptID, sol::protected_function pFunc)
{
	this->m_HooksList[ szHookName ].push_back( LuaHook_t ( iScriptID, pFunc ) );
}

void C_LuaHookManager::UnregisterHook(int iScriptID)
{
	for (auto& ev : this->m_HooksList) {
		int pos = 0;

		for (auto& hk : ev.second) {
			if (hk.m_iScriptID == iScriptID)
				ev.second.erase(ev.second.begin() + pos);

			pos++;
		}
	}
}

std::vector<LuaHook_t> C_LuaHookManager::GetHooksList(std::string szHookName)
{
	return this->m_HooksList[szHookName];
}
