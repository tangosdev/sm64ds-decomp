# The 72 unmatched functions, and which of them count

Measured at origin/main c52f63ca5, from the chaos-db.json that commit's own
generator produces (11,318 matched of 11,390, the figure the README bar carried).
Written for Tango's ruling of 2026-09-09, `count them and address it somewhere in
readme`, and kept because the partition is the evidence the rule rests on.

## What the 72 are

| group | what it is | count | counts now |
|---|---|---:|---|
| a | byte-exact hand-written assembly primitives, bannered `HAND-ASM PRIMITIVE`, with the word `NONMATCHING` also present | 20 | yes |
| b | ITCM rows: 11 are the compiler's own runtime (assembly, no original C), 2 are game code that merely lives there | 13 | 4 yes, 9 no |
| c | `NONMATCHING` C drafts: the original was C and mwccarm does not reproduce it yet | 18 | no |
| d | no source in the tree under any name | 21 | no |

Group (a) plus the four resolvable rows of group (b) is 24 functions and 4,632
bytes. The published count moves from 11,318 / 11,390 to 11,342 / 11,390 and the
denominator does not move: these are functions changing sides, not appearing.

## Group (a): the assembly primitives the policy already accepts

Every one of these carries the `HAND-ASM PRIMITIVE` banner in its header region AND
the word `NONMATCHING`, because `there is no C to chase` was written as a
NONMATCHING note. The counting tools read only the second half. Each row's source
was compiled with the pinned compiler and compared to the cartridge for this census:
the byte gate is `tools/build_pin.py`, the same one the ROM build uses, so a source
that reproduces only under some other sweep member could not pass.

| module | address | size | symbol | source | banner | classify | byte gate |
|---|---|---:|---|---|---|---|---|
| arm9 | 0x0200497c | 112 | `func_0200497c` | `src/func_0200497c.c` | HAND-ASM PRIMITIVE + NONMATCHING | None | PASS 2004/b56 |
| arm9 | 0x02052514 | 60 | `func_02052514` | `src/func_02052514.c` | HAND-ASM PRIMITIVE + NONMATCHING | None | PASS 2004/b56 |
| arm9 | 0x0205256c | 28 | `func_0205256c` | `src/func_0205256c.c` | HAND-ASM PRIMITIVE + NONMATCHING | None | PASS 2004/b56 |
| arm9 | 0x020527e8 | 22 | `func_020527e8` | `src/func_020527e8.c` | HAND-ASM PRIMITIVE + NONMATCHING | None | PASS 2004/b56 |
| arm9 | 0x02052800 | 30 | `func_02052800` | `src/func_02052800.c` | HAND-ASM PRIMITIVE + NONMATCHING | None | PASS 2004/b56 |
| arm9 | 0x02052820 | 26 | `func_02052820` | `src/func_02052820.c` | HAND-ASM PRIMITIVE + NONMATCHING | None | PASS 2004/b56 |
| arm9 | 0x0205283c | 26 | `func_0205283c` | `src/func_0205283c.c` | HAND-ASM PRIMITIVE + NONMATCHING | None | PASS 2004/b56 |
| arm9 | 0x02052ec8 | 44 | `func_02052ec8` | `src/func_02052ec8.c` | HAND-ASM PRIMITIVE + NONMATCHING | None | PASS 2004/b56 |
| arm9 | 0x020553c0 | 148 | `func_020553c0` | `src/func_020553c0.c` | HAND-ASM PRIMITIVE + NONMATCHING | None | PASS 2004/b56 |
| arm9 | 0x02057014 | 12 | `func_02057014` | `src/func_02057014.c` | HAND-ASM PRIMITIVE + NONMATCHING | None | PASS 2004/b56 |
| arm9 | 0x02057020 | 88 | `func_02057020` | `src/func_02057020.c` | HAND-ASM PRIMITIVE + NONMATCHING | None | PASS 2004/b56 |
| arm9 | 0x02057078 | 48 | `func_02057078` | `src/func_02057078.c` | HAND-ASM PRIMITIVE + NONMATCHING | None | PASS 2004/b56 |
| arm9 | 0x02058568 | 100 | `func_02058568` | `src/func_02058568.c` | HAND-ASM PRIMITIVE + NONMATCHING | None | PASS 2004/b56 |
| arm9 | 0x02059468 | 20 | `func_02059468` | `src/func_02059468.c` | HAND-ASM PRIMITIVE + NONMATCHING | None | PASS 2004/b56 |
| arm9 | 0x02059824 | 16 | `func_02059824` | `src/func_02059824.c` | HAND-ASM PRIMITIVE + NONMATCHING | None | PASS 2004/b56 |
| arm9 | 0x02059d98 | 60 | `func_02059d98` | `src/func_02059d98.c` | HAND-ASM PRIMITIVE + NONMATCHING | None | PASS 2004/b56 |
| arm9 | 0x0205a588 | 148 | `func_0205a588` | `src/func_0205a588.c` | HAND-ASM PRIMITIVE + NONMATCHING | None | PASS 2004/b56 |
| arm9 | 0x0205a61c | 304 | `CpuCopy8` | `src/CpuCopy8.c` | HAND-ASM PRIMITIVE + NONMATCHING | None | PASS 2004/b56 |
| arm9 | 0x02071790 | 48 | `func_02071790` | `src/func_02071790.c` | HAND-ASM PRIMITIVE + NONMATCHING | None | PASS 2004/b56 |
| arm9 | 0x020717c0 | 76 | `__rethrow` | `src/__rethrow.c` | HAND-ASM PRIMITIVE + NONMATCHING | None | PASS 2004/b56 |

## Group (b): the ITCM vendor rows

Thirteen unmatched rows sit in ITCM. Eleven of them are the toolchain's own runtime,
shipped as assembly: soft-float add and multiply, the 32-bit and 64-bit divide
helpers, and their `__aeabi_` entry points. TWO are not vendor code at all and are
only in this group because of where they live: `dBgW_Kc::DetectClsn(dBgCh_Lin&)` and
`IRQ::UserInterruptHandler` are ordinary game code that happens to sit in ITCM, they
were C in the original, and they belong with group (d).

Four of the eleven DO have a byte-exact source in this tree, filed under the name
their author knew the routine by: config gives each of those addresses a second,
zero-size symbol, and the source is named after that one. `src/_dmul.c` decompiles
the 1,776 bytes the symbol table calls `func_01ff8708`. Looked up by the sized
record's own name, srcpath finds nothing at all, so they read as never attempted.

The other nine rows have no source in the tree under any name. They cannot count:
there is nothing to reproduce them from, and a rule that counted them would be
counting an intention rather than a match. They are left unmatched, and Tango can
rule again now that the difference is measured rather than assumed.

| address | size | symbol | alias | source | byte gate | counts |
|---|---:|---|---|---|---|---|
| 0x01ff8000 | 1436 | `func_01ff8000` | `_dadd` | none | not run | no |
| 0x01ff8708 | 1776 | `func_01ff8708` | `_dmul` | `src/_dmul.c` | PASS 2004/b56 | yes |
| 0x01ff8e10 | 1384 | `func_01ff8e10` |  | none | not run | no |
| 0x01ff9378 | 1120 | `func_01ff9378` |  | none | not run | no |
| 0x01ffa440 | 124 | `func_01ffa440` |  | none | not run | no |
| 0x01ffa594 | 1096 | `func_01ffa594` |  | none | not run | no |
| 0x01ffa9dc | 12 | `__aeabi_uldiv` | `_ll_udiv` | none | not run | no |
| 0x01ffa9e8 | 76 | `__aeabi_ulmod` | `_ull_mod` | none | not run | no |
| 0x01ffaa34 | 432 | `func_01ffaa34` | `_ll_sdiv` | `src/_ll_sdiv.c` | PASS 2004/b56 | yes |
| 0x01ffabe4 | 524 | `__aeabi_idiv` | `_s32_div_f` | `src/_s32_div_f.c` | PASS 2004/b56 | yes |
| 0x01ffadf0 | 484 | `__aeabi_uidiv` | `_u32_div_f` | `src/_u32_div_f.c` | PASS 2004/b56 | yes |
| 0x01ffb0fc | 1844 | `_ZN7dBgW_Kc10DetectClsnER9dBgCh_Lin` |  | none | not run | no |
| 0x01ffd97c | 88 | `_ZN3IRQ20UserInterruptHandlerEv` |  | none | not run | no |

## Group (c): NONMATCHING C

Real compiler floors and open drafts. The original was C, the source in the tree is
C, and mwccarm does not yet produce the cartridge's bytes from it. Nothing here
changes: these stay unmatched, which is what unmatched is for.

| module | address | size | symbol | source |
|---|---|---:|---|---|
| arm9 | 0x02009e70 | 4252 | `func_02009e70` | `src/func_02009e70.cpp` |
| arm9 | 0x02020994 | 1680 | `_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii` | `src/_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii.cpp` |
| arm9 | 0x0202cc0c | 2692 | `_ZN5Stage13InitResourcesEv` | `src/_ZN5Stage13InitResourcesEv.cpp` |
| arm9 | 0x0202ffec | 472 | `func_0202ffec` | `src/func_0202ffec.c` |
| arm9 | 0x02045b58 | 184 | `_ZN5Model27LoadCompressedTextureToVramEPcjS0_` | `src/_ZN5Model27LoadCompressedTextureToVramEPcjS0_.cpp` |
| arm9 | 0x02059d8c | 12 | `func_02059d8c` | `src/func_02059d8c.c` |
| arm9 | 0x020610fc | 44 | `func_020610fc` | `src/func_020610fc.c` |
| arm9 | 0x02068398 | 120 | `func_02068398` | `src/func_02068398.c` |
| arm9 | 0x02071644 | 80 | `func_02071644` | `src/func_02071644.c` |
| ov003 | 0x020af038 | 2100 | `_ZN12dScStarSel_c8BehaviorEv` | `src/_ZN12dScStarSel_c8BehaviorEv.cpp` |
| ov007 | 0x020bfd70 | 220 | `func_ov007_020bfd70` | `src/func_ov007_020bfd70.cpp` |
| ov009 | 0x0211145c | 380 | `func_ov009_0211145c` | `src/func_ov009_0211145c.c` |
| ov015 | 0x021114f0 | 380 | `func_ov015_021114f0` | `src/func_ov015_021114f0.c` |
| ov071 | 0x02121734 | 664 | `_ZN3MrI13InitResourcesEv` | `src/_ZN3MrI13InitResourcesEv.cpp` |
| ov074 | 0x02121380 | 884 | `func_ov074_02121380` | `src/func_ov074_02121380.c` |
| ov075 | 0x02116128 | 244 | `func_ov075_02116128` | `src/func_ov075_02116128.cpp` |
| ov075 | 0x0211621c | 916 | `func_ov075_0211621c` | `src/func_ov075_0211621c.c` |
| ov075 | 0x0211afb0 | 540 | `func_ov075_0211afb0` | `src/func_ov075_0211afb0.c` |

## Group (d): no source at all

Twenty-one functions outside ITCM with no source file in the tree under any name.
Not drafts and not primitives: work nobody has started, or started and never landed.

| module | address | size | symbol |
|---|---|---:|---|
| ov002 | 0x020bb614 | 988 | `func_ov002_020bb614` |
| ov002 | 0x020cfea4 | 724 | `func_ov002_020cfea4` |
| ov002 | 0x020d3b9c | 1440 | `func_ov002_020d3b9c` |
| ov002 | 0x020d9fec | 964 | `_ZN6Player19St_SwingPlayer_MainEv` |
| ov002 | 0x020f3de4 | 792 | `func_ov002_020f3de4` |
| ov004 | 0x020b2220 | 548 | `func_ov004_020b2220` |
| ov006 | 0x020cf2fc | 1116 | `func_ov006_020cf2fc` |
| ov006 | 0x020d01e0 | 2048 | `func_ov006_020d01e0` |
| ov006 | 0x020d27dc | 3656 | `func_ov006_020d27dc` |
| ov006 | 0x020e20bc | 1504 | `func_ov006_020e20bc` |
| ov006 | 0x020e5450 | 1376 | `func_ov006_020e5450` |
| ov006 | 0x020ea914 | 804 | `func_ov006_020ea914` |
| ov006 | 0x020ee994 | 360 | `_ZN11dScMgJump_c13OnYoshiTryEatEi` |
| ov006 | 0x020fc8c0 | 240 | `func_ov006_020fc8c0` |
| ov006 | 0x020fdaf0 | 592 | `func_ov006_020fdaf0` |
| ov006 | 0x0210c9e0 | 2076 | `_ZN12dScMgSlot1_c8BehaviorEv` |
| ov006 | 0x0211e72c | 172 | `func_ov006_0211e72c` |
| ov006 | 0x02126b4c | 920 | `func_ov006_02126b4c` |
| ov060 | 0x021140c0 | 500 | `func_ov060_021140c0` |
| ov063 | 0x02117cdc | 1916 | `func_ov063_02117cdc` |
| ov080 | 0x021261f4 | 760 | `func_ov080_021261f4` |

## Reproducing this

```sh
python tools/chaos_db_ci.py --out chaos-db.json   # the record for every function
python tools/tiers.py --from-db chaos-db.json     # MATCHED and its assembly subset
```

The byte-gate column is `tools/build_pin.py`'s `verify(src, symbol, addr, size,
module)`, which needs the extracted ROM and mwccarm and so cannot run in CI. All 24
countable rows returned `(True, '2004/b56')` on 2026-09-09.
