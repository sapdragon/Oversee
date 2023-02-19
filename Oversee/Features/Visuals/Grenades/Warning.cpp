#include "Warning.hpp"

void C_GrenadePrediction::OnCreateMove(C_UserCmd* pCmd)
{
	m_data = { };
	const auto weapon = g_Globals->m_LocalPlayer->m_hActiveWeapon().Get();
	if (!weapon
		 || !weapon->m_bPinPulled() && weapon->m_fThrowTime() == 0.f)
		return;

	if (!g_SettingsManager->B[_S("misc.grenade_warning")])
		return;

	const auto weapon_data = weapon->GetWeaponData();
	if (!weapon_data
		 || weapon_data->m_iWeaponType != WEAPONTYPE_GRENADE)
		return;

	m_data.m_owner = g_Globals->m_LocalPlayer;
	m_data.m_index = weapon->m_nItemID();

	auto view_angles = pCmd->m_angViewAngles;

	if (view_angles.pitch < -90.f) {
		view_angles.pitch += 360.f;
	}
	else if (view_angles.pitch > 90.f) {
		view_angles.pitch -= 360.f;
	}

	view_angles.pitch -= (90.f - std::fabsf(view_angles.pitch)) * 10.f / 90.f;

	auto direction = Vector();

	Math::AngleVectors(view_angles, direction);

	const auto throw_strength = std::clamp< float >(weapon->m_flThrowStrength(), 0.f, 1.f);
	const auto eye_pos = g_Globals->m_LocalPlayer->GetShootPosition();
	const auto src = Vector(eye_pos.x, eye_pos.y, eye_pos.z + (throw_strength * 12.f - 12.f));

	auto trace = CGameTrace();

	SDK::Interfaces::EngineTrace->TraceHull(
		src, src + direction * 22.f, { -2.f, -2.f, -2.f }, { 2.f, 2.f, 2.f },
		MASK_SOLID | CONTENTS_CURRENT_90, g_Globals->m_LocalPlayer, COLLISION_GROUP_NONE, &trace
	);

	m_data.predict(
		trace.endpos - direction * 6.f,
		direction * (std::clamp< float >(
			weapon_data->m_flThrowVelocity * 0.9f, 15.f, 750.f
			) * (throw_strength * 0.7f + 0.3f)) + g_Globals->m_LocalPlayer->m_vecVelocity() * 1.25f,
		TICKS_TO_TIME(g_Globals->m_LocalPlayer->m_nTickBase()),
		0
	);
}