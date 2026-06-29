#include "actions.hpp"

#include "memory.hpp"
#include "offsets.hpp"

#include <cstdio>

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

		void DebugLog(const char* message) noexcept
		{
			OutputDebugStringA("[GiveAllItems] ");
			OutputDebugStringA(message);
			OutputDebugStringA("\n");
		}

		void DebugLogPtr(const char* label, const void* ptr) noexcept
		{
			char buffer[128]{};
			sprintf_s(buffer, "%s = %p", label, ptr);
			DebugLog(buffer);
		}

		void DebugLogStr(const char* label, const char* value) noexcept
		{
			char buffer[256]{};
			sprintf_s(buffer, "%s = \"%s\"", label, value ? value : "(null)");
			DebugLog(buffer);
		}

		void DebugLogVal(const char* label, const unsigned int value) noexcept
		{
			char buffer[128]{};
			sprintf_s(buffer, "%s = 0x%08X", label, value);
			DebugLog(buffer);
		}

		// The game's MSG_GIVEALLITEMS handler has a use-after-free: it iterates items and
		// string-compares each item's name, but after a map change one of those name
		// pointers is dangling, so the compare faults in MSVCR71!memcmp (confirmed from the
		// crash log - it dies inside execute(), not in our code). We can't hand the game a
		// valid pointer, so isolate the dispatch behind SEH: if it faults, swallow the
		// access violation and keep the game alive instead of letting it crash. Kept in its
		// own helper with no C++ objects so __try/__except is legal (C2712).
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
		DebugLog("==== GiveAllItems begin ====");

		const uintptr_t base = GetModuleBase();
		if (!base)
		{
			DebugLog("module base is null");
			return false;
		}
		DebugLogPtr("module base", reinterpret_cast<const void*>(base));

		void* const handler = GetCheatHandler();
		if (!handler)
		{
			DebugLog("handler is null");
			return false;
		}

		DebugLogPtr("handler", handler);
		DebugLogPtr("handler vtable", *reinterpret_cast<void**>(handler));

		// Give All (sub_65AD50) has no in-mission guard of its own, unlike Complete Level
		// (sub_65ACB0) which bails when RootManager+0xA48 is null. Outside an active mission
		// that object is null, the player/inventory doesn't exist, and the give-items
		// dispatch faults. Gate on the same object so we no-op instead of crashing.
		auto** const rootSlot = Global<void*>(offsets::pRootManager);
		void* const root = rootSlot ? *rootSlot : nullptr;
		if (!root)
		{
			DebugLog("root manager is null");
			return false;
		}

		void* const mission = *reinterpret_cast<void**>(
			reinterpret_cast<uintptr_t>(root) + offsets::kMissionOffset);
		if (!mission)
		{
			DebugLog("no active mission - skipping give all to avoid crash");
			return false;
		}

		DebugLogPtr("mission", mission);

		auto** contextManagerSlot = Global<void*>(offsets::pContextManager);
		if (!contextManagerSlot || !*contextManagerSlot)
		{
			DebugLog("context manager is null");
			return false;
		}

		void* const contextManager = *contextManagerSlot;
		DebugLogPtr("contextManager", contextManager);

		auto** contextSlot = reinterpret_cast<void**>(
			reinterpret_cast<uintptr_t>(contextManager) + offsets::kContextOffset);
		if (!contextSlot || !*contextSlot)
		{
			DebugLog("context object is null");
			return false;
		}

		void* const context = *contextSlot;
		DebugLogPtr("context", context);

		auto** const contextVtable = reinterpret_cast<void***>(context);
		if (!contextVtable || !*contextVtable)
		{
			DebugLog("context vtable is null");
			return false;
		}
		DebugLogPtr("context vtable", *contextVtable);

		const auto lookup = reinterpret_cast<MsgLookupFn>(
			(*contextVtable)[offsets::kVtableLookup / sizeof(void*)]);
		DebugLogPtr("lookup fn", reinterpret_cast<const void*>(lookup));

		const auto* const msgId = reinterpret_cast<const char*>(base + offsets::strGiveAllItems);
		const auto* const file = reinterpret_cast<const char*>(base + offsets::strCheatMenuCpp);
		DebugLogStr("msgId", msgId);
		DebugLogStr("file", file);

		// The crash is MSVCR71!memcmp (a key/name comparison on a bad pointer) inside one
		// of the two game calls below. Whichever marker is the LAST line in DebugView
		// before the fault is the culprit: "calling lookup" -> message registry;
		// "calling execute" -> give-items handler. Compare the pointer values logged on a
		// working map vs the crashing map - a vtable/context that turns to garbage on the
		// second map is the use-after-free.
		DebugLog("calling lookup...");
		const int msg = lookup(
			context,
			msgId,
			0,
			file,
			static_cast<int>(offsets::kMsgLine),
			0,
			0);
		DebugLogVal("lookup returned msg", static_cast<unsigned int>(msg));

		if (!msg)
		{
			DebugLog("lookup returned 0 (message not found) - skipping execute");
			return false;
		}

		auto** const handlerVtable = reinterpret_cast<void***>(handler);
		if (!handlerVtable || !*handlerVtable)
		{
			DebugLog("handler vtable is null");
			return false;
		}

		const auto execute = reinterpret_cast<CheatExecFn>(
			(*handlerVtable)[offsets::kVtableExecute / sizeof(void*)]);
		DebugLogPtr("execute fn", reinterpret_cast<const void*>(execute));

		DebugLog("calling execute (MSG_GIVEALLITEMS)...");
		if (TryExecuteGiveAll(execute, handler, reinterpret_cast<const char*>(msg)))
		{
			DebugLog("==== GiveAllItems done OK ====");
		}
		else
		{
			DebugLog("==== execute FAULTED (game UAF) - caught via SEH, game kept alive ====");
		}

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
