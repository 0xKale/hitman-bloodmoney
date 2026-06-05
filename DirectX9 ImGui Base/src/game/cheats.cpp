#include "cheats.hpp"

#include "memory.hpp"
#include "offsets.hpp"

#include <cstdint>

namespace game
{
	namespace
	{
		void WriteBoolGlobal(const uintptr_t rva, const bool enabled) noexcept
		{
			std::uint8_t* value = Global<std::uint8_t>(rva);
			if (!value) {
				return;
			}

			if (enabled) {
				*value = 1;
			}
			else {
				*value = 0;
			}
		}
	}

	void SetCheatsMenuEnabled(const bool enabled) noexcept
	{
		DWORD* pointerStorage = Global<DWORD>(offsets::cheatMenuPointerRva);
		if (!pointerStorage) {
			return;
		}

		const DWORD pointer = *pointerStorage;
		WriteBoolGlobal(pointer, enabled);
	}
}
