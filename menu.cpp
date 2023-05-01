#include "menu.h"
#include "hooks.h"

#include <windows.h>
#include <iostream>

#include "utils.h"

#include "game.h"

namespace menu {

	static float speed = 1.f;

	__int64 game_assembly = 0;
	__int64 unity_player = 0;

	void set_speed_global(float speed) {
		utils::write<float>(utils::read<__int64>(unity_player + 0x1D21D78) + 0xFC, speed);
	}

	void set_speed_battle(float speed) {
		utils::write<float>(utils::read<__int64>(utils::read<__int64>(game_assembly + 0x8CAA6A0) + 0xC0) + 0x1DC, speed);
	}

	void set_speed(float speed) {
		if (hooks::game::get_currect_phase() == RPG_BATTLE) {
			set_speed_battle(speed);
			set_speed_global(1.f);
		}
		else {
			set_speed_global(speed);
			set_speed_battle(1.f);
		}
	}

	DWORD speedhack_thread(void* lpThreadParameter) {

		while (!game_assembly) game_assembly = reinterpret_cast<uint64_t>(GetModuleHandleA("gameassembly.dll"));
		while (!unity_player) unity_player = reinterpret_cast<uint64_t>(GetModuleHandleA("unityplayer.dll"));

		do
		{
			static bool speedhack = 0;
			static bool auto_attack = 0;

			if (GetAsyncKeyState(VK_CAPITAL) && 1) {
				speedhack = !speedhack;
			}

			if (speedhack) {
				set_speed(speed);
			}
			else {
				set_speed(1.f);
			}

			Sleep(500);

		} while (true);
	}

	void menu() {

		Sleep(15000);

		CreateThread(0, 0, speedhack_thread, 0, 0, 0);

		while (true)
		{
			printf("currect speed value: %f\nchange to: ", speed);

			scanf_s("%f", &speed);
		}
	}
}