#pragma once

#include <Windows.h>
#include <cstdint>

namespace game
{
	inline uintptr_t GetModuleBase() noexcept
	{
		if (const HMODULE gameModule = GetModuleHandleA("hitmanbloodmoney.exe"))
		{
			return reinterpret_cast<uintptr_t>(gameModule);
		}

		return reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr));
	}

	template<typename T>
	T* Global(const uintptr_t rva) noexcept
	{
		const uintptr_t base = GetModuleBase();
		if (!base)
		{
			return nullptr;
		}

		return reinterpret_cast<T*>(base + rva);
	}

	inline bool ReadBoolGlobal(const uintptr_t rva) noexcept
	{
		const auto* value = Global<std::uint8_t>(rva);
		return value && *value != 0;
	}

	inline bool ReadIntGlobal(const uintptr_t rva) noexcept
	{
		const auto* value = Global<int>(rva);
		return value && *value != 0;
	}

	inline void WriteBoolGlobal(const uintptr_t rva, const bool enabled) noexcept
	{
		if (auto* value = Global<std::uint8_t>(rva))
		{
			*value = enabled ? 1 : 0;
		}
	}

	inline void WriteIntGlobal(const uintptr_t rva, const int value) noexcept
	{
		if (auto* slot = Global<int>(rva))
		{
			*slot = value;
		}
	}
}
