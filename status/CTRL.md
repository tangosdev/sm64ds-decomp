# Lane CTRL -- DELIVERY

Non-XInput controllers (Nintendo Switch Pro Controller, DualShock 4, DualSense,
generic HID pads) and XInput pads seated in a slot other than 0 now drive the
game.

* Worktree `C:\tmp\c3-ctrl`, branch `port/controllers-generic` off the cons tip
  `ad09cd9f5`
* Branch tip: see the report; this file is the last commit
* Nothing under `src/`, `port/hal/comms_*` or any lobby/net file was touched

---

## 1. What was wrong

`port/tests/walk_window.cpp` loaded XInput dynamically and, at both of its pad
read sites (the scene loop and the level loop), asked `XInputGetState(0, ...)`
and nothing else. Two classes of pad were therefore invisible:

* anything that is not an Xbox-class device to Windows. A Switch Pro
  Controller, a DualShock 4 or a DualSense is a plain HID game controller
  unless Steam Input or a third-party driver is translating it; XInput never
  lists it;
* an Xbox-class pad XInput has seated in slot 1, 2 or 3, which happens after a
  wireless receiver or another pad has come and gone.

## 2. The change

| file | what |
|---|---|
| `port/hal/pad_backend.h` | NEW. `PortPadState` (the exact XInput `XINPUT_STATE` layout, i.e. walk_window's `XPad`), `port_pad_init`, `port_pad_poll`, `port_pad_describe`. The banner is the design. |
| `port/hal/pad_backend.cpp` | NEW. XInput slots 0..3 (first connected wins, slot 0 still first), then a DirectInput8 fallback. Both DLLs `LoadLibraryA` + `GetProcAddress`; `INITGUID` instantiates the DirectInput GUIDs in this TU and the `DIJOYSTATE2` data format is spelled by hand, so no `dinput8.lib`, `dxguid.lib` or `xinput*.lib` is linked. Known layouts for Switch Pro (057e:2009), DualShock 4 (054c:05c4, 054c:09cc), DualSense (054c:0ce6); positional generic fallback (buttons 0..3 = A B X Y, X/Y left stick, Z/Rz right stick, hat = d-pad). XInput-class devices are skipped in the DirectInput pass (`IG_` in the device path). Disconnect drops the pad; reconnect is picked up on a slow rescan cadence (XInput every 60 frames, DirectInput every 120) so an empty slot is never polled per frame. |
| `port/tests/walk_window.cpp` | `struct XPad` + `XInputGetState_` replaced by `#include "hal/pad_backend.h"` and `typedef PortPadState XPad;`; the xinput DLL loop in `winapi_load` replaced by `port_pad_init()`; the two `XInputGetState_(0, &pad) == 0` reads replaced by `port_pad_poll(&pad)`. Nothing else in the file changes: the focus gate, scripted pad, menu, rebind capture and DS keypad mapping read the same struct they always did. The level loop keeps its `!selftest &&` guard, so a headless selftest still never polls a pad. |
| `port/CMakeLists.txt` | `hal/pad_backend.cpp` on both targets that compile `tests/walk_window.cpp` (`walk_window`, `walk_window_hires`). |

Startup prints exactly one line naming what was found, so a bug report says
what happened:

    [pad] XInput slot 0
    [pad] DirectInput: Pro Controller (057e:2009, Switch Pro Controller layout)
    [pad] none

A later change of answer (unplug, replug, a different pad) prints one more line;
nothing prints per frame.

### XInput users are not regressed

Slot 0 is asked first, on the same `XInputGetState` entry point from the same
DLL list (`xinput1_4`, `xinput1_3`, `xinput9_1_0`), and its state is copied
through unchanged. The only behavioural difference for a slot-0 user is that an
unplugged pad's slot is re-asked once a second instead of every frame.

## 3. How it was proven

* `port/hal/pad_backend.cpp` compiles clean at /W3 under the 32-bit toolchain.
  `dumpbin /symbols` on the object lists only kernel32's `LoadLibraryA`,
  `GetProcAddress` and `GetModuleHandleA` as imports: no dinput8, xinput or
  user32 import, as the port's rule requires.
* `port/build-port.cmd` (every pre-configure guard, the build, every post-link
  guard) -- see section 5.
* A direct headless run of the built exe (`SM64DS_WINDOW_SELFTEST=30
  SM64DS_LEVEL=1`) exits 0 and prints `[pad] XInput slot 0` as its second
  stderr line. That is also the finding that limits the proof: THIS MACHINE HAS
  A DEVICE XINPUT SEES IN SLOT 0, so every run here took the XInput arm, which
  is the arm that existed before. The DirectInput arm compiled, linked and
  enumerated nothing that a run could reach; see section 4.
* `python port/tools/battery.py` from the worktree root -- see section 5.

## 4. What is NOT proven

* **The DirectInput arm never ran on a real device.** XInput slot 0 answers on
  this machine, so `port_pad_poll` returned from the XInput arm on every run and
  the DirectInput enumeration, layout tables and translation were exercised by
  the compiler and linker only.
* **No physical pad was tested.** This machine has no Switch Pro Controller,
  DualShock, DualSense or generic HID pad attached, and no XInput pad in a slot
  other than 0. Every layout table is written from the devices' published HID
  button order and from what DirectInput does with it (button index = HID usage
  minus one; hat = POV 0; Y axis positive downward, inverted here). The
  face-button mapping is POSITIONAL (bottom = A, right = B, left = X, top = Y),
  which is what SDL does and what a player expects, but it is unverified on
  hardware.
* **Switch Pro Controller over USB.** Without a handshake the Pro Controller
  sends only a partial HID report over USB; over Bluetooth it reports fully.
  Windows' generic HID driver does not perform that handshake, so a Pro
  Controller may need Bluetooth pairing (or Steam) to report through
  DirectInput. This is a property of the controller, not of this code, but it
  is the first thing to ask a reporter.
* **Right stick on unknown pads.** The generic fallback reads Z/Rz for the right
  stick. Some pads report it on Rx/Ry instead; such a pad would walk and jump
  but not turn the camera on its right stick until a layout row is added.
* **DualShock analog triggers** are read from Rx/Ry OR'd with the L2/R2 buttons.
  The axis half is unverified; the button half alone is enough for the game.
* `SetCooperativeLevel` is asked for background/non-exclusive with whichever
  window is active (or NULL). Its return is ignored on purpose; `Acquire` is
  the real test. Whether a given driver honours background reads is unproven,
  and the caller's own focus gate is what decides who gets the input anyway.

## 5. Exact commands and results

Environmental note, not a change: `port/tools/tailjump_guard.py --selftest`
fabricates a `walk_window.exe` under `%TEMP%` and reads it straight back, and on
this machine that read fails with `PermissionError` every time under the
user's `%TEMP%` (something, almost certainly the antivirus's on-write scan,
holds a brand-new `.exe` there for a moment). It passes when `TEMP` points
inside `C:\tmp`. The guard was not edited; the build and the battery below
were run with `TMP`/`TEMP` set to `C:\tmp\c3-ctrl\build\tmp`.

    git worktree add C:/tmp/c3-ctrl -b port/controllers-generic ad09cd9f5
    (junction extracted, tools\mwccarm, tools\bin, build\objcache per decomp-worktree)
    python tools/asset_catalog.py generate "<ROM>.nds"     # files.tsv sha256 matches the main tree
    set TMP=C:\tmp\c3-ctrl\build\tmp & set TEMP=C:\tmp\c3-ctrl\build\tmp
    port\build-port.cmd
    python port/tools/battery.py

Battery output, verbatim (`python port/tools/battery.py C:	mp\c3-ctrl`, exit 0):

    build: ok
    smoke.exe: ok  smoke: all checks passed (math, Timer, Fader on host)
    smoke_actor.exe: ok  smoke_actor: all checks passed (an actor spawned, initialized, behaved and rendered throug
    smoke_anim.exe: ok  smoke_anim: all checks passed (the game posed and rendered the piano via its own recursive
    smoke_clsn.exe: ok  smoke_clsn: all checks passed (the game's octree walk answers ground queries over real KCL
    smoke_frames.exe: ok  smoke_frames: all checks passed (24 game-shaped frames through the fiber runtime, all with
    smoke_fs.exe: ok  smoke_fs: all checks passed (raw + LZ77 assets loaded through SharedFilePtr on the game he
    smoke_gx.exe: ok  smoke_gx: all checks passed (game DL pump byte-equals the harness path, 492 triangles, 186
    smoke_heap.exe: ok  smoke_heap: all checks passed (2504 allocs, 2496 frees, peak 63 live, 0 full-arena rejecti
    smoke_model.exe: ok  smoke_model: all checks passed (the game loaded, rebased, uploaded and rendered its own mo
    smoke_modelanim.exe: ok  smoke_modelanim: all checks passed (the game advanced, wrapped and re-posed its own animat
    smoke_oam.exe: ok  smoke_oam: all checks passed (the game's sprite engine emits, uploads and scans out on hos
    smoke_objwin.exe: ok  smoke_objwin: ok
    smoke_persist.exe: ok  smoke_persist: all checks passed (wrote disk state, a second process loaded it byte-exact;
    smoke_player.exe: ok  smoke_player: Mario walks on the castle grounds (gates 10+11 GREEN)
    smoke_roots.exe: ok  smoke_roots: all checks passed (root heap up, 1511 allocs, 1489 frees through Memory::Allo
    smoke_savestate.exe: ok  smoke_savestate: all checks passed (world evolved, saved, diverged, restored byte-exact, a
    smoke_sdat.exe: ok  OK
    smoke_soak.exe: ok  soak: 455 models, 455 rendered (89840 tris total), 0 empty, 0 load-fail, 0 faulted
    smoke_soak_anim.exe: ok  anim soak: 473 compatible pairs, 473 animated+rendered, 0 empty, 0 load-fail, 0 faulted
    levels: 50 mounted, from hal/level_boot.cpp
    selftest level 0: ok
    selftest level 1: ok
    selftest level 2: ok
    selftest level 3: ok
    selftest level 4: ok
    selftest level 5: ok
    selftest level 6: ok
    selftest level 7: ok
    selftest level 8: ok
    selftest level 9: ok
    selftest level 10: ok
    selftest level 11: ok
    selftest level 12: ok
    selftest level 13: ok
    selftest level 14: ok
    selftest level 15: ok
    selftest level 16: ok
    selftest level 17: ok
    selftest level 18: ok
    selftest level 19: ok
    selftest level 20: ok
    selftest level 21: ok
    selftest level 22: ok
    selftest level 23: ok
    selftest level 24: ok
    selftest level 25: ok
    selftest level 26: ok
    selftest level 27: ok with SM64DS_SKIP_CLASS=TTC_MOVING_BEAM, owned by the decomp (_ZN14TtcMovingCubeA8BehaviorEv has no matched body) (bare rc=3221226505)
    selftest level 28: ok
    selftest level 29: ok
    selftest level 30: ok
    selftest level 32: ok
    selftest level 33: ok
    selftest level 34: ok
    selftest level 35: ok
    selftest level 36: ok
    selftest level 37: ok
    selftest level 38: ok
    selftest level 39: ok
    selftest level 40: ok
    selftest level 42: ok
    selftest level 43: ok
    selftest level 44: ok
    selftest level 45: ok with SM64DS_SKIP_CLASS=GOOMBOSS, owned by the decomp (func_ov074_02121380 has no matched body) (bare rc=3221226505)
    selftest level 46: ok
    selftest level 47: ok
    selftest level 48: ok
    selftest level 49: ok
    selftest level 50: ok
    selftest level 51: ok
    scenes: 34 hosted, from hal/scene_boot.cpp
    selftest scene 1: ok
    selftest scene 4: ok
    selftest scene 5: ok
    selftest scene 6: ok
    selftest scene 361: ok
    selftest scene 362: ok
    selftest scene 363: ok
    selftest scene 364: ok
    selftest scene 365: ok
    selftest scene 366: ok
    selftest scene 367: ok
    selftest scene 368: ok
    selftest scene 369: ok
    selftest scene 370: ok
    selftest scene 371: ok
    selftest scene 372: ok
    selftest scene 373: ok
    selftest scene 374: ok
    selftest scene 375: ok
    selftest scene 376: ok
    selftest scene 377: ok
    selftest scene 378: ok
    selftest scene 379: ok
    selftest scene 380: ok
    selftest scene 381: ok
    selftest scene 382: ok
    selftest scene 383: ok
    selftest scene 384: ok
    selftest scene 385: ok
    selftest scene 386: ok
    selftest scene 387: ok
    selftest scene 388: ok
    selftest scene 389: ok
    selftest scene 390: ok
    default boot: ok -- a bare launch reaches the TITLE, 300 frames clean, and writes its frame (589,878 bytes, liveness only)
    linkage: 9139 (80.7%)
    ptr_audit: 0 unhosted code pointers
    shipcfg build: ok, walk_window.exe linked in build\port-kit (PORT_ROM_CLEAN, static CRT, 717s)
    shipcfg selftest: ok, rc=0 and walk_window_selftest.bmp written (589,878 bytes, 15s) -- LIVENESS ONLY, not a raster comparison and not compared against the developer build's BMP (see THE SHIPPING CONFIGURATION above)
    skips: level 27 without TTC_MOVING_BEAM (the decomp (_ZN14TtcMovingCubeA8BehaviorEv has no matched body)), level 45 without GOOMBOSS (the decomp (func_ov074_02121380 has no matched body))
    battery: ALL GREEN

Binary the battery ran: `build/port/walk_window.exe` off branch tip commit 2 of
this lane (the code commits); this file is commit 3 and changes no source.

