#pragma once
#include <cstdint>

namespace game
{
	namespace offsets
	{
		// RVA = MEM_LOC - 0x400000 = NEW_RVA_MEM_LOC
		constexpr uintptr_t bCheatsMenuEnabled = 0x4ABA89;
		constexpr uintptr_t bGodMode = 0x59B344;
		// InfAmmo has no static toggle flag like bInfClips. The dev-menu "InfAmmo" entry
		// is dead (type 0); its pointer chain *(pContextManager+0xB8)+0xB4 resolves into
		// the message-context object, not ammo storage - hence the old static RVA was junk
		// (a one-off heap address outside the module). Instead we patch ZItemWeapon's
		// reserve-ammo getter to always return 999, mirroring how bInfClips makes the clip
		// getter return 999. See game::SetInfAmmo.
		constexpr uintptr_t fnGetWeaponAmmo = 0x10EF90; // sub_50EF90 (VA 0x50EF90)
		constexpr uintptr_t bInfClips = 0x59A928;
		constexpr uintptr_t bInvisibleMode = 0x59B840;

		constexpr uintptr_t pRootManager = 0x41F83C;
		constexpr uintptr_t pContextManager = 0x41F820;
		constexpr uintptr_t kHandlerOffset = 0xA40;
		constexpr uintptr_t kMissionOffset = 0xA48; // RootManager+0xA48: mission/objectives object, null outside a mission
		constexpr uintptr_t kContextOffset = 0xB8;
		constexpr uintptr_t kVtableLookup = 0x78;
		constexpr uintptr_t kVtableExecute = 0x120;
		constexpr uintptr_t kMsgLine = 0x35;
		constexpr uintptr_t strGiveAllItems = 0x39D8A8;
		constexpr uintptr_t strCheatMenuCpp = 0x39D8BC;
		constexpr uintptr_t fnCompleteLevel = 0x25ACB0;
		constexpr uintptr_t fnGiveAllMenuCmd = 0x25AD50; // sub_65AD50: dev-menu Give All; derefs Root/Context managers with no null check
	}
}
