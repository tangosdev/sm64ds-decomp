# Naming Lab

Play the port and see, live, which functions each actor runs, how its fields
change, which files it loads, and what its textures look like. Then name things
and hot-patch functions to test what they do, while the game keeps running.

**The port only proposes.** Everything the lab shows is how the *port* behaves.
That is good evidence for a guess and is never proof of what the original game
does. Every note the lab writes says `evidence_source: port (naming lab): a
candidate until the ROM agrees`. A name lands in `src/` or `include/` only after
the ROM agrees: a melonDS recording, or the ROM bytes and RTTI.

## Run it

```bat
port\build-port-trace.cmd                       :: the /Gh traced build -> build\port-trace
port\mods_api\naming_lab\build.cmd              :: the plugin -> build\port-trace\mods\naming-lab
naming-lab.cmd                                  :: Bob-omb Battlefield; naming-lab.cmd <level id>
```

The server prints the dashboard address and writes it to
`%USERPROFILE%\sm64ds-lab\naming-lab\dashboard-url.txt`. The address includes
this run's access token. Open it in a browser next to the game window.

`naming-lab.cmd` presses A three times just after boot (`SM64DS_HOST_PAD`) to
close the level's star-name dialog, because no actor runs its `Behavior` while
the dialog is open. When you play by hand, just press A yourself.

## What you get

| panel | what it shows | where it comes from |
|---|---|---|
| Actors | every live actor, its class and its distance from the player; **tp** teleports Mario next to it | the plugin walks the actor list; the class comes from the vtable (MSVC `??_7` symbols or the ROM `_ZTV`) |
| Timeline | 300 frames of every function that ran with the focused actor as `this` (or as the first argument of the `extern "C"` shims), coloured by its own class, base classes, or other | `_penter` lab mode in `port/hal/fn_trace.cpp` (4 words per call: callee, call site, ecx, arg0) |
| Function info | port signature, **ROM address and symbol**, port address, a link to the source file | `walk_window.symtab` and `config/**/symbols.txt`; a thiscall body inherits the ROM address of its `__ZN` shim |
| Fields | the class layout from `include/` (`/* 0xNNN */` offsets, base chain, `unk_` gaps), live values, change counts, "changing only"; double-click a value to poke it. A field this tree still calls `unk_` but `origin/main` has named since is shown under main's name, in green italics | `peek`/`poke` through the plugin; main's names come from `git show origin/main:include/...` (`--names-from <rev>`, `""` to turn it off), matched by class and offset |
| Fields → **who?** | every function that wrote that field during one second of play, with the two callers above it, and the ROM address of each | a CPU data breakpoint (DR0–DR3, set by the plugin on the game thread) and a vectored exception handler that records the writing instruction and walks the frame-pointer chain. Only the game's own writes stop it, so the game runs at full speed |
| Hot | the game's busiest functions (port-only code is hidden) | `hot N` sampling |
| Assets | every file loaded, the class that owns it, the function that asked for it, and texture thumbnails for `.bmd`; a filter box | an entry hook on each `LoadFile` (Model, SharedFilePtr, TextureSequence, dBgW_Kc, dExtFrameCtrl_c), independent of the trace buffer, so a level load is complete (Bob-omb Battlefield: 184 files, 180 in the load frame, 0 unresolved). File ids resolve through ov0's handle table and the 13 NARC archive ranges (`data_0208ecf4`), both read from the running game. Textures are decoded locally from `extracted/` and never stored |
| Names | proposals (field / function / class / asset) and a "queue for ROM verify" list | `~/sm64ds-lab/naming-lab/*.jsonl`, outside the repo |

Pause, step 1 and step 10 freeze the game between frames so you can read a
state.

## Who writes this field?

Click **who?** on any field row. On Bob-omb Battlefield, Mario standing next to
a Goomba, one second each:

| field | writer ← caller ← caller | frames |
|---|---|---|
| `mPosX` (+0x5c) | `AddVec3` ← `dActor_c::UpdatePos` ← `daKrb_c::Behavior` | 28 of 29 |
| | `dActor_c::UpdatePos` ← `daKrb_c::Behavior` ← `fBase_c::Process` | 28 |
| | `dBgCh_Actr::UpdateDiscreteNoLava` ← `dEnemyBase_c::UpdateWMClsn` ← `daKrb_c::Behavior` | 3 |
| `unk_0a4`, `unk_0ac` | `dActor_c::UpdatePos` only | 31 of 32 |
| `unk_1f4` | `dBgPi::RecordHit` ← `dBgW_Kc::DetectClsn` ← `dBgCh_SphCrr` | 31 |

A gap that only `UpdatePos` writes is probably part of a motion vector. A gap
that `RecordHit` writes is collision state. These are leads for the ROM to
settle, not names.

The first lead was checked against the ROM. `origin/main` already names 0x0a4
and 0x0ac `mSpeedX` and `mSpeedZ`. The byte-matched
`dActor_c::UpdatePosWithHorzSpeedAndAng` (ROM 0x02010c5c), which
`UpdatePos` (0x02010c30) calls, stores both. The lab branch was cut before that
naming landed, which is why the dashboard now overlays main's names.

Four fields can be watched at once (the CPU has four debug registers), and each
must be 1, 2 or 4 bytes, aligned. The server arms the breakpoint, waits, reads
the hits and clears the breakpoint again, so nothing stays armed. Raw commands:
`wwatch <addr> [len]`, `writers`, `wwatch clear`.

## Hot-patching a function (command line)

```bat
python port\mods_api\naming_lab\patch.py find daKrb_c::Be
python port\mods_api\naming_lab\patch.py new daKrb_c::Behavior -o my_patch.c
python port\mods_api\naming_lab\patch.py apply daKrb_c::Behavior my_patch.c
python port\mods_api\naming_lab\patch.py undo daKrb_c::Behavior        :: or: undo all
```

`new` writes a C skeleton with the right calling convention: a thiscall member
becomes `__fastcall(self, edx, ...)`, and an `extern "C"` shim becomes
`__cdecl(self, ...)`. Each parameter is a one-word placeholder annotated with
its real type, and the skeleton warns when a parameter is not one word.
`apply` compiles the file with MSVC x86 into `build\naming_lab\patches\`. The
plugin then replaces the function's `call _penter` (5 bytes) with a `jmp` to
your `lab_patch`. It passes `orig` (entry + 5, the rest of the real body), so a
patch can run code before or after the original, or replace it. The DLL is
loaded from a temp copy, so you can edit, rebuild and apply again while the
game runs.

`examples/krb_freeze.c` skips every Goomba's `Behavior`. Measured on Bob-omb
Battlefield with Mario teleported next to a Goomba, over 1.5 s windows:

| | Goombas whose position moved | words changed per Goomba (first 0x400 bytes) |
|---|---|---|
| before | 2 of 15 | 28 and 39 for the two near Mario, 3–4 for the rest |
| patched | 0 of 15 | 3 for every Goomba |
| after undo | 2 of 15 | 45 and 23, then 3–4 |

The 3 words that still change are written outside `Behavior`. Finding that
split is what the patch is for.

## Security

The server is for one person on one machine.

- It binds 127.0.0.1 only, and the plugin's UDP port is 127.0.0.1 only.
- Every API call needs the per-run token, sent as a header or `?token=`.
- The `Host` header must be `127.0.0.1:<port>` or `localhost:<port>`, and any
  `Origin` must match (DNS-rebinding and CSRF guard). POST bodies must be JSON,
  at most 1 MB.
- The commands the dashboard may forward to the game are whitelisted.
- **Compiling and loading code never goes through HTTP.** `patch.py` is a
  separate command you run yourself. The dashboard reads, watches, pokes
  fields and steps frames.
- Notes are written under `~/sm64ds-lab/naming-lab/`, never into the repo.
  Texture PNGs exist only in HTTP responses. Nothing from the ROM is copied
  anywhere.

## Pieces

| file | role |
|---|---|
| `naming_lab.c` | the plugin (mod API): UDP command loop, actor list, watch/peek/poke, data breakpoints (`wwatch`), lab-mode trace drain, focus filter, hot counts, asset-loader hooks, tp/pause/step, detours |
| `lab_server.py` | stdlib HTTP + SSE server. Symbols (MSVC undecoration via dbghelp, Itanium via `tools/demangle.py`), ROM symbols, header layouts, asset resolution, notes |
| `lab.html` | the dashboard (one file, no build step) |
| `patch.py` | the hot-patch CLI |
| `bmd.py`, `narc.py` | BMD texture decoding (LZ77, all 7 NDS formats) and NARC member names and bytes |
| `probe.py` | the smallest UDP client: `python probe.py status actors` |

## Known limits

- The trace buffer holds 4M calls a frame, and a level load exceeds that
  (`status`: `maxcalls=4194304@1 dropframes=1`). The timeline, hot counts and
  focus hits for that one frame are therefore cut short. Assets are not: the
  loader hooks hand each call back to `_penter` and record it themselves.
  During the load frame the instance column falls back to the caller's class,
  because the trace that would name the running actor is full.
- A patch's parameters are placeholders. Doubles, 64-bit values and structs
  passed by value need hand-written types, and `new` flags them.
- **who?** names the callers by walking `ebp`. A function built without a
  frame pointer is skipped in that chain, so a caller can be one level higher
  than it looks. The writer itself (the faulting instruction) is always exact.
  Writes from other threads, and by the kernel (`ReadFile` into a buffer), are
  not seen.
- `orig` is valid only on the traced build, where every function starts with
  `call _penter`. `apply` warns when the first byte is not `e8`.
