#include "hooks.hpp"

#include "cheats.hpp"
#include "memory.hpp"
#include "offsets.hpp"

#include "../../ext/minhook/minhook.h"

#include <cstdint>

namespace game
{
	namespace
	{
		// sub_65AD50 - the dev-menu "Give All Items" command. Takes no args, returns a ptr.
		using GiveAllMenuFn = void*(__cdecl*)();
		GiveAllMenuFn oGiveAllMenuCmd = nullptr;
		void* giveAllTarget = nullptr;

		// The game's dev-menu Give All dereferences the Root manager (dword_81F83C) and
		// Context manager (dword_81F820) with NO null checks before dispatching
		// MSG_GIVEALLITEMS. If it is triggered during a level load/teardown window those
		// globals are briefly null/half-initialized and the game faults - this is the
		// "chance of crashing even from the debug menu" bug. Guard the same pointers the
		// command is about to walk; when they are not ready, skip the command instead of
		// crashing. In a valid state every pointer is non-null so this calls straight
		// through and behaves identically to the original.
		void* __cdecl GiveAllMenuGuard()
		{
			const auto* const rootSlot = Global<std::uintptr_t>(offsets::pRootManager);
			const auto* const contextManagerSlot = Global<std::uintptr_t>(offsets::pContextManager);
			if (!rootSlot || !contextManagerSlot || !*rootSlot || !*contextManagerSlot)
			{
				return nullptr;
			}

			const std::uintptr_t context =
				*reinterpret_cast<std::uintptr_t*>(*contextManagerSlot + offsets::kContextOffset);
			if (!context)
			{
				return nullptr;
			}

			return oGiveAllMenuCmd();
		}
	}

	void Setup()
	{
		// MinHook is already initialized by the bootstrapper's Setup() before this runs.
		void* const target = reinterpret_cast<void*>(GetModuleBase() + offsets::fnGiveAllMenuCmd);
		if (MH_CreateHook(target, &GiveAllMenuGuard, reinterpret_cast<void**>(&oGiveAllMenuCmd)) == MH_OK
			&& MH_EnableHook(target) == MH_OK)
		{
			giveAllTarget = target;
		}

		// Always-on hooks for "NPC One-Hit": tag 47 at his init and intercept
		// ZHM3Actor::ApplyDamage so the toggle can make any hit lethal to NPCs while the
		// player keeps normal health. The toggle only gates the lethal-damage swap.
		InstallActorTracking();
	}

	void Destroy() noexcept
	{
		// Revert any code patches so ejecting (VK_END) leaves the game untouched.
		SetInfAmmo(false);
		SetNpcOneHp(false);
		RemoveActorTracking();

		if (giveAllTarget)
		{
			MH_DisableHook(giveAllTarget);
		}
	}
}
