#pragma once
#include <windows.h>

namespace hooks {
	namespace game {
		extern int current_phase;
		extern bool is_dialogue;
	}

	void init();
}