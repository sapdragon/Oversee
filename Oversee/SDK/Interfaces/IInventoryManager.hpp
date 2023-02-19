#pragma once

class IInventoryManager
{
public:
	VFunc(EquipItemInLoadout(int32_t team, int32_t slot, uint64_t ItemID, bool swap), 20, void*(__thiscall*)(void*, int32_t, int32_t, uint64_t, bool), team, slot, ItemID, swap);
	VFunc(GetLocalPlayerInventory(), 23, IPlayerInventory* (__thiscall*)(void*) );
};