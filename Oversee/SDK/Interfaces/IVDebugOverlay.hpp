#pragma once

#include "../Misc/VFunc.hpp"
#include "../Math/Vector.hpp"
#include "../Math/QAngle.hpp"
#include "../Math/VMatrix.hpp"

class IVDebugOverlay
{
public:
	VFunc
	( 
		AddBoxOverlay( const Vector& vecOrigin, const Vector& vecMins, const Vector& vecMaxs, QAngle const& angOrientation, int r, int g, int b, int a, float flDuration ), 
		1, 
		void( __thiscall* )( void*, const Vector&, const Vector&, const Vector&, QAngle const&, int, int, int, int, float ),
		vecOrigin,
		vecMins,
		vecMaxs,
		angOrientation,
		r,
		g,
		b,
		a,
		flDuration
	);

	virtual void AddEntityTextOverlay( int ent_index, int line_offset, float duration, int r, int g, int b, int a, const char* format, ... ) = 0;
	virtual void unused( ) = 0; 
	virtual void AddSphereOverlay( const Vector& vOrigin, float flRadius, int nTheta, int nPhi, int r, int g, int b, int a, float flDuration ) = 0;
	virtual void AddTriangleOverlay( const Vector& p1, const Vector& p2, const Vector& p3, int r, int g, int b, int a, bool noDepthTest, float duration ) = 0;
	virtual void AddLineOverlay( const Vector& origin, const Vector& dest, int r, int g, int b, bool noDepthTest, float duration ) = 0;
	virtual void AddTextOverlay( const Vector& origin, float duration, const char* format, ... ) = 0;
	virtual void AddTextOverlay( const Vector& origin, int line_offset, float duration, const char* format, ... ) = 0;
	virtual void AddScreenTextOverlay( float flXPos, float flYPos, float flDuration, int r, int g, int b, int a, const char* text ) = 0;
	virtual void AddSweptBoxOverlay( const Vector& start, const Vector& end, const Vector& mins, const Vector& max, const Vector& angles, int r, int g, int b, int a, float flDuration ) = 0;
	virtual void AddGridOverlay( const Vector& origin ) = 0;
	virtual void AddCoordFrameOverlay( const matrix3x4_t& frame, float flScale, int vColorTable[ 3 ][ 3 ] = NULL ) = 0;
	virtual int ScreenPosition( const Vector& point, Vector& screen ) = 0;
	virtual int ScreenPosition( float flXPos, float flYPos, Vector& screen ) = 0;
	virtual void* GetFirst( void ) = 0;
	virtual void* GetNext( void* current ) = 0;
	virtual void ClearDeadOverlays( void ) = 0;
	virtual void ClearAllOverlays( void ) = 0;
	virtual void AddTextOverlayRGB( const Vector& origin, int line_offset, float duration, float r, float g, float b, float alpha, const char* format, ... ) = 0;
	virtual void AddTextOverlayRGB( const Vector& origin, int line_offset, float duration, int r, int g, int b, int a, const char* format, ... ) = 0;
	virtual void AddLineOverlayAlpha( const Vector& origin, const Vector& dest, int r, int g, int b, int a, bool noDepthTest, float duration ) = 0;
	virtual void AddBoxOverlay2( const Vector& origin, const Vector& mins, const Vector& max, Vector const& orientation, const Color faceColor, const Color edgeColor, float duration ) = 0;
	virtual void AddLineOverlay( const Vector& origin, const Vector& dest, int r, int g, int b, int a, float, float ) = 0;
	virtual void PurgeTextOverlays( ) = 0;
	virtual void AddCapsuleOverlay( const Vector& mins, const Vector& max, float& radius, int r, int g, int b, int a, float duration, char unknown, char ignorez ) = 0;

};