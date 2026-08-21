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

## Two linker behaviors the checker must model (else it false-flags correct source)

`linkcheck` reconstructs the LINKED bytes, so it has to link the way the real linker does.
Two cases were initially mishandled and reported correct source as WRONG:

- relocation addend. mwccarm emits RELA relocations; every base+offset access (struct
  field, array element) is encoded as the symbol's base address plus a nonzero `r_addend`,
  e.g. `&data[i].field`. Linking only the base mis-links the slot. The linked word is
  `base + addend` (REL objects carry the addend in the slot instead). `func_relocs_typed`
  now captures it and `link_function` applies it. This cleared the multi-slot same-base
  false positives (a struct array touched through one symbol at several field offsets).
- ARM->Thumb interworking. A `BL` whose target is a Thumb symbol (a 2-byte-aligned address
  such as the SDK `CpuSet` / `WaitByLoop`) is rewritten by the linker to `BLX` (the H bit
  carries the halfword). The candidate object emits a plain `BL` relocation. `is_interwork`
  recognizes a `BLX` ROM slot whose destination equals the address the candidate names and
  reports BENIGN: only the encoding differs, the call is correct.

## Corpus result (7,180 banked matches, 2026-06-29, after the full fix pass)

| verdict | count | meaning |
|---|---:|---|
| VERIFIED | 5,366 | every reloc resolved and the linked bytes equal the ROM |
| BENIGN | 26 | only veneer / twin / ARM->Thumb interworking diffs; source is correct |
| BLIND | 1,787 | a reloc targets an invented name with no address, unverifiable |
| WRONG | 1 | a resolved reloc links to bytes that differ from the ROM |

An initial pass fixed 53 genuine wrong destinations; this pass fixed 50 more and corrected
the two checker behaviors above, taking WRONG from 57 to 1. The fixes were mechanical
symbol repoints (rename the wrong identifier to the canonical name of the address the ROM
uses) verified per function with `linkcheck` (not WRONG, still reproduces) and the byte
oracle, reverting any that perturbed codegen. Shapes fixed: data symbols resolving to the
wrong overlay, swapped pointer pairs (e.g. `Player`'s ctor/dtor callback pair registered in
the wrong order), and `__sinit` initializers registering wrong data pointers. The two
`_ZThn80_N9Animation{D0,D1}Ev` thunks were re-identified as `ModelAnim2`'s secondary-base
thunks (they branch to `_ZN10ModelAnim2D{0,1}Ev`) and renamed.

The lone remaining WRONG is `_ZThn80_N10ModelAnim2D0Ev`: the secondary-base DELETING
destructor thunk. The ROM branches it to `D0` (0x02016320); CodeWarrior emits it as a
tail-branch to `D1` for this class under all 11 toolchain versions, so source cannot
reproduce the `D0` delegation. Left as a documented codegen residual, not a source error.

## The residual blind spot

The 1,776 BLIND matches use at least one symbol whose name carries no address (an
invented name not in `symbols.txt` and not a `func_<addr>` / `data_<addr>` form). No
static tool can verify those destinations; only a fuller symbol table would. This is a
symbol-coverage problem, not a tool one.

## Usage

```sh
python tools/linkcheck.py                                              # whole corpus
python tools/linkcheck.py --module ov006 -j 10
python tools/linkcheck.py --name func_02013a88 --addr 0x02013a88 --size 0x4c --module arm9
python tools/linkcheck.py --json out.json
```

## Next

WRONG is down to 1 (the `ModelAnim2` D0 thunk above). Re-run `linkcheck.py` after any
harvest batch so newly banked matches with wrong destinations are caught while the source
is fresh; the same repoint recipe applies. Closing more BLIND would need a fuller symbol
table (addresses for the invented names).
