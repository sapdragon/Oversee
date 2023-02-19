#include "../SDK.hpp"

C_SharedObjectTypeCache* C_ClientSharedObjectCache::FindBaseTypeCache(int32_t iClassID) {
	auto CreateBaseTypeCacheFn = reinterpret_cast<C_SharedObjectTypeCache * (__thiscall*)(void*, int32_t)>(SDK::EngineData::m_AddressList[CheatAddressList::CreateBaseTypeCache]);
	return CreateBaseTypeCacheFn(this, iClassID);
}