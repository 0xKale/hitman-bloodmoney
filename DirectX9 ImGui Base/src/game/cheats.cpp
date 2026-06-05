#include "cheats.hpp"

#include "memory.hpp"
#include "offsets.hpp"

namespace game
{
	void SetCheatsMenuEnabled(const bool enabled) noexcept
	{
		WriteBoolGlobal(offsets::bCheatsMenuEnabled, enabled);
	}

	void SetGodMode(const bool enabled) noexcept
	{
		WriteIntGlobal(offsets::bGodMode, enabled ? 1 : 0);
	}

	bool IsInfAmmo() noexcept
	{
		return ReadBoolGlobal(offsets::bInfAmmo);
	}

	void SetInfAmmo(const bool enabled) noexcept
	{
		WriteBoolGlobal(offsets::bInfAmmo, enabled);
	}

	bool IsInvisibleMode() noexcept
	{
		return ReadBoolGlobal(offsets::bInvisibleMode);
	}
}
