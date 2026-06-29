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

## Corpus result (7,180 banked matches, 2026-06-29)

| verdict | count | meaning |
|---|---:|---|
| VERIFIED | 5,275 | every reloc resolved and the linked bytes equal the ROM |
| BLIND | 1,776 | a reloc targets an invented name with no address, unverifiable |
| WRONG | 129 | a resolved reloc links to bytes that differ from the ROM |

`linkcheck` WRONG is a superset of `reloc_audit` WRONG-DEST: it catches all 118 the
audit flags plus 11 more that an address-only compare misses.

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
