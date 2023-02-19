#pragma once
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "../Misc/VFunc.hpp"
#include "../Color/Color.hpp"

struct C_EconPaintKitDefinition {
	int32_t m_nID;
	const char* m_szName;
	std::byte pad_0[0x1C];
	const char* m_nDescriptionTag;
	std::byte pad_1[0x50];
	int32_t m_nRarity;
	int32_t m_nStyle;
	Color m_cColor[4];
	std::byte pad_3[0x58];
	float_t m_flPearcelent;
};
struct C_EconMusicDefinition {
	int32_t m_nID;
	std::byte pad_0[0x4];
	const char* m_szLocalizatedName;
	std::byte pad_1[0x8];
	const char* m_szInventoryImage;
};

struct C_EconStickerDefinition {
	uint32_t m_nID;
	int32_t m_nRarity;
	std::byte pad_0[0x20];
	const char* m_szName;
	std::byte pad_1[0x2C];
	const char* m_szInventoryImage;
};

#pragma once

struct attribute_info {
	int16_t m_nID;
	int32_t m_nValue;

	bool operator ==(const int16_t& iOther) { return m_nID == iOther; }
};

class C_EconItemDefinition {
public:

	std::byte pad_0[0x8];
	int16_t m_nID;
	std::byte pad_1[0x40];
	const char* m_szName;
	std::byte pad_2[0x4];
	const char* m_szType;
	std::byte pad_3[0x14];
	const char* m_szInventoryImage;
	std::byte pad_4[0x22];
	const char* m_szDisplayModelName;
	std::byte pad_5[0x4];
	const char* m_szWorldModelName;
	std::byte pad_6[0x13C];
	const char* m_szItemName;
	std::byte pad_7[0x88];
	int32_t m_nLoadoutSlot;

	VFunc(GetRarityValue( ), 12, uint8_t(__thiscall*)( void* ) );
	VFunc(GetItemBaseName(), 2, const char* (__thiscall*)(LPVOID))


	std::vector<attribute_info> GetAttributes() {
		std::vector<attribute_info> aAttributes;

		auto iSize = *reinterpret_cast<int32_t*>(std::uintptr_t(this) + 0x3C);
		auto pData = *reinterpret_cast<uintptr_t*>(std::uintptr_t(this) + 0x30);

		if (pData) {
			for (int i = 0; i < iSize; i++) {
				auto iID = *reinterpret_cast<int16_t*>(pData + (i * 0xC));
				auto iValue = *reinterpret_cast<int32_t*>(pData + (i * 0xC) + 0x4);
				aAttributes.push_back({ iID,iValue });
			}
		}

		return aAttributes;
	}
};