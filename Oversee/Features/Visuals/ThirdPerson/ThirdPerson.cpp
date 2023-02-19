#include "ThirdPerson.hpp"

void TraceHull( const Vector& vecStart, const Vector& vecEnd, uint32_t nMask, const Vector& vecExtents, trace_t* pTrace )
{
	CTraceFilterWorldAndPropsOnly Filter;

	Ray_t ray;
	ray.Init( vecStart, vecEnd );
	ray.m_Extents = vecExtents;
	ray.m_IsRay = false;

	SDK::Interfaces::EngineTrace->TraceRay( ray, nMask, &Filter, pTrace );
}
constexpr float flThirdPersonAnimTime = 0.1f;
void C_ThirdPerson::Instance( C_ViewSetup* pViewSetup )
{
	if ( !g_Globals->m_LocalPlayer )
	{
		m_flThirdpersonDistance = 30.0f;
		return;
	}

	if ( !g_Globals->m_LocalPlayer->IsAlive( ) )
	{
		// set observer mode
		if ( m_bReset )
			g_Globals->m_LocalPlayer->m_iObserverMode( ) = ObserverType_t::OBS_MODE_CHASE;

		// set thirdperson distance
		if ( g_Globals->m_LocalPlayer->m_iObserverMode( ) == ObserverType_t::OBS_MODE_CHASE )
			pViewSetup->flFOV = 110;

		// remove reset boolean
		m_bReset = false;

		// reset input
		SDK::Interfaces::Input->m_bCameraInThirdPerson = false;

		// do not run thirdperson
		return;
	}

	// recalc view origin
	float flOriginZ = g_Globals->m_LocalPlayer->GetAbsOrigin( ).z + g_Globals->m_LocalPlayer->m_vecViewOffset( ).z;
	if ( g_Globals->m_LocalPlayer->m_fFlags( ) & FL_ONGROUND )
	{
		if ( g_Globals->m_Packet.m_bVisualFakeDucking )
			pViewSetup->vecOrigin.z = flOriginZ = g_Globals->m_LocalPlayer->GetAbsOrigin( ).z + 64.0f;
	}

	// setup field of view
	if ( g_SettingsManager->B[ _S( "misc.change_fov_while_scoped" ) ] || !g_Globals->m_LocalPlayer->m_bIsScoped( ) || !g_Globals->m_LocalPlayer->IsAlive( ) )
		pViewSetup->flFOV = ( float ) ( g_SettingsManager->I[ _S( "misc.camerafov" ) ] );
	/* Thirdperson active status */
	bool bIsActive = g_SettingsManager->B[ _S( "misc.third_person" ) ];

	/* Update chams distance */
	if ( bIsActive )

		m_flThirdpersonDistance
		=
		fmin
		(
			Math::Interpolate
			(
				m_flThirdpersonDistance,
				( float ) ( g_SettingsManager->I[ _S( "misc.third_person_distance" ) ] ),
				SDK::Interfaces::GlobalVars->m_flFrameTime * ( 1.0f / flThirdPersonAnimTime )
			),
				g_SettingsManager->I[ _S( "misc.third_person_distance" ) ]
		);
	else
		m_flThirdpersonDistance
		=
		fmax
		(
			Math::Interpolate
			(
				m_flThirdpersonDistance,
				0.0f,
				SDK::Interfaces::GlobalVars->m_flFrameTime * ( 1.0f / flThirdPersonAnimTime )
			),
			0.0f
		);

	if ( m_flThirdpersonDistance <= 30.0f )
	{
		SDK::Interfaces::Input->m_bCameraInThirdPerson = false;
		return;
	}

	Vector vecEyePos = g_Globals->m_LocalPlayer->GetAbsOrigin( );
	vecEyePos.z = flOriginZ;

	QAngle angViewAngles;
	SDK::Interfaces::EngineClient->GetViewAngles( &angViewAngles );

	SDK::Interfaces::Input->m_bCameraInThirdPerson = true;
	SDK::Interfaces::Input->m_vecCameraOffset = Vector( angViewAngles.pitch, angViewAngles.yaw, m_flThirdpersonDistance );

	CGameTrace Trace;

	float flExtent = 12.f + ( g_SettingsManager->I[ _S( "misc.camerafov" ) ] / 4.8f - 18.f );

	Vector vecForward = Vector( 0, 0, 0 );
	Math::AngleVectors( QAngle( angViewAngles.pitch, angViewAngles.yaw, 0.0f ), vecForward );
	TraceHull( vecEyePos, vecEyePos - vecForward * SDK::Interfaces::Input->m_vecCameraOffset.z, MASK_SOLID & ~CONTENTS_MONSTER, Vector( flExtent, flExtent, flExtent ), &Trace );
	SDK::Interfaces::Input->m_vecCameraOffset.z *= Trace.fraction;

	if ( SDK::Interfaces::Input->m_vecCameraOffset.z < 30.0f )
		SDK::Interfaces::Input->m_bCameraInThirdPerson = false;

	m_bReset = true;
}