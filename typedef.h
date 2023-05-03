#pragma once

struct HookData {
    std::string name;
    DWORD rva;

    void* hook;
    void* original;
};

typedef __int64(__fastcall* LEAVECURRENTPHASE)(__int64 a1, __int64 a2);
LEAVECURRENTPHASE o_leavecurrentphase = nullptr;

typedef __int64(__fastcall* SETCURRENTPHASE)(__int64 a1, int a2, __int64 a3, char a4);
SETCURRENTPHASE o_setcurrentphase = nullptr;

typedef HANDLE(WINAPI* CREATE_FILE_W)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
CREATE_FILE_W p_CreateFileW = nullptr;
CREATE_FILE_W t_CreateFileW;

typedef __int64(__fastcall* ISAUTOBATTLE)(__int64 a1);
ISAUTOBATTLE o_isautobattle = nullptr;

typedef char(__fastcall* SETAUTOBATTLEFLAG)(__int64 a1, unsigned __int8 a2);
SETAUTOBATTLEFLAG o_setautobattleflag = nullptr;