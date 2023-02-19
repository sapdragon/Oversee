#pragma once
#include <cstdint>
#include <vector>

#include "IEconItem.hpp"
#include "../Misc/Vfunc.hpp"
typedef unsigned long       DWORD;
class C_SharedObject {
public:

	VFunc(GetTypeID() , 1, int32_t(__thiscall*) (void*));

};

class C_SharedObjectTypeCache {
public:

	VFunc(AddObject(void* obj), 1, void(__thiscall*) (void*, void*), obj);

	VFunc(RemoveObject(void* obj), 3, void(__thiscall*) (void*, void*), obj);

	std::vector<C_EconItem*> GetEconItems() {
		std::vector<C_EconItem*> aReturn;

		auto iSize = *reinterpret_cast<uint32_t*>(uint32_t(this) + 0x18);
		auto pData = *reinterpret_cast<uint32_t**>(uint32_t(this) + 0x4);

		for (uint32_t i = 0; i < iSize; i++)
			aReturn.push_back(reinterpret_cast<C_EconItem*>(pData[i]));

		return aReturn;
	}
};

struct C_ClientSharedObjectCache {
	C_SharedObjectTypeCache* FindBaseTypeCache(int32_t iClassID);
};