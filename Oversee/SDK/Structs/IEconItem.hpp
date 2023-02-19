#pragma once
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <vector>

#include "IItemSystem.hpp"
#include "IEconItem.hpp"

struct econ_attirbute {
	char m_szData[16];
	int32_t m_nSize;
	int32_t m_nType;
	int32_t m_nID;

	bool operator ==(const int32_t& iOther) { return m_nID == iOther; }
};

struct C_EconItem
{

	std::byte pad_0[0x8];
	uint64_t m_nItemID;
	uint64_t m_nOriginalID;
	std::byte pad_1[0x4];
	uint32_t m_nAccountID;
	uint32_t m_nInventory;
	uint16_t m_nWeaponID;

	uint16_t m_nOrigin : 5;
	uint16_t m_nQuality : 4;
	uint16_t m_nLevel : 2;
	uint16_t m_nRarity : 4;
	uint16_t m_nDirtybitInUse : 1;

	uint16_t m_nItemSet;
	std::byte pad_2[0x8];
	uint8_t m_nFlags;

	static C_EconItem* CreateEconItem();

	void SetAttributeValue(uint32_t iIndex, void* pValue);
	void SetPaintKit(float_t flPaintKit) {
		SetAttributeValue(6, &flPaintKit);
	}

	void SetSeed(float_t flSeed) {
		SetAttributeValue(7, &flSeed);
	}

	void SetWear(float_t flWear) {
		SetAttributeValue(8, &flWear);
	}

	void SetMusicID(int32_t iMusicID) {
		SetAttributeValue(166, &iMusicID);
	}

	void SetStatTrack(int32_t iValue) {
		SetAttributeValue(80, &iValue);
	}

	void SetStatTrackType(int32_t iType) {
		SetAttributeValue(81, &iType);
	}

	void SetStickerID(int32_t iSlot, int32_t iStickerID) {
		if (iSlot >= 0 && iSlot <= 5)
			SetAttributeValue(113 + 4 * iSlot, &iStickerID);
	}

	void SetStickerWear(int32_t slot, float_t wear) {
		if (slot >= 0 && slot <= 5)
			SetAttributeValue(114 + 4 * slot, &wear);
	}

	std::vector<econ_attirbute> GetAttributes() {
		std::vector<econ_attirbute> aAttributes;

		auto v16 = *(WORD**)(std::uintptr_t(this) + 0x18);
		if (v16) {
			auto v17 = v16 + 1;
			auto v48 = (uint32_t)&v16[4 * ((uint32_t)*v16 >> 10) + 1];
			if ((unsigned int)(v16 + 1) < v48) {
				do {
					auto v18 = (DWORD*)C_ItemSystem::GetItemSystem();
					auto v19 = *v17;
					if (v19 < v18[75]) {
						auto v20 = *(DWORD*)(v18[72] + 4 * v19);
						if (v20) {
							auto attrib = econ_attirbute();
							attrib.m_nID = v19;

							auto pAttribute = &attrib;

							*(DWORD*)((uintptr_t)pAttribute + 20) = 15;
							*(DWORD*)((uintptr_t)pAttribute + 16) = 0;
							*(uint8_t*)(uintptr_t)pAttribute = 0;

							auto v45 = *(DWORD*)(v20 + 0xC);

							(*(void(__thiscall**)(DWORD, int32_t, DWORD))(*(DWORD*)v45 + 12))(v45, (int32_t)(v17 + 2), (uintptr_t)pAttribute);

							aAttributes.push_back(attrib);
						}
					}

					v17 += 4;
				} while ((uint32_t)v17 < v48);
			}
		}

		return aAttributes;
	}
};