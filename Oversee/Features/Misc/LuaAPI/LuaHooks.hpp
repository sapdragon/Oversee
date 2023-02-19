#pragma once
#include <map>
#include "../../../Libraries/LuaJIT/sol.hpp"

struct LuaHook_t
{
	LuaHook_t( int iScriptID, sol::protected_function pFunc )
	{
		m_iScriptID = iScriptID;
		m_pFunc     = pFunc;
	}

	int m_iScriptID;
	sol::protected_function m_pFunc;
};

class C_LuaHookManager 
{
public:
	virtual void RegisterHook( std::string szHookName, int iScriptID, sol::protected_function pFunc );
	virtual void UnregisterHook ( int iScriptID );

	virtual std::vector < LuaHook_t > GetHooksList( std::string szHookName );

private:
	std::map< std::string, std::vector< LuaHook_t > > m_HooksList;
};

inline C_LuaHookManager* g_LuaHookManager = new C_LuaHookManager();