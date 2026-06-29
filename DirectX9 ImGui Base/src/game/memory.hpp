#pragma once

#include <Windows.h>
#include <cstdint>
#include <cstring>

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

	// Copies `size` bytes out of the module's code at `rva` (e.g. to stash the
	// original bytes before patching). Returns false if the module isn't mapped.
	inline bool ReadCode(const uintptr_t rva, void* const out, const std::size_t size) noexcept
	{
		const auto* const src = Global<std::uint8_t>(rva);
		if (!src || !out)
		{
			return false;
		}

		std::memcpy(out, src, size);
		return true;
	}

	// Overwrites `size` bytes of the module's code at `rva`, temporarily flipping
	// the page to RWX. Returns false if the module isn't mapped or VirtualProtect
	// fails. Caller is responsible for stashing/restoring original bytes.
	inline bool PatchCode(const uintptr_t rva, const void* const bytes, const std::size_t size) noexcept
	{
		auto* const dst = Global<std::uint8_t>(rva);
		if (!dst || !bytes)
		{
			return false;
		}

		DWORD oldProtect = 0;
		if (!VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldProtect))
		{
			return false;
		}

		std::memcpy(dst, bytes, size);
		VirtualProtect(dst, size, oldProtect, &oldProtect);
		FlushInstructionCache(GetCurrentProcess(), dst, size);
		return true;
	}
}
