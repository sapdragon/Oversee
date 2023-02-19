#include <cstdint>
#include "../../SDK/SDK.hpp"

void IEngineTrace::TraceLine(const Vector src, const Vector dst, int mask, IHandleEntity* entity, int collision_group, CGameTrace* trace) {
	std::uintptr_t filter[4] = {
		*reinterpret_cast<std::uintptr_t*>( SDK::EngineData::m_AddressList[ CheatAddressList::TraceFilterSimple ] ),
		reinterpret_cast<std::uintptr_t>(entity),
		(uintptr_t)(collision_group),
		0
	};

	auto ray = Ray_t();
	ray.Init(src, dst);

	return TraceRay(ray, mask, (CTraceFilter*)(filter), trace);
}

void IEngineTrace::TraceHull(const Vector src, const Vector dst, const Vector mins, const Vector maxs, int mask, IHandleEntity* entity, int collision_group, CGameTrace* trace) {
	std::uintptr_t filter[4] = {
		*reinterpret_cast<std::uintptr_t*>(SDK::EngineData::m_AddressList[ CheatAddressList::TraceFilterSimple ]),
		reinterpret_cast<std::uintptr_t>(entity),
		 (uintptr_t)(collision_group),
		0
	};

	auto ray = Ray_t();
	ray.Init(src, dst, mins, maxs);

	return TraceRay(ray, mask, (CTraceFilter*)(filter), trace);
}