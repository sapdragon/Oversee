#include "../SDK.hpp"

 C_ItemSystem* C_ItemSystem::GetItemSystem() {
	auto fn_item_system = reinterpret_cast<C_ItemSystem * (__stdcall*)()>(SDK::EngineData::m_AddressList[CheatAddressList::ItemSystem]);
	return fn_item_system();
}

 const char* C_ItemSystem::GetCreateSeriesByID(int32_t iIndex) {
	 auto CreateSeriesByIDFn = reinterpret_cast<int32_t(__thiscall*)(uintptr_t, int32_t*)>(SDK::EngineData::m_AddressList[CheatAddressList::CreateSeriesByID]);

	 auto iID = CreateSeriesByIDFn((uintptr_t)this + 0x17C, &iIndex);

	 if (iID == -1)
		 return nullptr;

	 auto v11 = *(uint32_t*)((uintptr_t)this + 0x17C + 4) + 24 * iID;

	 return *reinterpret_cast<const char**>(v11 + 0x14);
 }