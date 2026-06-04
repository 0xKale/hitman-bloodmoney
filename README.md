# DX9c ImGui Base

C++ DLL template for DirectX 9.0c overlays using **ImGui** and **MinHook**.

## Controls

| Key | Action |
|-----|--------|
| `INSERT` | Toggle menu |
| `END` | Unload DLL |

Menu tabs: **Player**, **Visuals**, **Misc** (theme, accent, save/load config).

## Build

1. Open `DirectX9 ImGui Base.sln` in Visual Studio 2022 (Release \| Win32).
2. Build — output goes to `Build/`.

**Requirements:** DLL, Windows SDK 10, toolset v143, C++20, Multi-Byte, x86. No legacy DirectX SDK install required — headers and `d3d9.lib` come from the Windows SDK.

## Layout

```
src/
  dllmain.cpp
  gui/             — menu, DX9c hook
  gui/pages/       — player, visuals, misc
  game/            — hooks, offsets, structs
  project/         — your code: config (INI), helpers, features
ext/               — imgui, minhook, mINI
```

Put config and your own logic in `src/project/`. Saves to `Corrupted.ini` next to the game exe.

Inject the compiled .dll into a DirectX 9.0c process.

![Screenshot](https://i.ibb.co/1YjmL9gj/iw3mp-Iw-KVZJb-NN0.jpg)
