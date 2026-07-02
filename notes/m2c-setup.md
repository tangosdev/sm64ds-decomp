# m2c integration (semantic drafts for LARGE functions)

Wiring [m2c](https://github.com/matt-kempster/m2c) (matt-kempster's decompiler,
ARM support via `--target arm-gcc-c`) as a free semantic-draft tier for the
fan-out. For a LARGE function (0x400+ bytes) with no close matched sibling,
coddog has no scaffold to attach; m2c gives the agent a structured C skeleton
(control flow, resolved callee names, field offsets) for ~1s of CPU and zero
tokens.

The draft is **gcc-flavored pseudo-C**. It is NOT a matching candidate, it will
not compile under mwccarm as-is, and its register-derived temps say nothing
about mwccarm regalloc. It answers "what does this function do", not "what C
matches it". Treat it exactly like a Ghidra draft, but with our symbol names
and much cleaner control flow (m2c reconstructs switches, loops, and && chains).

## Setup

```sh
git clone https://github.com/matt-kempster/m2c vendor/m2c
```

vendor/ is gitignored; each clone sets this up locally. No compatibility patch
is needed: upstream m2c (tested at the 2026-07 head) runs unmodified on native
Windows Python 3.14 with no extra dependencies (it vendors its own pycparser;
we invoke it in a subprocess, its stdout is the draft).

## Usage

```sh
# one function, draft to stdout (live mode: reads extracted/ like every tool)
python tools/m2c_draft.py --name func_0206a928
python tools/m2c_draft.py --module ov002 --addr 0x020d0178

# from a worklist row (no ROM needed: bytes from target_hex, relocs from config/)
python tools/m2c_draft.py --wl progress/wl_ab.jsonl --name func_020e2708

# debug the generated GAS asm
python tools/m2c_draft.py --name X --dump-asm 2>x.s

# fan-out wire-in: attach drafts to the rows that have no sibling scaffold
# (coddog_sim < 0.5 and size > 0x300); abrow.py prints them to the agent
python tools/coddog.py --min 0x400 --max 0x800 --limit 24 --draft --out progress/wl_ab.jsonl
```

`tools/crackloop.py prep` prints a non-fatal warning when the locked worklist
contains low-sim large rows without `m2c_draft`; install `vendor/m2c` and rerun
prep with `--draft` before launching those rows when token cost matters.

## How the converter works (tools/m2c_draft.py)

m2c consumes GAS-style assembly text; our functions are ROM bytes. The
converter bridges capstone -> the dialect m2c's ARM parser accepts (studied
from vendor/m2c/tests/end_to_end/*/agbcc-o2.s and m2c/arch_arm.py):

- **Per-word decode, not linear sweep.** Every aligned 4-byte word is decoded
  independently, so a mid-function literal pool cannot truncate the
  disassembly. Code vs data is then classified by recursive descent from the
  entry point.
- **Branch targets** become local labels (`.L_<va>`); `bl`/`blx` outside the
  function are renamed to the resolved callee symbol from the module's
  relocs.txt (else `func_<addr>`). External `b` tail calls are left as symbol
  branches (m2c's TailCallPattern turns them into `return f(...)`).
- **pc-relative `ldr`** is rewritten to the `ldr rX, =value` pseudo-instruction:
  `=SYM` when the pool slot is a relocation (m2c reads it as `&SYM`), `=0x<word>`
  for a plain constant. The pool words themselves need no emission for this.
  mwccarm sometimes places the pool just past the symbol-table size, so live
  mode reads a padded window (+0x400) for out-of-range slot values.
- **`add rX, pc, #imm`** (adr) becomes `ldr rX, =<label-or-address>`.
- **Switches, both forms.** `add pc, pc, rX, lsl #2` branch tables are native
  m2c (AddPcPcPattern) and reconstruct as real `switch` statements; the branch
  rows after the dispatch are walked explicitly during classification.
  `ldr pc, [pc, rX, lsl #2]` word tables are handled on the classification
  side (a data word relocated or pointing into the function marks its target
  as code and is emitted as `.word .L_<va>`), but m2c reads the dispatch as an
  indirect tail call, so those case blocks may render as unreachable labels.
- **Data words** are emitted as labeled `.word` directives (safe inside .text;
  m2c attaches them to the enclosing label as data).
- Capstone's operand text is otherwise GAS-compatible for ARM, including cc
  suffixes (`movne`, `addls`), register aliases (`ip`/`sb`/`sl`/`fp`), barrel
  shifts, writeback (`[r0, #4]!`), and ldm/stm directions. m2c's arch_arm
  covers the ARMv5TE ops the ARM9 uses (`clz`, `smull`, `smulbb`, ...); system
  ops (`mrs`/`msr`/`mcr`) are not supported and fail the draft cleanly.

## Status: WORKING (validated 2026-07-01)

- Trivial oracle case: `mov r0,#1; bx lr` -> `s32 f(void) { return 1; }`.
- 4 already-matched mid-size functions (0x140-0x3f4, arm9 + ov002, C and C++)
  produce drafts whose control flow, callees, and field offsets line up with
  the committed src 1:1 -- including the C++ vtable dispatch in
  func_02038a38 (`(*temp_r4)->unk20(...)` vs the matched `o->f8(arg0)`) and
  the full case structure of GetSceneOverlayID's switch.
- LARGE tier sample: **25/25 unmatched 0x400-0x113c functions draft OK**
  (~1s each), including the 4.8KB func_0206a928 (946-line draft).
- --wl mode output is byte-identical to live mode on the same function.

## Caveats

- `?` types and `(? *)` casts are m2c saying "unknown"; agents should treat
  them as holes, not signatures. Callee signatures in the worklist row
  (knowledge.py) are authoritative where present.
- 64-bit helpers (`__aeabi_uldiv` etc.) decompile as opaque calls with pair
  registers; fine as a scaffold.
- The draft's variable count explodes on very large stack frames; it is still
  useful for control-flow orientation, less so as line-by-line C.
- Do not feed the draft to the oracle or the permuter as-is; it is a reading
  aid for the agent writing the real mwccarm candidate.
