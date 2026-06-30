#include "cheats.hpp"

#include "memory.hpp"
#include "offsets.hpp"

#include "../../ext/minhook/minhook.h"

#include <cstddef>
#include <cstdint>

namespace game
{
	namespace
	{
		// ZItemWeapon's reserve-ammo getter (sub_50EF90) pulls the round count out
		// of the weapon's ItemState. There is no flag to flip like bInfClips, so we
		// overwrite its prologue with `mov eax, 0x3E7 ; ret` to make every weapon
		// report 999 rounds - the same value bInfClips forces from the clip getter.
		//   B8 E7 03 00 00   mov eax, 3E7h
		//   C3               retn
		// The original 6 bytes (56 8B F1 8B 46 50) are stashed on first enable and
		// restored on disable, so the toggle is reversible.
		constexpr std::size_t kInfAmmoPatchSize = 6;
		constexpr std::uint8_t kInfAmmoPatch[kInfAmmoPatchSize] = {
			0xB8, 0xE7, 0x03, 0x00, 0x00, 0xC3
		};

		std::uint8_t infAmmoOriginal[kInfAmmoPatchSize] = {};
		bool infAmmoOriginalSaved = false;
		bool infAmmoEnabled = false;

		// ---- "NPC One-Hit (1 HP)" --------------------------------------------------
		// Make every non-player actor die from a single hit while 47 keeps normal health.
		// Enumerating actors misses everyone spawned before injection (we only ever saw 1),
		// so instead we hook the single choke point every hit funnels through:
		// ZHM3Actor::ApplyDamage (sub_632EA0) = `*(float*)(actor+0x928) -= damage` (current
		// HP @ +0x928, per the game's own one-shot table). In the detour any actor that is
		// not 47 takes a massive lethal damage instead of the real value, so one hit kills
		// it; 47 - matched by the pointer/vtable captured at his init (sub_5EAE10) - always
		// receives the real damage. No per-frame work, no tracking.
		using HeroInitFn = void(__thiscall*)(void* self);
		using ApplyDamageFn = double(__thiscall*)(void* self, float damage);

		HeroInitFn oHeroInit = nullptr;
		ApplyDamageFn oApplyDamage = nullptr;
		void* heroInitTarget = nullptr;
		void* applyDamageTarget = nullptr;

		void* g_hero = nullptr;     // 47's object pointer, captured at hero init
		void* heroVtable = nullptr; // 47's class vtable (backup discriminator)
		bool npcOneHpEnabled = false;

		void __fastcall HeroInitDetour(void* const self, void* /*edx*/)
		{
			oHeroInit(self);
			if (self)
			{
				g_hero = self;
				heroVtable = *reinterpret_cast<void**>(self);
			}
		}

		// Fires on every hit to any actor. For non-47 actors, swap the real damage for a
		// lethal amount so a single hit kills. Gated on the toggle and on having tagged 47,
		// and 47 is matched by both pointer and vtable, so the player can never be one-shot.
		double __fastcall ApplyDamageDetour(void* const self, void* /*edx*/, const float damage)
		{
			if (npcOneHpEnabled
				&& self
				&& g_hero
				&& damage > 0.0f
				&& self != g_hero
				&& *reinterpret_cast<void**>(self) != heroVtable)
			{
				return oApplyDamage(self, 1000000.0f);
			}
			return oApplyDamage(self, damage);
		}
	}

	void SetCheatsMenuEnabled(const bool enabled) noexcept
	{
		WriteBoolGlobal(offsets::bCheatsMenuEnabled, enabled);
	}

	void SetGodMode(const bool enabled) noexcept
	{
		WriteBoolGlobal(offsets::bGodMode, enabled);
	}

	void SetInvisibleMode(const bool enabled) noexcept
	{
		WriteBoolGlobal(offsets::bInvisibleMode, enabled);
	}

	bool IsInfClips() noexcept
	{
		return ReadBoolGlobal(offsets::bInfClips);
	}

	void SetInfClips(const bool enabled) noexcept
	{
		WriteBoolGlobal(offsets::bInfClips, enabled);
	}

	bool IsInfAmmo() noexcept
	{
		return infAmmoEnabled;
	}

	void SetInfAmmo(const bool enabled) noexcept
	{
		if (enabled == infAmmoEnabled)
		{
			return;
		}

		if (enabled)
		{
			if (!infAmmoOriginalSaved)
			{
				if (!ReadCode(offsets::fnGetWeaponAmmo, infAmmoOriginal, kInfAmmoPatchSize))
				{
					return;
				}
				infAmmoOriginalSaved = true;
			}

			if (PatchCode(offsets::fnGetWeaponAmmo, kInfAmmoPatch, kInfAmmoPatchSize))
			{
				infAmmoEnabled = true;
			}
		}
		else if (infAmmoOriginalSaved)
		{
			if (PatchCode(offsets::fnGetWeaponAmmo, infAmmoOriginal, kInfAmmoPatchSize))
			{
				infAmmoEnabled = false;
			}
		}
	}

	void InstallActorTracking() noexcept
	{
		const std::uintptr_t base = GetModuleBase();
		if (!base)
		{
			return;
		}

		void* const heroInit = reinterpret_cast<void*>(base + offsets::fnHeroInit);
		if (MH_CreateHook(heroInit, &HeroInitDetour, reinterpret_cast<void**>(&oHeroInit)) == MH_OK
			&& MH_EnableHook(heroInit) == MH_OK)
		{
			heroInitTarget = heroInit;
		}

		void* const applyDamage = reinterpret_cast<void*>(base + offsets::fnApplyDamage);
		if (MH_CreateHook(applyDamage, &ApplyDamageDetour, reinterpret_cast<void**>(&oApplyDamage)) == MH_OK
			&& MH_EnableHook(applyDamage) == MH_OK)
		{
			applyDamageTarget = applyDamage;
		}
	}

	void RemoveActorTracking() noexcept
	{
		if (heroInitTarget)
		{
			MH_DisableHook(heroInitTarget);
			heroInitTarget = nullptr;
		}
		if (applyDamageTarget)
		{
			MH_DisableHook(applyDamageTarget);
			applyDamageTarget = nullptr;
		}
	}

	bool IsNpcOneHp() noexcept
	{
		return npcOneHpEnabled;
	}

	void SetNpcOneHp(const bool enabled) noexcept
	{
		npcOneHpEnabled = enabled;
	}

	bool IsInvisibleMode() noexcept
	{
		return ReadBoolGlobal(offsets::bInvisibleMode);
	}
}
