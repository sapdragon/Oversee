#include "../SDK.hpp"

void C_EconItemView::GetClearInventoryImageRGBA() {
	auto ClearInventoryImageRGBAFn = reinterpret_cast<int32_t * (__thiscall*)(C_EconItemView*)>(SDK::EngineData::m_AddressList[CheatAddressList::ClearInventoryImageRGBA]);
	ClearInventoryImageRGBAFn(this);
}

C_EconItem* C_EconItemView::GetSocData() {
	auto GetSocDataFn = reinterpret_cast<C_EconItem * (__thiscall*)(C_EconItemView*)>(SDK::EngineData::m_AddressList[CheatAddressList::SOCData]);
	return GetSocDataFn(this);
}

C_EconItemDefinition* C_EconItemView::GetStaticData() {
	auto GetStaticDataFn = reinterpret_cast<C_EconItemDefinition * (__thiscall*)(C_EconItemView*)>(SDK::EngineData::m_AddressList[CheatAddressList::StaticData]);
	return GetStaticDataFn(this);
}

bool __declspec(noinline) C_EconItemView::ToolCanApplyTo(C_EconItemView* pItem) {
	bool bReturnValue;
	void* pAddr = SDK::EngineData::m_AddressList[CheatAddressList::ClearInventoryImageRGBA];
	__asm {
		mov eax, this
		add eax, 0xC
		mov ecx, eax
		mov eax, pItem
		add eax, 0xC
		mov edx, eax
		push 0x4
		call pAddr
		mov bReturnValue, al
		add esp, 4
	};

	return bReturnValue;
}
