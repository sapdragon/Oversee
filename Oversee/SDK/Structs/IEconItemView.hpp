#pragma once

#pragma once
#include <cstdint>
#include "IEconItem.hpp"

// FIX ME; This define using in 1 place, recode pls
#define mGetOffset( type, func, offset ) type& func() { return *reinterpret_cast<type*>( reinterpret_cast<uint32_t>( this ) + offset ); }

struct C_EconItemView {
	void GetClearInventoryImageRGBA();

	C_EconItem* GetSocData();

	C_EconItemDefinition* GetStaticData();

	const char* GetCrateSeries() {


		auto aAttributesCase = this->GetStaticData()->GetAttributes();

		auto aSupplyCreateSeries = std::find(aAttributesCase.begin(), aAttributesCase.end(), 68);
		if (aSupplyCreateSeries != aAttributesCase.end())
			return C_ItemSystem::GetItemSystem()->GetCreateSeriesByID(aSupplyCreateSeries->m_nValue);

		return nullptr;
	}

	bool ToolCanApplyTo(C_EconItemView* pItem);

	mGetOffset(uint32_t, GetInventory, 0x20C);
};
