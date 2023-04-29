#include <minhook.h>

#include "hooks.h"

#include <iostream>

typedef DWORD64(__stdcall* BATTLE_ACTION)(DWORD64 v1, DWORD32 v2, DWORD64 v3, DWORD64 v4, DWORD64 v5);
typedef HANDLE(WINAPI* CREATE_FILE_W)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);

namespace hooks {
	BATTLE_ACTION o_battle_action = nullptr;
	CREATE_FILE_W p_CreateFileW = nullptr;
	CREATE_FILE_W t_CreateFileW;

	HANDLE WINAPI h_createfilew(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
	{
		if (memcmp(lpFileName, L"\\\\.\\ACE-BASE", 24) == 0) {
			wprintf(L"Thread (%i) attempting to communicate with anti-cheat driver -> %s\n", GetCurrentThreadId(), lpFileName);

			SuspendThread(GetCurrentThread()); // 200iq bypass for memory protection
		}

		return p_CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}

	int __stdcall h_battle_action(DWORD64 v1, DWORD32 v2, DWORD64 v3, DWORD64 v4, DWORD64 v5) {
		wprintf(L"game::battle_action(0x%I64X, 0x%I32X, 0x%I64X, \"%s\", 0x%I64X)\n", v1, v2, v3, &((wchar_t*)v4)[10], v5);
		
		/*
			* v3 is the number of skill points that are added in (shl rax, 0x21)
			* v4 - this is what action the performer is now
		*/

		return o_battle_action(v1, v2, 0, v4, v5);
	}

	uint64_t base_address = 0;

	DWORD init(LPVOID lpThreadParameter) {

		if (MH_Initialize() != MH_OK)
		{
			printf("Error initializing MinHook library\n");

			return 0;
		}

		if (MH_CreateHookApiEx(L"kernelbase", "CreateFileW", &h_createfilew, reinterpret_cast<void**>(&p_CreateFileW), reinterpret_cast<void**>(&t_CreateFileW)) != MH_OK)
		{
			printf("Error creating hook for CreateFileW function\n");

			return 0;
		}

		if (MH_EnableHook(t_CreateFileW) != MH_OK)
		{
			printf("Error enabling hook for CreateFileW function\n");

			return 0;
		}

		while ((base_address = reinterpret_cast<uint64_t>(GetModuleHandleA("GameAssembly.dll"))) == 0);

		auto h_address = reinterpret_cast<void**>(base_address + 0x5656FF0); // That RVA for global version ( idk for China version )
		
		if (MH_CreateHook(h_address, &h_battle_action, reinterpret_cast<void**>(&o_battle_action)) != MH_OK) {
			printf("Error creating hook for battle_action function\n");

			return 0;
		}

		if (MH_EnableHook(h_address) != MH_OK)
		{
			printf("Error enabling hook for battle_action function\n");

			return 0;
		}

		return 1;
	}
}
