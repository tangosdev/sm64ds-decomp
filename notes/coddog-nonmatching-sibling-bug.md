# coddog.py schedules NONMATCHING hatches as byte-match siblings

**Tool:** `tools/coddog.py` (fuzzy opcode-similarity scheduler)
**Type:** correctness bug in the corpus split — it inverts the tool's own goal for a
class of targets.
**Found:** 2026-07-01, live, during a 15-candidate fan-out batch.

## Summary

`build_corpus()` classifies a function as an **example-eligible "matched" sibling iff
it has a committed `src/` file** (coddog.py:86-89). But ~27 committed `src/` files are
**`// NONMATCHING` hatches** — functions that are *decompiled but explicitly NOT
byte-matchable* from C at mwccarm 1.2/sp2p3 (base-materialization / regalloc walls;
each file's first line is `// NONMATCHING: <reason> ... not byte-matchable from C`).

Those hatches are pulled into the sibling/example pool. So a target that is a fuzzy
opcode-twin of a known wall gets the **highest** `coddog_sim` (up to 1.000) and is
scheduled **first** — the exact opposite of the tool's stated purpose ("functions that
have a close matched sibling go first — they get the best in-context example, highest
hit rate"). The attached few-shot `examples` source is then the hatch's own
"not byte-matchable from C" body, which actively steers the matcher toward the wall.

## The asymmetry

`load_parked()` (coddog.py:46-65) builds a parked set from `progress/nonmatching.jsonl`
+ `nearmiss/db.jsonl`, and `build_corpus()` subtracts it **only on the unmatched/target
branch** (coddog.py:90 `elif (label, addr) not in parked ...`). It is **never** applied
to the matched/sibling branch (coddog.py:86-89). Result: NONMATCHING functions are
parked out of *targets* but still used as *examples*.

Two things make this worse:
1. `progress/nonmatching.jsonl` is **gitignored** (absent on a fresh clone), so the
   parked set is usually empty anyway — even the target-side parking silently no-ops.
2. The authoritative, portable signal is already **in the committed src** (the
   `// NONMATCHING` banner), but coddog only looks at *file existence*, not content.

## Impact (measured on origin/main @ f95f8f8, 66.0%)

- **27** committed `src/` files carry a `// NONMATCHING` marker; all 27 are currently
  example-eligible siblings.
- Their fuzzy-twins sort to the **top** of `--out` schedules and `--explain`.

## Repro

```
$ python tools/coddog.py --explain func_0200f760
func_0200f760  arm9  size 0x48  (18 ops)
closest matched siblings (fuzzy opcode ratio):
  1.000  _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn arm9  0x48   <- NONMATCHING hatch
  0.606  func_ov002_020dd8b8              ov002  0x3c              <- first real byte-match
  ...
$ head -1 src/_ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn.c
// NONMATCHING: base materialization / addressing (div=7). ... not byte-matchable from C
```

The wall-twin (1.000) buries every genuine byte-match sibling (best real one 0.606). In
a live batch `func_0200f760` was scheduled #1 and would have been dispatched with a
"can't be matched from C" example.

## Proposed fix (minimal, portable)

In `build_corpus()`, when a committed `src/` file exists, only treat it as
example-eligible `matched` if its text is **not** a NONMATCHING hatch; otherwise route
it to `parked`/exclude (like the near-miss floor). Because the marker lives in the
committed src, this works on a fresh clone and does not depend on the gitignored
`nonmatching.jsonl`:

```python
src = WL.read_src_text(name)
if src is not None and "// NONMATCHING" not in src[:200]:
    rec["src"] = src
    matched.append(rec)
elif (label, addr) not in parked and not S.is_thunk(...):
    unmatched.append(rec)     # hatch also stays OUT of the target pool
```

(Or: keep NONMATCHING functions in a third `walls` pool and use them as *negative*
examples — "this shape is a known wall for reason X" — instead of positive scaffolding.)

## Workaround (until fixed)

Post-filter the schedule: drop any candidate whose top `examples[].c_source[:200]`
contains `NONMATCHING` before selecting a batch. Doing this lifted our coddog batch to
11/15 (73%) with no wall-twins dispatched.
