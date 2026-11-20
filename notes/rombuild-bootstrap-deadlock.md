# `rombuild.py` cannot bootstrap in a workspace with no `build/sm64ds.nds`

`AGENTS.md` prescribes one local gate before opening a PR:

```sh
python tools/rombuild.py -j16 --no-rom
```

In a workspace that has never completed a stock ROM build, that command
**always exits 1**, and no flag avoids it. Byte reproduction is not the
problem: every run below reports `106/106 exact, 100.000000% of compared
bytes`, `3,029 functions reproducing, 0 mismatching`. What fails is a
provenance assertion about the strict control's *own* ROM.

## The cycle

1. `tools/rombuild.py` runs the strict stock control early -- step `[2/6]`,
   right after `dsd lcf`.
2. The control builds its own ROM, having demoted 350 intact TUs to
   independently extracted retail gap bytes, so its digest is
   `ddab9300b24aeae254192c2a30d812b07ece3adff83230e75c865ed915c9de40`. That
   value is deterministic -- five runs produced it byte for byte.
3. `tools/tubuild.py:6243` sets `report["rom"]["matchesStockRom"]` only in two
   cases:
   - `build/sm64ds.nds` exists, so there is a same-worker stock ROM to compare
     against; or
   - `not baseline` -- a non-baseline run, which can be graded against the
     control's digest.

   On the baseline control itself, with no `build/sm64ds.nds` present, neither
   branch fires and the key is never written.
4. `tools/tu_production.py:222` then raises, because both halves of its
   condition hold:

   ```python
   if baseline["matchesStockRom"] is not True \
           and admitted_roms != {baseline["romSha256"]}:
   ```

   `matchesStockRom` is absent (so `is not True`), and the control's
   `ddab9300...` is not the admitted bootstrap proof
   `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`.
5. `build/sm64ds.nds` is only written at the *end* of a successful run. The
   gate that blocks the run needs an artifact only a completed run produces.

## Measurements

Five runs, two different `main` commits (`07ef042f51` and `5d11ed3db8`), cold
and warm `build/`:

| Command | Result |
|---|---|
| `rombuild.py -j16 --no-rom` (cold) | exit 1, `ddab9300...` |
| `rombuild.py -j16 --no-rom` (warm, same tree) | exit 1, `ddab9300...` |
| `rombuild.py -j16` (full, with ROM packaging) | exit 1, dies at `[2/6]` before packaging |
| `rombuild.py -j16 --no-check --rom-out build/sm64ds.nds` | exit 1, dies at `[2/6]`; no ROM written |
| `rombuild.py -j16 --no-rom` (fresh `main`) | exit 1, `ddab9300...` |

So it is neither a cold-cache artifact nor a `--no-rom` artifact, and
`--no-check` does not skip the control.

## What this note does not claim

Whether this reproduces on other machines is **unresolved**. No CI workflow
runs `rombuild.py` -- the merge gate is the external `PR validation` service --
so there is no second data point to compare against. A worker that already
carries a `build/sm64ds.nds` from an earlier successful build takes the first
branch at `tubuild.py:6243` and never reaches the assertion, which would
explain the condition going unnoticed.

## Deliberately not fixed here

Two one-line changes would each clear it, and both are maintainer calls rather
than a drive-by edit, because this assertion is what protects the integrity of
the byte proof:

- **Admit the control's deterministic digest.** `ddab9300...` is reproducible
  from the current config and tools, so it could join the admitted list. That
  is a provenance policy decision about what counts as a bootstrap proof.
- **Grade the baseline control differently.** Comparing the baseline control
  against its own digest is vacuous, so the honest alternative is for
  `tu_production.py:222` not to demand `matchesStockRom` on the baseline run at
  all. That weakens an assertion deliberately placed there.

Until one is chosen, the working local gates for a rename or a match are
per-function `tools/match.py` and
`tools/prepush_linkcheck.py --range origin/main..HEAD`, both of which run fine
in this workspace, as does `python tools/tubuild.py verify <module>/<Class>`
for a promoted TU. The naming PRs #5703-#5732 were gated locally with per-TU
strict `tubuild.py verify`, then by `PR validation`.
