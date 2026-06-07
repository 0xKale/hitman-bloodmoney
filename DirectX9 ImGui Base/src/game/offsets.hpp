#pragma once
#include <cstdint>

namespace game
{
	namespace offsets
	{
		inline uintptr_t playerBase = 0x00ABCDEF;
		inline uintptr_t entityList = 0x00FEDCBA;
		inline uintptr_t viewMatrix = 0x00123456;

		// RVA = MEM_LOC - 0x400000 = NEW_RVA_MEM_LOC
		constexpr uintptr_t bCheatsMenuEnabled = 0x4ABA89;
		constexpr uintptr_t bGodMode = 0x59B344;
		constexpr uintptr_t bInfAmmo = 0x1871C6FC; // broken
		constexpr uintptr_t bInfClips = 0x59A928;
		constexpr uintptr_t bInvisibleMode = 0x59B840;

		constexpr uintptr_t pRootManager = 0x41F83C;
		constexpr uintptr_t pContextManager = 0x41F820;
		constexpr uintptr_t kHandlerOffset = 0xA40;
		constexpr uintptr_t kContextOffset = 0xB8;
		constexpr uintptr_t kVtableLookup = 0x78;
		constexpr uintptr_t kVtableExecute = 0x120;
		constexpr uintptr_t kMsgLine = 0x35;
		constexpr uintptr_t strGiveAllItems = 0x39D8A8;
		constexpr uintptr_t strCheatMenuCpp = 0x39D8BC;
	}
}
