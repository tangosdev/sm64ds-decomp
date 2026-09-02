# Lane PADCAL -- DELIVERY

A press-to-learn controller layout for non-XInput pads. A player whose pad
hit the DirectInput backend's generic guess reported "only dpad controls
though, and the controls in the menu do not reflect what's gonna be pressed
in game: x is a, b is x and a is b... would have preferred if you had to
press a button to choose." Now they can.

* Worktree `C:/tmp/padcal`, branch `port/pad-calibrate` off the cons tip
  `9cf4eb8b8`
* Branch tip: the commit that adds this file; the four code commits are
  listed in section 5
* Nothing under `src/` or `include/` was touched; nothing was merged or
  pushed

---

## 1. What was wrong

`port/hal/pad_backend.cpp` maps a DirectInput pad onto the XInput mask
through a table keyed by USB vendor:product id (Switch Pro, DualShock 4,
DualSense) and a positional guess for everything else: buttons 0..3 are
A B X Y, the left stick is X/Y, the right stick Z/Rz. The reporter's pad
is not in the table and its buttons are not in that order, so the face
buttons came out rotated, and its left stick evidently is not on X/Y, so
only the d-pad (the hat, which the guess got right) walked. There was no
way to correct either short of a new table row in a new build.

## 2. The change

| file | what |
|---|---|
| `port/hal/host_settings.h` / `.cpp` | `HostPadLayout` and a `PadLayouts` array in settings.json, one flat object per vid:pid with the field names of `PadLayout` (`a b x y lb rb back start lthumb rthumb lt_btn rt_btn lt_axis rt_axis lx_axis ly_axis rx_axis ry_axis lx_sign ly_sign rx_sign ry_sign`, plus `vid`, `pid` in decimal and a `name` for humans). Parsed object by object with the file's own readers: an object without a usable vid:pid is dropped, any other missing or out-of-range field reads as the generic fallback's value, a sign of 0 is refused, at most 8 objects. `host_setting_pad_layout(vid, pid, &out)`, `_count`, `_at(i)`, `host_pad_layout_default`, and `host_setting_save_pad_layout` which upserts by vid:pid and rewrites the `PadLayouts` value in place through the existing `save_keys` path, so every other key survives. `json_value_end` now walks an array or object to its matching bracket so that replace works. One `[settings] PadLayouts: learned layout for vvvv:pppp name` line per row at boot. |
| `port/hal/pad_backend.h` / `.cpp` | `PadLayout` grows `lx_axis`, `ly_axis` and four signs (built-in rows carry `0, 1, +1, -1, +1, -1`, which is exactly the old hardwired X/Y with DirectInput's downward Y undone; nothing changes for a table pad). An override table keyed by vid:pid, under the worker's lock, beats the built-in table: seeded from settings.json in `port_pad_init` before the worker starts, and `port_pad_set_layout` installs one live (the worker re-resolves on its next 4 ms pass, no restart). `port_pad_raw` publishes the DirectInput pad's raw button indices (bit i = button i, 32 of them) and the six axes before any layout, with vid:pid, name, presence and a `learned` flag; it answers not-live while an XInput slot is the pad the game hears. The `[pad]` line reads `DirectInput: <name> (vvvv:pppp, learned layout)` when an override is in effect. `port_pad_selftest` pushes a hand-built `DIJOYSTATE2` through the real `di_translate` with a learned row installed. |
| `port/tests/walk_window.cpp` | A `pad layout` row in the F5 menu after `rebind run`. Enter arms the flow only when `port_pad_raw` says a DirectInput pad is live; with an XInput pad or none the row says so and enter refuses in words. Fourteen prompts on the toast, one per frame while it runs: bottom, right, left, top face button, LB, RB, LT, RT, Back, Start, then left stick right, left stick up, right stick right, right stick up. A button step takes the FIRST fresh button index (lowest set bit of the fresh mask); a stick step takes the axis with the largest swing past half travel from a quiet baseline and the sign of that swing; a trigger step also accepts an axis that rested at its minimum and swung up (DualShock-style analog triggers). Escape cancels with nothing saved, backspace skips (that control reads unbound), a repeated answer is refused. The stick clicks keep the generic guess unless a learned button took their index; the d-pad stays the hat. On the last step the layout goes to `port_pad_set_layout` (live at once) and `host_setting_save_pad_layout` (next boot). While it runs the window is deaf the way the rebind capture makes it deaf: `key_live`, the window procedure's key swallow and the mouse capture all test `g_padlearn` beside `g_rebind_capture`, and `padlearn_frame` clears `pad_live` in both frame loops. |
| `port/tests/smoke_settings.cpp` | Seven new cases (section 3). |
| `port/CMakeLists.txt` | `hal/pad_backend.cpp` on the `smoke_settings` target for the translation case. |

### Quiet before a stick step

The baseline a stick deflection is measured from is taken only after all
six axes have held within 3000 of their previous frame for six frames.
That is what stops a stick still travelling back to centre from the
previous step answering the next one with its return swing, and what stops
a trigger that rests at -32767 being read as a push on a stick step.

## 3. How it was proven

* `port/build-port.cmd` from the worktree, every pre-configure guard, the
  build and every post-link guard: exit 0.
* `build/port/smoke_settings.exe`: `smoke_settings: ok, 14 cases`, rc 0.
  The seven new ones:
  * `padlayouts`: two objects parse and answer by vid:pid with every field
    checked; `y` 99 (out of range), `lthumb` "abc" and `rx_sign` 0 each read
    as the fallback while the rest of that object is honoured; an object
    with `vid` 0 and one with no `pid` are dropped; an unknown pad is 0.
  * `padlayouts_save`: a save into a file that already carries one pad
    leaves both; a re-save of the same vid:pid replaces rather than
    appends; out-of-range fields are clamped before the write; a vid of 0
    is refused; the launcher's `Volume` survives; a product name with
    quotes is escaped in the file.
  * `padlayouts_reread`: a fresh process reads that file back to the same
    values, name unescaped, other keys at their defaults.
  * `padlayouts_bad` (twice): `PadLayouts` as a string, and as an array of
    junk, both read as no layouts with `KeyJump` and `CameraMode` beside
    them still parsed.
  * `padtranslate`: `port_pad_selftest`. A learned row with the reporter's
    rotation (index 2 is A, 1 is B, 3 is X, 0 is Y), the left stick on Z
    mirrored and Rz, the right stick on X/Y, RT analog on Rx: the mask,
    both sticks, both signs and the trigger come out right; the same
    report through the generic guess reads index 2 as X (the reported
    bug); the raw view sees indices, not the mask; an absent axis reads 0
    through a learned row too.
* Headless `SM64DS_WINDOW_SELFTEST=30 SM64DS_LEVEL=1` off
  `build/port/walk_window.exe`, rc 0 in all four modes, `[pad]` lines
  verbatim:

      SM64DS_PAD_BACKEND unset:   [pad] XInput slot 0
      SM64DS_PAD_BACKEND=dinput:  [pad] forced: dinput only (SM64DS_PAD_BACKEND)
                                  [pad] none
      SM64DS_PAD_BACKEND=xinput:  [pad] forced: xinput only (SM64DS_PAD_BACKEND)
                                  [pad] XInput slot 0
      SM64DS_PAD_BACKEND=none:    [pad] forced: none (SM64DS_PAD_BACKEND)
                                  [pad] none (forced)

* `dumpbin /symbols` on the rebuilt `pad_backend.cpp.obj`: the undefined
  symbols are kernel32 (`LoadLibraryA`, `GetProcAddress`,
  `GetModuleHandleA`, `CreateThread`, `CloseHandle`, the critical section
  trio, `GetTickCount`, `Sleep`, `GetCurrentProcessId`), the CRT, and the
  three host_settings entry points. No dinput8, xinput or user32 import,
  as the port's rule requires.
* `python port/tools/battery.py --skip-build` from the worktree root,
  exit 0. Summary lines verbatim:

      smoke_settings.exe: ok  smoke_settings: ok, 14 cases
      levels: 50 mounted, from hal/level_boot.cpp      (50 selftest level rows ok)
      scenes: 34 hosted, from hal/scene_boot.cpp       (34 selftest scene rows ok)
      default boot: ok -- a bare launch reaches the TITLE, 300 frames clean, and writes its frame (589,878 bytes, liveness only)
      linkage: 9148 (80.8%)
      ptr_audit: 0 unhosted code pointers
      shipcfg build: ok, walk_window.exe linked in build\port-kit (PORT_ROM_CLEAN, static CRT, 157s)
      shipcfg selftest: ok, rc=0 and walk_window_selftest.bmp written (589,878 bytes, 4s)
      skips: level 27 without TTC_MOVING_BEAM (the decomp), level 45 without GOOMBOSS (the decomp)
      battery: ALL GREEN

  The two skips are the same two CTRL.md carries; they are the decomp's,
  not this lane's.

Environmental note, unchanged from CTRL.md: the build and the battery ran
with `TEMP` and `TMP` set to `C:/tmp/padcal/build/tmp`, because a brand-new
exe under the user's `%TEMP%` is unreadable on this machine for a moment.

## 4. What is NOT proven

* **The learn flow never ran against a real DirectInput pad.** This machine
  has a device XInput seats in slot 0 and nothing DirectInput-only
  attached, so `port_pad_raw` answered not-live in every interactive
  situation reachable here and the fourteen steps were exercised by the
  compiler only. The translation underneath them is proven on a synthetic
  report (`padtranslate`), the file round trip is proven, and the live
  install path (`port_pad_set_layout` then the worker's re-resolve) is the
  same code the seed at boot goes through, but the prompts, the quiet
  baseline, the fresh-button edge and the trigger-axis fallback are
  untested on hardware. THE FIRST THING TO DO WITH A REPORTER is ask them
  to open F5, walk the row, and paste the `[pad] learn N: ...` and
  `[pad] learned layout ...` lines.
* **The quiet thresholds are guesses.** 3000 of 32767 for "still", six
  frames of it, 16000 for "a push". A very noisy stick could never arm; a
  very short-throw one could never trip. Both would show as a stick step
  that never advances, and backspace skips it.
* **A d-pad that is buttons rather than a hat** is not learned; such a pad
  keeps a dead d-pad. The reporter's pad is not one (their d-pad was the
  part that worked).
* **A trigger that rests at centre** (a combined-axis trigger pair) will not
  be taken by the trigger-axis fallback, which requires a rest below
  -20000; press-to-learn takes the button half if there is one, else skip.
* **The launcher** reads settings.json with System.Text.Json. An unknown
  `PadLayouts` array is ignored by its default deserialization, but that
  is from memory of the library, not from running the launcher against
  the new file. If the launcher's own save drops keys it does not know,
  it would drop this one; `host_setting_save_run` and friends in this
  program do not, and that half is tested.
* **Two learned pads with one vid:pid** (two identical cheap pads with
  different firmware) share one row; the second one to be learned wins.
* **`PadLayouts` is not reloaded live** by `host_settings_poll`; the learn
  flow installs its result directly, and a hand edit of the file takes
  effect on the next boot.

## 5. Commits

    a21a2e057 host_settings: a PadLayouts array in settings.json, one learned controller map per vid:pid
    3d649a10d pad_backend: a learned layout beats the built-in table, plus the raw view the learn flow reads
    921c02d85 walk_window: a pad layout learn row in the F5 menu, one press per control
    522af3025 smoke_settings: PadLayouts parse, lookup, bad values, save and re-read, and the pad_backend translation selftest

Binary the battery ran: `build/port/walk_window.exe` built at `522af3025`.
