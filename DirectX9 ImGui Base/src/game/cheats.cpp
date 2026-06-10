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

	bool IsInvisibleMode() noexcept
	{
		return ReadBoolGlobal(offsets::bInvisibleMode);
	}
}
