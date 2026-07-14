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

## Prerequisites: melonDS GDB stub

1. Install melonDS (Qt frontend — the GDB stub lives there).
2. Emu settings → enable the GDB stub. In `melonDS.ini` the keys are roughly:
   ```
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

```
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
