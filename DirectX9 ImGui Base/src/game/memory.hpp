#pragma once

#include <Windows.h>
#include <cstdint>

namespace game
{
	inline uintptr_t GetModuleBase() noexcept
	{
		return reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr));
	}

	template<typename T>
	T* Global(const uintptr_t rva) noexcept
	{
		const uintptr_t base = GetModuleBase();
		if (!base) {
			return nullptr;
		}

		return reinterpret_cast<T*>(base + rva);
	}
}
