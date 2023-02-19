#include "../Hooks.hpp"

void FASTCALL C_Hooks::hkGetColorModulation(LPVOID pEcx, uint32_t, float* flRed, float* flGreen, float* flBlue)
{
	IMaterial* pMaterial = (IMaterial*)(pEcx);
	if (!pMaterial || pMaterial->IsErrorMaterial())
		return Hooks::o_GetColorModulation(pEcx, flRed, flGreen, flBlue);

	Hooks::o_GetColorModulation(pEcx, flRed, flGreen, flBlue);

	const char* strName = pMaterial->GetTextureGroupName();
	if (strstr(strName, _S(TEXTURE_GROUP_WORLD)))
	{
		*flRed = g_SettingsManager->C[_S("misc.world_modulation")].r() / 255.0f;
		*flGreen = g_SettingsManager->C[_S("misc.world_modulation")].g() / 255.0f;
		*flBlue = g_SettingsManager->C[_S("misc.world_modulation")].b() / 255.0f;
	}
	else if (strstr(strName, _S(TEXTURE_GROUP_STATIC_PROP)))
	{
		*flRed = g_SettingsManager->C[_S("misc.prop_modulation")].r() / 255.0f;
		*flGreen = g_SettingsManager->C[_S("misc.prop_modulation")].g() / 255.0f;
		*flBlue = g_SettingsManager->C[_S("misc.prop_modulation")].b() / 255.0f;
	}
	else if (strstr(strName, _S(TEXTURE_GROUP_SKYBOX)))
	{
		*flRed = g_SettingsManager->C[_S("misc.sky_modulation")].r() / 255.0f;
		*flGreen = g_SettingsManager->C[_S("misc.sky_modulation")].g() / 255.0f;
		*flBlue = g_SettingsManager->C[_S("misc.sky_modulation")].b() / 255.0f;
	}
}

float C_Hooks::hkGetAlphaModulation(LPVOID pEcx, uint32_t)
{
	IMaterial* pMaterial = (IMaterial*)(pEcx);
	if (!pMaterial || pMaterial->IsErrorMaterial())
		return Hooks::o_GetAlphaModulation(pEcx);

	const char* strName = pMaterial->GetTextureGroupName();
	if (strstr(strName, _S(TEXTURE_GROUP_WORLD)))
		return g_SettingsManager->C[_S("misc.world_modulation")].a() / 255.0f;
	else if (strstr(strName, _S(TEXTURE_GROUP_STATIC_PROP)))
		return g_SettingsManager->C[_S("misc.prop_modulation")].a() / 255.0f;
	else if (strstr(strName, _S(TEXTURE_GROUP_SKYBOX)))
		return g_SettingsManager->C[_S("misc.sky_modulation")].a() / 255.0f;

	return Hooks::o_GetAlphaModulation(pEcx);
}

bool __cdecl C_Hooks::hkIsUsingDebugStaticProps()
{
	return true;
}
