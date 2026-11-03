# tools/trace — emu-trace pipeline

Runtime-trace tooling for the emulator-assisted matching/labeling plan.
Strategy: `notes/emu-trace-plan.md`. Build spec: `notes/emu-trace-build.md`.

Status: **Phase 0 spike** (attach + breakpoint + canary + register/mem dump).

## Files
- `rsp.py` — minimal GDB Remote Serial Protocol client over a TCP socket (no
  gdb dependency). Set/clear breakpoints, read registers/memory, continue.
- `symindex.py` — addr→symbol index from `config/**/symbols.txt` + `nearmiss/db.jsonl`,
  used to resolve a captured `lr` to a caller name.
- `gdb_harness.py` — the Phase-0 spike runner. Breaks at always-resident arm9
  functions and validates the pipeline against ground truth.
- `mg/` — the minigame play kit: a watcher for the shared minigame functions,
  plus input and capture scripts for melonDS (`mg/README.md`).

## Prerequisites: melonDS GDB stub

1. Install melonDS (Qt frontend — the GDB stub lives there).
2. Emu settings → enable the GDB stub. In `melonDS.ini` the keys are roughly:
   ```ini
   GdbEnabled=1
   GdbPortARM9=3333
   GdbPortARM7=3334
   JIT_Enable=0          ; REQUIRED — the stub needs the interpreter core
   ```
   (Exact key names vary by melonDS version — confirm in the UI. The two that
   matter: stub ON, JIT OFF.)
3. Load `sm64.nds` and **load a savestate / let the game run past boot** so the
   CPU is actually executing game code (otherwise the arm9 targets never fire).
   melonDS suspends the CPU while a GDB client is attached, then resumes on
   `continue` — that's expected.

DeSmuME fallback: launch with `--arm9gdb=3333`; everything below is identical.

## Run the spike

```sh
# default: 6 small always-resident arm9 funcs, 20s window
python tools/trace/gdb_harness.py --duration 20

# pick your own targets (must exist in nearmiss/db.jsonl)
python tools/trace/gdb_harness.py --names func_02043288,DMASyncFillTransfer

# write every hit record to JSONL for inspection
python tools/trace/gdb_harness.py --duration 30 --out traces/spike.jsonl

# canary-rejection test: bp an address whose overlay is swapped out (or any
# address you expect NOT to hold the named func) and confirm canary_ok=false
python tools/trace/gdb_harness.py --negative 0x0211640c
```

## What to look for (Phase-0 acceptance gates)

The run prints a summary. Report these back:

1. **Register layout** — the startup line `register packet: N words; pc=... cpsr=...`.
   - `pc` on a breakpoint hit MUST equal the target function's address.
   - Confirm `cpsr` has a sane mode (low byte one of 10/11/12/13/1f) and note
     `cpsr_src` (`word16` = core layout, `last` = legacy-FPA layout). **Write the
     confirmed layout here** once known — it's the classic stub gotcha.
2. **Canary** — `canary clean` count > 0 on resident targets; the `--negative`
   test should log a rejected/unexpected hit (canary_ok=false). Both directions
   must work.
3. **Throughput** — `total hits (X/s)`. **Gate: ≥ 5 hits/s.** Below that after
   capping derefs, the pivot (per the build plan) is a small melonDS fork with
   an in-process addr-set logger instead of stop-the-world breakpoints.

## Confirmed facts (Phase 0 spike, 2026-07-14, melonDS 1.1 windows-x86_64)

**Register packet layout** (from `src/debug/GdbArch.h`, validated live): 39 words.
Indices 0–12 = r0–r12, 13 = sp, 14 = lr, 15 = pc, 16 = cpsr, then banked/spsr
regs. `rsp.py` reads word13/14/15/16 — correct. cpsr src = `word16`.

**Throughput: ~1,500 breakpoint hits/sec** with 206 breakpoints armed on the
title/intro scene (14,998 hits in 10s, all canary-clean). The <5/s kill
criterion is not remotely a concern — stop-the-world breakpoints are plenty fast.

**melonDS 1.1 GDB-stub quirks (these shaped rsp.py — do not regress them):**
1. **Config is per-instance.** Enabling `[Gdb] Enabled=true` alone does NOTHING;
   you must set `[Instance0.Gdb] Enabled=true`. JIT is `[JIT] Enable=false`.
   Config lives in `melonDS.toml` next to the exe.
2. **Mandatory handshake:** on accept the stub blocks in `WaitAckBlocking`
   expecting the CLIENT to send a `+` byte first, then it replies `+`. Without
   that leading `+` it closes the connection on the first packet. `rsp.py`
   `connect()` sends it.
3. **Single session.** The stub services ONE client. You MUST send `D` (detach)
   before disconnecting, or it keeps holding the dead ConnFd and the next
   connection's packets go unanswered (times out). It rebuilds its listen
   socket on each disconnect, so a fresh `connect()` can be briefly refused —
   `rsp.py` retries.
4. **Never send a `0x03` break.** Halting a running target (or breaking while
   stopped) resets/wedges the Windows stub. Not needed anyway: set breakpoints
   while the target runs, and each bp HIT stops it on its own. Teardown = `D`.
5. **A SIGKILL'd client wedges the stub** (dead ConnFd, single session). If runs
   start timing out, restart melonDS. Restarting between trace sessions is
   expected anyway (to cycle savestates).

**Working model (what the collector should do):** launch melonDS with ROM +
savestate → open ONE persistent RSP connection → set all breakpoints while
running → continue → loop {wait_for_stop → read regs/mem → continue} → `D` +
close → kill melonDS. One connection per trace session, detach cleanly.

Launch recipe used:
`Start-Process melonDS.exe -ArgumentList '<rom>'` with the pre-written
`melonDS.toml`; stub binds ~6–8s after boot.

## actorcam: live actor-list heartbeat (`actors.py`)

Human-friendly runtime naming tool: while YOU play in melonDS, it walks the
game's global Actor list every snapshot and prints what is alive - actor ID
joined to the community `ActorList.h` name, vtable, uniqueID, param, world
position - and diffs snapshots so "what spawned/despawned when X happened" is
one keypress. Snapshots append to `traces/actors/session_*.jsonl` with your
event labels for later vtable->name joining.

Setup (once): `melonDS.toml` needs `[Gdb] Enabled=true`, `[Gdb.ARM9]
Port=3333 BreakOnStartup=true`, `[JIT] Enable=false`; clone
`reference/DynamicAllocationDecomp` for actor-ID names.

Use:
```sh
1. start melonDS with sm64.nds        (game waits, halted, for the client)
2. python tools/trace/actors.py       (attaches, releases the boot, prompts)
3. play; press Enter to snapshot, or type "star exploded" + Enter to tag one
```

melonDS 1.1 stub facts this tool obeys (learned the hard way):
- one client per emulator launch; if attach probes time out -> restart melonDS
- memory reads work fine while the game RUNS; `?` only answers when halted
- with BreakOnStartup the game powers on halted; the tool auto-continues
- every reply must be ACKed - only talk to the stub through `rsp.py`

Addresses are EU (this repo's ROM): actor list head `0x0209b468` (node
embedded at actor+0x50), `LOADED_LEVEL_OVL_ID 0x02092130`; layouts credited to
SplattyDS/DynamicAllocationDecomp (see CREDITS.md), vtable slot semantics
validated in `tools/actor_names.py`.

## ask.py: pause and ask

Runtime evidence for a field's meaning, gathered blind. At a pause the tool
pokes a field, or a trigger breakpoint lets the game write it. It then advances
a few frames and shows a before/after pair of the screen with the question
"what changed?". Sham pauses are mixed in at random: no-op writes, writes of
the same value, and flips of a bit believed unused. The key that says which
pause was real lives in a separate file, and `score` unblinds it. Example:
`specs/capicon_bit1.json` asks whether `dCapIcon_c::mFlags` bit 1 (`0x02`)
draws the cap on a Goomba's head.

Files: `ask.py` (CLI), `emusession.py` (one long-lived stub session plus a
local RPC daemon), `emuwin.py` (launch, window capture, focus-free input,
config swap), `uia_menu.ps1` (menu clicks by UI Automation), `specs/*.json`.
Python 3.11 standard library only; tkinter is needed for human mode only.

### A human session

```sh
# 0. close every melonDS window. The tool refuses to start if one is open.
# 1. launch melonDS, load the savestate (checked like load-state), hold the stub (leave this running)
python tools/trace/ask.py serve --state C:/path/to/S3.ml --workdir C:/somewhere/outside/repo/emu
# 2. in another shell: run the spec. A topmost popup asks each question.
python tools/trace/ask.py run tools/trace/specs/capicon_bit1.json --mode human --out C:/somewhere/outside/repo/runs/r1
# 3. unblind and tally (writes score.json; per-condition hit/miss/abstain)
python tools/trace/ask.py score C:/somewhere/outside/repo/runs/r1
# 4. detach, close melonDS, restore your melonDS.toml, delete the workdir
python tools/trace/ask.py stop --kill --clean
```

The first launch of the installed `melonDS.exe` with the GDB stub on can raise
a Windows Firewall prompt (the stub listens on a socket). Approve it once. The
prompt is per executable path, so the tool always runs the installed exe and
never copies it. During `run`, leave the melonDS window alone: input is posted
to the window, so the window does not need focus. The popup shows only
"Question N of M" with a progress line, "A: before / B: after", the question,
the choices, and a confidence row. It never shows the condition name, whether
the pause was real, how many pauses are real or sham, or any RAM values.

Progress counts only questions that are actually asked. The first popup
shows the plan, e.g. "12 questions, about 5 minutes" (a rough first guess).
Later popups show "about K minutes left", measured from the questions so far.
M is the number asked so far plus the plan entries still to come. When
`frame_check` skips an off-screen event and requeues it, M stays the same.
When an event is dropped for good (no trigger, object not found, requeue
budget spent, an error), M goes down. The progress line then adds "R
retaken (target off screen), D dropped". The skip rule is the same for real
and sham events, so these counts do not unblind anything. Every per-event
stdout line, in every mode, carries the same counts: `asked`, `of`,
`retaken`, `dropped`, `progress` ("5 of 12 asked") and `eta`. The start line
carries `plan`.

Agent mode (`--mode agent`) writes the same `pairs/Qxx.png` images and an
`answers.json` template. Fill in `answer` and `confidence` for each id, then run
`score`. Do not open `key/` before scoring. `--mode none` only records data.
`--answer-after MS` auto-answers the popup and screenshots it (`popup_Qxx.png`)
as a smoke test. It also reads the progress label back from the widget, into
the stdout line and the key log as `popup_label`. Those answers are tagged
`"auto": true`, and `score` excludes them.

Run directory layout: `pairs/` (blind images), `answers.json`, `key/key.json`
(condition, real/sham, expected answer, located object, pokes with old/new
values, pause trigger/pc/lr, before/after RAM snapshots, framing), `key/log.jsonl`
(every daemon event), `key/events.jsonl`, and `key/` raw shots.

### Savestates, framing, overlays

```sh
python tools/trace/ask.py load-state C:/path/to/any.ml     # load any .ml file, verified
python tools/trace/ask.py save-state C:/path/to/out.ml     # save the running game to a file
python tools/trace/ask.py where "[0x0209f394]+0x5c" --mark C:/tmp/where.png   # is it framed?
python tools/trace/ask.py shot C:/tmp/s.png --require-framed "[0x0209f394]+0x5c"  # capture only if framed
python tools/trace/ask.py overlays                         # loaded overlays, suspended probes
```

- **Any savestate file loads.** melonDS enables a File > Load state slot only
  if `game.mlN` existed when the ROM was loaded. `serve` therefore creates
  zero-byte `game.ml1`..`game.ml8` before it starts melonDS, so every slot is
  enabled. `load-state FILE` (spec key `load_state_file`, and `serve --state`,
  which takes the same path) copies FILE over slot 8 and loads that slot.
- **The load verdict is a marker check.** Before the click, a sentinel is
  written into a RAM word that nothing writes or reads while the game runs:
  the start of the arm9 `.rodata` string "ASSERTION FAILED  FILE=[%s] ...",
  at 0x020868a0, found by content. The sentinel is chosen to differ from the
  value the state file holds there. If the word holds the file's value
  afterwards, the load overwrote it: `verdict: loaded`. If the sentinel is
  still there: `not_loaded`, and the game's value is written back. `run` and
  `serve --state` stop on `not_loaded`. `marker` in the result has the address
  and the values, and `verdict_by` says `marker`. The older RAM-window
  comparison still runs, and its fields are kept: `window_verdict`,
  `windows`, `differ`, `match_file`, `match_before`, `match_file_all`. It
  compares windows before and after with the RAM image inside the file
  (`MELN`/`NDSG`, main RAM at file offset 0x24). Nearby states share most of
  those windows, so it can say `indistinguishable` after a real load. A dry
  run (no click at all) also gave `indistinguishable`. It decides the verdict
  only when the marker cannot (`--no-marker`, or a ROM without the string).
  `load-state --dry-run` does everything but the click, as a self-test: the
  verdict must be `not_loaded`.
- **A savestate from another melonDS major version crashed melonDS 1.1**
  (header `MELN`, u16 major at +4; 13 for 1.1). It did not refuse the file.
  `serve` notices that its melonDS has exited, stops, and restores the config.
- A plain `load_state slot=N` refuses an empty slot file instead
  of clicking a disabled menu item. `save-state FILE` saves through slot 8,
  waits until melonDS has finished writing it, and copies it out. Every
  breakpoint is disarmed while a menu action runs and re-armed afterwards:
  the RPC thread is busy driving the menu, so a probe hit would leave the
  emulator halted and the UI deadlocked.
- **Probes follow the overlay table.** A probe or trigger in an overlay is
  armed only while the game's loaded-overlay table (`data_0209d3c4`, 12 slots
  of `{s32 id, u32 start, u32 size}`, filled by `LoadOverlay` and freed by
  `UnloadOverlay`; see `tools/overlay_residency.py`) lists that overlay.
  Otherwise it is suspended. Overlays share load addresses (ov002 and ov003
  both start at 0x020ad660), so without this a probe for one fires on the
  other's code, and every hit costs a halt and a canary skip. The table is
  re-read on every valid hit, on a canary skip, after a savestate load, and
  every 0.25 s while running (`ovl_poll`). A newly loaded overlay can
  therefore run for up to about 0.25 s before its probes come back. Events
  `suspend`, `resume` and `overlays` go to the log.
  `ctl overlay_suspend on=false` turns this off. arm9 and frame breakpoints
  are never suspended.
- **Framing check (coarse).** `where` / `ctl project` project a world point
  (Fix12, as actors store it at `+0x5c`) onto the top screen. They use the
  game's view matrix (`data_0209b3ec`, which `dView_c` fills from the camera's
  `LookAt`, so it maps `pos >> 3`) and dClipper's projection (`data_0209f43c`:
  aspect +0x4c, near +0x50, far +0x54, half-FOV +0x58). The viewport is
  assumed to be the full 256x192 top screen. It is honest only to this level:
  it projects one point, so it does not know the actor's size and does not
  test occlusion. It uses the last frame the game rendered. `dy` lifts the
  point (for example to a head) and `margin` asks for slack in pixels.
- **Framing guard for your own capture loops.** `where` and `project` only
  report framing. A loop that does not read the answer keeps capturing an
  off-screen subject. In `run`, `frame_check` skips those events. Anywhere
  else, opt in to the guard. `ask.py shot FILE --require-framed POS [--obj X]
  [--margin 8] [--dy N]` (repeat `--require-framed` for several points)
  refuses to capture when any point projects off the top screen. It writes no
  file, prints `{"refused": true, ...}` and exits 3. The RPC form is
  `c.shot(path=..., require_framed="[obj+4]+0x5c", env={"obj": o}, margin=8)`.
  It raises `RemoteError("NotFramed: ...")` with the projection, and logs a
  `shot_refused` event. On success the result carries `framing`.

### Spec format (JSON)

| key | meaning |
|---|---|
| `frame` | `{addr, module}`: a function that runs once per game frame. `step n` = run to its next hit n times. |
| `residency` | overlay canaries (`{module, addr}`). A breakpoint whose overlay is swapped out is refused. |
| `load_state_file` | load this `.ml` file (any path) before the run, verified as for `load-state`. Takes precedence over `load_state_slot`. |
| `load_state_slot`, `settle_frames` | reload this savestate slot before the run, then step this many frames |
| `locate` | `walk {heads, next, max}` plus `where` expressions. Finds the object (`obj`) for each event. |
| `aim` | pokes applied before each event (for example, put the player 500 units in front of the subject so it stays framed) |
| `frame_check` | `{pos, margin=8, dy=0, on_fail: skip\|flag, retries=2, retry_frames=20, requeue}`. `pos` is an expression (or a list of them) for the address of a position, e.g. `"[obj+4]+0x5c"`. It is tested at the pause before any write, and again after the "after" capture. On `skip`, an event whose target projects off screen is dropped (no pair) and its condition goes again at the end of the plan, at most `requeue` times (default: the plan length). On `flag`, the pair is kept and its answer gets `framing_warning`. A countdown pause first steps `retries` x `retry_frames` frames looking for a framed moment; a trigger pause does not. The rule is the same for real and sham events. |
| `fields` | `{name, expr, size}` snapshotted before and after (into the key, never into the pairs) |
| `question` | `{text, choices}`. A choice containing "tell"/"unsure" counts as an abstention. |
| `pause` | `{countdown: [lo, hi]}`: random frame count before a countdown pause |
| `after_frames` | frames advanced between the poke and the "after" capture (default 4) |
| `capture` | `{screen: top\|bottom\|both, settle_ms}` |
| `conditions[]` | `{name, real, count, expect, prepare, prepare_frames, pokes, trigger, timeout, restore}` |
| `triggers[]` | `{name, addr, module, obj, when, dump}`: a pause happens when the game itself reaches this address |
| `probes[]` | the same as triggers plus `ret` (also log the return value) and `max_hits`. Log-and-continue, used by `probe`. |

Expressions: `[addr]` = u32 read, `u8[..]` `u16[..]` `s8[..]` `s16[..]`
`s32[..]`, `+ - * & |`, `== !=` (1/0), parentheses, and hex numbers. Names:
`obj` / `node` / `r0..r15` / `sp` / `lr` / `pc`. A poke is
`{expr, op: set|clear|flip|same|write, mask, value, size}`. `value` can be an
expression (it can read RAM). `prepare` pokes put the field into a known state
before the "before" capture. With `restore: true` (the default), every prepare
and poke byte is written back after the "after" capture, so each event starts
from the game's own state.

### Why it does what it does

- **One stub connection.** melonDS 1.1 accepts one GDB client per launch.
  `serve` holds it and every other command talks to the daemon (`--rpc`,
  default 3390). It sends a keepalive and detects a drop: a dead stub means
  restarting melonDS.
- **Request vs stop-packet race.** The stub closes the connection if a request
  is sent while an asynchronous stop packet (a breakpoint hit) is in flight. It
  expects the `+` acknowledgement for that stop first. With breakpoints armed,
  `Session._pkt` therefore halts and drains (`_quiesce`), sends the request,
  and resumes. A trigger that lands during the drain is kept as the next stop. A
  breakpoint being removed counts as armed until its `z0` is answered, so
  removing a busy probe while the game runs also drains first.
- **Capture lag.** The DS 3D image lags the CPU by about one frame. A capture
  taken right after a poke can show the old state. `after_frames` defaults to
  4 (use at least 2). `shot` waits `settle_ms`, then re-captures until two
  grabs match, because the halted window repaints late. A visible window has
  once reported "no client area", and a rerun was fine. A capture is
  therefore retried up to 5 times, 0.25 s apart, with a `capture_retry` event
  for each retry. A minimised window fails at once.
- **Savestates** load through the File > Load state menu (UI Automation via
  `uia_menu.ps1`) while the target runs. A first attempt has failed with
  "missing: Load state", and an immediate retry worked. A path step that
  finds no item clicked nothing, so the whole path is retried: up to 3
  attempts, 0.8 s apart, with a `menu_retry` event on stderr and in the log
  each time. Any other failure, or a timeout, is not retried, because the
  click may already have landed. **Buttons and touch** are posted
  window messages, so no focus is stolen and no global hotkeys fire.
- **Config.** melonDS reads `melonDS.toml` from next to the exe (portable mode).
  `serve` backs up your toml (and its sha256) into `--workdir`, writes one
  with GDB on (ARM9 port 3333) and JIT off, and puts yours back after melonDS
  exits. It deletes the backup only when the hashes match. Any `serve`
  failure after the swap kills the melonDS it started and restores the
  toml. That covers launch, attach, a `--state` that does not load, a busy
  RPC port (bound before the load) and Ctrl+C. If melonDS exits while being
  served, the daemon stops too and restores the toml. After a hard kill of
  `serve` itself, run `ask.py restore-config --workdir <dir>`.
- Keep run outputs and the work directory outside the repo. The ROM is copied
  into the workdir as `game.nds`, and `stop --clean` deletes that copy.
