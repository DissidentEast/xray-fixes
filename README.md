X-Ray Fixes
==========================
A compilation of fixes for S.T.A.L.K.E.R. Clear Sky from multitude of other projects. Source code based on ForserX's VS 2022 X-Ray port.

## What's beyond the scope of the project
* New features
* Modding resources
* Quality of Life changes
* Refactoring
* Optimizations
* Fixes for bugs not reproducable in vanilla game

## Fixes ported

### Crash fixes
* Fixed a crash upon trying to drop an item from a container or enemy's inventory using G key. ([original commit](https://github.com/ixray-team/ixray-1.5-stcs/commit/157d9a49950a8c1d94ecff537bc3818aa8ad32e1) by [Hrusteckiy](https://github.com/Hrusteckiy) for [IX-Ray 1.5](https://github.com/ixray-team/ixray-1.5-stcs))

### Gameplay fixes
* Fixed inability to trade with NPCs that have supposedly infinite amount of money when the value of the sold items exceeded NPCs max money value. 
* Fixed traders paying slightly more than expected for broken weapons and outfits due to the non-linearization of the formula being done after setting its limits instead of before, effectively changing the minimum value.
* Fixed NPCs accuracy per rank value being capped at the rank value of 100 rather than 1000 (a remnant from S.T.A.L.K.E.R.: Oblivion Lost era rank system), resulting in plateau in the function due to all NPCs with rank higher than a hundred, including most novices, and all experienced stalkers, veterans and masters using the max rank value. 
* Fixed NPCs accuracy per rank value between 78 and 99 being higher than the same value even for more experienced NPCs, reaching up to a 100x increase at the rank value of 89. This was caused by a mistake in linear interpolation formula resulting in the dispersion going from negative to positive values and coming close to zero. In practice, this means that some NPCs with very specific rank values were more accurate than intended, such as Agroprom swamp zombies having 10x lower dispersion and thus 10x higher accuracy.

### Graphical fixes
* Fixed visual artifacts occasionally occuring during the rain. ([original commit](https://github.com/OpenXRay/xray-15/commit/b8fd8b925c8593e75addd158deed327c0aff8d0b) by [Xottab_DUTY](https://github.com/Xottab-DUTY) for [OpenXRay 1.5](https://github.com/OpenXRay/xray-15/))
* Fixed the sun not properly moving from west to east on dynamic lighting due to a mistake in the formula. ([original commit](https://github.com/abramcumner/xray15/commit/42737bfa3e8a3d3df9b927ce34ffa91a27a14faf) by [abramcumner](https://github.com/abramcumner/) for [abramcumner's X-Ray 1.5](https://github.com/abramcumner/xray15/))
* Fixed pistol holding animation continuing being played after dropping the gun while holding a detector. ([original commit](https://github.com/ixray-team/ixray-1.5-stcs/commit/610265d70aae77e83a1f28f40df0d9884630fd38) by [Drombeys](https://github.com/Drombeys/) for [IX-Ray 1.5](https://github.com/ixray-team/ixray-1.5-stcs))
* Fixed player model ressetting to default leather jacket upon any other suit entering player's inventory. ([original commit](https://github.com/OpenXRay/xray-15/commit/4e570d4eaef710625f738d29bbadf78eb930f710) by [Xottab_DUTY](https://github.com/Xottab-DUTY) for [OpenXRay 1.5](https://github.com/OpenXRay/xray-15/))

### Audio fixes
* Fixed controller roars persisting into loaded savegames. ([original commit](https://github.com/OpenXRay/xray-15/commit/3fec6f648c3a4de558a01a89cc4304ce95c4e920) by [Decane](https://github.com/Decane) for [OpenXRay 1.5](https://github.com/OpenXRay/xray-15))

### UI fixes
* Fixed Shock protection being mislabeled as Strike in outfit protection values. ([original commit](https://github.com/ixray-team/ixray-1.5-stcs/commit/4372f70fa5d9e3c18c0ce1d848bf52063fbabdf0) by [Hrusteckiy](https://github.com/Hrusteckiy) for [IX-Ray 1.5](https://github.com/ixray-team/ixray-1.5-stcs))
* Fixed map spots for squads, quests, important NPCs and the player being stretched and occasionally misplaced on 16:9 aspect ratio. ([original commit](https://github.com/ixray-team/ixray-1.5-stcs/commit/7a5de7a6cd3cbfffa10b17dd6d778fd07bfba01c) by [Hrusteckiy](https://github.com/Hrusteckiy) for [IX-Ray 1.5](https://github.com/ixray-team/ixray-1.5-stcs))
* Fixed outfit bullet resistance stat in outfit protection values always showing the percentage for fully repaired outfit even when it's not fully applied. ([original commit](https://github.com/ixray-team/ixray-1.5-stcs/commit/fad3b1c6ffd58135a58e03956abed6444c7a91d4) by [Hrusteckiy](https://github.com/Hrusteckiy) for [IX-Ray 1.5](https://github.com/ixray-team/ixray-1.5-stcs))
* Fixed red hue being applied to all NPCs in trade/upgrade window after searching a body of a dead NPC, as well as red hue being occasionally applied to Scar after dying with inventory screen open. ([original commit](https://github.com/ixray-team/ixray-1.5-stcs/commit/1e2dfdbbb2b8cd6bf1306120e2ece13b08923239) by [Hrusteckiy](https://github.com/Hrusteckiy) for [IX-Ray 1.5](https://github.com/ixray-team/ixray-1.5-stcs))
* Fixed weapon addons being misaligned when the weapon is placed into a vertical inventory slot. ([original commit](https://github.com/ixray-team/ixray-1.5-stcs/commit/57caea76d24888741ed97bc113586fe4e1504755) by [Drombeys](https://github.com/Drombeys) for [IX-Ray 1.5](https://github.com/ixray-team/ixray-1.5-stcs))

## Installation
1. Paste *bin* into the game's root directory, overwrite files.

## Building from source (CMake)
Requires Visual Studio 2022 (v143 toolset) with C++ and MFC/ATL components.
Both **32-bit** (the original shipping architecture) and **64-bit** builds are
supported; the legacy *engine/engine.sln* is left untouched and keeps working
(32-bit only, with the original LuaJIT 1.1.4 sources).

```bat
cmake -S . -B build     -G "Visual Studio 17 2022" -A Win32
cmake -S . -B build-x64 -G "Visual Studio 17 2022" -A x64
cmake --build build     --config Release
cmake --build build-x64 --config Release
```

Binaries land in *build/bin/Release* and *build-x64/bin/Release* (import libs
in *build*/lib/Release*). `Debug` and `Mixed` configs work the same way
(`--config Debug|Mixed`). `CMakePresets.json` provides ready-made presets,
including dedicated-server variants (`-DXRAY_DEDICATED_SERVER=ON`).

The build covers the game runtime plus the `utils/` tools (`xrAI`,
`xrCompress`, `xrDXT`, `ETools`, …) on Win32; disable with
`-DXRAY_BUILD_UTILS=OFF`. The utils are Win32-only (MFC front-ends + 32-bit-era
assembly) and are skipped automatically in x64 builds. Four tool projects are
skipped with a configure-time message because their third-party/editor
dependencies don't exist in the repo (same failure in `engine.sln`): `xrLC`/
`xrLC_Light` (need NVIDIA `nvDXTlib`) and `xrDO_Light`/`xrAI` (need the
dropped `editors/` tree).

To install straight into a game folder, point `CMAKE_INSTALL_PREFIX` at the
game root when configuring, then run `cmake --install`:

```bat
cmake -S . -B build -G "Visual Studio 17 2022" -A Win32 -DCMAKE_INSTALL_PREFIX="C:\Games\S.T.A.L.K.E.R. - Clear Sky"
cmake --build build --config Release
cmake --install build --config Release
```

This copies the built `.dll`/`.exe` files into the game's `bin/` directory.
Every push/PR is also built on GitHub Actions (see `.github/workflows`).

### Notes on the 64-bit build
* **Scripting**: LuaJIT is upgraded from 1.1.4 (x86-only) to **LuaJIT 2.1**
  (GC64 mode on x64) for *both* architectures. It is a drop-in Lua 5.1 API —
  mod scripts are unaffected. The DLL keeps the historical
  `lua.JIT.1.1.4.dll` name; the original 1.1.4 sources remain in
  `3rd party/luajit` for the legacy solution. The deprecated Lua 5.0/5.1
  aliases the game scripts rely on (`string.gfind`, `math.mod`,
  `table.setn`) are restored by the engine at script-system startup.
* **Physics materials**: some level geometry references material indices past
  the 60 entries in the shipped `gamemtl.xr` (e.g. 12312). Win32 silently read
  heap garbage for these; x64 detects the out-of-range index, logs it (at most
  three times per session) and substitutes the default material.
* **Crash reporting**: the BugTrap-based crash dialog is 32-bit-only code; x64
  builds use the simpler built-in handler and write minidumps through
  Windows Error Reporting instead.
* **Deploying x64 binaries**: because the renderers bind to `d3dx9_37.dll` /
  `d3dx10_37.dll`, the 32-bit copies of those DLLs shipping in the game's
  `bin/` folder must be removed (or replaced by 64-bit ones from the DirectX
  end-user runtime), otherwise the loader picks up the wrong architecture.
* `Mixed` builds everything except *xrGame.dll*: its link needs the
  non-virtual `CEnvironment` interface while the Mixed *xrEngine* (built with
  `INGAME_EDITOR`) only exports the virtual one. The stock *.sln* fails the
  same way; use `Release` (or `Debug`) for a complete build.

## Known issues
See: [FunXRay/xray-csky](https://github.com/FunXRay/xray-csky)'s readme.
