#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <thread>

#include "gui/bootstrapper.hpp"
#include "game/hooks.hpp"
#include "project/config.hpp"

DWORD WINAPI MainThread(LPVOID param)
{
	const auto instance = static_cast<HMODULE>(param);

	try
	{
		Setup();
		game::Setup();
		config::TryLoadOnStartup();
	}
	catch (const std::exception& error)
	{
		MessageBeep(MB_ICONERROR);
		MessageBoxA(0, error.what(), "Error", MB_OK | MB_ICONEXCLAMATION);
		goto UNLOAD;
	}

	while (true)
	{
		if (GetAsyncKeyState(VK_END) & 1)
		{
			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

UNLOAD:
	game::Destroy();
	Destroy();

	FreeLibraryAndExitThread(instance, 0);
}

BOOL WINAPI DllMain(const HINSTANCE instance, const DWORD reason, const LPVOID reserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(instance);
		const auto thread = CreateThread(
			nullptr,
			0,
			MainThread,
			instance,
			0,
			nullptr);
		if (thread)
		{
			CloseHandle(thread);
		}
	}

	return TRUE;
}
