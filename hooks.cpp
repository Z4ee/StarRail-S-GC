#include "minhook.h"

#include "hooks.h"
#include "menu.h"
#include "utils.h"

#include <iostream>

typedef __int64(__stdcall* BATTLE_ACTION)(__int64 v1, __int32 v2, __int64 v3, __int64 v4, __int64 v5);
typedef __int64(__fastcall* DIALOGUE)(__int64 v1, unsigned __int8 v2);

typedef HANDLE(WINAPI* CREATE_FILE_W)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);

namespace hooks {
	namespace anti_cheat {

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
	}

	namespace game {
		
	}

	DWORD init(LPVOID lpThreadParameter) {

		if (MH_Initialize() != MH_OK)
		{
			printf("Error initializing MinHook library\n");

			return 0;
		}

		if (MH_CreateHookApiEx(L"kernelbase", "CreateFileW", &anti_cheat::h_createfilew, reinterpret_cast<void**>(&anti_cheat::p_CreateFileW), reinterpret_cast<void**>(&anti_cheat::t_CreateFileW)) != MH_OK)
		{
			printf("Error creating hook for CreateFileW function\n");

			return 0;
		}

		if (MH_EnableHook(anti_cheat::t_CreateFileW) != MH_OK)
		{
			printf("Error enabling hook for CreateFileW function\n");

			return 0;
		}

		menu::menu();

		return 1;
	}
}