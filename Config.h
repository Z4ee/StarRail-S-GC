#pragma once

#include <string>
#include <fstream>

#include "GlobalSetting.hpp"
#include "dependencies/Json/json.hpp"

using json = nlohmann::json;

namespace Config 
{
	struct ConfigPreset 
	{
		bool EnableWorldSpeedHack = false;
		float GlobalSpeed = 1.f;
		float DialogueSpeed = 1.f;

		bool DumpEnemies = false;
		bool Peeking = false;
		bool AutoDialogue = false;
		bool MouseMode = false;
		bool Invisibility = false;

		bool EnableBattleSpeedHack = false;
		float BattleSpeed = 1.f;
		bool AutoBattleUnlock = false;
		bool ForceBattle = false;

		bool FpsUnlock = false;
		int Fps = 60;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
		ConfigPreset,
		EnableWorldSpeedHack,
		GlobalSpeed,
		DialogueSpeed, 
		DumpEnemies,
		Peeking,
		AutoDialogue,
		MouseMode,
		Invisibility,
		EnableBattleSpeedHack,
		BattleSpeed,
		AutoBattleUnlock,
		ForceBattle,
		FpsUnlock,
		Fps
	)

	bool LoadConfig() 
	{
		json j;

		std::ifstream f("C:\\StarRail-S-GC\\config.json", std::ifstream::binary);
		if (f.fail()) return false;

		f >> j;

		GlobalSetting::world::speed_hack = j.get<ConfigPreset>().EnableWorldSpeedHack;
		GlobalSetting::world::global_speed = j.get<ConfigPreset>().GlobalSpeed;
		GlobalSetting::world::dialogue_speed = j.get<ConfigPreset>().DialogueSpeed;

		GlobalSetting::world::dump_enemys = j.get<ConfigPreset>().DumpEnemies;
		GlobalSetting::world::peeking = j.get<ConfigPreset>().Peeking;
		GlobalSetting::world::auto_dialogue = j.get<ConfigPreset>().AutoDialogue;
		GlobalSetting::world::mouse_mode = j.get<ConfigPreset>().MouseMode;
		GlobalSetting::world::invisibility = j.get<ConfigPreset>().Invisibility;

		GlobalSetting::battle::speed_hack = j.get<ConfigPreset>().EnableBattleSpeedHack;
		GlobalSetting::battle::battle_speed = j.get<ConfigPreset>().BattleSpeed;
		GlobalSetting::battle::auto_battle_unlock = j.get<ConfigPreset>().AutoBattleUnlock;
		GlobalSetting::battle::force_battle = j.get<ConfigPreset>().ForceBattle;

		GlobalSetting::other::fps_unlock = j.get<ConfigPreset>().FpsUnlock;
		GlobalSetting::other::fps = j.get<ConfigPreset>().Fps;

		return true;
	}

	void SaveConfig() 
	{
		const json j = ConfigPreset
		{
			GlobalSetting::world::speed_hack,
			GlobalSetting::world::global_speed,
			GlobalSetting::world::dialogue_speed,

			GlobalSetting::world::dump_enemys,
			GlobalSetting::world::peeking,
			GlobalSetting::world::auto_dialogue,
			GlobalSetting::world::mouse_mode,
			GlobalSetting::world::invisibility,

			GlobalSetting::battle::speed_hack,
			GlobalSetting::battle::battle_speed,
			GlobalSetting::battle::auto_battle_unlock,
			GlobalSetting::battle::force_battle,

			GlobalSetting::other::fps_unlock,
			GlobalSetting::other::fps
		};

		if (CreateDirectory(L"C:\\StarRail-S-GC", NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
			std::ofstream o("C:\\StarRail-S-GC\\config.json");
			o << std::setw(4) << j << std::endl;
		}
		else return;
	}
}

