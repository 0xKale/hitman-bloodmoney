#pragma once

namespace game
{
	void SetCheatsMenuEnabled(const bool enabled) noexcept;
	void SetGodMode(const bool enabled) noexcept;
	void SetInvisibleMode(const bool enabled) noexcept;
	bool IsInfClips() noexcept;
	void SetInfClips(const bool enabled) noexcept;
	bool IsInfAmmo() noexcept;
	void SetInfAmmo(const bool enabled) noexcept;
	bool IsNpcOneHp() noexcept;
	void SetNpcOneHp(const bool enabled) noexcept;
	void InstallActorTracking() noexcept;
	void RemoveActorTracking() noexcept;
	bool IsInvisibleMode() noexcept;
}
