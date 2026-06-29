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

## Corpus result (7,180 banked matches, 2026-06-29, after the first fix pass)

| verdict | count | meaning |
|---|---:|---|
| VERIFIED | 5,320 | every reloc resolved and the linked bytes equal the ROM |
| BENIGN | 19 | only veneer/twin diffs; source is correct |
| BLIND | 1,784 | a reloc targets an invented name with no address, unverifiable |
| WRONG | 57 | a resolved reloc links to bytes that differ from the ROM |

An initial pass fixed 53 genuine wrong destinations (repoint the reference at the address
the ROM uses; matched bytes unchanged). Of the remaining 57 WRONG, 35 are genuine and still
need fixing (a tail where the swap changes codegen, plus `__sinit` initializers with several
wrong data pointers), and 22 are veneer/twin cases the tool could not auto-confirm.

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

Review the 129 WRONG matches (`linkcheck.py` prints the per-slot detail). The common
shapes are C++ destructor variants (D0/D1/D2), data symbols whose name resolves to the
wrong overlay address, and `__sinit` initializers registering the wrong data pointers.
Fix or re-match them so the headline count stays honest.
