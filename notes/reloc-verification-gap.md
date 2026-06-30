# Relocation-destination verification gap

**Status:** historical measurement note. The stronger fix now exists as
`tools/linkcheck.py`; see [`notes/link-verification.md`](link-verification.md) for the
current workflow and corpus result.
**Tools:** `tools/linkcheck.py` (current linked-byte verification),
`tools/reloc_audit.py` (older destination audit), `tools/match.py --strict-relocs`
(opt-in gate check).

## TL;DR

The match gate wildcards every relocated word without checking *where* the relocation
points, so a function can be banked as "matched" while its source calls the wrong
callee or reads the wrong global. This note documents the original audit that measured
that gap. Since then, `tools/linkcheck.py` has superseded the audit for day-to-day
verification by reconstructing each candidate's linked bytes and comparing them to the
ROM.

Corpus result (6,316 banked matches, all of which still reproduce):

| verdict | count | meaning |
|---|---:|---|
| CLEAN | 4,472 | every reloc destination verified correct |
| UNRESOLVED | 1,701 | source uses invented names; **destination unverifiable by any static tool** |
| WRONG-DEST | 107 | candidate relocation points at a different address than config |
| EXTRA | 28 | candidate relocates a word config does not |
| MISSING | 8 | config relocates a word the candidate does not |

Of the 107 WRONG-DEST, after filtering byte-identical duplicates and
interworking veneers (both behaviorally harmless), **83 are genuine concerns**
(68 of them same-module, high-confidence). That's ~1.8% of the 4,625 matches
whose destinations are checkable, ~1.3% of the corpus.

**The reviewer's theoretical gap was real and confirmed.** The audit measured the
problem; `linkcheck.py` is the link-based check that now closes it as far as the
symbol table allows. Its remaining BLIND verdicts are symbol-coverage gaps, not a
missing verifier implementation.

## The gap

`tools/match.py:compare()` (the core verifier, also used by
`tools/reverify_corpus.py:97`) compares the candidate object to the ROM
word-by-word, but at every offset where the *candidate* emitted a relocation it
forces `match = True`:

```python
if i in relocs:           # match.py:102
    match = True          # wildcard — destination never examined
```

`extract_func()` (`match.py:84-87`) keeps only the word offset and throws away
the relocation's target symbol, kind, and module. Neither `match.py` nor
`reverify_corpus.py` ever reads `config/**/relocs.txt`. So the gate cannot tell
`bl correct_fn` from `bl wrong_fn`: both are zero-filled + relocated in the
unlinked object, both get wildcarded.

The wildcard itself is *necessary* — the object is unlinked, so the slot is
genuinely zero and can't be byte-compared. The bug is that nothing checks the
destination by another route, even though the route exists: `tools/relocs.py`
already has a module-aware `(module, addr)` resolver and is used by
`clone/swarm/sweep/triage`, just never by the verifier.

## What the audit does

For each banked match `tools/reloc_audit.py`:

1. Recompiles the committed source exactly as reverify does and grabs the object
   that reproduces the ROM bytes.
2. Reads each relocation the object emits inside the function → target symbol →
   resolved destination address (from `func_<addr>` names, or the symbol index).
3. Looks up the ground-truth relocation at the same source address in
   `config/<module>/relocs.txt` (module-aware, via `relocs.py`).
4. Compares destination addresses and buckets the result.

It changes nothing in the match path. Run it:

```
python tools/reloc_audit.py                 # whole corpus (~50s)
python tools/reloc_audit.py --module ov006
python tools/reloc_audit.py --json out.json # full per-reloc detail
```

The audit was validated three ways: it reports CLEAN on a hand-traced
known-good function, it catches a deliberately corrupted config destination
(`WRONG-DEST`), and it buckets a deleted config reloc as `EXTRA`.

## Accuracy — what this can and cannot see

This section matters as much as the numbers; the audit has real limits and an
earlier revision was wrong.

**Confirmed-real examples (hand-verified):**
- `func_02007fcc` calls `ReadUnalignedInt` (`0x0200e748`); ROM relocates to
  `0x0200e728`. The two are *byte-identical duplicates*, so this one is benign —
  the source comment even names the correct address. Classified **twin**.
- `_ZN8CapEnemyD2Ev` calls `_ZN5EnemyD2Ev` (`0x020aed74`); ROM relocates to
  `0x020aed18`, a *different* destructor variant (bytes differ by the inner BL).
  Genuine wrong-target. Classified **genuine**.

**Three things the audit deliberately does NOT count as problems:**
- **twin** — candidate and config addresses hold byte-identical code (duplicate
  function). Behaviorally identical. 24 of the 107.
- **veneer** — config's target is a 12-byte ARM/Thumb interworking trampoline
  (`LDR ip,[pc]; BX ip; .word real`) that jumps to the address the source names.
  The source is correct; the linker inserts the veneer. Common for
  `operator delete` (`_ZdlPv`).
- **UNRESOLVED (1,701)** — the source declares the target with an *invented*
  name (`someFunc`, `ApproachLinear`, `CAM_DEF_A`, `Camera_vtable`, …) that
  isn't in `symbols.txt` and encodes no address. The audit could not verify these.
  `linkcheck.py` narrows this by linking every resolvable destination, but symbols
  that still cannot resolve to an address remain BLIND until the symbol table is
  improved.

**The correction worth flagging:** an earlier version of the audit also emitted a
`WRONG-MODULE` verdict (~110 matches) when a candidate's address matched config
but the *overlay module* of the symbol's name differed. Those proved to be
almost entirely **name-aliasing noise** — e.g. a C++ name like `_ZN5EnemyC2Ev`
is defined only in ov007, but an ov002 function legitimately relocates to the
same address in ov002's own image. The candidate pins an *address*, not a
module; when the address matches config, there is no observable error. That
verdict was removed (`reloc_audit.py:classify`). A genuine wrong-overlay target
still surfaces — as a `WRONG-DEST` address mismatch, not a same-address module
disagreement. **Net effect: the honest headline is 83 genuine concerns, not the
~180 an over-eager classifier would report.**

So treat the numbers as: 83 credible (68 high-confidence same-module), 1,701
unverifiable, the rest clean. The genuine cases cluster on two C++ ABI hazards —
destructor variants (D0/D1/D2) and `operator delete` — plus a tail of wrong-data
and wrong-callee references in `func_*` source.

## The opt-in gate check

`tools/match.py --strict-relocs` adds destination verification on top of the
byte compare, reusing the audit's checker (`reloc_audit.check_destinations`).
When the bytes match but a reloc points at the wrong address, the match is
rejected:

```
$ python tools/match.py --c src/func_02013a88.c --func func_02013a88 \
      --addr 0x02013a88 --size 0x4c --module arm9 --strict-relocs --brief
  1.2/sp2p3: bytes match but 1 reloc destination(s) WRONG -- not a real match
MATCHING VERSIONS: none
```

A clean function still reports MATCH. Default behavior is unchanged unless
`--strict-relocs` is passed.

**What it covers:** every reloc whose candidate target resolves to an address
(canonical names and `func_<addr>`/`data_<addr>` names) — the same set the audit
can see (CLEAN covers 4,472 of 6,316 matches fully).
**What it does NOT cover:** the UNRESOLVED invented-name slots.

## The current linkcheck workflow

`tools/linkcheck.py` is the stronger follow-up to this audit. It recompiles each
banked match, applies the candidate object's relocations with linker-accurate encodings
where the destination can be resolved, and byte-compares those reconstructed linked
bytes to the ROM. That catches wrong destination, relocation type, and addend mistakes
that an address-only audit or unlinked byte compare can miss.

Run it after harvest batches:

```
python tools/linkcheck.py
python tools/linkcheck.py --module ov006 -j 10
python tools/linkcheck.py --json out.json
```

See [`notes/link-verification.md`](link-verification.md) for the current verdicts,
known benign cases, and the remaining documented residual.

## Recommendation

1. Use `tools/linkcheck.py` as the regular corpus/batch reporting pass.
2. Keep `tools/reloc_audit.py` and `tools/match.py --strict-relocs` as targeted
   diagnostics when reviewing a specific destination mismatch.
3. Reduce BLIND by giving invented names real symbol-table addresses where possible.
