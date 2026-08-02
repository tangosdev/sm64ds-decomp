# port/ - the native PC build

This tree hosts the PC port of the decompiled game: the same `src/` sources
compiled for the host, a platform seam instead of DS hardware, and its own
build that never touches the byte-matching pipeline.

## Rules

- **`src/` stays the byte-verified source of truth.** The port compiles files
  FROM `src/`; it never edits them for host convenience. A file that needs a
  host-side change gets that change behind `SM64DS_PLATFORM_*` guards or a
  seam header, reviewed like any other src change.
- **Logically-correct-but-unmatched implementations live in
  `port/unmatched/`, never in `src/`.** The NDS hybrid build keeps selecting
  delinked ROM bytes for those functions; the PC build selects these.
- **32-bit host first.** The recovered ABI assumes 4-byte pointers. x64 comes
  after struct recovery makes layout host-independent.

## Gate ledger

Each gate is a slice manifest + a smoke binary that proves one seam with
game data. `build-port.cmd` builds all of them into `build\port\`.

| Gate | Smoke | What runs on host |
|---|---|---|
| 1 | `smoke` | types, fx math, matrices, Timer, Fader hierarchy |
| 2 | `smoke_heap` | ExpandingHeapAllocator, 5,000-op torture |
| 3a | `smoke_roots` | SetupRootHeap + the Memory:: layer (game global heap) |
| 3b | `smoke_fs` | SharedFilePtr over the catalog card seam, LZ77 cross-checked |
| 4a | `smoke_gx` | the interrupt-driven display-list pump, byte-equal vs harness |
| 4b | `smoke_model` | the whole Model pipeline: load, rebase, VRAM upload, materials, render (Mario, textured) |
| 4c | `smoke_anim` | Animation/UpdateBones recursion (the Mad Piano, posed) |

Supporting machinery: `tools/hostgen.py` (MMIO transform into the build
tree; src/ is never edited), `tools/romdata.py` (ROM constants from the
dsd-extracted image; Nintendo bytes stay out of git), `tools/ntr_manifest.py`
(fs manifest for the ntr backend), the `ntr/` platform layer (see its
README), and `hal/` (seams, bridges, and the storage for DS BSS globals).

Grow a gate by adding files to its `slice_gate*.txt` and assertions to its
smoke; `tools/host_frontier.py` reports how much of `src/` syntax-compiles
for the host so the next subsystem is a measurement, not a guess.

## Why the emulator is not here

The hybrid (tangOS-SM64DS) stays the verification harness: it proves
functions byte-for-byte against real hardware behaviour. The port is the
destination. Benchmarks on 2026-08-02 showed natives inside the hybrid are
performance-neutral (~2% of the emulated clock), which is the measured
argument for building the port rather than growing the hybrid further.

## Frontier baseline (2026-08-02)

First full sweep of `tools/host_frontier.py` (cl /Zs, 32-bit, all of src/):

    compiles for host: 11,005 / 11,222  (98.1%)

Refined the same day: the C2054 x98 bucket decoded as the asm-hatch TUs
(`asm void`, mwccarm syntax) -- DS hardware operations that SHOULD not
compile on a host. CP15 cache management is a host no-op, block copy/fill is
memcpy, context switch is the threads seam. The tool now classifies them as
HAL-owned rather than as syntax defects, which makes the honest frontier:

    compiles for host: 11,005 / 11,125  (98.9%)   + 97 asm TUs -> HAL backlog

120 real failures remain: generic syntax x44, C2732 extern-C linkage
contradictions x19, undeclared identifiers x13, unknown types x11, the rest
single digits. The takeaway stands, stronger: the port's work is platform
seams and link closure, not mass source repair. Re-run after each seam
lands; the number should only go up.
