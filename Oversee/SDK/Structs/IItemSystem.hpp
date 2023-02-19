#pragma once
#include <cstddef>
#include <cstdint>
#include "IEconDefentions.hpp"
class C_ItemSchema {
public:
	VFunc(GetAttributeDefinitionInterface(int32_t iIndex), 27, uint32_t(__thiscall*)(void*, int32_t), iIndex);
	VFunc(GetItemDefentionByName(const char* szName), 42, C_EconItemDefinition*(__thiscall*)(void*, const char*), szName);

};

class C_ItemSystem {
public:
	std::byte pad_0[0xD4];
	uint32_t m_nItemID;
	std::byte pad_1[0xE];
	uint32_t m_nCountItemID;
	std::byte pad_2[0x138];
	uint32_t m_nAlternativeIcon;
	std::byte pad_3[0x14];
	uint32_t m_nCountAlternativeIcon;
	std::byte pad_4[0x50];
	uint32_t m_nPaintKit;
	std::byte pad_5[0x14];
	uint32_t m_nCountPaintKit;
	std::byte pad_6[0x8];
	uint32_t m_nStickerKit;
	std::byte pad_7[0x14];
	uint32_t m_nCountStickerKit;
	std::byte pad_8[0x124];
	uint32_t m_nMusicKit;
	std::byte pad_9[0x14];
	uint32_t m_nCountMusicKit;

	static C_ItemSystem* GetItemSystem();
	VFunc(GetItemSchemaInterface( ), 0, C_ItemSchema*(__thiscall*)( void* ) );


	const char* GetCreateSeriesByID(int32_t index);
	C_EconItemDefinition* GetItemByDefinitionIndex(int32_t iIndex) {
		for (size_t i = 0; i < m_nCountItemID; i++) {
			auto pItem = *reinterpret_cast<C_EconItemDefinition**>(m_nItemID + 0x4 * (0x3 * i) + 0x4);
			if (pItem->m_nID == iIndex)
				return pItem;
		}
		return nullptr;
	}

	C_EconPaintKitDefinition* GetPaintKitBySkinIndex(int32_t iIndex) {
		for (size_t i = 0; i <= m_nCountPaintKit; i++) {
			auto pKit = *reinterpret_cast<C_EconPaintKitDefinition**>(m_nPaintKit + 0x18 * i + 0x14);

			if (pKit->m_nID == iIndex)
				return pKit;
		}
		return nullptr;
	}
};