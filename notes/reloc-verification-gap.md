# Relocation-destination verification gap

**Status:** measured, fix prototyped (opt-in). Decision needed on rollout.
**Tools:** `tools/reloc_audit.py` (measurement), `tools/match.py --strict-relocs` (gate fix).

## TL;DR

The match gate wildcards every relocated word without checking *where* the
relocation points, so a function can be banked as "matched" while its source
calls the wrong callee or reads the wrong global. I built an audit that checks
every banked match's reloc destinations against `config/**/relocs.txt`.

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

**The reviewer's theoretical gap is real and confirmed. The empirical blast
radius is moderate (tens of genuinely-wrong matches), not catastrophic — but it
is unmeasurable by the existing gate, and 1,701 matches sit in a permanent blind
spot that only a link-based check could close.**

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
  isn't in `symbols.txt` and encodes no address. No static tool can verify these
  — not the gate, not reverify, not this audit. They are the true residual blind
  spot; only linking the object and comparing linked bytes would cover them.

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

## The fix (prototyped, opt-in)

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
**What it does NOT cover:** the UNRESOLVED invented-name slots. Closing those
needs the heavier fix below.

**The complete fix** (not built): link the candidate object against the module's
symbol table and compare *linked* bytes to the ROM. That checks every
destination including invented names, models veneers correctly, and removes the
wildcard entirely. It's a bigger change (needs a linker step / address binding
for the local symbol set) and is the right long-term direction if false matches
are deemed worth eliminating wholesale.

## Recommendation

1. **Adopt `--strict-relocs` in reverify** as a reporting pass first (don't
   relabel the ledger yet): run the audit, review the 83 genuine cases, fix or
   re-bank them. Several are likely quick (wrong destructor variant, off-by-one
   data symbol).
2. **Decide on the 1,701 UNRESOLVED.** They are matches whose correctness rests
   entirely on the author having declared the right thing, with zero automated
   check. If byte-exact link reproduction is a project goal, the link-based fix
   is the only thing that closes this.
3. Relative to the other two review items: the progress-ledger and
   `ingest_batch` issues are hygiene/dead-code; **this is the one that affects
   whether the headline match count can be trusted.** The measurement now exists
   — the open question is policy, not detection.
