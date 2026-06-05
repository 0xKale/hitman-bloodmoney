#pragma once
#include <cstdint>

namespace game
{
	namespace offsets
	{
		inline uintptr_t playerBase = 0x00ABCDEF;
		inline uintptr_t entityList = 0x00FEDCBA;
		inline uintptr_t viewMatrix = 0x00123456;

		constexpr uintptr_t bCheatsMenuEnabled = 0x4ABA89;
		constexpr uintptr_t bGodMode = 0x00ABCDEF;
		constexpr uintptr_t bInfAmmo = 0x00FEDCBA;
		constexpr uintptr_t bInvisibleMode = 0x00123456;
	}
}
