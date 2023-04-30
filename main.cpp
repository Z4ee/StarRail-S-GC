#define _CRT_SECURE_NO_WARNINGS

#include "main.h"

BOOL DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH) {

		auto base_address = reinterpret_cast<uint64_t>(GetModuleHandleA("starrailbase.dll"));

		utils::write<uint32_t>(base_address + 0xFCDC0, 0xCCC3C031);
		utils::write<uint8_t>(base_address + 0xF9940, 0xC3);

		AllocConsole();

		FILE* file_out;
		FILE* file_in;

		errno_t err_out = freopen_s(&file_out, "CONOUT$", "w", stdout);
		errno_t err_in = freopen_s(&file_in, "CONIN$", "r", stdin);

		if (err_out == 0 || err_in == 0) {
			CreateThread(0, 0, hooks::init, 0, 0, 0);
		}
	}

	return TRUE;
}