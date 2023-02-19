#pragma once
#include <iomanip>
#include "../../../SDK/SDK.hpp"
#include "../../../SDK/Math/Math.hpp"
#include "../../../Interface/Renderer/Renderer.hpp"
#include "../../../Interface/GUI/Menu.hpp"

#define TIME_TO_TICKS( flTime ) ( ( int )( 0.5f + ( float )( ( flTime ) ) / SDK::Interfaces::GlobalVars->m_flIntervalPerTick ) )
#define TICKS_TO_TIME( iTick ) ( float )( ( iTick ) * SDK::Interfaces::GlobalVars->m_flIntervalPerTick )
inline float CSGO_Armor( float flDamage, int ArmorValue )
{
	float flArmorRatio = 0.5f;
	float flArmorBonus = 0.5f;
	if ( ArmorValue > 0 ) {
		float flNew = flDamage * flArmorRatio;
		float flArmor = ( flDamage - flNew ) * flArmorBonus;

		if ( flArmor > static_cast< float >( ArmorValue ) ) {
			flArmor = static_cast< float >( ArmorValue ) * ( 1.f / flArmorBonus );
			flNew = flDamage - flArmor;
		}

		flDamage = flNew;
	}
	return flDamage;
}

class C_GrenadePrediction
{
public:
	struct data_t {
		data_t( ) = default;
		data_t( C_BasePlayer* owner, int index, const Vector& origin, const Vector& velocity, float throw_time, int offset )
		{
			m_owner = owner;
			m_index = index;

			predict( origin, velocity, throw_time, offset );
		}

		bool physics_simulate( ) {
			if ( m_detonated )
				return true;

			const auto sv_gravity = SDK::EngineData::m_ConvarList[ CheatConvarList::Gravity ];

			const auto new_velocity_z = m_velocity.z - ( sv_gravity->GetFloat( ) * 0.4f ) * SDK::Interfaces::GlobalVars->m_flIntervalPerTick;

			const auto move = Vector(
				m_velocity.x * SDK::Interfaces::GlobalVars->m_flIntervalPerTick,
				m_velocity.y * SDK::Interfaces::GlobalVars->m_flIntervalPerTick,
				( m_velocity.z + new_velocity_z ) / 2.f * SDK::Interfaces::GlobalVars->m_flIntervalPerTick
			);

			m_velocity.z = new_velocity_z;

			auto trace = trace_t( );

			physics_push_entity( move, trace );

			if ( m_detonated )
				return true;

			if ( trace.fraction != 1.f ) {
				update_path< true >( );

				perform_fly_collision_resolution( trace );
			}

			return false;
		}

		void physics_trace_entity( const Vector& src, const Vector& dst, std::uint32_t mask, trace_t& trace ) {
			SDK::Interfaces::EngineTrace->TraceHull(
				src, dst, { -2.f, -2.f, -2.f }, { 2.f, 2.f, 2.f },
				mask, m_owner, m_collision_group, &trace
			);

			if ( trace.startsolid
				&& ( trace.contents & CONTENTS_CURRENT_90 ) ) {
				trace.CleanOut( );

				SDK::Interfaces::EngineTrace->TraceHull(
					src, dst, { -2.f, -2.f, -2.f }, { 2.f, 2.f, 2.f },
					mask & ~CONTENTS_CURRENT_90, m_owner, m_collision_group, &trace
				);
			}

			if ( !trace.DidHit( )
				|| !trace.hit_entity
				|| !reinterpret_cast< C_BaseEntity* >( trace.hit_entity )->IsPlayer( ) )
				return;

			trace.CleanOut( );

			SDK::Interfaces::EngineTrace->TraceLine( src, dst, mask, m_owner, m_collision_group, &trace );
		}

		void physics_push_entity( const Vector& push, trace_t& trace ) {
			physics_trace_entity( m_origin, m_origin + push,
				m_collision_group == COLLISION_GROUP_DEBRIS
				? ( MASK_SOLID | CONTENTS_CURRENT_90 ) & ~CONTENTS_MONSTER
				: MASK_SOLID | CONTENTS_OPAQUE | CONTENTS_IGNORE_NODRAW_OPAQUE | CONTENTS_CURRENT_90 | CONTENTS_HITBOX,
				trace
			);

			if ( trace.startsolid ) {
				m_collision_group = COLLISION_GROUP_INTERACTIVE_DEBRIS;

				SDK::Interfaces::EngineTrace->TraceLine(
					m_origin - push, m_origin + push,
					( MASK_SOLID | CONTENTS_CURRENT_90 ) & ~CONTENTS_MONSTER,
					m_owner, m_collision_group, &trace
				);
			}

			if ( trace.fraction ) {
				m_origin = trace.endpos;
			}

			if ( !trace.hit_entity )
				return;

			if ( reinterpret_cast< C_BaseEntity* >( trace.hit_entity )->IsPlayer( )
				|| m_index != WEAPON_TAGRENADE && m_index != WEAPON_MOLOTOV && m_index != WEAPON_INCGRENADE )
				return;

			const auto weapon_molotov_maxdetonateslope = SDK::Interfaces::CVar->FindVar( _S( "weapon_molotov_maxdetonateslope" ) );

			if ( m_index != WEAPON_TAGRENADE
				&& trace.plane.normal.z < std::cos( DEG2RAD( weapon_molotov_maxdetonateslope->GetFloat( ) ) ) )
				return;

			detonate< true >( );
		}

		void perform_fly_collision_resolution( trace_t& trace ) {
			auto surface_elasticity = 1.f;

			if ( trace.hit_entity )
			{
				if ( reinterpret_cast< C_BaseEntity* >( trace.hit_entity )->IsBreakableEntity( ) ) {
				BREAKTROUGH:
					m_velocity *= 0.4f;

					return;
				}

				const auto is_player = reinterpret_cast< C_BaseEntity* >( trace.hit_entity )->IsPlayer( );
				if ( is_player ) {
					surface_elasticity = 0.3f;
				}

				if ( trace.hit_entity->EntIndex( ) ) {
					if ( is_player
						&& m_last_hit_entity == trace.hit_entity ) {
						m_collision_group = COLLISION_GROUP_DEBRIS;

						return;
					}

					m_last_hit_entity = trace.hit_entity;
				}
			}

			auto velocity = Vector( );

			const auto back_off = m_velocity.Dot( trace.plane.normal ) * 2.f;

			for ( auto i = 0u; i < 3u; i++ ) {
				const auto change = trace.plane.normal[ i ] * back_off;

				velocity[ i ] = m_velocity[ i ] - change;

				if ( std::fabs( velocity[ i ] ) >= 1.f )
					continue;

				velocity[ i ] = 0.f;
			}

			velocity *= std::clamp< float >( surface_elasticity * 0.45f, 0.f, 0.9f );

			if ( trace.plane.normal.z > 0.7f ) {
				const auto speed_sqr = velocity.LengthSqr( );
				if ( speed_sqr > 96000.f ) {
					const auto l = velocity.Normalized( ).Dot( trace.plane.normal );
					if ( l > 0.5f ) {
						velocity *= 1.f - l + 0.5f;
					}
				}

				if ( speed_sqr < 400.f ) {
					m_velocity = { };
				}
				else {
					m_velocity = velocity;

					physics_push_entity( velocity * ( ( 1.f - trace.fraction ) * SDK::Interfaces::GlobalVars->m_flIntervalPerTick ), trace );
				}
			}
			else {
				m_velocity = velocity;

				physics_push_entity( velocity * ( ( 1.f - trace.fraction ) * SDK::Interfaces::GlobalVars->m_flIntervalPerTick ), trace );
			}

			if ( m_bounces_count > 20 )
				return detonate< false >( );

			++m_bounces_count;
		}

		void think( ) {
			switch ( m_index ) {
				case WEAPON_SMOKEGRENADE:
				if ( m_velocity.LengthSqr( ) <= 0.01f ) {
					detonate< false >( );
				}

				break;
				case WEAPON_DECOY:
				if ( m_velocity.LengthSqr( ) <= 0.04f ) {
					detonate< false >( );
				}

				break;
				case WEAPON_FLASHBANG:
				case WEAPON_HEGRENADE:
				case WEAPON_MOLOTOV:
				case WEAPON_INCGRENADE:
				if ( TICKS_TO_TIME( m_tick ) > m_detonate_time ) {
					detonate< false >( );
				}

				break;
			}

			m_next_think_tick = m_tick + TIME_TO_TICKS( 0.2f );
		}

		template < bool _bounced >
		void detonate( ) {
			m_detonated = true;

			update_path< _bounced >( );
		}

		template < bool _bounced >
		void update_path( ) {
			m_last_update_tick = m_tick;

			m_path.emplace_back( m_origin, _bounced );
		}

		void predict( const Vector& origin, const Vector& velocity, float throw_time, int offset ) {
			m_origin = origin;
			m_velocity = velocity;
			m_collision_group = COLLISION_GROUP_PROJECTILE;

			const auto tick = TIME_TO_TICKS( 1.f / 30.f );

			m_last_update_tick = -tick;

			switch ( m_index ) {
				case WEAPON_SMOKEGRENADE: m_next_think_tick = TIME_TO_TICKS( 1.5f ); break;
				case WEAPON_DECOY: m_next_think_tick = TIME_TO_TICKS( 2.f ); break;
				case WEAPON_FLASHBANG:
				case WEAPON_HEGRENADE:
				m_detonate_time = 1.5f;
				m_next_think_tick = TIME_TO_TICKS( 0.02f );

				break;
				case WEAPON_MOLOTOV:
				case WEAPON_INCGRENADE:
				const auto molotov_throw_detonate_time = SDK::Interfaces::CVar->FindVar( _S( "molotov_throw_detonate_time" ) );

				m_detonate_time = molotov_throw_detonate_time->GetFloat( );
				m_next_think_tick = TIME_TO_TICKS( 0.02f );

				break;
			}

			m_SourceTime = throw_time;
			for ( ; m_tick < TIME_TO_TICKS( 17.f ); ++m_tick ) {
				if ( m_next_think_tick <= m_tick ) {
					think( );
				}

				if ( m_tick < offset )
					continue;

				if ( physics_simulate( ) )
					break;

				if ( m_last_update_tick + tick > m_tick )
					continue;

				update_path< false >( );
			}

			if ( m_last_update_tick + tick <= m_tick ) {
				update_path< false >( );
			}

			m_ExpireTime = throw_time + TICKS_TO_TIME( m_tick );
		}

		bool draw( ) const {
			if ( m_path.size( ) <= 1u
				|| TICKS_TO_TIME( g_Globals->m_LocalPlayer->m_nTickBase( ) ) >= m_ExpireTime )
				return false;

			Vector vecPrevious = Vector( 0, 0, 0 );
			Vector vecPreviousOrigin = Vector( 0, 0, 0 );
			Vector vecScreenPosition = Vector( 0, 0, 0 );

			Vector vecTimerPosition = m_path.back( ).first;
			vecTimerPosition.z += 10.0f;

			if ( !SDK::Interfaces::DebugOverlay->ScreenPosition( vecTimerPosition, vecScreenPosition ) )
			{
				std::string szIcon = "";
				switch ( m_index )
				{
					case WEAPON_HEGRENADE: szIcon = _S( "j" ); break;
					case WEAPON_SMOKEGRENADE: szIcon = _S( "k" ); break;
					case WEAPON_FLASHBANG: szIcon = _S( "i" ); break;
					case WEAPON_DECOY: szIcon = _S( "m" ); break;
					case WEAPON_INCGRENADE: szIcon = _S( "n" ); break;
					case WEAPON_MOLOTOV: szIcon = _S( "l" ); break;
				}

				ImVec2 vecTextSize = g_Menu->m_sFonts.WeapIcons->CalcTextSizeA( 20.0f, FLT_MAX, NULL, szIcon.c_str( ) );
				if ( m_owner != g_Globals->m_LocalPlayer )
				{
					g_Renderer->AddCircle2DFilled( vecTimerPosition, 180, 25.0f, ImColor( 0, 0, 0, 255 ) );
					g_Renderer->AddText( g_Menu->m_sFonts.WeapIcons, 20, ImVec2( vecScreenPosition.x - vecTextSize.x / 2, vecScreenPosition.y - vecTextSize.y / 2 ), szIcon, ImColor( 255, 255, 255 ) );
				}
				else
				{
					auto prev_screen = Vector( );
					auto prev_on_screen = !SDK::Interfaces::DebugOverlay->ScreenPosition( std::get< Vector >( m_path.front( ) ), prev_screen );

					for ( auto i = 1u; i < m_path.size( ); ++i ) {
						auto cur_screen = Vector( );
						const auto cur_on_screen = !SDK::Interfaces::DebugOverlay->ScreenPosition( std::get< Vector >( m_path.at( i ) ), cur_screen );

						if ( prev_on_screen
							&& cur_on_screen )
						{
							g_Renderer->AddLine( ImVec2( prev_screen.x, prev_screen.y ), ImVec2( cur_screen.x, cur_screen.y ), ImColor( 255, 255, 255 ), 2.0f );
						}

						prev_screen = cur_screen;
						prev_on_screen = cur_on_screen;
					}
				}
			}
			return true;
		}

		bool											m_detonated { };
		C_BasePlayer* m_owner { };
		Vector											m_origin { }, m_velocity { };
		IClientEntity* m_last_hit_entity { };
		float											m_SourceTime;
		int												m_collision_group { };
		float											m_detonate_time { }, m_ExpireTime { };
		int												m_index { }, m_tick { }, m_next_think_tick { },
			m_last_update_tick { }, m_bounces_count { };
		std::vector< std::pair< Vector, bool > >		m_path { };
	} m_data { };

	std::unordered_map< unsigned long, data_t > m_list { };
public:
	__forceinline C_GrenadePrediction( ) = default;

	virtual void OnCreateMove( C_UserCmd* cmd );

	virtual const data_t& get_local_data( ) const { return m_data; }

	std::unordered_map< unsigned long, data_t >& get_list( ) { return m_list; }
};

inline C_GrenadePrediction* g_GrenadePrediction = new C_GrenadePrediction( );