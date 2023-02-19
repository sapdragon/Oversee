#pragma once
#include "../../../SDK/SDK.hpp"
#include "../../../SDK/Misc/KeyValues.hpp"
#include "../../Misc/Voice Communication/VoiceCommunication.hpp"
enum ChamsType_t : int
{
	CHAMS_REALMODEL,
	CHAMS_DESYNCMODEL,
	CHAMS_ENEMY,
	CHAMS_TEAMMATE,
	CHAMS_LOCALWEAPON,
	CHAMS_HANDS,
	CHAMS_DROPPEDWEAPONS
};

enum ChamsMaterial_t
{
	FLAT = 0,
	REGULAR,
	SKY,
	GLOW,
	GHOST,
	GLASS,
	GOLD,
	PULSE
};

class C_ChamsSystem	
{
public:
	virtual void Instance( IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix );
	virtual void InitializeMaterials( );

	virtual void OnSharedChamsDataReceived(C_SharedChamsData pData, int iSenderIndex) { this->m_aSharedChamsData[iSenderIndex] = pData; };

	virtual void PullMaterial(IMaterial* pMaterial, Color cColor, bool bIngoreZ, bool bUniqueMaterial = false, bool bOnShotChams = false);
	virtual IMaterial* CreateMaterial(const char* szName, const char* szContent);

	std::array < IMaterial*, 8 >m_aMaterials{};
private:


	virtual void HandlingLocalPlayer(IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix);
	virtual void HandlingTeamPlayer(C_BasePlayer* pPlayer, IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix);
	virtual void HandlingEnemyPlayer(C_BasePlayer* pPlayer, IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix);
	virtual void HandlingLocalWeapon(C_BaseCombatWeapon* pWeapon, IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix);
	virtual void HandlingDroppedWeapons(C_BaseCombatWeapon* pWeapon, IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix);


	virtual void DrawMaterialsOnEntity(ChamsSettings_t Settings, IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix, bool bForceNull, bool bXQZ, bool bLocalScoped = false);
	virtual void DrawSharedMateraisOnEntity(C_SharedChamsData pData, IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix, bool bForceNull, bool bXQZ);


	std::array<C_SharedChamsData, 64> m_aSharedChamsData {};

};

inline C_ChamsSystem* g_ChamsSystem = new C_ChamsSystem( );