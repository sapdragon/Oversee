#pragma once
#include "../../../Libraries/LuaJIT/sol.hpp"
#include "../../../SDK/SDK.hpp"

class C_LuaAPI
{
public:
	virtual void Instance();
	virtual std::string GetCurrentScript(sol::this_state s);
	virtual void Load(int id);
	virtual void Unload(int id);
	void UnloadAll();
	virtual void InEditManager(int id, bool yeah);

	std::unordered_map<int, bool> InEdit;
	std::unordered_map<int, bool> Loaded;

	std::unordered_map<int, std::string> Scripts;
	std::unordered_map<int, std::string> InEditMD5;
	std::unordered_map<int, std::string> InEditFile;

private:
	virtual void SetupLuaJIT();
	virtual void SetupUserTypes();
	virtual void SetupLuaTables();
private:
	sol::state pLuaState;
};


inline C_LuaAPI* g_LuaAPI = new C_LuaAPI();