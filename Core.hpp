#pragma once

#include <Windows.h>
#include <iostream>
#include <cstdint>
#include <memory.h>
#include <map>
#include <mutex>
#include <vector>
#include <atomic>
#include <array>
#include <list>
#include <string>
#include <unordered_map>	
#include <optional>
#include <filesystem>
#include <fstream>

#include "Dependencies/Direct3D/Direct3D.h"

//#include "Dependencies/Json/json.h"

#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_dx11.h>
#include <ImGui/backends/imgui_impl_win32.h>
#include <ImGui/imgui_internal.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>

#include "GlobalSetting.hpp"

#include "Dependencies/MinHook/MinHook.h"


#define CLASS(assembly, namespaze, klass) static Il2CppClass* ThisClass() { return I2Hrame->GetClassEx(assembly, namespaze, klass); }
#define MEMBER(klass, name, offset)  struct { char __pad__##name[offset]; klass name; }
#define STATIC_MEMBER(klass, name, offset) static klass get_##name() { return *reinterpret_cast<klass*>(*reinterpret_cast<uintptr_t*>((uintptr_t)ThisClass() + 0xB8) + offset); } static void set_##name(klass value) { *reinterpret_cast<klass*>(*reinterpret_cast<uintptr_t*>((uintptr_t)ThisClass() + 0xB8) + offset) = value; }

inline ID3D11Device* pDevice = NULL;
inline ID3D11DeviceContext* pContext = NULL;
inline ID3D11RenderTargetView* mainRenderTargetView = NULL;
inline WNDPROC oWndProc = NULL;
inline HWND hWnd = NULL;
inline HMODULE dll_hmodule = NULL;

#include "Utils.h"
#include "Cheat.h"
#include "Device.h"