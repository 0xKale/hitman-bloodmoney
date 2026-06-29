#include "actions.hpp"

#include "memory.hpp"
#include "offsets.hpp"

namespace game
{
	namespace
	{
		using MsgLookupFn = int(__thiscall*)(
			void* ctx,
			const char* msgId,
			int arg2,
			const char* file,
			int line,
			int arg5,
			int arg6);

		using CheatExecFn = void(__thiscall*)(void* handler, const char* msg);
		using CompleteLevelFn = int(__thiscall*)(void* handler);

		bool pendingGiveAllItems = false;
		bool pendingCompleteLevel = false;

		void* GetCheatHandler() noexcept
		{
			auto** rootSlot = Global<void*>(offsets::pRootManager);
			if (!rootSlot || !*rootSlot)
			{
				return nullptr;
			}

			void* const root = *rootSlot;
			auto** handlerSlot = reinterpret_cast<void**>(
				reinterpret_cast<uintptr_t>(root) + offsets::kHandlerOffset);
			if (!handlerSlot || !*handlerSlot)
			{
				return nullptr;
			}

			return *handlerSlot;
		}

		// The game's MSG_GIVEALLITEMS handler has a use-after-free: it iterates items and
		// string-compares each item's name, but after a map change one of those name
		// pointers is dangling, so the compare faults in MSVCR71!memcmp (it dies inside
		// execute(), not in our code). We can't hand the game a valid pointer, so isolate
		// the dispatch behind SEH: if it faults, swallow the access violation and keep the
		// game alive instead of letting it crash. Kept in its own helper with no C++ objects
		// so __try/__except is legal (C2712).
		bool TryExecuteGiveAll(CheatExecFn execute, void* handler, const char* msg) noexcept
		{
			__try
			{
				execute(handler, msg);
				return true;
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				return false;
			}
		}
	}

	bool GiveAllItems() noexcept
	{
		const uintptr_t base = GetModuleBase();
		if (!base)
		{
			return false;
		}

		void* const handler = GetCheatHandler();
		if (!handler)
		{
			return false;
		}

		// Give All (sub_65AD50) has no in-mission guard of its own, unlike Complete Level
		// (sub_65ACB0) which bails when RootManager+0xA48 is null. Outside an active mission
		// that object is null, the player/inventory doesn't exist, and the give-items
		// dispatch faults. Gate on the same object so we no-op instead of crashing.
		auto** const rootSlot = Global<void*>(offsets::pRootManager);
		void* const root = rootSlot ? *rootSlot : nullptr;
		if (!root)
		{
			return false;
		}

		void* const mission = *reinterpret_cast<void**>(
			reinterpret_cast<uintptr_t>(root) + offsets::kMissionOffset);
		if (!mission)
		{
			return false;
		}

		auto** contextManagerSlot = Global<void*>(offsets::pContextManager);
		if (!contextManagerSlot || !*contextManagerSlot)
		{
			return false;
		}

		void* const contextManager = *contextManagerSlot;

		auto** contextSlot = reinterpret_cast<void**>(
			reinterpret_cast<uintptr_t>(contextManager) + offsets::kContextOffset);
		if (!contextSlot || !*contextSlot)
		{
			return false;
		}

		void* const context = *contextSlot;

		auto** const contextVtable = reinterpret_cast<void***>(context);
		if (!contextVtable || !*contextVtable)
		{
			return false;
		}

		const auto lookup = reinterpret_cast<MsgLookupFn>(
			(*contextVtable)[offsets::kVtableLookup / sizeof(void*)]);

		const auto* const msgId = reinterpret_cast<const char*>(base + offsets::strGiveAllItems);
		const auto* const file = reinterpret_cast<const char*>(base + offsets::strCheatMenuCpp);

		const int msg = lookup(
			context,
			msgId,
			0,
			file,
			static_cast<int>(offsets::kMsgLine),
			0,
			0);
		if (!msg)
		{
			return false;
		}

		auto** const handlerVtable = reinterpret_cast<void***>(handler);
		if (!handlerVtable || !*handlerVtable)
		{
			return false;
		}

		const auto execute = reinterpret_cast<CheatExecFn>(
			(*handlerVtable)[offsets::kVtableExecute / sizeof(void*)]);

		// Dispatch behind SEH so the game's give-items use-after-free can't take the
		// process down (see TryExecuteGiveAll).
		TryExecuteGiveAll(execute, handler, reinterpret_cast<const char*>(msg));
		return true;
	}

	void RequestGiveAllItems() noexcept
	{
		pendingGiveAllItems = true;
	}

	bool CompleteLevel() noexcept
	{
		const uintptr_t base = GetModuleBase();
		if (!base)
		{
			return false;
		}

		void* const handler = GetCheatHandler();
		if (!handler)
		{
			return false;
		}

		const auto completeLevel = reinterpret_cast<CompleteLevelFn>(
			base + offsets::fnCompleteLevel);
		completeLevel(handler);
		return true;
	}

	void RequestCompleteLevel() noexcept
	{
		pendingCompleteLevel = true;
	}

	void Tick() noexcept
	{
		if (pendingGiveAllItems)
		{
			pendingGiveAllItems = false;
			GiveAllItems();
		}

		if (pendingCompleteLevel)
		{
			pendingCompleteLevel = false;
			CompleteLevel();
		}
	}
}
