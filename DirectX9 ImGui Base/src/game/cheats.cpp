#include "cheats.hpp"

#include "memory.hpp"
#include "offsets.hpp"

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

	bool IsInvisibleMode() noexcept
	{
		return ReadBoolGlobal(offsets::bInvisibleMode);
	}
}
