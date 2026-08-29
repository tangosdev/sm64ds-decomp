# When does assembly count as decompiled?

This project's goal is recovering the original C. Byte-equality with the ROM is how we
*prove* a function is right, not the goal itself. That distinction decides whether a
function written as an `asm { ... }` block counts as matched.

mwccarm lets you write a whole function as inline assembly:

```c
asm void func_02071974(void) {
    swi 0x90000
    bx  lr
}
```

That assembles to exactly the instructions you typed, so it reproduces the ROM trivially
and the byte gate passes. Whether that is a *decompilation* depends entirely on what the
function was in Nintendo's original source.

## The rule

**Assembly counts as matched only when assembly is what the original source contained.**

| Original was | Correct decompilation | Counts as matched |
|---|---|---|
| Hand-written asm (BIOS calls, coprocessor ops, context switching) | the same asm | **yes** |
| C that we cannot yet reproduce byte-for-byte | the C | **no** — it stays a draft |

The second row is the important one. If the original was C and we ship asm, we have not
recovered anything; we have transcribed the disassembly. Counting it inflates the progress
number with work that still needs doing. Every GBA/DS decomp draws this line the same way.

## The objective test

Intent is not observable, so we use the instruction set as the evidence. A function
qualifies for the assembly exception when its body contains instructions that C **cannot
express under any compiler flags**:

- `mcr` / `mrc` — coprocessor (CP15 cache, MPU, TCM control)
- `swi` — BIOS software interrupt
- `msr` / `mrs` — program status register, processor mode switching
- `ldm` / `stm` with the `^` suffix — banked user-mode registers
- `swp` / `swpb` — atomic read-modify-write swap; mwccarm has no construct (this
  predates C11 `_Atomic`) that emits it, so it only comes from hand-written asm

No C construct compiles to these. A compiler will never emit them from portable source, so
their presence is proof the original was assembly.

Conversely, a function made **entirely of ordinary ARM instructions** is by definition
something a C compiler could emit. If we cannot get mwccarm to emit exactly those bytes,
that is an unsolved matching problem — a register-allocation or instruction-scheduling
wall — not a function that was assembly to begin with. Those stay `// NONMATCHING` drafts
until someone finds the C.

## History

**2026-07-24 (`fcdfc66d`)** — 116 functions that had been counted as matched were found to
be whole-function `asm` blocks and reclassified as `// NONMATCHING`, dropping them from the
matched count (95.2% → 94.2%). 35 SDK/BIOS routines were deliberately excluded and left as
matched: `Div`, `CpuSet`, the `CpuFastSet`/memcpy family, `ARMSaveContext`/`ARMRestoreContext`,
`Sqrt`, `Halt`, `IntrWait`, and the CP15 cache operations — all assembly in the original.

**2026-07-25** — auditing the 116 against the objective test above showed the rule had been
applied too broadly. Ten of them contain the same inexpressible instructions as the 35 that
were excluded:

| Function | Inexpressible instructions |
|---|---|
| `func_020048d8` | `mcr` (cache maintenance) |
| `func_020593f4` | `mrs` |
| `func_0205947c` | `mrs`, `msr` |
| `func_0205950c` | `msr` |
| `func_0206d9cc` | `mrs`, `msr` |
| `func_0206da18` | `mrs`, `msr` |
| `func_0206da64` | `mcr`, `mrc` (CP15) |
| `func_02071974` | `swi` |
| `func_02071988` | `swi` |
| `func_0207199c` | `swi` |

These are indistinguishable in kind from `ARMSaveContext` (`mrs`/`msr`), `Div` (`swi`), and
`CP15::FlushDataCache` (`mcr`), which were never in question. They were restored to matched
status — not as a change of policy, but as a correction of how it was applied. Each was
restored to its exact pre-reclassify content and re-verified: nine `VERIFIED`, one
(`func_0205950c`) `BLIND-3`, meaning three relocation slots reference symbols not yet in
`config/**/symbols.txt` and could not be link-checked; the rest of that function verified.

The remaining **106 stay `// NONMATCHING`**. They are ordinary ARM — game logic
(`daObjHmBskt_c::Behavior`, actor `InitResources`/`CleanupResources`, and similar) that was C in
the original and is sitting on a codegen wall. The byte-exact reference and the wall analysis
stay in each file, so the work is preserved and the next attempt starts from it. They are
drafts, not losses.

**2026-07-25 (later)** — while writing C for those 106, the veneer subset (cross-overlay
tail-calls) was found matchable after all: `#pragma long_calls on` forces mwccarm to emit the
pooled `ldr ip,[pc]; bx ip` indirect tail-call, and 45 of them matched from real C and were
removed from the asm-only set. Separately, two `swp` primitives (`func_0205a74c`,
`func_0206dab4`) and two more CP15 `mrc` reads (`func_0206da94`, `func_0206da9c`, missed by the
ten above) were restored to matched under the test — `swp` was added to the list at that time.

## Adding a new one

If you believe a function was assembly in the original:

1. Confirm its body contains at least one instruction from the list above.
2. If it is ordinary ARM, it does not qualify — bank the best C attempt as a near-miss
   (`tools/nearmiss_db.py`) instead.
3. Note in a header comment which construct forces assembly and why C cannot express it.

If you are unsure, leave it `// NONMATCHING`. An honest draft costs nothing; a matched
function that nobody actually decompiled has to be found and rescinded later.
