#include "SettingsManager.hpp"

void C_SettingsManager::SetupData( )
{
	B.reserve( 4096 );
	I.reserve( 4096 );
	F.reserve( 4096 );
	IC.reserve( 4096 );
	C.reserve( 4096 );

	this->B = {
		{ _S( "ragebot.enable" ), false },
		{ _S( "ragebot.forced_safety" ), false },
		{ _S( "ragebot.double_tap" ), false },
		{ _S( "ragebot.hide_shots" ), false },
		{ _S( "ragebot.multi_thread" ), false },

		{ _S( "antiaims.enable" ), false },
		{ _S( "antiaims.at_knife" ), true },
		{ _S( "antiaims.slow_walk" ), false },
		{ _S( "antiaims.jitter_sides" ), false },
		{ _S( "antiaims.avoid_overlap" ), false },
		{ _S( "antiaims.jitter_move" ), false },
		{ _S( "antiaims.at_targets" ), false },
		{ _S( "antiaims.invert_side" ), false },
		{ _S( "antiaims.anti_peek" ), false },
		{ _S( "antiaims.fake_flick" ), false },

		{ _S( "misc.auto_jump" ), false },
		{ _S( "misc.auto_strafe" ), false },
		{ _S( "misc.fog.enable" ), false },
		{ _S( "misc.fake_lag.enable" ), false },
		{ _S( "misc.fake_lag.randomization" ), false },
		{ _S( "misc.ad_block" ), false },
		{ _S( "misc.unlock_inventory_access" ), false },

		{ _S( "misc.buybot.enable" ), false },
		{ _S( "misc.buybot.keep_awp" ), false },
		{ _S( "misc.buybot.molotov" ), false },
		{ _S( "misc.buybot.smokegrenade" ), false },
		{ _S( "misc.buybot.hegrenade" ), false },
		{ _S( "misc.buybot.taser" ), false },
		{ _S( "misc.buybot.heavyarmor" ), false },
		{ _S( "misc.buybot.vesthelm" ), false },
		{ _S( "misc.buybot.defuser" ), false },

		{ _S( "misc.team_ragdolls" ), false },
		{ _S( "misc.enemy_ragdolls" ), false },
		{ _S( "misc.local_ragdolls" ), false },
		{ _S( "misc.infinity_duck" ), false },
		{ _S( "misc.edge_jump" ), false },
		{ _S( "misc.fake_duck" ), false },
		{ _S( "misc.quick_stop" ), false },
		{ _S( "misc.visual_interpolation" ), false },
		{ _S( "misc.visual_interpolation_dead_only" ), false },
		{ _S( "misc.change_fov_while_scoped" ), false },
		{ _S( "misc.hidden_convars" ), false },
		{ _S( "misc.filter_console" ), false },
		{ _S( "misc.grenade_warning" ), false },
		{ _S( "misc.watermark" ), false },
		{ _S( "misc.spectator_list" ), false },
		{ _S( "misc.shared_esp" ), false },
		{ _S( "misc.shared_chams" ), false },
		{ _S( "misc.auto_pick" ), false },
		{ _S( "misc.auto_pick_button" ), false },
		{ _S( "misc.auto_pick_on_shot" ), false },
		{ _S( "misc.out_of_view" ), false },
		{ _S( "misc.third_person" ), false },
		{ _S( "misc.penetration_crosshire" ), false },
		{ _S( "misc.preserve_killfeed" ), false },
		{ _S( "misc.client_impacts" ), false },
		{ _S( "misc.server_impacts" ), false },
		{ _S( "misc.hitmarker_world" ), false },
		{ _S( "misc.hitmarker_crosshair" ), false },

		{ _S( "world.show_world_grenades" ), false },

		{ _S( "ui.watermark" ), false },
		{ _S( "ui.spectator_list" ), false },
		{ _S( "ui.keybindlist" ), false },
		{ _S( "ui.chat" ), false },
		{ _S( "ui.custom_hud" ), false },

		{ _S( "misc.hud.kill_list_colorize" ), false },
		{ _S( "misc.hud.clear_kill_feed_on_round" ), false },
		{ _S( "misc.hud.crosshire_dot" ), false },

		{ _S( "misc.radar.enable" ), false },
		{ _S( "misc.radar.item_1" ), false },
		{ _S( "misc.radar.item_2" ), false },
		{ _S( "misc.radar.item_3" ), false },
		{ _S( "misc.radar.item_4" ), false },
		{ _S( "misc.radar.item_5" ), false },
		{ _S( "misc.radar.item_6" ), false },
		{ _S( "misc.radar.item_7" ), false },

		{ _S( "misc.logs.item_1" ), false },
		{ _S( "misc.logs.item_2" ), false },
		{ _S( "misc.logs.item_3" ), false },
		{ _S( "misc.logs.item_4" ), false },
		{ _S( "misc.logs.item_5" ), false },

		{ _S( "misc.removals.item_1" ), false },
		{ _S( "misc.removals.item_2" ), false },
		{ _S( "misc.removals.item_3" ), false },
		{ _S( "misc.removals.item_4" ), false },
		{ _S( "misc.removals.item_5" ), false },
		{ _S( "misc.removals.item_6" ), false },
		{ _S( "misc.removals.item_7" ), false },
		{ _S( "misc.removals.item_8" ), false },
		{ _S( "misc.removals.item_9" ), false },
		{ _S( "misc.removals.item_10" ), false },
		{ _S( "misc.removals.item_11" ), false },
		{ _S( "misc.removals.item_12" ), false },
	};

	this->I = {
		{ _S( "ragebot.double_tap_option" ), 0 },

		{ _S( "antiaims.yaw_offset" ), 0 },
		{ _S( "antiaims.jitter_offset" ), 0 },
		{ _S( "antiaims.leg_mode" ), 0 },
		{ _S( "antiaims.roll_antiaim" ), 0 },
		{ _S( "antiaims.desync_on_shot" ), 0 },
		{ _S( "antiaims.left_limit" ), 60 },
		{ _S( "antiaims.right_limit" ), 60 },
		{ _S( "antiaims.anti_peek_mode" ), 0 },

		{ _S( "misc.viewmodel.fov" ), 60 },
		{ _S( "misc.camerafov" ), 90 },
		{ _S( "misc.no_scope_mode" ), 0 },
		{ _S( "misc.fog.min_distance" ), 0 },
		{ _S( "misc.fog.max_distance" ), 0 },
		{ _S( "misc.fake_lag.amount" ), 1 },
		{ _S( "misc.fake_lag.randomize" ), 0 },
		{ _S( "misc.transparent_in_tperson" ), 100 },
		{ _S( "misc.third_person_distance" ), 150 },
		{ _S( "misc.buybot.primary" ), 0 },
		{ _S( "misc.buybot.secondary" ), 0 },
		{ _S( "misc.visual_interpolation_amount" ), 1 },
		{ _S( "misc.hit_sound" ), 0 },
		{ _S( "misc.hit_sound_volume" ), 100 },
		{ _S( "misc.skybox" ), 0 },

		{ _S( "misc.hud.crosshire_dot_size" ), 2 },
		{ _S( "misc.hud.crosshire_line_size" ), 0 },
		{ _S( "misc.hud.crosshire_gap_size" ), 0 },
		{ _S( "misc.hud.crosshire_width_size" ), 1 },
		{ _S( "misc.hud.kill_list_time" ), 6000 },
	};

	this->F = {
		{ _S( "misc.viewmodel.x" ), 1.0f },
		{ _S( "misc.viewmodel.y" ), 1.0f },
		{ _S( "misc.viewmodel.z" ), -1.0f },
		{ _S( "misc.aspect_ratio" ), 1.7777f },
		{ _S( "misc.client_impacts_duration" ), 4.0f },
		{ _S( "misc.server_impacts_duration" ), 4.0f },
	};

	this->IC = {
		{ _S( "misc.out_of_view" ), ImColor( 255, 255, 255, 255 ).Value },
		{ _S( "misc.server_impacts" ), ImColor( 0, 0, 255, 255 ).Value },
		{ _S( "misc.client_impacts" ), ImColor( 255, 0, 0, 128 ).Value },
		{ _S( "world.grenades_flashbang" ), ImColor( 255, 255, 255, 255 ).Value },
		{ _S( "world.grenades_hegrenade" ), ImColor( 255, 255, 255, 255 ).Value },
		{ _S( "world.grenades_molotov" ), ImColor( 255, 255, 255, 255 ).Value },
		{ _S( "world.grenades_inc" ), ImColor( 255, 255, 255, 255 ).Value },
		{ _S( "world.grenades_decoy" ), ImColor( 255, 255, 255, 255 ).Value },
		{ _S( "world.grenades_smoke" ), ImColor( 255, 255, 255, 255 ).Value },

		{ _S( "misc.hud.kill_terrorist" ), ImColor( 186, 184, 96 ).Value },
		{ _S( "misc.hud.kill_counter_terrorist" ), ImColor( 101, 149, 183 ).Value },
		{ _S( "misc.hud.kill_list_color" ), ImColor( 255, 255, 255 ).Value },
		{ _S( "misc.hud.kill_list_headshot" ), ImColor( 255, 255, 255 ).Value },
		{ _S( "misc.hud.crosshire_color" ), ImColor( 0, 255, 0 ).Value },
	};

	this->C = {
		{ _S( "misc.fog.color" ), Color( 255, 255, 255, 255 ) },
		{ _S( "misc.world_modulation" ), Color( 255, 255, 255, 255 ) },
		{ _S( "misc.prop_modulation" ), Color( 255, 255, 255, 255 ) },
		{ _S( "misc.sky_modulation" ), Color( 255, 255, 255, 255 ) },
	};

	auto GenerateRagebotSettings = [ this ] ( std::string subname ) {
		this->B.insert( {  _S( "ragebot." )  + subname +  _S( ".auto_stop" ), false } );
		this->B.insert( {   _S( "ragebot." )  + subname +  _S( ".auto_scope" ), false } );
		this->B.insert( {   _S( "ragebot." )  + subname +  _S( ".early_auto_stop" ), false } );
		this->B.insert( {  _S( "ragebot." )  + subname +  _S( ".extended_teleport" ), false } );
		this->B.insert( {   _S( "ragebot."  ) + subname +  _S( ".safe_priority" ), false } );
		this->B.insert( {   _S( "ragebot."  ) + subname +  _S( ".dynamic_points" ), false } );

		this->I.insert( {   _S( "ragebot."  ) + subname +  _S( ".min_damage" ), 80 } );
		this->I.insert( {   _S( "ragebot."  ) + subname +  _S( ".hit_chance" ), 80 } );
		this->I.insert( {   _S( "ragebot."  ) + subname +  _S( ".accuracy_boost" ), 80 } );
		this->I.insert( {   _S( "ragebot."  ) + subname +  _S( ".head_scale" ), 80 } );
		this->I.insert( {   _S( "ragebot."  ) + subname +  _S( ".body_scale" ), 80 } );

		this->B.insert( {   _S( "ragebot."  ) + subname +  _S( ".double_tap_1" ), false } );
		this->B.insert( {   _S( "ragebot."  ) + subname +  _S( ".double_tap_2" ), false } );

		this->B.insert( {  _S( "ragebot." )  + subname +  _S( ".hit_box_1" ), true } );
		this->B.insert( {   _S( "ragebot." )  + subname +  _S( ".hit_box_2" ), true } );
		this->B.insert( {   _S( "ragebot." )  + subname +  _S( ".hit_box_3" ), true } );
		this->B.insert( {  _S( "ragebot." )  + subname +  _S( ".hit_box_4" ), true } );
		this->B.insert( {  _S( "ragebot." )  + subname +  _S( ".hit_box_5" ), true } );
		this->B.insert( {  _S( "ragebot." )  + subname +  _S( ".hit_box_6" ), true } );
		this->B.insert( { _S( "ragebot." )  + subname + _S( ".hit_box_7" ), true } );
		this->B.insert( {  _S( "ragebot." )  + subname + _S( ".hit_box_8" ), true } );
		this->B.insert( { _S( "ragebot." )  + subname + _S( ".hit_box_9" ), true } );
		this->B.insert( {  _S( "ragebot." )  + subname + _S( ".hit_box_10" ), true } );
		this->B.insert( {  _S( "ragebot." )  + subname + _S( ".hit_box_11" ), true } );
		this->B.insert( { _S( "ragebot." ) + subname + _S( ".hit_box_12" ), true } );
	};

	GenerateRagebotSettings( _S( "auto" ) );
	GenerateRagebotSettings( _S( "scout" ) );
	GenerateRagebotSettings( _S( "awp" ) );
	GenerateRagebotSettings( _S( "deagle" ) );
	GenerateRagebotSettings( _S( "revolver" ) );
	GenerateRagebotSettings( _S( "pistol" ) );
	GenerateRagebotSettings( _S( "rifle" ) );
	GenerateRagebotSettings( _S( "shotgun" ) );
	GenerateRagebotSettings( _S( "smg" ) );

	auto GenerateGlowSettings = [ this ] ( std::string subname ) {
		this->B.insert( { _S( "glow." ) + subname + _S( ".enabled" ), false } );
		this->I.insert( { _S( "glow." ) + subname + _S( ".style" ), 0 } );
		this->IC.insert( { _S( "glow." ) + subname + _S( ".color" ), ImColor( 255, 255, 255, 255 ).Value } );
	};

	GenerateGlowSettings( _S( "local" ) );
	GenerateGlowSettings( _S( "team" ) );
	GenerateGlowSettings( _S( "enemy" ) );
	GenerateGlowSettings( _S( "weapon" ) );

	auto GenerateChamsSettings = [ this ] ( std::string subname ) {
		this->B.insert( { _S( ( "chams." ) ) + subname + _S( ".draw_chams" ), false } );
		this->C.insert( { _S( ( "chams." ) ) + subname + _S( ".color" ), Color( 0, 128, 255, 255 ) } );
		this->I.insert( { _S( ( "chams." ) ) + subname + _S( ".main_style" ), 0 } );

		this->B.insert( { _S( ( "chams." ) ) + subname + _S( ".mat_param_1" ), false } );
		this->B.insert( { _S( ( "chams." ) ) + subname + _S( ".mat_param_2" ), false } );
		this->B.insert( { _S( ( "chams." ) ) + subname + _S( ".mat_param_3" ), false } );
		this->B.insert( { _S( ( "chams." ) ) + subname + _S( ".mat_param_4" ), false } );
		this->B.insert( { _S( ( "chams." ) ) + subname + _S( ".mat_param_5" ), false } );

		this->C.insert( { _S( ( "chams." ) ) + subname + _S( ".color1" ), Color( 0, 128, 255, 255 ) } );
		this->C.insert( { _S( ( "chams." ) ) + subname + _S( ".color2" ), Color( 0, 128, 255, 255 ) } );
		this->C.insert( { _S( ( "chams." ) ) + subname + _S( ".color3" ), Color( 0, 128, 255, 255 ) } );
		this->C.insert( { _S( ( "chams." ) ) + subname + _S( ".color4" ), Color( 0, 128, 255, 255 ) } );
		this->C.insert( { _S( ( "chams." ) ) + subname + _S( ".color5" ), Color( 0, 128, 255, 255 ) } );
	};

	GenerateChamsSettings( _S( "real" ) );
	GenerateChamsSettings( _S( "desync" ) );
	GenerateChamsSettings( _S( "arms" ) );
	GenerateChamsSettings( _S( "local_weapon" ) );
	GenerateChamsSettings( _S( "weapon_visible" ) );
	GenerateChamsSettings( _S( "weapon_invisible" ) );
	GenerateChamsSettings( _S( "enemy_visible" ) );
	GenerateChamsSettings( _S( "enemy_invisible" ) );
	GenerateChamsSettings( _S( "enemy_backtrack" ) );
	GenerateChamsSettings( _S( "on_shot" ) );
	GenerateChamsSettings( _S( "team_visible" )) ;
	GenerateChamsSettings( _S( "team_invisible" ) );

	auto GenerateEspSettings = [ this ] ( std::string subname ) {
		this->B.insert( { "esp." + subname + ".enabled", false } );
		this->B.insert( { "esp." + subname + ".only_visible", false } );
		this->I.insert( { "esp." + subname + ".box.bbox", false } );
		this->B.insert( { "esp." + subname + ".box.outline_upper", false } );
		this->B.insert( { "esp." + subname + ".box.outline_inner", false } );
		this->B.insert( { "esp." + subname + ".skeleton", false } );
		this->B.insert( { "esp." + subname + ".money", false } );
		this->B.insert( { "esp." + subname + ".ammo_amount", false } );
		this->IC.insert( { "esp." + subname + ".health_upper", ImColor( 235, 52, 52, 190 ).Value } );
		this->IC.insert( { "esp." + subname + ".health_bttom", ImColor( 103, 235, 52, 190 ).Value } );
		this->IC.insert( { "esp." + subname + ".health_outline_upper", ImColor( 45, 45, 45, 70 ).Value } );
		this->IC.insert( { "esp." + subname + ".health_outline_bottom", ImColor( 45, 45, 45, 180 ).Value } );
		this->IC.insert( { "esp." + subname + ".armor", ImColor( 0, 221, 255, 255 ).Value } );
		this->IC.insert( { "esp." + subname + ".armor_outline_upper", ImColor( 45, 45, 45, 70 ).Value } );
		this->IC.insert( { "esp." + subname + ".armor_outline_bottom", ImColor( 45, 45, 45, 180 ).Value } );

		this->IC.insert( { "esp." + subname + ".ammo", ImColor( 219, 97, 26, 255 ).Value } );
		this->IC.insert( { "esp." + subname + ".ammo_outline_upper", ImColor( 45, 45, 45, 70 ).Value } );
		this->IC.insert( { "esp." + subname + ".ammo_outline_bottom", ImColor( 45, 45, 45, 180 ).Value } );

		this->IC.insert( { "esp." + subname + ".box", ImColor( 240, 240, 240, 190 ).Value } );
		this->IC.insert( { "esp." + subname + ".box_outline_upper", ImColor( 45, 45, 45, 70 ).Value } );
		this->IC.insert( { "esp." + subname + ".box_outline_bottom", ImColor( 45, 45, 45, 70 ).Value } );
		this->IC.insert( { "esp." + subname + ".skeleton", ImColor( 240, 240, 240, 190 ).Value } );
		this->IC.insert( { "esp." + subname + ".name", ImColor( 240, 240, 240, 190 ).Value } );
		this->IC.insert( { "esp." + subname + ".weapon", ImColor( 240, 240, 240, 190 ).Value } );

		this->IC.insert( { "esp." + subname + ".ping", ImColor( 240, 240, 240, 190 ).Value } );
		this->IC.insert( { "esp." + subname + ".scope", ImColor( 240, 240, 240, 190 ).Value } );
		this->IC.insert( { "esp." + subname + ".defusing", ImColor( 240, 240, 240, 190 ).Value } );
		this->IC.insert( { "esp." + subname + ".flashed", ImColor( 240, 240, 240, 190 ).Value } );
		this->IC.insert( { "esp." + subname + ".taser", ImColor( 240, 240, 240, 190 ).Value } );
		this->IC.insert( { "esp." + subname + ".lc", ImColor( 240, 240, 240, 190 ).Value } );
		this->IC.insert( { "esp." + subname + ".ducking", ImColor( 240, 240, 240, 190 ).Value } );

		for ( auto a = 0; a < 9; a++ )
			this->I.insert( { "esp." + subname + ".font_sizing." + std::to_string( a ), 14 } );
	};

	GenerateEspSettings( _S( "enemy" ) );
	GenerateEspSettings( _S( "team" ) );
}

RageSettings_t& C_SettingsManager::GetRagebotSettings( std::string subname ) {
	RageSettings_t Settings;

	Settings.m_bAutoStop = this->B.at( _S( "ragebot." ) + subname + _S( ".auto_stop" ) );
	Settings.m_bAutoScope = this->B.at( _S( "ragebot." ) + subname + _S( ".auto_scope" ) );
	Settings.m_bEarlyAutoStop = this->B.at( _S( "ragebot." ) + subname + _S( ".early_auto_stop" ) );
	Settings.m_bExtendedTeleport = this->B.at( _S( "ragebot." ) + subname + _S( ".extended_teleport" ) );
	Settings.m_bSafePriority = this->B.at( _S( "ragebot." ) + subname + _S( ".safe_priority" ) );
	Settings.m_bDynamicPoints = this->B.at( _S( "ragebot." ) + subname + _S( ".dynamic_points" ) );
	Settings.m_nMinDamage = this->I.at( _S( "ragebot." ) + subname + _S( ".min_damage" ) );
	Settings.m_nHitChance = this->I.at( _S( "ragebot." ) + subname + _S( ".hit_chance" ) );
	Settings.m_nAccuracyBoost = this->I.at( _S( "ragebot." ) + subname + _S( ".accuracy_boost" ) );
	Settings.m_nHeadScale = this->I.at( _S( "ragebot." ) + subname + _S( ".head_scale" ) );
	Settings.m_nBodyScale = this->I.at( _S( "ragebot." ) + subname + _S( ".body_scale" ) );

	Settings.m_Hitboxes[ 0 ] = this->B.at(  _S(  "ragebot."  ) + subname + _S( ".hit_box_1" ) );
	Settings.m_Hitboxes[ 1 ] = this->B.at(  _S(  "ragebot."  ) + subname + _S( ".hit_box_2" ) );
	Settings.m_Hitboxes[ 2 ] = this->B.at(  _S( "ragebot."  ) + subname + _S( ".hit_box_3" ) );
	Settings.m_Hitboxes[ 3 ] = this->B.at(  _S(  "ragebot."  ) + subname + _S( ".hit_box_4" ) );
	Settings.m_Hitboxes[ 4 ] = this->B.at(  _S(  "ragebot."  ) + subname + _S( ".hit_box_5" ) );
	Settings.m_Hitboxes[ 5 ] = this->B.at( _S(  "ragebot."  ) + subname + _S( ".hit_box_6" ) );
	Settings.m_Hitboxes[ 6 ] = this->B.at(  _S( "ragebot."  ) + subname + _S( ".hit_box_7" ) );
	Settings.m_Hitboxes[ 7 ] = this->B.at(  _S( "ragebot."  ) + subname + _S( ".hit_box_8" ) );
	Settings.m_Hitboxes[ 8 ] = this->B.at( _S( "ragebot." ) + subname + _S( ".hit_box_9" ) );
	Settings.m_Hitboxes[ 9 ] = this->B.at(  _S( "ragebot."  ) + subname + _S( ".hit_box_10" ) );
	Settings.m_Hitboxes[ 10 ] = this->B.at(  _S( "ragebot."  ) + subname + _S( ".hit_box_11" ) );
	Settings.m_Hitboxes[ 11 ] = this->B.at( _S( "ragebot." ) + subname + _S( ".hit_box_12" ) );

	return Settings;
}

GlowSettings_t C_SettingsManager::GetGlowSettings( std::string subname ) {
	GlowSettings_t Settings;

	Settings.m_bEnable = this->B.at( _S( "glow." ) + subname + _S( ".enabled" ) );
	Settings.m_nGlowStyle = this->I.at( _S( "glow." ) + subname + _S( ".style" ) );
	Settings.m_Color = this->IC.at( _S( "glow." ) + subname + _S( ".color" ) );

	return Settings;
}

ChamsSettings_t C_SettingsManager::GetChamsSettings( std::string subname ) {
	ChamsSettings_t Settings;

	Settings.m_bDrawChams = this->B.at( _S( "chams." ) + subname + _S( ".draw_chams" ) );
	Settings.m_Color = this->C.at( _S( "chams." ) + subname + _S( ".color" ) );
	Settings.m_iFirstChamsLayerStyle = this->I.at( _S( "chams." ) + subname + _S( ".main_style" ) );

	Settings.m_aMaterialParams[ 0 ] = this->B.at( _S( "chams." ) + subname + _S( ".mat_param_1" ) );
	Settings.m_aMaterialParams[ 1 ] = this->B.at( _S( "chams." ) + subname + _S( ".mat_param_2" ) );
	Settings.m_aMaterialParams[ 2 ] = this->B.at( _S( "chams." ) + subname + _S( ".mat_param_3" ) );
	Settings.m_aMaterialParams[ 3 ] = this->B.at( _S( "chams." ) + subname + _S( ".mat_param_4" ) );
	Settings.m_aMaterialParams[ 4 ] = this->B.at( _S( "chams." ) + subname + _S( ".mat_param_5" ) );

	Settings.m_aMaterialsColor[ 0 ] = this->C.at( _S( "chams." ) + subname + _S( ".color1" ) );
	Settings.m_aMaterialsColor[ 1 ] = this->C.at( _S( "chams." ) + subname + _S( ".color2" ) );
	Settings.m_aMaterialsColor[ 2 ] = this->C.at( _S( "chams." ) + subname + _S( ".color3" ) );
	Settings.m_aMaterialsColor[ 3 ] = this->C.at( _S( "chams." ) + subname + _S( ".color4" ) );
	Settings.m_aMaterialsColor[ 4 ] = this->C.at( _S( "chams." ) + subname + _S( ".color5" ) );

	return Settings;
}

PlayerESPSettings_t C_SettingsManager::GetEspSettings( std::string subname ) {
	PlayerESPSettings_t Settings;

	Settings.m_bEnable = this->B.at( "esp." + subname + ".enabled" );
	Settings.m_bOnlyVisible = this->B.at( "esp." + subname + ".only_visible" );
	Settings.m_bBox = this->I.at( "esp." + subname + ".box.bbox" );
	Settings.m_bBoxOutlineUpper = this->B.at( "esp." + subname + ".box.outline_upper" );
	Settings.m_bBoxOutlineInner = this->B.at( "esp." + subname + ".box.outline_inner" );
	Settings.m_bSkeleton = this->B.at( "esp." + subname + ".skeleton" );
	Settings.m_bMoney = this->B.at( "esp." + subname + ".money" );
	Settings.m_bAmmoAmount = this->B.at( "esp." + subname + ".ammo_amount" );
	Settings.m_cHealthColorBottom = this->IC.at( "esp." + subname + ".health_bttom" );
	Settings.m_cHealthColorOutlineUpper = this->IC.at( "esp." + subname + ".health_outline_upper" );
	Settings.m_cHealthColorOutlineInner = this->IC.at( "esp." + subname + ".health_outline_bottom" );
	Settings.m_cArmorColor = this->IC.at( "esp." + subname + ".armor" );
	Settings.m_cArmorColorOutlineUpper = this->IC.at( "esp." + subname + ".armor_outline_upper" );
	Settings.m_cArmorColorOutlineInner = this->IC.at( "esp." + subname + ".armor_outline_bottom" );
	Settings.m_cBoxColor = this->IC.at( "esp." + subname + ".box" );
	Settings.m_cBoxColorOutlineUpper = this->IC.at( "esp." + subname + ".box_outline_upper" );
	Settings.m_cBoxColorOutlineInner = this->IC.at( "esp." + subname + ".box_outline_bottom" );

	Settings.m_cAmmoColor = this->IC.at( "esp." + subname + ".ammo" );
	Settings.m_cAmmoColorOutlineInner = this->IC.at( "esp." + subname + ".ammo_outline_bottom" );
	Settings.m_cAmmoColorOutlineUpper = this->IC.at( "esp." + subname + ".ammo_outline_upper" );

	Settings.m_cNameColor = this->IC.at( "esp." + subname + ".name" );
	Settings.m_cWeaponColor = this->IC.at( "esp." + subname + ".weapon" );
	Settings.m_cPingColor = this->IC.at( "esp." + subname + ".ping" );
	Settings.m_cScopeColor = this->IC.at( "esp." + subname + ".scope" );
	Settings.m_cDefusingColor = this->IC.at( "esp." + subname + ".defusing" );
	Settings.m_cFlashedColor = this->IC.at( "esp." + subname + ".flashed" );
	Settings.m_cTaserColor = this->IC.at( "esp." + subname + ".taser" );
	Settings.m_cLCColor = this->IC.at( "esp." + subname + ".lc" );
	Settings.m_cDuckingcColor = this->IC.at( "esp." + subname + ".ducking" );

	for ( auto a = 0; a < 9; a++ )
		Settings.m_aFontSizes[a] = this->I.at( "esp." + subname + ".font_sizing." + std::to_string( a ) );

	return Settings;
}