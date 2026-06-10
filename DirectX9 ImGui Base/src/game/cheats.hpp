#pragma once

namespace game
{
	void SetCheatsMenuEnabled(const bool enabled) noexcept;
	void SetGodMode(const bool enabled) noexcept;
	void SetInvisibleMode(const bool enabled) noexcept;
	bool IsInfClips() noexcept;
	void SetInfClips(const bool enabled) noexcept;
	bool IsInvisibleMode() noexcept;
}
