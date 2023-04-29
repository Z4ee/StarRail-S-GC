#define _CRT_SECURE_NO_WARNINGS

#include "main.h"


BOOL DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH) {

		auto base_address = reinterpret_cast<uint64_t>(GetModuleHandleA("starrailbase.dll"));

		utils::write<uint32_t>(base_address + 0xFCDC0, 0xCCC3C031);
		utils::write<uint8_t>(base_address + 0xF9940, 0xC3);

		AllocConsole();

		FILE* file = freopen("CONOUT$", "w", stdout);

		CreateThread(0, 0, hooks::init, 0, 0, 0);
	}

	return TRUE;
}