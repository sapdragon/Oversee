#include "../Hooks.hpp"
#include "../../Features/Visuals/Chams/Chams.hpp"

void FASTCALL C_Hooks::hkDrawModelExecute( LPVOID pEcx, uint32_t, IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix )
{
	if ( SDK::Interfaces::StudioRender->IsForcedMaterialOverride( ) &&
		 !strstr( pInfo.pModel->szName, _S( "arms" ) ) &&
		 !strstr( pInfo.pModel->szName, _S( "weapons/v_" ) )
		 ) 
		return Hooks::o_DrawModelExecute( pEcx, pCtx, pState, pInfo, aMatrix );

	g_ChamsSystem->Instance( pCtx, pState, pInfo, aMatrix );

	Hooks::o_DrawModelExecute( pEcx, pCtx, pState, pInfo, aMatrix );
	return SDK::Interfaces::ModelRender->ForcedMaterialOverride( nullptr );
}