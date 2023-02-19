#include "../SDK.hpp"

C_EconItem* C_EconItem::CreateEconItem() {
	auto CreateEconitemFn = reinterpret_cast<C_EconItem * (__stdcall*)()>(*reinterpret_cast<uintptr_t*>(SDK::EngineData::m_AddressList[CheatAddressList::CreateEconItem]));
	return CreateEconitemFn();
}

void C_EconItem::SetAttributeValue(uint32_t iIndex, void* pValue) {
	if (const auto attribute = C_ItemSystem::GetItemSystem()->GetItemSchemaInterface()->GetAttributeDefinitionInterface(iIndex))
		reinterpret_cast<void(__thiscall*)(C_EconItem*, uint32_t, void*)>(SDK::EngineData::m_AddressList[CheatAddressList::SetAttributeValue])(this, attribute, pValue);
}