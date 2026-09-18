# ApplyOpacity arity: ROM-wide call-site census + control (BATCH3-2707-01, PR #2707)

Answers source-review finding BATCH3-2707-01, which asked whether the third
argument register in the retained `_ZN9ModelBase12ApplyOpacityEj` extern is a
real parameter or a zero incidentally left in a caller register.

- tested_commit: 0b68cf343 (`cleanup-leftover-overlay-batch-3` at measurement time)
- image: `extracted/dsd/arm9/arm9.bin` (618,496 bytes, loads at 0x02004000) plus
  all 104 `extracted/dsd/arm9_overlays/ovNNN.bin`, each at its
  `overlays.yaml` `base_address`. NOT the packed `extracted/arm9.bin`.
- method: scan every 4-byte-aligned word for a `BL` (bits 27:25 == 0b101, L
  set, cond != 0xF), resolve `target = addr + 8 + (simm24 << 2)`, then
  disassemble each hit's predecessors one word at a time with capstone
  (`CS_ARCH_ARM` / `CS_MODE_ARM`) so a single undecodable word cannot halt the
  walk. "Writes r2" means an instruction whose capstone `regs_access()` write
  set contains `r2`, reached without crossing an intervening `bl`/`blx`.

## The callee

```text
02016a9c  e59fc004  ldr ip, [pc, #4]      ; -> 0x020461b4
02016aa0  e2800008  add r0, r0, #8
02016aa4  e12fff1c  bx  ip
02016aa8  020461b4  .word

020461b4  e92d40f0  push {r4, r5, r6, r7, lr}
020461bc  e1a07000  mov r7, r0
020461c0  e5970000  ldr r0, [r7]
020461c4  e1a06001  mov r6, r1
020461c8  e5904024  ldr r4, [r0, #0x24]
020461cc  e3a05000  mov r5, #0
020461d0  e3540000  cmp r4, #0
020461d4  d28dd004  addle sp, sp, #4
020461d8  d8bd40f0  pople {r4, r5, r6, r7, lr}
020461dc  d12fff1e  bxle lr
020461e0  e1a00007  mov r0, r7
020461e4  e1a01006  mov r1, r6
020461e8  e1a02005  mov r2, r5     <- incoming r2 dies here, unread
020461ec  eb000005  bl  #0x2046208
```

The reviewer's reading of the callee is correct: incoming `r2` is overwritten
before any read. That test is one-way. A callee that ignores an argument
register is exactly what an UNUSED parameter looks like, so it cannot decide
the arity by itself.

## The two sites that copy a register (the finding's decisive question)

Both were disassembled in full. Neither carries a live value; both provably
carry a compile-time zero.

```text
ov002  0x020b8028  e3500001  cmp r0, #1
       0x020b802c  8a000003  bhi #0x20b8040      ; guards the whole block
       0x020b8030  e3a01000  mov r1, #0
       0x020b8034  e1a02001  mov r2, r1          <- r2 = 0, one insn old
       0x020b8038  e2840c03  add r0, r4, #0x300
       0x020b803c  ebfd7a96  bl  #0x2016a9c
```

`r1` is assigned the constant 0 in the immediately preceding instruction, so
`mov r2, r1` is a `mov r2, #0` written through a register. Nothing reads `r2`
afterwards -- the next write is `mov r2, #3` at 0x020b8048 for a different call
-- so the copy is dead unless it is an argument.

```text
ov002  0x020ec13c  e1a01006  mov r1, r6
       0x020ec140  b1a00640  asrlt r0, r0, #0xc
       0x020ec144  b20010ff  andlt r1, r0, #0xff
       0x020ec148  e1a0000a  mov r0, sl
       0x020ec14c  e1a02005  mov r2, r5          <- r5 == 0
       0x020ec150  ebfcaa51  bl  #0x2016a9c
```

This is `daTree_c::Render` itself (0x020ec0a4..0x020ec1c0; its literal pool
holds 0x02110a48, 0x0209f318, 0x0209f43c, 0x0209b3ec and 0x00578000). `r5` is
written exactly once in the entire function, at 0x020ec0d0 `mov r5, r0` with
`r0 = #0` from 0x020ec0c0, and the function has no other assignment to it. So
`r2 = 0` here too.

**Verdict on the finding's decisive question: neither site passes a live
value.** The earlier source comment's "2 move a live register in" overstated
what those two instructions show, and its split ("15 set 0, 8 set 1") was
arithmetically wrong. Corrected census over the 25 sites: 13 `mov r2, #0`,
10 `mov r2, #1`, 2 register copies of a constant zero -- value census 15
zero / 10 one.

## What does decide it: the caller-side census against a control

All 25 `bl` sites to 0x02016a9c in the cartridge write `r2` on the
straight-line path into the branch, inside the `r0`/`r1`/`r2` setup run, with
no call in between (window of 8 instructions, the same window every row of
the table below uses). Every one was inspected individually; the two whose write
sits further back (ov081:0x02126a88 at seven instructions, ov002:0x020f8160
whose `r1` is live from 0x020f813c) are still single-path with no intervening
call or branch target.

25 of 25 only means something against a base rate:

| population | sites | write r2 first | rate |
|---|---|---|---|
| every `bl` in the cartridge | 47,746 | 15,956 | 33.4% |
| 0x02016a9c (this callee) | 25 | 25 | **100%** |
| 0x0203bd6c (two-argument neighbour this TU calls) | 184 | 31 | 17% |
| 0x02015024 (two-argument neighbour) | 237 | 13 | 5% |
| 0x0203c0b4 (multi-argument neighbour) | 40 | 40 | 100% |

Only 76 of the 588 callees with 10 or more call sites (12.9%) reach 100%.
0x02016a9c sits with the multi-argument callees and nowhere near the
two-argument ones, and the value it carries varies by call site (0 at 15, 1 at
10) -- a register no caller needed would be left alone at some of 25 sites and
would not carry two different values.

## Corroboration: the thunk's own family shares the shape

0x02016a9c is one of a run of `add r0, r0, #8` this-adjusting thunks. The
adjacent thunk 0x02016aac targets 0x0204605c, which **reads** `r2` as an
element index:

```text
0204605c  e5903004  ldr r3, [r0, #4]
02046060  e3a00030  mov r0, #0x30
02046064  e0203092  mla r0, r2, r0, r3    <- r2 * 0x30 + base
02046068  e2802024  add r2, r0, #0x24
```

Its one call site sets `mov r2, #1`. So the three-register shape is genuinely
consumed elsewhere in the family; 0x020461b4 ignores slot 3 because it loops
over every index (`r5` from 0 to `[r0+0x24]`) instead of taking one.

## Disposition

The third parameter is real and unused by this member. The extern stays at
three words. `include/ModelBase.h` spells two, and 17 files across the tree
already bank the three-word form in `config/decl-agreement-baseline.json`
(`arity:3`, `mangled:3 parameter(s)`) -- including this file, unchanged by this
PR. Reconciling the header renames the symbol and touches all 17; that is its
own change. Recorded here so the next reader does not re-derive it.

- verdict: PARAMETER CONFIRMED (unused by the callee); source comment corrected
