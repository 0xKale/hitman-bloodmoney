#include "cheats.hpp"

#include "memory.hpp"
#include "offsets.hpp"

namespace game
{
	void SetCheatsMenuEnabled(const bool enabled) noexcept
	{
		WriteIntGlobal(offsets::bCheatsMenuEnabled, enabled ? 1 : 0);
	}

	void SetGodMode(const bool enabled) noexcept
	{
		WriteIntGlobal(offsets::bGodMode, enabled ? 1 : 0);
	}

	void SetInvisbleMode(const bool enabled) noexcept
	{
		WriteIntGlobal(offsets::bInvisibleMode, enabled ? 1 : 0);
	}

	bool IsInfAmmo() noexcept
	{
		return ReadIntGlobal(offsets::bInfAmmo);
	}

	void SetInfAmmo(const bool enabled) noexcept
	{
		WriteIntGlobal(offsets::bInfAmmo, enabled ? 1 : 0);
	}

	bool IsInfClips() noexcept
	{
		return ReadIntGlobal(offsets::bInfClips);
	}

	void SetInfClips(const bool enabled) noexcept
	{
		WriteIntGlobal(offsets::bInfClips, enabled ? 1 : 0);
	}

	bool IsInvisibleMode() noexcept
	{
		return ReadIntGlobal(offsets::bInvisibleMode);
	}
}
