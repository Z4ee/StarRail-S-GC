#pragma once

#include "Config.h"

namespace Cheat
{
	namespace hooks {
		__int64(__fastcall* o_setcurrentphase)(__int64 a1, int a2, __int64 a3, char a4) = nullptr;
		char(__fastcall* o_get_isindialog)(__int64 a1) = nullptr;
		__int64(__fastcall* o_isautobattle)(__int64 a1);
		char(__fastcall* o_setautobattleflag)(__int64 a1, unsigned __int8 a2);

		struct HookData {
			std::string name;
			DWORD rva;

			void* hook;
			void* original;
		};

		namespace game {

			int phase = 0;

			std::chrono::steady_clock::time_point last_call_time;

			/* because get_isindialog returns 0 in some dialogs (HSR developers, please fix this) */
			bool get_is_in_dialog() {
				auto current_time = std::chrono::steady_clock::now();
				auto time_since_last_call = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_call_time);
				return time_since_last_call.count() < 25;
			}

			const char* get_phase_in_text() {
				const char* phaseText = "Indefinite";


				phase == 12 ? phaseText = "World" : 0;
				phase == 15 ? phaseText = "Battle" : 0;
				hooks::game::get_is_in_dialog() ? phaseText = "Dialogue" : 0;

				return phaseText;
			}

		}

		char __fastcall h_setautobattleflag(__int64 a1, unsigned __int8 a2) {
			return o_setautobattleflag(a1, a2);
		}

		__int64 __fastcall h_isautobattle(__int64 a1) {
			auto ret = o_isautobattle(a1);

			if (!ret && GlobalSetting::battle::force_battle) {
				h_setautobattleflag(a1, 1);
			}

			return ret;
		}

		/* (RPG.Client.GamePhaseManager.SetCurrentPhase) */
		__int64 __fastcall h_setcurrentphase(__int64 a1, int a2, __int64 a3, char a4) {
			game::phase = a2;
			return o_setcurrentphase(a1, a2, a3, a4);
		}

		/* (RPG.Client.DialogueManager.get_IsInDialog) */
		char __fastcall h_get_isindialog(__int64 a1) {
			//printf("called->get_isindialog()\n");
			game::last_call_time = std::chrono::steady_clock::now();
			return o_get_isindialog(a1);;
		}

		inline void Setup() {

			std::vector<HookData> v_hooks = {};

			if (GlobalSetting::ChinaVersion) {
				v_hooks.push_back({ "setcurrentphase", 0x5B9DFD0, &h_setcurrentphase, &o_setcurrentphase });
				v_hooks.push_back({ "get_isindialog", 0x5ADD450, &h_get_isindialog, &o_get_isindialog });
				v_hooks.push_back({ "isautobattle", 0x512F580, &h_isautobattle, &o_isautobattle });
				v_hooks.push_back({ "setautobattleflag", 0x5130040,&h_setautobattleflag,&o_setautobattleflag });
			}
			else {
				v_hooks.push_back({ "setcurrentphase", 0x5B9E130, &h_setcurrentphase, &o_setcurrentphase });
				v_hooks.push_back({ "get_isindialog", 0x5ADD460, &h_get_isindialog, &o_get_isindialog });
				v_hooks.push_back({ "isautobattle", 0x512F580, &h_isautobattle, &o_isautobattle });
				v_hooks.push_back({ "setautobattleflag", 0x5130040,&h_setautobattleflag,&o_setautobattleflag });
			}

			static auto CreateHook = [](LPVOID pTarget, LPVOID pDetour, LPVOID* ppOriginal)->bool
			{
				return MH_CreateHook(pTarget, pDetour, ppOriginal) == MH_OK && MH_EnableHook(pTarget) == MH_OK;
			};

			uint64_t game_assembly = 0;

			while (!game_assembly)
				game_assembly = reinterpret_cast<uint64_t>(GetModuleHandleA("gameassembly.dll"));

			for (auto& hook : v_hooks) {
				if (!CreateHook(reinterpret_cast<void**>(game_assembly + hook.rva), hook.hook, reinterpret_cast<void**>(hook.original))) {
					printf("[-] Failed to create/enable hook for %s\n", hook.name.c_str());
				}
			}
		}
	}

	inline void Update()
	{
		if (!GlobalSetting::ShowMenu)
			return;

		static ImGuiWindowFlags classFinderWindowFlags = ImGuiWindowFlags_AlwaysAutoResize;

		ImGui::Begin("HSR-GC", 0, classFinderWindowFlags);

		ImGui::BeginTabBar("##tabs");

		if (ImGui::BeginTabItem("World"))
		{
			ImGui::Checkbox("Speed Modifier", &GlobalSetting::world::speed_hack);

			if (GlobalSetting::world::speed_hack) {
				ImGui::SliderFloat("Global", &GlobalSetting::world::global_speed, 0.1f, 10.f, "%.1f");
				ImGui::SliderFloat("Dialogue", &GlobalSetting::world::dialogue_speed, 0.1f, 10.f, "%.1f");
			}

			ImGui::Checkbox("Peeking", &GlobalSetting::world::peeking);

			ImGui::Checkbox("Auto-Dialogue", &GlobalSetting::world::auto_dialogue);

			if (GlobalSetting::world::auto_dialogue) {
				ImGui::Text("also works on hotkey (CAPSLOCK)");
				ImGui::Checkbox("Mouse Mode", &GlobalSetting::world::mouse_mode);
			}

			ImGui::Checkbox("Invisibility", &GlobalSetting::world::invisibility);

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Battle"))
		{
			ImGui::Checkbox("Speed Modifier", &GlobalSetting::battle::speed_hack);

			if (GlobalSetting::battle::speed_hack) {

				ImGui::SliderFloat("Battle", &GlobalSetting::battle::battle_speed, 0.1f, 100.f, "%.1f");

			}

			//ImGui::Checkbox("Auto-Battle Unlock", &GlobalSetting::battle::auto_battle_unlock);

			ImGui::Checkbox("Force Auto-Battle", &GlobalSetting::battle::force_battle);

			if (GlobalSetting::battle::force_battle) {
				ImGui::Text("if you enabled it in battle then you need to do some action to make it work");
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Other"))
		{
			ImGui::Checkbox("FPS Unlock", &GlobalSetting::other::fps_unlock);

			if (GlobalSetting::other::fps_unlock) {
				ImGui::InputInt("FPS", &GlobalSetting::other::fps);
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Debug"))
		{

			ImGui::Text("Phase: %s", hooks::game::get_phase_in_text());
			ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Settings"))
		{
			static bool pinWindow = false;

			if (ImGui::Checkbox("Pin Window", &pinWindow))
			{
				if (pinWindow)
				{
					classFinderWindowFlags |= ImGuiWindowFlags_NoMove;
					classFinderWindowFlags |= ImGuiWindowFlags_NoResize;
				}
				else
				{
					classFinderWindowFlags &= ~ImGuiWindowFlags_NoMove;
					classFinderWindowFlags &= ~ImGuiWindowFlags_NoResize;
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Reset Position"))
				ImGui::SetWindowPos(ImVec2(0, 0));

			ImGui::SameLine();

			if (ImGui::Button("Unload"))
				GlobalSetting::Unload = true;

			if (ImGui::Button("Save Config")) {
				Config::SaveConfig();
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("About"))
		{
			ImGui::Text("About this project");
			ImGui::Text("This project was created by Z4ee");
			ImGui::Text("This project is free, not for sell");
			ImGui::Text("List of Contributing Developers");
			ImGui::Text("Z4ee");
			ImGui::Text("ky-ler");

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
		ImGui::End();
	}



	inline void Dialogue() {
		HWND target_window = 0;

		while (!target_window) target_window = FindWindowA("UnityWndClass", nullptr);

		while (true) {
			if (hooks::game::phase != 12) {
				Sleep(500);
				continue;
			}

			if (GlobalSetting::world::auto_dialogue && !GlobalSetting::ShowMenu) {

				if (hooks::game::get_is_in_dialog() || GetAsyncKeyState(VK_CAPITAL)) {

					Sleep(16);

					if (GetForegroundWindow() == target_window) {
						if (!GlobalSetting::world::mouse_mode) {
							keybd_event(VK_SPACE, 0, 0, 0);
							Sleep(20);
							keybd_event(VK_SPACE, 0, KEYEVENTF_KEYUP, 0);
						}
						else {
							POINT cursor_pos;
							GetCursorPos(&cursor_pos);
							mouse_event(MOUSEEVENTF_LEFTDOWN, cursor_pos.x, cursor_pos.y, 0, 0);
							Sleep(10);
							mouse_event(MOUSEEVENTF_LEFTUP, cursor_pos.x, cursor_pos.y, 0, 0);
						}
					}
				}
			}
			else {
				Sleep(100);
			}
		}
	}

	inline void UpdateSpeed(const float speed, uint64_t game_assembly, uint64_t unity_player, const bool is_battle) {

		if (is_battle) {
			Utils::Write<float>(Utils::Read<__int64>(Utils::Read<__int64>(game_assembly + 0x8CAA6A0) + 0xC0) + 0x1DC, speed);
			Utils::Write<float>(Utils::Read<__int64>(unity_player + 0x1D21D78) + 0xFC, 1.f);
		}
		else {
			Utils::Write<float>(Utils::Read<__int64>(Utils::Read<__int64>(game_assembly + 0x8CAA6A0) + 0xC0) + 0x1DC, 1.f);
			Utils::Write<float>(Utils::Read<__int64>(unity_player + 0x1D21D78) + 0xFC, speed);
		}
	}

	inline void UpdateWorld(uint64_t game_assembly, uint64_t unity_player) {
		if (hooks::game::phase != 12) {
			return;
		}

		float speed = 1.f;

		if (GlobalSetting::world::speed_hack) {
			speed = hooks::game::get_is_in_dialog() || GetAsyncKeyState(VK_CAPITAL) ? GlobalSetting::world::dialogue_speed : GlobalSetting::world::global_speed;
		}

		UpdateSpeed(speed, game_assembly, unity_player, false);

		Utils::Write<uint8_t>(game_assembly + 0x51292C0, GlobalSetting::world::peeking ? 0xC3 : 0x40);
		Utils::Write<uint8_t>(game_assembly + 0x5800F40, GlobalSetting::world::invisibility ? 0xC3 : 0x40);
	}

	inline void UpdateBattle(uint64_t game_assembly, uint64_t unity_player) {
		if (hooks::game::phase != 15) {
			return;
		}

		UpdateSpeed(GlobalSetting::battle::speed_hack ? GlobalSetting::battle::battle_speed : 1.f, game_assembly, unity_player, true);

		Utils::Write<uint32_t>(game_assembly + 0x5DA5F20, GlobalSetting::battle::auto_battle_unlock ? 0x90C3C031 : 0x83485340);
	}

	inline void UpdateOther(uint64_t unity_player) {
		if (GlobalSetting::other::fps_unlock) {
			if (GlobalSetting::other::fps > 59) {
				Utils::Write<uint32_t>(unity_player + 0x1C4E000, GlobalSetting::other::fps);
			}
		}
		else {
			Utils::Write<uint32_t>(unity_player + 0x1C4E000, 60);
		}
	}

	inline void Main() {
		uint64_t game_assembly = 0, unity_player = 0;

		while (!game_assembly && !unity_player) {
			game_assembly = reinterpret_cast<uint64_t>(GetModuleHandleA("gameassembly.dll"));
			unity_player = reinterpret_cast<uint64_t>(GetModuleHandleA("unityplayer.dll"));
		}

		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Dialogue, 0, 0, 0);

		while (true) {
			UpdateWorld(game_assembly, unity_player);
			UpdateBattle(game_assembly, unity_player);
			UpdateOther(unity_player);

			Sleep(500);
		}
	}

	inline void Setup()
	{
		hooks::Setup();

		auto hook = ImGuiContextHook();
		hook.Callback = (ImGuiContextHookCallback)Update;
		hook.Type = ImGuiContextHookType_NewFramePost;
		ImGui::AddContextHook(ImGui::GetCurrentContext(), &hook);
	}
}