#include "SDK.hpp"
#include "../Hooks/Hooks.hpp"
#include "../Interface/GUI/Menu.hpp"
#include "../Data/WeaponFont.hpp"
#include "../Interface/Renderer/Renderer.hpp"
#include "../Features/Fonts/FontManager.hpp"
#include "../Features/Visuals/Chams/Chams.hpp"
#include "../Libraries/Ethernet API/API/CloudConfigs.hpp"
#include "../Libraries/Ethernet API/API/CloudScripts.hpp"
#include "../Features/Inventory Changer/InventoryChanger.hpp"
#include "../Features/Inventory Changer/ItemsManager.hpp"
#include "Utils/Utils.hpp"

bool C_BaseEntity::IsFriend( )
{
	if ( this == g_Globals->m_LocalPlayer )
		return false;

	if ( SDK::EngineData::m_ConvarList[ CheatConvarList::TeammatesAreEnemies ]->GetBool( ) )
		return false;

	if ( this->m_iTeamNum( ) != g_Globals->m_LocalPlayer->m_iTeamNum( ) )
		return false;

	if ( this->m_nSurvivalTeam( ) != g_Globals->m_LocalPlayer->m_nSurvivalTeam( ) )
		return false;

	return true;
}
inline void SetupConVars( )
{
	SDK::EngineData::m_ConvarList[ CheatConvarList::Yaw ] = SDK::Interfaces::CVar->FindVar( _S( "m_yaw" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::Pitch ] = SDK::Interfaces::CVar->FindVar( _S( "m_pitch" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::Sensitivity ] = SDK::Interfaces::CVar->FindVar( _S( "sensitivity" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::DamageReductionBullets ] = SDK::Interfaces::CVar->FindVar( _S( "ff_damage_reduction_bullets" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::RollSpeed ] = SDK::Interfaces::CVar->FindVar( _S( "sv_rollspeed" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::Friction ] = SDK::Interfaces::CVar->FindVar( _S( "sv_friction" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::StopSpeed ] = SDK::Interfaces::CVar->FindVar( _S( "sv_stopspeed" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::SkyBox3D ] = SDK::Interfaces::CVar->FindVar( _S( "r_3dsky" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::MaxSpeed ] = SDK::Interfaces::CVar->FindVar( _S( "sv_maxspeed" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::Gravity ] = SDK::Interfaces::CVar->FindVar( _S( "sv_gravity" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::JumpImpulse ] = SDK::Interfaces::CVar->FindVar( _S( "sv_jump_impulse" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::ViolenceHBlood ] = SDK::Interfaces::CVar->FindVar( _S( "violence_hblood" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::ViolenceABlood ] = SDK::Interfaces::CVar->FindVar( _S( "violence_ablood" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::CsmWpnSwayAmount ] = SDK::Interfaces::CVar->FindVar( _S( "cl_wpn_sway_scale" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::CsmShadows ] = SDK::Interfaces::CVar->FindVar( _S( "cl_csm_shadows" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::FootContactShadows ] = SDK::Interfaces::CVar->FindVar( _S( "cl_foot_contact_shadows" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::CsmStaticPropShadows ] = SDK::Interfaces::CVar->FindVar( _S( "cl_csm_static_prop_shadows" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::CsmWorldShadows ] = SDK::Interfaces::CVar->FindVar( _S( "cl_csm_world_shadows" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::CsmViewmodelShadows ] = SDK::Interfaces::CVar->FindVar( _S( "cl_csm_viewmodel_shadows" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::CsmSpriteShadows ] = SDK::Interfaces::CVar->FindVar( _S( "cl_csm_sprite_shadows" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::CsmRopeShadows ] = SDK::Interfaces::CVar->FindVar( _S( "cl_csm_rope_shadows" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::HeadDamageScaleCT ] = SDK::Interfaces::CVar->FindVar( _S( "mp_damage_scale_ct_head" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::HeadDamageScaleT ] = SDK::Interfaces::CVar->FindVar( _S( "mp_damage_scale_t_head" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::BodyDamageScaleCT ] = SDK::Interfaces::CVar->FindVar( _S( "mp_damage_scale_ct_body" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::BodyDamageScaleT ] = SDK::Interfaces::CVar->FindVar( _S( "mp_damage_scale_t_body" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::HideHud ] = SDK::Interfaces::CVar->FindVar( _S( "cl_drawhud" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::ClInterp ] = SDK::Interfaces::CVar->FindVar( _S( "cl_interp" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::ClInterpRatio ] = SDK::Interfaces::CVar->FindVar( _S( "cl_interp_ratio" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::ClUpdateRate ] = SDK::Interfaces::CVar->FindVar( _S( "cl_updaterate" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::SvMinUpdateRate ] = SDK::Interfaces::CVar->FindVar( _S( "sv_minupdaterate" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::SvMaxUpdateRate ] = SDK::Interfaces::CVar->FindVar( _S( "sv_maxupdaterate" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::SvClientMinInterpRatio ] = SDK::Interfaces::CVar->FindVar( _S( "sv_client_min_interp_ratio" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::SvClientMaxInterpRatio ] = SDK::Interfaces::CVar->FindVar( _S( "sv_client_max_interp_ratio" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::ClLagCompensation ] = SDK::Interfaces::CVar->FindVar( _S( "cl_lagcompensation" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::WeaponRecoilScale ] = SDK::Interfaces::CVar->FindVar( _S( "weapon_recoil_scale" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::WeaponAccuracyNoSpread ] = SDK::Interfaces::CVar->FindVar( _S( "weapon_accuracy_nospread" ) );
	SDK::EngineData::m_ConvarList[ CheatConvarList::TeammatesAreEnemies ] = SDK::Interfaces::CVar->FindVar( _S( "mp_teammates_are_enemies" ) );

	/* remove callbacks */
	SDK::EngineData::m_ConvarList[ CheatConvarList::CsmWpnSwayAmount ]->m_fnChangeCallbacks = 0;
	SDK::EngineData::m_ConvarList[ CheatConvarList::CsmShadows ]->m_fnChangeCallbacks = 0;
	SDK::EngineData::m_ConvarList[ CheatConvarList::FootContactShadows ]->m_fnChangeCallbacks = 0;
	SDK::EngineData::m_ConvarList[ CheatConvarList::CsmStaticPropShadows ]->m_fnChangeCallbacks = 0;
	SDK::EngineData::m_ConvarList[ CheatConvarList::CsmWorldShadows ]->m_fnChangeCallbacks = 0;
	SDK::EngineData::m_ConvarList[ CheatConvarList::CsmViewmodelShadows ]->m_fnChangeCallbacks = 0;
	SDK::EngineData::m_ConvarList[ CheatConvarList::CsmSpriteShadows ]->m_fnChangeCallbacks = 0;
	SDK::EngineData::m_ConvarList[ CheatConvarList::CsmRopeShadows ]->m_fnChangeCallbacks = 0;
	SDK::EngineData::m_ConvarList[ CheatConvarList::HideHud ]->m_fnChangeCallbacks = 0;
	SDK::EngineData::m_ConvarList[ CheatConvarList::SkyBox3D ]->m_fnChangeCallbacks = 0;
	SDK::EngineData::m_ConvarList[ CheatConvarList::Sensitivity ]->m_fnChangeCallbacks = 0;
}
inline void SetupAddressList( )
{
	SDK::EngineData::m_AddressList[ CheatAddressList::PredictionPlayer ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "89 35 ? ? ? ? F3 0F 10 48 20" ) ) ) + 0x2 );
	SDK::EngineData::m_AddressList[ CheatAddressList::PredictionSeed ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04" ) ) ) + 0x2 );
	SDK::EngineData::m_AddressList[ CheatAddressList::InvalidatePhysicsRecursive ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 E4 F8 83 EC 0C 53 8B 5D 08 8B C3" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::ReadSubChannelData_Return ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "engine.dll" ), _S( "FF 50 34 85 C0 74 12 53 FF 75 0C 68" ) ) ) + 0x3 );
	SDK::EngineData::m_AddressList[ CheatAddressList::CheckReceivingList_Return ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "engine.dll" ), _S( "FF 50 34 8B 1D ? ? ? ? 85 C0 74 16 FF B6" ) ) ) + 0x3 );
	SDK::EngineData::m_AddressList[ CheatAddressList::ClPredDoResetLatch ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "85 C0 75 28 8B 0D ? ? ? ? 81" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::CamThink_Return ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "85 C0 75 30 38 86" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::SetupBones_Return ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "84 C0 74 0A F3 0F 10 05 ? ? ? ? EB 05" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::PostProcessing ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "83 EC 4C 80 3D" ) ) ) + 0x5 );
	SDK::EngineData::m_AddressList[ CheatAddressList::GetSequenceLinearMotion ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 EC 0C 56 8B F1 57 8B FA 85 F6 75 14" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::SmokeCount ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "A3 ? ? ? ? 57 8B CB" ) ) ) + 0x1 );
	SDK::EngineData::m_AddressList[ CheatAddressList::IsBreakableEntity ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 51 56 8B F1 85 F6 74 68" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::WalkMove ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "8B 81 54 0E 00 00 83 B8 E4 9A 00 00 00 74 0A C7" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::SetAbsAngles ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::SetAbsOrigin ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8 1F" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::GetSequenceActivity ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 53 8B 5D 08 56 8B F1 83" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::ModelBoneCounter ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::SetupBonesAttachmentHelper ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 EC 48 53 8B 5D 08 89 4D F4" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::TraceFilterSimple ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 E4 F0 83 EC 7C 56 52" ) ) ) + 0x3D );
	SDK::EngineData::m_AddressList[ CheatAddressList::TraceFilterSkipTwoEntities ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 81 EC BC 00 00 00 56 8B F1 8B 86" ) ) ) + 0x226 );
	SDK::EngineData::m_AddressList[ CheatAddressList::UpdateCollisionBounds ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "56 57 8B F9 8B 0D ? ? ? ? F6 87 04 01" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::WriteUsercmd ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::ClipRayToHitbox ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 E4 F8 F3 0F 10 42" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::KeyValues_FindKey ] = ( void* ) ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 EC 1C 53 8B D9 85 DB" ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::KeyValues_GetString ] = ( void* ) ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 E4 C0 81 EC ? ? ? ? 53 8B 5D 08" ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::KeyValues_LoadFromBuffer ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89 4C 24 04" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::KeyValues_SetString ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC A1 ? ? ? ? 53 56 57 8B F9 8B 08 8B 01" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::FindHudElement ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::ClearDeathList ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 EC 0C 53 56 8B 71" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::FindHudElementPtr ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 5D 08" ) ) ) + 0x1 );
	SDK::EngineData::m_AddressList[ CheatAddressList::GetEconItemView ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "8B 81 ? ? ? ? 81 C1 ? ? ? ? FF 50 04 83 C0 40 C3" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::GetStaticData ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 51 53 8B D9 8B 0D ?? ?? ?? ?? 56 57 8B B9 0C 10 00 00 85 FF" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::SetupBonesForAttachmentQueries ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 EC 14 83 3D ? ? ? ? ? 53" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::DisableRenderTargetAllocationForever ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "80 B9 ? ? ? ? ? 74 0F" ) ) ) + 0x2 );
	SDK::EngineData::m_AddressList[ CheatAddressList::SetMergedMDL ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 57 8B F9 8B 0D ? ? ? ? 85 C9 75" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::CreateModel ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "53 8B D9 56 57 8D 4B 04 C7 03 ? ? ? ? E8 ? ? ? ? 6A" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::LoadSkybox ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "engine.dll" ), _S( "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::ShowImpacts ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "EB 07 8B 01 8B 40 34 FF D0 83 F8 02 0F 85 ? ? ? ? 8B 0D ? ? ? ? 81 F9 ? ? ? ? 75 1D" ) ) ) + 0x9 );
	SDK::EngineData::m_AddressList[ CheatAddressList::CL_Move_Return ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "engine.dll" ), _S( "6A 00 8B 01 FF 90 B8 00 00 00 83 BF 08 01" ) ) ) + 0xA );
	SDK::EngineData::m_AddressList[ CheatAddressList::CL_ReadPackets ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "engine.dll" ), _S( "53 8A D9 8B 0D ? ? ? ? 56 57 8B B9" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::CL_SendMove ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "engine.dll" ), _S( "55 8B EC 8B 4D 04 81 EC FC 0F 00 00 53 56 57" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::CL_ReadPackets_Return ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "engine.dll" ), _S( "FF 50 34 85 C0 0F 95  C0 84 C0 75 0C" ) ) ) + 0x3 );
	SDK::EngineData::m_AddressList[ CheatAddressList::Extrapolate ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "0F B6 0D ? ? ? ? 84 C0 0F 44" ) ) ) + 0x3 );
	SDK::EngineData::m_AddressList[ CheatAddressList::CVarConstructor ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC F3 0F 10 45 ?? 56 6A 00" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::CreateConVar ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 E4 F8 8B 45 0C 83 EC 10" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::ScopeArc_Return ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "6A 00 FF 50 3C 8B 0D ? ? ? ? FF B7 ? ? ? ?" ) ) ) + 0x5 );
	SDK::EngineData::m_AddressList[ CheatAddressList::ScopeLens_Return ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "FF 50 3C 8B 4C 24 20" ) ) ) + 0x3 );
	SDK::EngineData::m_AddressList[ CheatAddressList::ScopeLinesClear_Return ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "0F 82 ? ? ? ? FF 50 3C" ) ) ) + 0x9 );
	SDK::EngineData::m_AddressList[ CheatAddressList::ScopeLinesBlurry_Return ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "FF B7 ? ? ? ? 8B 01 FF 90 ? ? ? ? 8B 4C 24 24" ) ) ) - 0x6 );
	SDK::EngineData::m_AddressList[ CheatAddressList::ScopeLinesBlurry_Return ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "FF B7 ? ? ? ? 8B 01 FF 90 ? ? ? ? 8B 4C 24 24" ) ) ) - 0x6 );
	SDK::EngineData::m_AddressList[ CheatAddressList::ClearInventoryImageRGBA ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 81 EC ? ? ? ? 57 8B F9 C7 47" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::InventoryItemByItemID ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 8B 55 08 83 EC 10 8B C2" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::AddEconItem ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 E4 F8 A1 ? ? ? ? 83 EC 14 53 56 57 8B F9 8B 08" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::CreateBaseTypeCache ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 51 53 56 8B D9 8D 45 08" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::SOCData ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 E4 F0 83 EC 18 56 8B F1 57 8B 86" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::StaticData ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 51 53 8B D9 8B 0D ? ? ? ? 56 57 8B B9" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::CreateEconItem ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 EC 1C 8D 45 E4 C7 45" ) ) ) + 0x14 );
	SDK::EngineData::m_AddressList[ CheatAddressList::ItemSystem ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "A1 ? ? ? ? 85 C0 75 53" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::SetAttributeValue ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 E4 F8 83 EC 3C 53 8B 5D 08 56 57 6A" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::PreCacheCustomMaterials ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 EC 0C 53 8B 5D 08 56 57 8B 7D 0C" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::DeleteItem ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 EC 24 56 57 83" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::UpdateGeneratedMaterial ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 53 56 57 8B 7D 10 8B F1" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::EquipWearable ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 EC 10 53 8B 5D 08 57 8B F9" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::InitializeAttibutes ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 E4 F8 83 EC 0C 53 56 8B F1 8B 86" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::GetLootListItemsCount_Return ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "85 C0 0F 84 ? ? ? ? 8B C8 E8 ? ? ? ? 52 50 E8 ? ? ? ? 83 C4 08 89 44 24 14 85 C0 0F 84 ? ? ? ? 8B 0D" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::FailedToOpenCrate ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "68 ? ? ? ? 8B 01 8B 80 ? ? ? ? FF D0 84 C0 74 1A 8B 35 ? ? ? ? 8B D3 33 C9 8B 3E E8 ? ? ? ? 50 8B CE FF 97 ? ? ? ? 5F 5E B0 01 5B 8B E5 5D C2 04" ) ) ) + 0x1 );
	SDK::EngineData::m_AddressList[ CheatAddressList::CreateSeriesByID ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 8B 45 08 56 57 8B 30 8B 41 10 83 F8 FF 74 1E 8B 79 04 8D 0C 40 8B 54 CF 10 3B D6 7E 05" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::IsConnected_Return ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "84 C0 75 05 B0 01 5F" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::ItemCustomizationNotification ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "68 ? ? ? ? 8B 80 ? ? ? ? FF D0 84 C0 74 28" ) ) ) + 0x1 );
	SDK::EngineData::m_AddressList[ CheatAddressList::GetLootListByName ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 E4 F8 8B 55 08 81 EC ? ? ? ? 56 57" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::GetLootListInterfaceByIndex ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 8B 55 08 56 8B F1 85 D2 78 47" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::ToolCanApplyTo ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 EC 18 53 56 8B F1 57 8B FA" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::FireBullets_Return ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "3B 3D ?? ?? ?? ?? 75 4C" ) ) ) + 0x6 );
	SDK::EngineData::m_AddressList[ CheatAddressList::RunFrame ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "engine.dll" ), _S( "55 8B EC 83 EC ? 53 56 8A D9" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::SetClantag ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "engine.dll" ), _S( "53 56 57 8B DA 8B F9 FF 15" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::ConstructVoiceMessage ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "engine.dll" ), _S( "56 57 8B F9 8D 4F 08 C7 07 ? ? ? ? E8 ? ? ? ? C7" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::OnSimulationTimeChanging ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "55 8B EC 83 EC 10 56 8B F1 F3 0F 11 55 FC F3 0F" ) ) ) );
	SDK::EngineData::m_AddressList[ CheatAddressList::SetCollisionBounds ] = ( void* ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "53 8B DC 83 EC 08 83 E4 F8 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 83 EC 18 56 57 8B 7B" ) ) ) );
}
void __declspec( noinline ) events( )
{
	ICustomEventListener* pEventListener = new ICustomEventListener( );
	SDK::Interfaces::EventManager->AddListener( pEventListener, _S( "player_hurt" ), false );
	SDK::Interfaces::EventManager->AddListener( pEventListener, _S( "player_spawned" ), false );
	SDK::Interfaces::EventManager->AddListener( pEventListener, _S( "bullet_impact" ), false );
	SDK::Interfaces::EventManager->AddListener( pEventListener, _S( "item_equip" ), false );
	SDK::Interfaces::EventManager->AddListener( pEventListener, _S( "player_death" ), false );
	SDK::Interfaces::EventManager->AddListener( pEventListener, _S( "round_start" ), false );
	SDK::Interfaces::EventManager->AddListener( pEventListener, _S( "round_mvp" ), false );
	SDK::Interfaces::EventManager->AddListener( pEventListener, _S( "bullet_impact" ), false );
	SDK::Interfaces::EventManager->AddListener( pEventListener, _S( "item_purchase" ), false );
	SDK::Interfaces::EventManager->AddListener( pEventListener, _S( "bomb_beginplant" ), false );
	SDK::Interfaces::EventManager->AddListener( pEventListener, _S( "bomb_begindefuse" ), false );
}

void SDK::Initialize( HINSTANCE hInstance )
{
	while ( !GetModuleHandle( _S( "serverbrowser.dll" ) ) )
		Sleep( 1000 );

#ifdef OVERSEE_DEV
	/* get CPU Id */
	auto GetCPUId = [ ] ( ) -> std::string
	{
		int CPUInfo[ 4 ] = { -1 };
		char CPUBrandString[ 0x40 ];
		__cpuid( 0x80000000, CPUInfo[ 0 ], CPUInfo[ 1 ], CPUInfo[ 2 ], CPUInfo[ 3 ] );
		unsigned int nExIds = CPUInfo[ 0 ];

		memset( CPUBrandString, 0, sizeof( CPUBrandString ) );

		for ( size_t i = 0x80000000; i <= nExIds; ++i )
		{
			__cpuid( i, CPUInfo[ 0 ], CPUInfo[ 1 ], CPUInfo[ 2 ], CPUInfo[ 3 ] );
			if ( i == 0x80000002 )
				memcpy( CPUBrandString, CPUInfo, sizeof( CPUInfo ) );
			else if ( i == 0x80000003 )
				memcpy( CPUBrandString + 16, CPUInfo, sizeof( CPUInfo ) );
			else if ( i == 0x80000004 )
				memcpy( CPUBrandString + 32, CPUInfo, sizeof( CPUInfo ) );
		}

		return std::string( CPUBrandString );
	};

	/* get Volume ID */
	auto GetVolumeID = [ ] ( ) -> std::string
	{
		HANDLE hFile = CreateFileA( _S( "\\\\.\\PhysicalDrive0" ), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL );
		if ( hFile == INVALID_HANDLE_VALUE )
			return { };

		std::unique_ptr< std::remove_pointer <HANDLE >::type, void( * )( HANDLE ) > hDevice
		{
			hFile, [ ] ( HANDLE handle )
		{
			CloseHandle( handle );
		}
		};

		STORAGE_PROPERTY_QUERY PropertyQuery;
		PropertyQuery.PropertyId = StorageDeviceProperty;
		PropertyQuery.QueryType = PropertyStandardQuery;

		STORAGE_DESCRIPTOR_HEADER DescHeader;
		DWORD dwBytesReturned = 0;
		if ( !DeviceIoControl( hDevice.get( ), IOCTL_STORAGE_QUERY_PROPERTY, &PropertyQuery, sizeof( STORAGE_PROPERTY_QUERY ),
			&DescHeader, sizeof( STORAGE_DESCRIPTOR_HEADER ), &dwBytesReturned, NULL ) )
			return { };

		const DWORD dwOutBufferSize = DescHeader.Size;
		std::unique_ptr< BYTE[ ] > pOutBuffer { new BYTE[ dwOutBufferSize ] { } };

		if ( !DeviceIoControl( hDevice.get( ), IOCTL_STORAGE_QUERY_PROPERTY, &PropertyQuery, sizeof( STORAGE_PROPERTY_QUERY ),
			pOutBuffer.get( ), dwOutBufferSize, &dwBytesReturned, NULL ) )
			return { };

		STORAGE_DEVICE_DESCRIPTOR* pDeviceDescriptor = reinterpret_cast< STORAGE_DEVICE_DESCRIPTOR* >( pOutBuffer.get( ) );
		if ( !pDeviceDescriptor )
			return { };

		const DWORD dwSerialNumberOffset = pDeviceDescriptor->SerialNumberOffset;
		if ( !dwSerialNumberOffset )
			return { };

		std::string sResult = reinterpret_cast< const char* >( pOutBuffer.get( ) + dwSerialNumberOffset );
		return sResult;
	};

	g_Globals->m_szHWID = md5( md5( md5( GetCPUId( ) ) ) + md5( GetCPUId( ) ) + md5( GetVolumeID( ) ) );
	g_Globals->m_szName = "Developer";
#endif
#ifndef OVERSEE_DEV
	g_Globals->m_szHWID = std::string( ( char* )( hInstance ), 32 );
	g_CloudConfigs->Auth( );
#endif

	// get module list
	HMODULE hClientDll = GetModuleHandle( _S( "client.dll" ) );
	HMODULE hVGuiDLL = GetModuleHandle( _S( "vguimatsurface.dll" ) );
	HMODULE hVGui2DLL = GetModuleHandle( _S( "vgui2.dll" ) );
	HMODULE hEngineDll = GetModuleHandle( _S( "engine.dll" ) );
	HMODULE hMatSysDll = GetModuleHandle( _S( "materialsystem.dll" ) );
	HMODULE hDx9ApiDll = GetModuleHandle( _S( "shaderapidx9.dll" ) );
	HMODULE hValveSTD = GetModuleHandle( _S( "vstdlib.dll" ) );
	HMODULE hLocalizeDll = GetModuleHandle( _S( "localize.dll" ) );
	HMODULE hVPhysicsDll = GetModuleHandle( _S( "vphysics.dll" ) );
	HMODULE hSteamApi = GetModuleHandle( _S( "steam_api.dll" ) );
	HMODULE hStudioRender = GetModuleHandle( _S( "studiorender.dll" ) );
	HMODULE hFileSystem = GetModuleHandle( _S( "filesystem_stdio" ) );
	HMODULE hPanorama = GetModuleHandle( _S( "panorama.dll" ) );
	HMODULE hDataCache = GetModuleHandle( _S( "datacache.dll" ) );

	/* get some steam addr's */
	void* m_pSteamUser = ( void* ) ( GetProcAddress( hSteamApi, _S( "SteamAPI_GetHSteamUser" ) ) );
	void* m_pSteamPipe = ( void* ) ( GetProcAddress( hSteamApi, _S( "SteamAPI_GetHSteamPipe" ) ) );
	auto hSteamUser = ( ( ( HSteamUser( __cdecl* )( void ) ) m_pSteamUser ) )( );
	auto hSteamPipe = ( ( ( HSteamPipe( __cdecl* )( void ) ) m_pSteamPipe ) )( );

	/* setup steam API */
	SDK::Steam::SteamClient = ( ( ISteamClient * ( __cdecl* )( void ) ) GetProcAddress( hSteamApi, _S( "SteamClient" ) ) )( );
	SDK::Steam::SteamHTTP = SDK::Steam::SteamClient->GetISteamHTTP( hSteamUser, hSteamPipe, _S( "STEAMHTTP_INTERFACE_VERSION002" ) );
	SDK::Steam::SteamUser = SDK::Steam::SteamClient->GetISteamUser( hSteamUser, hSteamPipe, _S( "SteamUser019" ) );
	SDK::Steam::SteamFriends = SDK::Steam::SteamClient->GetISteamFriends( hSteamUser, hSteamPipe, _S( "SteamFriends015" ) );
	SDK::Steam::SteamInventory = SDK::Steam::SteamClient->GetISteamInventory( hSteamUser, hSteamPipe, _S( "STEAMINVENTORY_INTERFACE_V002" ) );
	SDK::Steam::GameCoordinator = ( ISteamGameCoordinator* ) SDK::Steam::SteamClient->GetISteamGenericInterface( hSteamUser, hSteamPipe, _S( "SteamGameCoordinator001" ) );
	SDK::Steam::SteamMatchMaking = SDK::Steam::SteamClient->GetISteamMatchmaking( hSteamUser, hSteamPipe, STEAMMATCHMAKING_INTERFACE_VERSION );
	SteamUtils = SDK::Steam::SteamClient->GetISteamUtils( hSteamPipe, "SteamUtils009" );

	/* setup interfaces, game useful classes */
	SDK::Interfaces::CHLClient = GetInterface < ICHLClient >( hClientDll, _S( "VClient018" ) );
	SDK::Interfaces::GameMovement = GetInterface < IGameMovement >( hClientDll, _S( "GameMovement001" ) );
	SDK::Interfaces::Prediction = GetInterface < IPrediction >( hClientDll, _S( "VClientPrediction001" ) );
	SDK::Interfaces::EntityList = GetInterface < IClientEntityList >( hClientDll, _S( "VClientEntityList003" ) );
	SDK::Interfaces::MaterialSystem = GetInterface < IMaterialSystem >( hMatSysDll, _S( "VMaterialSystem080" ) );
	SDK::Interfaces::ModelRender = GetInterface < IVModelRender >( hEngineDll, _S( "VEngineModel016" ) );
	SDK::Interfaces::ModelInfo = GetInterface < IVModelInfoClient >( hEngineDll, _S( "VModelInfoClient004" ) );
	SDK::Interfaces::EngineClient = GetInterface < IVEngineClient >( hEngineDll, _S( "VEngineClient014" ) );
	SDK::Interfaces::EngineSound = GetInterface < IEngineSound >( hEngineDll, _S( "IEngineSoundClient003" ) );
	SDK::Interfaces::EngineTrace = GetInterface < IEngineTrace >( hEngineDll, _S( "EngineTraceClient004" ) );
	SDK::Interfaces::RenderView = GetInterface < IVRenderView >( hEngineDll, _S( "VEngineRenderView014" ) );
	SDK::Interfaces::Surface = GetInterface < ISurface >( hVGuiDLL, _S( "VGUI_Surface031" ) );
	SDK::Interfaces::Panel = GetInterface < IPanel >( hVGui2DLL, _S( "VGUI_Panel009" ) );
	SDK::Interfaces::CVar = GetInterface < ICVar >( hValveSTD, _S( "VEngineCvar007" ) );
	SDK::Interfaces::DebugOverlay = GetInterface < IVDebugOverlay >( hEngineDll, _S( "VDebugOverlay004" ) );
	SDK::Interfaces::Localize = GetInterface < ILocalize >( hLocalizeDll, _S( "Localize_001" ) );
	SDK::Interfaces::EventManager = GetInterface < IGameEventManager >( hEngineDll, _S( "GAMEEVENTSMANAGER002" ) );
	SDK::Interfaces::PhysicsSurface = GetInterface < IPhysicsSurfaceProps >( hVPhysicsDll, _S( "VPhysicsSurfaceProps001" ) );
	SDK::Interfaces::StudioRender = GetInterface < IStudioRender >( hStudioRender, _S( "VStudioRender026" ) );
	SDK::Interfaces::FileSystem = GetInterface < IFileSystem >( hFileSystem, _S( "VBaseFileSystem011" ) );
	SDK::Interfaces::PanoramaEngine = GetInterface < IPanoramaUIEngine >( hPanorama, _S( "PanoramaUIEngine001" ) );
	SDK::Interfaces::MDLCache = GetInterface<C_MDLCache>( hDataCache, _S( "MDLCache004" ) );
	SDK::Interfaces::Effects = GetInterface<IVEffects>( hEngineDll, _S( "VEngineEffects001" ) );
	SDK::Interfaces::DirectDevice = **( IDirect3DDevice9*** ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "shaderapidx9.dll" ), _S( "A1 ? ? ? ? 50 8B 08 FF 51 0C" ) ) ) + 0x1 );
	SDK::Interfaces::GlowObjManager = *( IGlowObjectManager** ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "0F 11 05 ? ? ? ? 83 C8 01" ) ) ) + 0x3 );
	SDK::Interfaces::MoveHelper = **( IMoveHelper*** ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01" ) ) ) + 0x2 );
	SDK::Interfaces::GlobalVars = **( IGlobalVarsBase*** ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "A1 ? ? ? ? 5E 8B 40 10" ) ) ) + 0x1 );
	SDK::Interfaces::RenderBeams = *( IViewRenderBeams** ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "A1 ? ? ? ? FF 10 A1 ? ? ? ? B9" ) ) ) + 0x1 );
	SDK::Interfaces::ClientState = **( IClientState*** ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "engine.dll" ), _S( "A1 ? ? ? ? 8B 80 ? ? ? ? C3" ) ) ) + 0x1 );
	SDK::Interfaces::GameRules = *( IGameRules*** ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "A1 ? ? ? ? 85 C0 0F 84 ? ? ? ? 80 B8 ? ? ? ? ? 74 7A" ) ) ) + 0x1 );
	SDK::Interfaces::Input = *( IInput** ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "B9 ? ? ? ? F3 0F 11 04 24 FF 50 10" ) ) ) + 0x1 );
	SDK::Interfaces::MemAlloc = *( IMemAlloc** ) ( GetProcAddress( GetModuleHandleA( _S( "tier0.dll" ) ), _S( "g_pMemAlloc" ) ) );
	SDK::Interfaces::ClientMode = **( void*** ) ( ( *( uintptr_t** ) ( SDK::Interfaces::CHLClient ) )[ 10 ] + 0x5 );
	SDK::Interfaces::InventoryManager = *( IInventoryManager** ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "C7 05 ? ? ? ? ? ? ? ? BF ? ? ? ? BE" ) ) ) + 0x2 );
	SDK::Interfaces::PanoramaHelper = *( CPanoramaMarshallHelper** ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "68 ? ? ? ? 8B C8 E8 ? ? ? ? 8D 4D F4 FF 15 ? ? ? ? 8B CF FF 15 ? ? ? ? 5F 5E 8B E5 5D C3" ) ) ) + 0x1 );
	SDK::Interfaces::PlayerResource = *( IPlayerResource*** ) ( ( DWORD ) ( g_Utils->PatternScan( _S( "client.dll" ), _S( "A1 ? ? ? ? 89 44 24 60 85" ) ) ) + 0x1 );

	/* 02.02.2022 | Valve's cringe retaddr check bypass */
	{
		/* define module list */
		std::vector < std::string > m_szModuleList
			=
		{
			_S( "client.dll" ),
			_S( "engine.dll" ),
			_S( "antitamper.dll" ),
			_S( "inputsystem.dll" ),
			_S( "materialsystem.dll" ),
			_S( "server.dll" ),
			_S( "matchmaking.dll" ),
			_S( "cairo.dll" ),
			_S( "datacache.dll" ),
			_S( "filesystem_stdio.dll" ),
			_S( "icui18n.dll" ),
			_S( "icuuc.dll" ),
			_S( "imemanager.dll" ),
			_S( "launcher.dll" ),
			_S( "libavcodec-56.dll" ),
			_S( "libavformat-56.dll" ),
			_S( "libavresample-2.dll" ),
			_S( "libavutil-54.dll" ),
			_S( "libfontconfig-1.dll" ),
			_S( "libfreetype-6.dll" ),
			_S( "libglib-2.0-0.dll" ),
			_S( "libgmodule-2.0-0.dll" ),
			_S( "libgobject-2.0-0.dll" ),
			_S( "libpango-1.0-0.dll" ),
			_S( "libpangoft2-1.0-0.dll" ),
			_S( "libswscale-3.dll" ),
			_S( "localize.dll" ),
			_S( "mss32.dll" ),
			_S( "panorama.dll" ),
			_S( "panorama_text_pango.dll" ),
			_S( "panoramauiclient.dll" ),
			_S( "parsifal.dll" ),
			_S( "phonon.dll" ),
			_S( "scenefilecache.dll" ),
			_S( "serverbrowser.dll" ),
			_S( "shaderapidx9.dll" ),
			_S( "soundemittersystem.dll" ),
			_S( "soundsystem.dll" ),
			_S( "stdshader_dbg.dll" ),
			_S( "stdshared_dx9.dll" ),
			_S( "steam_api.dll" ),
			_S( "steamnetworkingsockets.dll" ),
			_S( "studiorender.dll" ),
			_S( "tier0.dll" ),
			_S( "v8.dll" ),
			_S( "v8_libbase.dll" ),
			_S( "v8_libplatform.dll" ),
			_S( "valve_avi.dll" ),
			_S( "vaudio_celt.dll" ),
			_S( "vaudio_miles.dll" ),
			_S( "vaudio_speex.dll" ),
			_S( "vgui2.dll" ),
			_S( "vguimatsurface.dll" ),
			_S( "video.dll" ),
			_S( "vphysics.dll" ),
			_S( "vscript.dll" ),
			_S( "vstdlib.dll" ),
			_S( "vtex_dll.dll" ),
			_S( "fmod.dll" ),
			_S( "fmodstudio.dll" )
		};

		/* parse modules and patch them */
		for ( auto& Module : m_szModuleList )
		{
			/* check module */
			HMODULE hModule = GetModuleHandleA( Module.c_str( ) );
			if ( !hModule )
				continue;

			/* scan pattern */
			LPVOID lpValveCringe = g_Utils->PatternScan( Module.c_str( ), _S( "55 8B EC 56 8B F1 33 C0 57 8B 7D 08 8B 8E ? ? ? ? 85 C9 7E 12 3B 3C 86 72 06 3B 7C 86 04 72 49 83 C0 02 3B C1 7C EE" ) );

			/* patch */
			if ( lpValveCringe )
			{
				/* protect memory */
				VirtualProtect( lpValveCringe, 0x5, PAGE_EXECUTE_READWRITE, NULL );

				/* copy instruction */
				std::memcpy( lpValveCringe, "\xB0\x01\xC2\x04\x00", 5 );
			}
		}
	}

	/* setup settings */
	g_SettingsManager->SetupData( );

	/* setup convars and addresses */
	SetupAddressList( );
	SetupConVars( );

	/* create convar function */
	auto CreateConVar = [ ]( void* CVar, const char* pName, const char* pDefaultValue, int flags = 0,
		const char* pHelpString = NULL, bool bMin = false, float fMin = 0.0,
		bool bMax = false, float fMax = false, FnChangeCallback_t callback = NULL ) -> uintptr_t
	{
		/* convar create declaration */
		typedef uintptr_t( __thiscall* CreateConVar_t )( void* CVar, const char* pName, const char* pDefaultValue, int flags, const char* pHelpString, bool bMin, float fMin, bool bMax, float fMax, FnChangeCallback_t callback );
		return ( ( CreateConVar_t ) ( SDK::EngineData::m_AddressList[ CheatAddressList::CVarConstructor ] ) )( CVar, pName, pDefaultValue, flags, pHelpString, bMin, fMin, bMax, fMax, callback );
	};

	/* alloc memory */
	SDK::EngineData::m_ConvarList[ CheatConvarList::SvLegacyDesync ] = ( ConVar* ) ( malloc( 0x58 ) );

	/* init convars */
	CreateConVar( SDK::EngineData::m_ConvarList[ CheatConvarList::SvLegacyDesync ], "sv_legacy_desync", "0", FCVAR_RELEASE | FCVAR_REPLICATED, "Enables legacy desync on supported servers", true, 0.0f, false, 0.0f );

	/* init renderer */
	g_Renderer->SetupData( );
	g_Menu->SetupData( );

	CloseHandle( CreateThread( NULL, NULL, ( LPTHREAD_START_ROUTINE ) ( HandleConfigs ), NULL, NULL, NULL ) );
	std::thread
	(
		[ ]( )
		{
			g_ItemManager.ImagePrecache( );
			return ExitThread( NULL );
		}
	).detach( );

	//CloseHandle( CreateThread( NULL, NULL, ( LPTHREAD_START_ROUTINE ) ( HandleScripts ), NULL, NULL, NULL ) );
	//CloseHandle( CreateThread( NULL, NULL, ( LPTHREAD_START_ROUTINE ) ( HandleScriptsChanges ), NULL, NULL, NULL ) );

	/* init multithread */
	Threading::InitThreads( );

	/* create fonts */
	g_FontManager->PushFont( _S( "Tahoma" ), FnvHash( "Player ESP" ), 72 );
	g_FontManager->PushFont( _S( "SegoeUI" ), FnvHash( "Logs text" ), 16 );
	g_FontManager->PushFont( rawData, 3156, FnvHash( "Logs icons" ), 11 );
	g_FontManager->PushFont( weaponData, 47404, FnvHash( "Weapon icons" ), 11 );
	g_FontManager->CreateFonts( );

	/* set fucking callback for anti-defensive */
	ConVar* cl_lagcompensation = SDK::Interfaces::CVar->FindVar( _S( "cl_lagcompensation" ) );
	if ( cl_lagcompensation )
		cl_lagcompensation->m_fnChangeCallbacks[ cl_lagcompensation->m_fnChangeCallbacks.AddToTail( ) ] = C_Hooks::CL_LagCompensation_Callback;

	/* parse netvars */
	NetVars::Initialize( );

	/* inventory changeer */
	g_ItemManager.Initialize( );

	/* create chams materials. NOTE: CRASHING SOMETIMES */
	g_ChamsSystem->InitializeMaterials( );

	/* create event listeners */
	events( );

	/* setup hooks */
	g_Hooks->Initialize( );
}