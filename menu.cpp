#include "menu.h"
#include "hooks.h"

#include <windows.h>
#include <iostream>

#include "utils.h"

namespace menu {

	static float speed = 1.f;

	__int64 game_assembly = 0;

	void set_speed(float speed) {

		if (game_assembly) {
			utils::write<float>(utils::read<__int64>(utils::read<__int64>(game_assembly + 0x8CAA6A0) + 0xC0) + 0x1DC, speed);
		}
		else {
			printf("Failed to find GameAssembly.dll");
		}
		/* [["GameAssembly.dll" + 0x8CAA6A0]+ 0xC0] + 0x1DC */
	}

	DWORD speedhack_thread(void* lpThreadParameter) {

		while (!game_assembly) game_assembly = reinterpret_cast<uint64_t>(GetModuleHandleA("GameAssembly.dll"));
		
		do
		{
			static bool speedhack = 0;

			if (GetAsyncKeyState(VK_CAPITAL) && 1) {
				speedhack = !speedhack;

				if (speedhack) {
					Beep(800, 500);
					set_speed(speed);
				}
				else {
					Beep(600, 500);
					set_speed(1.f);
				}
			}



			Sleep(100);
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