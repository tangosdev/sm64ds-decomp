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

## Gate 1 (this directory today)

A Win32 headless executable that compiles a curated slice - types,
fixed-point math, matrices, Timer, the Fader class hierarchy - and runs ABI
assertions plus known-value smoke tests. No renderer, no audio, no input.
Build it with `build-port.cmd`; run `build\port\smoke.exe`.

`slice_gate1.txt` is the manifest. Grow the slice by adding files there and
assertions to `tests/smoke.cpp`; `tools/host_frontier.py` reports how much of
`src/` syntax-compiles for the host and why the rest does not, so the next
subsystem to pull in is a measurement, not a guess.

## Why the emulator is not here

The hybrid (tangOS-SM64DS) stays the verification harness: it proves
functions byte-for-byte against real hardware behaviour. The port is the
destination. Benchmarks on 2026-08-02 showed natives inside the hybrid are
performance-neutral (~2% of the emulated clock), which is the measured
argument for building the port rather than growing the hybrid further.

## Frontier baseline (2026-08-02)

First full sweep of `tools/host_frontier.py` (cl /Zs, 32-bit, all of src/):

    compiles for host: 11,005 / 11,222  (98.1%)

217 failures, largest buckets: C2054 x98 (one idiom, worth one targeted
fix), generic syntax errors x44, C2732 linkage contradictions x19, the rest
single digits. The takeaway: the decomp's C is already overwhelmingly
host-clean, so the port's work is platform seams and link closure, not mass
source repair. Re-run the sweep after each seam lands; the number should
only go up.
