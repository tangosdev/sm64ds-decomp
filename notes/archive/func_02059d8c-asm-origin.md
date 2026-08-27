# func_02059d8c — carry-flag floor (asm-origin determination)

**Module / addr / size:** arm9 `0x02059d8c` / `0xc` (3 instructions)
**Compiler:** mwccarm `1.2/sp2p3` and `2004/b56` (`-O4,p -enum int -lang c99 -char signed -interworking -proc arm946e`)
**Status (2026-07-29):** no pure-C preimage exists. Recommend reclassifying from
`// NONMATCHING` (a "register-allocation/scheduling wall", per `fcdfc66d`) to **ASM-PRIMITIVE**.
**Current file:** `src/unnamed/arm9/0205/func_02059d8c.c` — asm hatch, no provenance row, not counted as matched.

## ROM shape

```arm
02059d8c  subs r0, r0, #4
02059d90  bhs  0x02059d8c
02059d94  bx   lr
```

A busy-wait / delay loop (`WaitByLoop` / `OS_SpinWait` idiom). `subs` sets `C = NOT borrow`, so
`bhs` loops while the value **before** the subtraction was unsigned `>= 4`.

## Why no C reaches it

`bhs` reads the **carry** flag. Carry after `subs r0,r0,#4` encodes `n_old >= 4`, which is **not a
function of the instruction's result** — it depends on the pre-subtraction value. C has no carry
operator, so the predicate cannot be named from the post-subtraction value.

mwccarm fuses a subtract and its test into a flag-setting `subs` **only for result-vs-zero tests**
(the Z and N flags). Every such form produces the exact 12-byte target shape with a different
condition nibble, and `2a` (HS) is not among the reachable ones:

| C loop condition (`do { n -= 4; } while (…)`) | emitted | bytes |
|---|---|---|
| `n` / `n != 0` / `n > 0` (u32) | `subs` + `bne` | `040050e2 fdffff`**`1a`**` 1eff2fe1` |
| `n >= 0` (s32) | `subs` + `bpl` | `040050e2 fdffff`**`5a`**` 1eff2fe1` |
| `p >= (void*)0` (pointer) | `subs` + `b` (tautology folded, S-bit kept) | `040050e2 fdffff`**`ea`**` 1eff2fe1` |
| **ROM** | `subs` + `bhs` | `040050e2 fdffff`**`2a`**` 1eff2fe1` |

HS is unreachable from a zero-compare on principle: unsigned `x >= 0` is a tautology, and the front
end constant-folds it to an unconditional `b` before instruction selection (verified for `n >= 0`,
`n >= 0u`, `(u32)n >= (u32)0`, `n >= 4-4`, `!(n < 0)`, `(n|0) >= 0`, `n - 0 >= 0`, and the pointer
spellings).

The only C that *does* name the pre-subtraction value forces a separate `cmp`:

```arm
cmp r0, #4       ; <- extra word
sub r0, r0, #4
bhs …
bx  lr
```

**mwccarm has no `cmp rX,#k` + `sub rY,rX,#k` → `subs rY,rX,#k` peephole.** That fusion is always
legal (a flag-setting sub sets exactly the flags of the corresponding cmp), and mwccarm never
performs it — confirmed across **all 20 local builds** (1.2 base/sp2/sp2p3/sp3/sp4, 2004/b56,
2.0 base…sp2p4, dsi 1.1…1.6sp2) × **7 optimization levels** (-O0…-O4,p / -O4,s, `-ipa file`) ×
**8 pragmas** (`peephole on|off`, `opt_common_subs off`, `opt_strength_reduction off`,
`opt_loop_invariants off`, `optimize_for_size on`, `opt_unroll_loops off`).

## ROM-internal evidence: the shape only occurs in hand-written asm

Scanned every known ARM function in `.text` (`0x02004000`–`0x020736f4`) for the defining pattern —
an **unpredicated flag-setting ALU op with a destination register, immediately followed by a
carry-flag branch** (`hs`/`lo`/`hi`/`ls`). This is precisely "a branch on carry taken from flags
produced by a value-computing instruction", the thing C cannot request. Whole-arm9 result:

| site | function | insns | classification |
|---|---|---|---|
| `0x02059d8c` | `func_02059d8c` | `subs r0,r0,#4 ; bhs` | **this function** |
| `0x0205a674` | `func_0205a61c` | `subs r2,r2,#2 ; blo` | already ASM-PRIMITIVE |
| `0x0205a68c` | `func_0205a61c` | `subs r2,r2,#2 ; bhs` | already ASM-PRIMITIVE |

**Three sites in the entire arm9, and no others.** Two of them are inside `func_0205a61c`, the SDK
unaligned block copy the repo already ships as an asm primitive. Zero occurrences anywhere in
compiled game C.

## Module neighborhood

`func_02059d8c` sits in the middle of the SDK's CP15/processor-state assembly file, between two
routines already accepted as asm-origin and counted as matched:

```arm
02059d74  mrs r0, cpsr / and r0,r0,#0x1f / bx lr     ; ARMProcessorMode          (asm, matched)
02059d80  mov r0,#0 / mcr p15,0,r0,c7,c0,4 / bx lr   ; CP15::WaitForInterrupt    (asm, matched)
02059d8c  subs r0,r0,#4 / bhs / bx lr                ; func_02059d8c             <- this one
02059d98  …                                          ; func_02059d98
```

`mrs` and `mcr` are on the `notes/asm-policy.md` inexpressible list. A delay loop wedged between a
CPSR read and a CP15 wait-for-interrupt, in a linker-contiguous run, is the same translation unit.

## Retracted lead — do not re-run this scan

An earlier version of this analysis claimed the ROM contained 4 *unfused* `cmp #k` / `sub #k` pairs
in compiled code, which would have proven the original compiler also lacked the peephole. **That
was wrong** — a naive word scan; on disassembly three are *predicated* (`subhs`, `subge`), where
fusion is not legal, and the fourth (`sub lr,lr,#1 ; cmp lr,#1` at `func_0206f46c+0x2dc`) compares
the *new* value, which a fused `subs` would not do. There are in fact **zero** legal-but-missed
fusion sites in the ROM, so that scan proves nothing in either direction. The argument rests on the
carry-semantics floor plus the three-site locality result above, both of which stand.

Likewise, a first pass mis-based the scan at `0x02000000`; the arm9 image base is **`0x02004000`**
(`tools/match.py: ARM9_BASE`). Seven further "hits" reported at `0x0209b014`–`0x0209c5a0` are in
`.bss` (starts `0x0209b000`) and one at `0x020042d0` is Thumb code decoded as ARM — all spurious.

## Best C near-misses

Correct semantics, one extra word (`0x10` vs `0xc`) — the unfused `cmp`:

```c
typedef unsigned long u32;

u32 func_02059d8c(u32 n)
{
    u32 t;
    do {
        t = n;
        n = t - 4;
    } while (t >= 4);
    return n;
}
```

Correct size, one word differs (condition nibble `1a` vs `2a`):

```c
typedef unsigned long u32;

u32 func_02059d8c(u32 n)
{
    do {
        n -= 4;
    } while (n);
    return n;
}
```

### Verify

```sh
PYTHONIOENCODING=utf-8 python tools/match.py --c <file>.c --func func_02059d8c \
  --addr 0x02059d8c --size 0xc --module arm9 --versions 2004/b56,1.2/sp2p3
# form 1: size differs: target 0xc vs candidate 0x10
# form 2: +0x04 | fdffff2a bhs | fdffff1a bne | <<< MISMATCH   (1/3 words)
```

## Tried levers (67 distinct C spellings, ~1,400 compiles)

- **Loop form:** `while`, `do/while`, `for(;;)`+`break`, goto-structured, pre-test vs post-test
- **Typing:** `u32` / `s32` / `int` / pointer (`void*`, `char*`, `unsigned char*`, `int*`) params
- **Return:** `void` (loop is DCE'd entirely → 1 insn) vs value-returning (keeps the loop live)
- **Predicate rewrites:** `n >= 4` on a saved pre-value; `n + 4 >= 4`; `n <= 0xFFFFFFFB`;
  `~n >= 4`; `n >> 2`; `n / 4`; `!(n < 0)`; `(n|0) >= 0`
- **Borrow/underflow idioms:** `r = n-4; while (r <= n)`, `while (r < n)`, `if (r > n) break`
  → `sub; cmp; movls; bls`, never fused (mwccarm 2004 predates that idiom recognition)
- **`volatile` parameter** → forces stack traffic, `0x34`
- **Flags/pragmas:** the full 20 × 7 × 8 sweep described above

## Recommendation

`notes/asm-policy.md`'s objective test is an instruction list (`mcr`/`mrc`, `swi`, `msr`/`mrs`,
`ldm`/`stm ^`, `swp`). `func_02059d8c` is ordinary ARM, so it fails that test as written and the
`fcdfc66d` reclassification was correct *by the letter of the rule*. But the rule's stated
principle is "instructions C cannot express under any compiler flags", and this function meets the
principle by a different route: it consumes a **flag** C cannot name.

Proposed addition to the policy's list, phrased to stay narrow and mechanically checkable:

> - a branch on the **carry** flag (`hs`/`lo`/`hi`/`ls`) whose flags come from a preceding
>   *value-producing* instruction (S-bit ALU op with a destination register) rather than from a
>   `cmp`/`cmn`/`tst`/`teq`. C has no carry operator, and mwccarm fuses a flag-setting ALU op with
>   its test only for result-vs-zero conditions (Z/N), so this shape cannot be requested from C.
>   Whole-arm9 audit: 3 sites, all in hand-written assembly.

Note the qualifier is load-bearing: `bhs` after a plain `cmp` is ordinary C (`if (a >= b)`) and
must not qualify. Under this addition `func_02059d8c` is reclassified ASM-PRIMITIVE alongside its
CP15 neighbors, and the wording in `src/unnamed/arm9/0205/func_02059d8c.c` calling it a
"register-allocation/scheduling wall" should be corrected — that framing implies C could reach it
with the right levers, which is disproven.

If the policy is left unchanged, the file stays `// NONMATCHING`, but this note should still be
linked from it so the next attempt does not re-grind the search space.

## Reproduction

The locality result — `subs rX,rX,#k` followed by a carry-conditional branch, three sites in all of
arm9 — reproduces with no dependencies beyond the stdlib:

```python
import struct
data = open('extracted/arm9_dec.bin', 'rb').read()
BASE = 0x02004000
w = [struct.unpack_from('<I', data, i)[0] for i in range(0, len(data) - 3, 4)]
is_subs_imm = lambda x: (x & 0x0FF00000) == 0x02500000     # subs rD,rN,#imm
for i in range(len(w) - 1):
    a, b = w[i], w[i + 1]
    if not (is_subs_imm(a) and ((a >> 16) & 0xF) == ((a >> 12) & 0xF)):
        continue                                            # same src/dst register
    if (b & 0x0E000000) == 0x0A000000 and ((b >> 28) & 0xF) in (2, 3):
        print("0x%08x" % (BASE + i * 4))                    # cond 2 = HS/CS, 3 = LO/CC
# -> 0x02059d8c, 0x0205a674, 0x0205a68c
```

Note the image base: `0x02004000`, not `0x02000000` (`tools/match.py: ARM9_BASE`). Getting that
wrong puts phantom hits in `.bss` (starts `0x0209b000`).

Disassemble the neighborhood with:

```sh
python tools/disasm.py extracted/arm9_dec.bin \
  --offset $((0x02059d70 - 0x02004000)) --length 0x2c --base 0x02004000
```

The compiler sweep drives `tools/match.py` over `tools/mwccarm/**/mwccarm.exe`; the two near-miss
sources above are the seeds.
