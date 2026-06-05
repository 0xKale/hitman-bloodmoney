#include "functions.hpp"

#include "../gui/pages/player/player.hpp"

#include <Windows.h>
#include <cstdint>

namespace functions
{
	constexpr uintptr_t cheatMenuMemory = 0x4ABA89;
	constexpr uintptr_t base = 0x400000;

	void cheatEnableMenu() noexcept
	{
		const DWORD pointer = *reinterpret_cast<DWORD*>(cheatMenuMemory);

		if (vars::bDebugMenu)
		{
			*reinterpret_cast<int*>(pointer + base) = 1;
		}
		else
		{
			*reinterpret_cast<int*>(pointer + base) = 0;
		}
	}
}
