# CTLGAME: camera mode and control bindings move from the F5 menu to settings.json

Branch `port/controls-settings`, based on ad09cd9f5. Worktree C:/tmp/c3-ctlgame.

## What (plain English)

The F5 debug menu could change the camera mode and rebind exactly one thing
(the run button), and only in a window somebody was sitting at. A lobby match
boots straight into VS and never shows that menu. Now the game reads the camera
mode and every gameplay key and pad button out of settings.json at boot, which
the launcher writes and every copy of the game reads, single-player and lobby
alike. A settings.json without any of the new keys behaves exactly like the
program did before.

## The key list (verbatim, for the launcher lane)

Camera:

| key | values | default |
|---|---|---|
| `CameraMode` | `"analog"`, `"freecam"`, `"ds"` (an integer 0/1/2 is accepted too) | `"analog"`, which is what main promoted an interactive run to before this key existed. A selftest ignores it and stays DS-exact. |
| `SwapCameraTurnDirection` | true/false | false, unchanged |

Keyboard, Win32 virtual-key integers (0 = unbound):

| key | default | today's binding it replaces |
|---|---|---|
| `KeyUp` | 87 (0x57, W) | W |
| `KeyDown` | 83 (0x53, S) | S |
| `KeyLeft` | 65 (0x41, A) | A |
| `KeyRight` | 68 (0x44, D) | D |
| `KeyUpAlt` | 38 (0x26, up arrow) | up arrow |
| `KeyDownAlt` | 40 (0x28, down arrow) | down arrow |
| `KeyLeftAlt` | 37 (0x25, left arrow) | left arrow |
| `KeyRightAlt` | 39 (0x27, right arrow) | right arrow |
| `KeyJump` | 32 (0x20, space) | space |
| `KeyAttack` | 88 (0x58, X) | X (the DS B button: punch, kick, Yoshi's tongue) |
| `KeyCrouch` | 17 (0x11, ctrl) | ctrl (the DS R shoulder). NOT 0: crouch has always had a keyboard binding. |
| `KeyRun` | 16 (0x10, shift) | shift. Alias of `RunButtonKey`. |
| `KeyStart` | 13 (0x0d, enter) | enter (scene path only: the minigames' own menus) |
| `KeySelect` | 8 (0x08, backspace) | backspace (scene path only). NOT 0: select has always been on backspace. |

The Alt keys exist because the shipped default has always been "WASD and the
arrows both walk". Either half of a pair may be 0.

Pad, XInput button masks (0 = unbound):

| key | default | today's binding it replaces |
|---|---|---|
| `PadJump` | 4096 (0x1000, A) | A |
| `PadAttack` | 8192 (0x2000, B) | B |
| `PadCrouch` | 0 | none as a BUTTON. The right trigger crouches and still does; it is an axis, not a button mask, so it stays fixed. A PadCrouch button crouches in addition. |
| `PadRun` | 16384 (0x4000, X) | X. Alias of `RunButtonPad`. |
| `PadStart` | 16 (0x0010, START) | START |
| `PadSelect` | 0 | none. BACK (0x0020) opens the debug menu, so Select had no pad binding. Bind it here if wanted. |

Aliases: `RunButtonKey` / `RunButtonPad` keep working. When both spellings
are in the file `KeyRun` / `PadRun` win; when only the old name is there it is
read exactly as before; the in-game rebind row writes BOTH spellings.

Fixed (not bindings): the menu's own navigation (arrows, enter, escape, F5,
d-pad, A, B, BACK), the left stick and d-pad walking, the right stick, bumpers
and right-stick click on the camera, the right trigger crouch, F1 camera cycle.

Not read live. Boot-latched like RunMode; the launcher's dialog should say a
restart is needed, as it does for the Mods keys.

## What changed where

- `port/hal/host_settings.h` / `.cpp`: CameraMode and the 20 binding keys,
  documented in the existing style, each read against its own default, one
  `[settings]` stderr line per non-default value. `host_setting_camera_mode`,
  `host_setting_key(action)`, `host_setting_pad(action)`, the `HOST_KEY_` /
  `HOST_PAD_` enums, `host_setting_save_camera_mode`. `host_setting_save_run`
  now writes both spellings of the run binding through a shared `save_keys`.
- `port/tests/walk_window.cpp`: `host_ds_buttons`, the level loop's walk keys
  and the scene loop's d-pad/start/select read the bound codes through
  `key_act` / `pad_act` (0 never reads down). `cam_mode`'s interactive boot
  promotion is `host_setting_camera_mode()` (selftest still pins CAM_DS; the
  SM64DS_*_CAMERA env knobs still win). The menu camera row persists
  CameraMode; the run rows are unchanged and show the live value. The menu's
  WASD aliases follow the bound walk keys, the arrows stay fixed. The rebind
  capture refuses a key or pad button bound to another action. The key-name
  table covers arrows, enter, escape, nav cluster, numpad, F-keys, backslash,
  and the pad table covers BACK, both stick clicks and the d-pad. A `[keys]`
  line at boot states every binding in words.
- `port/tests/smoke_settings.cpp` + CMake target `smoke_settings`: six cases in
  child processes (defaults, remap with an out-of-range value, CameraMode by
  number, old alias alone, both spellings disagreeing, the save path).
- One deliberate behaviour change with default settings: in a minigame
  (scene path) W/A/S/D now drive the DS d-pad alongside the arrows. Before,
  only the arrows did. The walk keys are one binding with two halves, and a
  player who moves them to IJKL should not find the minigames still want the
  arrows.

## Multiplayer

Read-only check of C:/Users/bmanu/Documents/SM64DSLauncher: `MainForm.cs`
calls `GameRunner.Launch(_bundleDir, ...)` for the single-player Play button
(line ~801) and for the lobby match (line ~609, with the lobby's set/clear env
lists). `GameRunner.Launch` sets `WorkingDirectory = bundleDir` and
`SM64DS_ASSET_ROOT = bundleDir` on both paths. `host_settings.cpp`'s
`find_settings` looks beside the exe first, which is the bundle directory in a
kit. So a lobby launch reads the same settings.json the single-player launch
does; the launcher lane needs to change nothing for these keys to reach a
match. `LobbyLaunch.BuildChildEnv` clears only `SM64DS_COMMS_*` / `SM64DS_VS_*`
names, none of which touch settings.

## Proof

- `port/build-port.cmd`: exit 0 with all pre- and post-link guards (see the
  TEMP note under Unproven).
- `build/port/smoke_settings.exe`: 6 of 6 cases ok.
- Headless: `walk_window.exe` with `SM64DS_LEVEL=1 SM64DS_WINDOW_SELFTEST=40`
  and a settings.json in the working directory setting KeyJump 74, KeyCrouch 0,
  PadJump 32768, CameraMode ds: rc 0, stderr carries
  `[settings] KeyJump 0x4a (default 0x20)`, `[settings] CameraMode ds`, and
  `[keys] ... jump J attack X crouch unbound ...` / `[keys] pad jump pad Y ...`.
  With only `{ "RunButtonKey": 82 }`: `[run] ... run on R / pad X` and
  `[settings] KeyRun 0x52`, so the alias path is live in the real exe.
- Battery (`python port/tools/battery.py --skip-build`, run 1 on the three
  code commits): `battery: ALL GREEN`. 15 smoke exes ok including the new
  `smoke_settings.exe` (6 cases), 84 level and scene selftests ok, default
  boot ok, linkage 9139 (80.7%), ptr_audit 0 unhosted code pointers, shipcfg
  build ok (PORT_ROM_CLEAN, static CRT) and its selftest ok. Log at
  `build/tmp/battery.log` in the worktree.
- Battery run 2, on the final tip (after the one-line `[keys]` log spacing
  fix, rebuilt through build-port.cmd with all guards): `battery: ALL GREEN`
  again, same counts (15 smokes, 84 selftests, linkage 9139, ptr_audit 0,
  shipcfg ok). Log at `build/tmp/battery2.log`.

## Unproven

- No windowed run was driven by a person: that CameraMode "freecam" boots the
  freecam in a real window rests on the numbering being identical (0/1/2 is the
  CAM_ enum on both sides, proven by the getter) rather than on a screenshot.
  Same for pressing a remapped key: key_live is dead under a selftest by
  design, so the headless check proves the binding is loaded and reported, not
  that GetAsyncKeyState is asked for it. The code path is a one-line change
  from the literal it replaced.
- Pad bindings were not exercised on hardware (no controller on the box).
- Two actions on one key is legal and untested beyond the getter.
- Environmental: on this PC a freshly written .exe under %TEMP% is
  unreadable (Permission denied, for as long as it exists), which makes
  `tailjump_guard.py --selftest` fail before the build starts. The build and
  battery here ran with TEMP/TMP pointed at `build/tmp` inside the worktree.
  Nothing in the tree was changed for it; the guard's fixture is what it is.
- `build/assets` needed `python tools/asset_catalog.py generate <rom>` in the
  fresh worktree (the decomp-worktree skill's item 5); files.tsv sha256 matches
  the other worktrees.

## Commands

```
git worktree add C:/tmp/c3-ctlgame -b port/controls-settings ad09cd9f5
# junction extracted, tools/mwccarm, tools/bin, build/objcache; copy build/assets
python tools/asset_catalog.py generate "<rom>.nds"
set TEMP=C:\tmp\c3-ctlgame\build\tmp
set TMP=C:\tmp\c3-ctlgame\build\tmp
port\build-port.cmd
build\port\smoke_settings.exe
python port/tools/battery.py --skip-build
```
