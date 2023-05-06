#pragma once


namespace Cheat
{
	namespace world {
		bool speed_hack = false;
		float global_speed = 1.f;
		float dialogue_speed = 1.f;

		bool dump_enemys = false;
		bool peeking = false;
		bool auto_dialogue = false;
		bool mouse_mode = false;
		bool invisibility = false;
	}

	namespace battle {
		bool speed_hack = false;
		float battle_speed = 1.f;

		bool auto_battle_unlock = false;
	}

	namespace other {
		bool fps_unlock = false;
		int fps = 60;
	}

	inline void Update()
	{
		if (!GlobalSetting::ShowMenu)
			return;

		static ImGuiWindowFlags classFinderWindowFlags = 0;

		ImGui::Begin("HSR-GC", 0, classFinderWindowFlags);

		ImGui::BeginTabBar("##tabs");

		if (ImGui::BeginTabItem("World"))
		{
			ImGui::Checkbox("Speed Modifier", &world::speed_hack);

			if (world::speed_hack) {
				ImGui::SliderFloat("Global", &world::global_speed, 0.1f, 10.f, "%.1f");
				ImGui::SliderFloat("Dialogue", &world::dialogue_speed, 0.1f, 10.f, "%.1f");
			}

			ImGui::Checkbox("Peeking", &world::peeking);

			ImGui::Checkbox("Auto-Dialogue", &world::auto_dialogue);

			if (world::auto_dialogue) {
				ImGui::Checkbox("Mouse Mode", &world::mouse_mode);
			}

			ImGui::Checkbox("Invisibility", &world::invisibility);

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Battle"))
		{
			ImGui::Checkbox("Speed Modifier", &battle::speed_hack);

			if (battle::speed_hack) {

				ImGui::SliderFloat("Battle", &battle::battle_speed, 0.1f, 100.f, "%.1f");

			}

			ImGui::Checkbox("Auto-Battle Unlock", &battle::auto_battle_unlock);

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Other"))
		{
			ImGui::Checkbox("FPS Unlock", &other::fps_unlock);

			if (other::fps_unlock) {

				ImGui::InputInt("FPS", &other::fps);

			}

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

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
		ImGui::End();
	}

	namespace hooks {
		__int64(__fastcall* o_setcurrentphase)(__int64 a1, int a2, __int64 a3, char a4) = nullptr;
		char(__fastcall* o_get_isindialog)(__int64 a1) = nullptr;

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
		}

		/* (RPG.Client.GamePhaseManager.SetCurrentPhase) */
		__int64 __fastcall h_setcurrentphase(__int64 a1, int a2, __int64 a3, char a4) {
			game::phase = a2;
			return o_setcurrentphase(a1, a2, a3, a4);
		}

		/* (RPG.Client.DialogueManager.get_IsInDialog) */
		char __fastcall h_get_isindialog(__int64 a1) {
			game::last_call_time = std::chrono::steady_clock::now();
			return o_get_isindialog(a1);;
		}

		inline void Setup() {

			std::vector<HookData> v_hooks = {};

			if (GlobalSetting::ChinaVersion) {
				v_hooks.push_back({ "setcurrentphase", 0x5B9DFD0, &h_setcurrentphase, &o_setcurrentphase });
				v_hooks.push_back({ "get_isindialog", 0x5ADD460, &h_get_isindialog, &o_get_isindialog });
			}
			else {
				v_hooks.push_back({ "setcurrentphase", 0x5B9E130, &h_setcurrentphase, &o_setcurrentphase });
				v_hooks.push_back({ "get_isindialog", 0x5ADD460, &h_get_isindialog, &o_get_isindialog });
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

	inline void Dialogue() {

		HWND target_window = 0;
		while (!target_window) target_window = FindWindowA("UnityWndClass", nullptr);

		while (true)
		{
			if (hooks::game::phase == 12 && world::auto_dialogue && !GlobalSetting::ShowMenu && hooks::game::get_is_in_dialog()) {

				// idk, but SendMessage not working ):
				// SendMessageA(target_window, WM_KEYDOWN, VK_SPACE, 0); 
				// SendMessageA(target_window, WM_KEYUP, VK_SPACE, 0);
				// if you know how to fix this -> create GitHub issue or Pull Req

				Sleep(16);

				if (GetForegroundWindow() == target_window) {
					if (!world::mouse_mode) {
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
	}

	inline void Main() {
		uint64_t game_assembly = 0, unity_player = 0;

		while (!game_assembly && !unity_player) {
			game_assembly = reinterpret_cast<uint64_t>(GetModuleHandleA("gameassembly.dll"));
			unity_player = reinterpret_cast<uint64_t>(GetModuleHandleA("unityplayer.dll"));
		}

		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Dialogue, 0, 0, 0);

		while (true)
		{
			if (hooks::game::phase == 12) { // hooks::game::phase == WORLD
				if (world::speed_hack) {
					if (hooks::game::get_is_in_dialog()) {
						Utils::Write<float>(Utils::Read<uint64_t>(unity_player + 0x1D21D78) + 0xFC, world::dialogue_speed);
					}
					else {
						Utils::Write<float>(Utils::Read<uint64_t>(unity_player + 0x1D21D78) + 0xFC, world::global_speed);
						Utils::Write<float>(Utils::Read<uint64_t>(Utils::Read<uint64_t>(game_assembly + 0x8CAA6A0) + 0xC0) + 0x1DC, 1.f);
					}

				}
				else {
					Utils::Write<float>(Utils::Read<uint64_t>(unity_player + 0x1D21D78) + 0xFC, 1.f);
					Utils::Write<float>(Utils::Read<uint64_t>(Utils::Read<uint64_t>(game_assembly + 0x8CAA6A0) + 0xC0) + 0x1DC, 1.f);
				}

				/* (RPG.Client.BaseShaderPropertyTransition.SetElevationDitherAlphaValue ) */
				if (world::peeking) {
					Utils::Write<uint8_t>(game_assembly + 0x51292C0, 0xC3);
				}
				else {
					Utils::Write<uint8_t>(game_assembly + 0x51292C0, 0x40);
				}

				/* (RPG.GameCore.NPCComponent.set_AlertValue) */
				if (world::invisibility) {
					Utils::Write<uint8_t>(game_assembly + 0x5800F40, 0xC3);
				}
				else {
					Utils::Write<uint8_t>(game_assembly + 0x5800F40, 0x40);
				}
			}

			if (hooks::game::phase == 15) {
				if (battle::speed_hack) {
					Utils::Write<float>(Utils::Read<uint64_t>(Utils::Read<uint64_t>(game_assembly + 0x8CAA6A0) + 0xC0) + 0x1DC, battle::battle_speed);
				}
				else {
					Utils::Write<float>(Utils::Read<uint64_t>(Utils::Read<uint64_t>(game_assembly + 0x8CAA6A0) + 0xC0) + 0x1DC, 1.f);
				}

				/* (RPG.GameCore.BattleInstance.IsStageForbidAutoBattle) */
				if (battle::auto_battle_unlock) {
					Utils::Write<uint32_t>(game_assembly + 0x5DA5F20, 0x90C3C031);
				}
				else {
					Utils::Write<uint32_t>(game_assembly + 0x5DA5F20, 0x83485340);
				}
			}

			if (other::fps_unlock) {
				if (other::fps > 59) {
					Utils::Write<uint32_t>(unity_player + 0x1C4E000, other::fps);
				}
			}
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