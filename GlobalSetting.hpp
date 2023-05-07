
#pragma once

namespace GlobalSetting
{
	inline bool ShowMenu = true;
	inline bool Unload = false;
	inline bool ChinaVersion = false;

	namespace world 
	{
		inline bool speed_hack = false;
		inline float global_speed = 1.f;
		inline float dialogue_speed = 1.f;

		inline bool dump_enemys = false;
		inline bool peeking = false;
		inline bool auto_dialogue = false;
		inline bool mouse_mode = false;
		inline bool invisibility = false;
	}

	namespace battle 
	{
		inline bool speed_hack = false;
		inline float battle_speed = 1.f;

		inline bool auto_battle_unlock = false;
		bool force_battle = false;
	}

	namespace other 
	{
		inline bool fps_unlock = false;
		inline int fps = 60;
	}
}