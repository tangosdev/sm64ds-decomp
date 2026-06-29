# Link-based match verification

`tools/linkcheck.py` closes the relocation-destination blind spot in the match gate
as far as the symbol table allows.

## What it does

The match gate (`match.py:compare`) wildcards every word the candidate relocates, so
it never checks where a relocation points. `reloc_audit.py` compared each reloc's
resolved destination address to `config/**/relocs.txt`. `linkcheck.py` goes the whole
way: it links the candidate by writing the resolved target into each reloc slot with
the real ARM encoding (`R_ARM_ABS32` pool words, `R_ARM_PC24` / `R_ARM_CALL` branches),
then byte-compares the linked function to the ROM. A correct match links to bytes
identical to the ROM. A wrong callee or global produces a byte difference at that slot,
which the wildcard would otherwise hide. This also catches reloc type and addend errors
that an address-only compare misses.

## Veneers and twins

A slot can differ from the ROM and still be correct source. `linkcheck` recognizes two:

- veneer: the ROM reloc points at a 12-byte interworking trampoline
  (`LDR ip,[pc]; BX ip; .word real`) that jumps to the address the source names. The
  linker inserts the veneer, so naming the real symbol is right. Common for `operator delete`.
- twin: the ROM target and the candidate's target hold byte-identical code (a duplicate
  function), so calling either is behaviorally identical.

These are reported BENIGN, not WRONG. The check reads the trampoline/twin from the ROM, so
it is best-effort across overlapping overlays; a few benign cases it cannot confirm stay in
WRONG conservatively.

## Corpus result (6,317 banked matches, 2026-06-29, after the second fix pass)

| verdict | count | meaning |
|---|---:|---|
| VERIFIED | 4,587 | every reloc resolved and the linked bytes equal the ROM |
| BENIGN | 16 | only veneer/twin diffs; source is correct |
| BLIND | 1,701 | a reloc targets an invented name with no address, unverifiable |
| WRONG | 12 | a resolved reloc links to bytes that differ from the ROM |
| NO-REPRO | 1 | source no longer reproduces ROM bytes (pre-existing) |

The second pass fixed 39 more WRONG destinations (51 → 12). The fixes were: wrong overlay
prefix in `data_ovXXX_ADDR` symbol names (`__sinit` initializers and overlay-local
globals), swapped 4th/5th arguments to `func_020733a8` ctor/dtor pair (pool constant
ordering), and swapped argument lines in `func_02030aa4`.

## Remaining 12 WRONG entries

| name | reason |
|---|---|
| `_ZN14BlendModelAnimD1Ev` | `_ZdlPv` → different `operator delete` variant; likely veneer unconfirmed |
| 6× `_ZThn80_N9Animation{D0,D1}Ev` | call `_ZN9AnimationD1Ev` but ROM uses `ModelAnim2` D1; class name mismatch in nearmiss DB |
| `func_0206a318`, `func_0206a3a4`, `func_0206a5c0` | BL→BLX false positive: `WaitByLoop`/`CpuSet` are Thumb; linkcheck encodes BL but ROM has BLX |
| `func_ov005_020c0250` | `data_020a0de8` accessed as struct array; ROM has byte-offset symbols `+1/+2/+3` |
| `func_ov098_02137eec` | `data_ov098_0213c380[idx2].b/.c` — ROM pool constants are `base+4`/`base+8`; struct layout differs |

## The residual blind spot

The 1,776 BLIND matches use at least one symbol whose name carries no address (an
invented name not in `symbols.txt` and not a `func_<addr>` / `data_<addr>` form). No
static tool can verify those destinations; only a fuller symbol table would. This is a
symbol-coverage problem, not a tool one.

## Usage

```
python tools/linkcheck.py                                              # whole corpus
python tools/linkcheck.py --module ov006 -j 10
python tools/linkcheck.py --name func_02013a88 --addr 0x02013a88 --size 0x4c --module arm9
python tools/linkcheck.py --json out.json
```

## Next

The 12 residual WRONG entries fall into three categories:

1. **Animation thunks** — `_ZThn80_N9AnimationD{0,1}Ev` thunks call `_ZN9AnimationD1Ev`
   but ROM calls a `ModelAnim2` destructor. Fix requires changing the class name from
   `Animation` to `ModelAnim2` in the nearmiss DB (`nearmiss/db.jsonl`).
2. **BL→BLX false positives** — `func_0206a318/a3a4/a5c0` call `WaitByLoop`/`CpuSet`
   (Thumb entry points). The candidate object emits a `BL` placeholder but the linker
   writes a `BLX`; `link_function()` keeps the source `0xEB` upper byte, ROM has `0xFA/0xFB`.
   These are not genuine wrong matches — fix requires `linkcheck` to handle the ARM→Thumb
   BL→BLX H-bit rewrite in `link_function()`.
3. **Struct layout / byte-offset** — `func_ov005_020c0250` and `func_ov098_02137eec` need
   source restructuring to generate the exact pool constants the ROM uses.
