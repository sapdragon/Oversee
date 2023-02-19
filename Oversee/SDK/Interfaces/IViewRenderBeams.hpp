#pragma once

#include "IEngineTrace.hpp"
#include "../Structs/IClientEntity.hpp"

struct BeamInfo_t {
	int			m_nType;

	// Entities
	IClientEntity* m_pStartEnt;
	int			m_nStartAttachment;
	IClientEntity* m_pEndEnt;
	int			m_nEndAttachment;

	// Points
	Vector		m_vecStart;
	Vector		m_vecEnd;

	int			m_nModelIndex;
	const char* m_pszModelName;

	int			m_nHaloIndex;
	const char* m_pszHaloName;
	float		m_flHaloScale;

	float		m_flLife;
	float		m_flWidth;
	float		m_flEndWidth;
	float		m_flFadeLength;
	float		m_flAmplitude;

	float		m_flBrightness;
	float		m_flSpeed;

	int			m_nStartFrame;
	float		m_flFrameRate;

	float		m_flRed;
	float		m_flGreen;
	float		m_flBlue;

	bool		m_bRenderable;

	int			m_nSegments;

	int			m_nFlags;

	// Rings
	Vector		m_vecCenter;
	float		m_flStartRadius;
	float		m_flEndRadius;

	BeamInfo_t() { //-V730
		m_nType = TE_BEAMPOINTS;
		m_nSegments = -1;
		m_pszModelName = NULL;
		m_pszHaloName = NULL;
		m_nModelIndex = -1;
		m_nHaloIndex = -1;
		m_bRenderable = true;
		m_nFlags = 0;
	}
};

class Beam_t;
class C_Beam;

class IViewRenderBeams
{
public:
	// Construction
public:
	IViewRenderBeams( void );
	virtual				~IViewRenderBeams( void );

	// Implement IViewRenderBeams
public:
	virtual	void		InitBeams( void );
	virtual	void		ShutdownBeams( void );
	virtual	void		ClearBeams( void );

	// Updates the state of the temp ent beams
	virtual void		UpdateTemeBeams( );

	virtual void		DrawBeam( Beam_t *pbeam );
	virtual void		DrawBeam( C_Beam* pbeam, ITraceFilter *pEntityBeamTraceFilter = NULL );

	virtual	void		KillDeadBeams( IClientEntity *pDeadEntity );

	virtual	void		CreateBeamEnts( int startEnt, int endEnt, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float endWidth, float fadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b, int type = -1 );
	virtual Beam_t		*CreateBeamEnts( BeamInfo_t &beamInfo );

	virtual	void		CreateBeamEntPoint( int	nStartEntity, const Vector *pStart, int nEndEntity, const Vector* pEnd,
		int modelIndex, int haloIndex, float haloScale,
		float life, float width, float endWidth, float fadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b );
	virtual Beam_t		*CreateBeamEntPoint( BeamInfo_t &beamInfo );

	virtual	void		CreateBeamPoints( Vector& start, Vector& end, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float endWidth, float fadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b );
	virtual	Beam_t		*CreateBeamPoints( BeamInfo_t &beamInfo );

	virtual	void		CreateBeamRing( int startEnt, int endEnt, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float endWidth, float fadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b, int flags );
	virtual Beam_t		*CreateBeamRing( BeamInfo_t &beamInfo );

	virtual void		CreateBeamRingPoint( const Vector& center, float start_radius, float end_radius, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b, int flags );
	virtual Beam_t		*CreateBeamRingPoint( BeamInfo_t &beamInfo );

	virtual	void		CreateBeamCirclePoints( int type, Vector& start, Vector& end,
		int modelIndex, int haloIndex, float haloScale, float life, float width,
		float endWidth, float fadeLength, float amplitude, float brightness, float speed,
		int startFrame, float framerate, float r, float g, float b );
	virtual Beam_t		*CreateBeamCirclePoints( BeamInfo_t &beamInfo );

	virtual	void		CreateBeamFollow( int startEnt, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float endWidth, float fadeLength, float r, float g, float b,
		float brightness );
	virtual Beam_t		*CreateBeamFollow( BeamInfo_t &beamInfo );
};