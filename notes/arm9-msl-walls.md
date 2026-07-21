# The last two arm9 functions: why they need a 2.0 compiler or an asm hatch

**TL;DR** — arm9 has two remaining functions that cannot be matched by writing better C.
They are standard-library printf plumbing that the ROM built with a **newer compiler
(mwccarm 2.0)** than the one this project uses (**1.2**). Version 1.2 physically cannot
reproduce their byte layout from any source. Reaching a literal 100% on arm9 needs a
project-level decision: **(A)** wire the 2.0 compiler into the build for these files, or
**(B)** accept a hand-written assembly ("asm hatch") for them. This is a maintainer call,
not more matching effort.

---

## The two functions

| Function | Module | Address | Size | What it is |
|---|---|---|---|---|
| `func_0206e4a4` | arm9 | `0x0206e4a4` | 0x83c (2108 B) | MSL `__pformatter` — the core of `printf` |
| `func_0206f46c` | arm9 | `0x0206f46c` | 0x3b4 (948 B) | MSL `float2hex` — `%a`/`%A` hex-float formatting |

Both are internals of **MSL** (Metrowerks Standard Library), the C runtime that ships
*with* the compiler rather than being written by the game's developers.

## What "matching" requires

This project doesn't just need C that behaves correctly — it needs C that, compiled by
**one specific compiler (mwccarm 1.2/sp2p3)**, produces the **exact same bytes** as the
retail cartridge. Byte-identical, or it doesn't count.

## The root cause: the ROM wasn't built by only one compiler

A shipped game is assembled from many pieces, compiled at different times. The bulk of
SM64DS was built with **mwccarm 1.2**. But the MSL library objects linked into it carry
the fingerprint of a **later compiler, version 2.0**.

The tell is in how the two compilers use the **stack** (the CPU's scratch workspace during
a function call):

- **2.0** stages a by-value argument struct *below* the stack pointer with **no stack
  adjustment and no frame pointer** — a compact, bookkeeping-free sequence.
- **1.2** cannot emit that. From *every* source phrasing it instead emits
  `sub ip, sp, #8 / mov sp, ip / … / add sp, ip, #8`, which also pins `mov fp, sp`, pulls
  the frame-pointer register into the saved-register set, and grows the stack frame.

Those extra instructions shift every subsequent stack offset in the function. The
divergence is baked into the compiler's code-generation habits, **below the level any C
source can control**. This was verified exhaustively — multiple agents drove `func_0206e4a4`
from 872 down to 253 differing instructions and hit a hard floor that is purely this
frame-shape artifact; the honest by-value draft of `func_0206f46c` floors at 149. Neither
gap is reachable at 1.2.

> This is not "our C is wrong." It's "these bytes came from a different compiler, and we're
> holding the wrong one."

## The two ways to close them

### Option A — the "2.0 path" (the correct fix)

Obtain the **mwccarm 2.0** compiler, wire it into the build, and compile *just these MSL
files* with it — the way the original developers did. Right tool for the right wall.

- **Pro:** genuine, readable C; matches the way the ROM was actually produced.
- **Con:** a real infrastructure change — sourcing, integrating, and configuring a second
  compiler version in the build, plus per-file compiler selection.

### Option B — the "asm hatch" (the pragmatic fix)

"Asm hatch" = *assembly escape hatch*. Instead of C, drop in the exact machine instructions
(raw assembly) for these two functions and tell the build to use those bytes verbatim
rather than compile anything.

- **Pro:** guaranteed byte-match immediately; small, contained.
- **Con:** not truly "decompiled" — it's a transcription, not readable C. Standard practice
  in decomp projects for a handful of genuinely unreachable functions.

## Recommendation

Both are legitimate. Most decomp projects take the **asm hatch** for isolated
library-runtime functions like these and reserve the effort of integrating a second
compiler for cases where many functions need it. If MSL turns out to contribute *many*
2.0-built functions across the ROM, the **2.0 path** pays for itself; if it's just these
two, the **asm hatch** is the lower-cost close.

Either way, no amount of additional C-matching effort will crack them — the decision is
about how the project wants to handle compiler-version-mismatched library code.

---

*Context: these two are the only functions blocking a literal 100% match of the arm9
module. The rest of arm9's remaining work is ordinary near-misses (register-allocation /
instruction-ordering residuals) that are being closed the normal way.*
