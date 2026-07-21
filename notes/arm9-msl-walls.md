# The two arm9 MSL functions: why they stay asm (and why that's by design, not a gap)

**TL;DR** — two arm9 functions (`__pformatter`, `float2hex`) do not match under mwccarm 1.2.
The cause is now **confirmed by reproduction**: they were built by a **2.0-family compiler**,
whose stack-frame codegen 1.2 cannot emit. But matching them is **not worth doing** — they are
IO-adjacent MSL plumbing that the recomp trampoline carries as an asm remainder *by design*, and
a matched C version would sit behind that trampoline unused. **99.98% and 100% matched produce
byte-identical recomp behavior.** No action is needed; this doc exists so the "wall" is recorded
as understood-and-intentional, not as open work.

---

## The two functions

| Function | Module | Address | Size | What it is |
|---|---|---|---|---|
| `func_0206e4a4` | arm9 | `0x0206e4a4` | 0x83c (2108 B) | MSL `__pformatter` — the core of `printf` |
| `func_0206f46c` | arm9 | `0x0206f46c` | 0x3b4 (948 B) | MSL `float2hex` — `%a`/`%A` hex-float formatting |

Both are internals of **MSL** (Metrowerks Standard Library), the C runtime that ships *with* the
compiler. Both do their work by writing formatted output through **output callbacks** — the
IO-adjacent library shape the recomp screening layer is built to keep as asm.

## The cause, confirmed by reproduction

Earlier this was documented as an *inference* ("probably 2.0"). It is now a reproduced fact.

The tell is the function prologue's stack-frame shape. Compiling the honest by-value draft of
`float2hex` and comparing the prologue to the ROM:

| | frame pointer | frame size | reads args via | epilogue |
|---|---|---|---|---|
| **ROM** | none | `sub sp,#0x48` | `sp` | `addgt sp,#0x48 / popgt {…no fp…} / addgt sp,#0x10` |
| **mwccarm 1.2/sp2p3** | `mov fp,sp` | `sub sp,#0x4c` | `fp` | `popgt {…with fp…}` |
| **mwccarm 2.0** (base / sp2p3 / sp2p4) | none | `sub sp,#0x48` | `sp` | `addgt sp,#0x48 / popgt {…no fp…} / addgt sp,#0x10` |

2.0 reproduces the ROM's frame **exactly** — no frame pointer, the same `0x48`, sp-relative
argument reads, and the identical `addgt / popgt / addgt` epilogue. 1.2 always emits the
frame-pointer form (`mov fp,sp`, `sub sp,#0x4c`, `fp`-relative reads), which pulls `fp` into the
saved-register set and shifts every stack offset. This is a code-generation habit **below the
level any C source can reach** — verified across 1.2 base/sp2/sp2p3/sp3/sp4 and ~75 flag
combinations, C and C++; the residual is invariant to source phrasing.

So: **1.2 provably cannot build these; the 2.0 family provably can** (the frame is reproduced).
The compiler is on hand (`tools/mwccarm/2.0/`), and `tools/match.py` already drives it — finishing
a full byte-match under 2.0 would be ordinary near-miss work (the honest draft compiles ~7
instructions short with different register coloring). **We are choosing not to, for the reason
below.**

## Why we don't match them anyway

Even a perfect matched-C version of these two would change nothing:

- They write through **output callbacks** — exactly the IO-adjacent shape the recomp **screening
  layer exists to reject**. Matched or not, they stay behind the **trampoline**.
- The trampoline was designed from day one to carry an **asm remainder indefinitely**. Two MSL
  functions is the smallest remainder imaginable — it is the intended steady state, not a
  fallback.
- Recomp output is **byte-identical** whether these two are asm or matched C. A matched version
  would be a repo artifact the runtime never executes.

In other words, the "asm hatch" framing from the earlier draft of this note was wrong: there is
no hatch and no decision. These functions living as asm is the architecture working as designed.

## Bottom line

- **Not a bug, not an open task.** 1.2 can't build them; that's fully explained (2.0-family
  codegen, reproduced).
- **No value in matching them.** They stay behind the trampoline regardless, and 99.98% ==
  100% in recomp behavior.
- **Nothing to decide.** The earlier "2.0-path vs asm-hatch" question is moot — the asm remainder
  is intentional and permanent.

If the trampoline / screening rationale is documented elsewhere in the project, treat that as the
authoritative source; this note only records *why these two specific functions are in the asm
remainder* so nobody re-opens the wall as if it were unfinished work.

---

*Context: these two are the only functions that would block a literal 100% match of the arm9
module — a number the recomp is indifferent to. The rest of arm9's remaining work is ordinary
near-misses being closed the normal way. Codegen detail is in `notes/mwccarm-codegen.md`
section 9.*
