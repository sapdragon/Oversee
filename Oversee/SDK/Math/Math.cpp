#include "Math.hpp"
#include "../SDK.hpp"
#include "../../ImGui/imgui.h"
#include "../../ImGui/imgui_internal.h"

void Math::FixMovement( void* pCmd )
{
	C_UserCmd* pUserCmd = static_cast < C_UserCmd* > ( pCmd );
	if ( !pUserCmd || !pUserCmd->m_nCommand )
		return;

	/* get wish angles */
	QAngle angWishAngles;
	SDK::Interfaces::EngineClient->GetViewAngles( &angWishAngles );

	Vector PureForward, PureRight, PureUp, CurrForward, CurrRight, CurrUp;
	Math::AngleVectors( angWishAngles, PureForward, PureRight, PureUp );
	Math::AngleVectors( pUserCmd->m_angViewAngles, CurrForward, CurrRight, CurrUp );

	PureForward[2] = PureRight[2] = CurrForward[2] = CurrRight[2] = 0.f;

	auto VectorNormalize = [ ]( Vector& vec )->float {
		float radius = sqrtf( vec.x * vec.x + vec.y * vec.y + vec.z * vec.z );
		float iradius = 1.f / ( radius + FLT_EPSILON );

		vec.x *= iradius;
		vec.y *= iradius;
		vec.z *= iradius;

		return radius;
	};
	VectorNormalize( PureForward );
	VectorNormalize( PureRight );
	VectorNormalize( CurrForward );
	VectorNormalize( CurrRight );
	Vector PureWishDir;
	for ( auto i = 0u; i < 2; i++ )
		PureWishDir[i] = PureForward[i] * pUserCmd->m_flForwardMove + PureRight[i] * pUserCmd->m_flSideMove;
	PureWishDir[2] = 0.f;

	Vector CurrWishDir;
	for ( auto i = 0u; i < 2; i++ )
		CurrWishDir[i] = CurrForward[i] * pUserCmd->m_flForwardMove + CurrRight[i] * pUserCmd->m_flSideMove;
	CurrWishDir[2] = 0.f;

	if ( PureWishDir != CurrWishDir ) {
		pUserCmd->m_flForwardMove = ( PureWishDir.x * CurrRight.y - CurrRight.x * PureWishDir.y ) / ( CurrRight.y * CurrForward.x - CurrRight.x * CurrForward.y );
		pUserCmd->m_flSideMove = ( PureWishDir.y * CurrForward.x - CurrForward.y * PureWishDir.x ) / ( CurrRight.y * CurrForward.x - CurrRight.x * CurrForward.y );
	}
}

bool Math::ScreenTransform( const Vector& in, Vector& out )
{
	const VMatrix& w2sMatrix = SDK::Interfaces::EngineClient->WorldToScreenMatrix( );

	out.x = w2sMatrix.m[ 0 ][ 0 ] * in.x + w2sMatrix.m[ 0 ][ 1 ] * in.y + w2sMatrix.m[ 0 ][ 2 ] * in.z + w2sMatrix.m[ 0 ][ 3 ];
	out.y = w2sMatrix.m[ 1 ][ 0 ] * in.x + w2sMatrix.m[ 1 ][ 1 ] * in.y + w2sMatrix.m[ 1 ][ 2 ] * in.z + w2sMatrix.m[ 1 ][ 3 ];
	out.z = 0.0f;

	float w = w2sMatrix.m[ 3 ][ 0 ] * in.x + w2sMatrix.m[ 3 ][ 1 ] * in.y + w2sMatrix.m[ 3 ][ 2 ] * in.z + w2sMatrix.m[ 3 ][ 3 ];
	if ( w < 0.001f )
	{
		out.x *= 100000;
		out.y *= 100000;
		return false;
	}

	out.x /= w;
	out.y /= w;

	return true;
}

bool Math::WorldToScreen( const Vector& in, Vector& out )
{
	if ( ScreenTransform( in, out ) )
	{
		int w, h;
		SDK::Interfaces::EngineClient->GetScreenSize( w, h );

		out.x = ( w / 2.0f ) + ( out.x * w ) / 2.0f;
		out.y = ( h / 2.0f ) - ( out.y * h ) / 2.0f;

		return false;
	}

	return true;
}

RECT Math::GetBBox( C_BaseEntity* pEntity ) 
{
	auto pCollideable = pEntity->GetCollideable( );
	if ( !pCollideable )
		return { };

	Vector vecMins = pCollideable->OBBMins( );
	Vector vecMaxs = pCollideable->OBBMaxs( );

	const matrix3x4_t& m_Transformation = pEntity->m_rgflCoordinateFrame( );
	std::array < Vector, 8 > m_Points
		=
	{
		Vector( vecMins.x, vecMins.y, vecMins.z ),
		Vector( vecMins.x, vecMaxs.y, vecMins.z ),
		Vector( vecMaxs.x, vecMaxs.y, vecMins.z ),
		Vector( vecMaxs.x, vecMins.y, vecMins.z ),
		Vector( vecMaxs.x, vecMaxs.y, vecMaxs.z ),
		Vector( vecMins.x, vecMaxs.y, vecMaxs.z ),
		Vector( vecMins.x, vecMins.y, vecMaxs.z ),
		Vector( vecMaxs.x, vecMins.y, vecMaxs.z )
	};

	std::array < Vector, 8 > m_PointsTransformated;
	for ( int nVector = 0; nVector < 8; nVector++ )
		Math::VectorTransform( m_Points[ nVector ], m_Transformation, m_PointsTransformated[ nVector ] );

	std::array < Vector, 8 > m_ScreenPoints;
	for ( int nVector = 0; nVector < 8; nVector++ )
	{
		if ( SDK::Interfaces::DebugOverlay->ScreenPosition( m_PointsTransformated[ nVector ], m_ScreenPoints[ nVector ] ) )
			return { };
	}

	float flLeft = m_ScreenPoints[ 0 ].x;
	float flTop = m_ScreenPoints[ 0 ].y;
	float flRight = m_ScreenPoints[ 0 ].x;
	float flBottom = m_ScreenPoints[ 0 ].y;

	for ( int nID = 1; nID < 8; nID++ )
	{
		if ( flLeft > m_ScreenPoints[ nID ].x )
			flLeft = m_ScreenPoints[ nID ].x;

		if ( flTop < m_ScreenPoints[ nID ].y )
			flTop = m_ScreenPoints[ nID ].y;

		if ( flRight < m_ScreenPoints[ nID ].x )
			flRight = m_ScreenPoints[ nID ].x;

		if ( flBottom > m_ScreenPoints[ nID ].y )
			flBottom = m_ScreenPoints[ nID ].y;
	}

	return RECT { ( LONG ) ( flLeft ), ( LONG ) ( flBottom ), ( LONG ) ( flRight ), ( LONG ) ( flTop ) };
}