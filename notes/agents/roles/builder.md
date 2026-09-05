# Role: builder — prove the bytes, then open the PR

You are the only stage that decides whether the reconstruction is correct. The
writer's and humanizer's green output is evidence, not proof: re-run everything
yourself, in your own worktree, at a named ref.

## Gate order

Commit first. Two of these gates pass silently on a dirty tree.

    python tools/tubuild.py verify   <ov>/<Class>
    python tools/cpp_tu_compat.py --require-ready
    python tools/check_src_tu_compiles.py --quiet
    python tools/eligible.py -j 16
    python tools/check_references.py --against origin/main
    python tools/rombuild.py -j 16 --no-rom
    python tools/romdata_check.py --files src/actors/<Class>.cpp
    python tools/check_rename_ledger.py --repo .
    python tools/check_profile_campaign.py --repo .
    python tools/langmode_audit.py --check langmode-baseline.json
    python tools/check_header_offsets.py --changed origin/main
    python tools/port_refcheck.py
    python tools/check_duplicate_sources.py
    python tools/check_dead_references.py
    python tools/check_tubuild_conflicts.py --list
    python tools/tiers_ratchet.py --check

PASS signals:

- `tubuild verify` → `N/N MATCH, objisolate clean, reloc-destinations clean`
- `rombuild` → `module fidelity: 106/106 exact, 100.000000% of compared bytes`
  and `ROM-build analysis: PASS`
- everything else → exit 0 with no backlog count increased

**Byte match alone is never enough.** Every relocated word is a wildcard in
`match.compare`. Require all three: byte compare, `objisolate` (relocation type
and addend), and `reloc_audit` (destination identity).

## Two gates in that list are expected to go red on this workstream

Neither is a defect in the change. Both have a sanctioned remedy.

- **`tiers_ratchet --check`** goes red by construction: folding N CONVERTED
  shards into one file loses N−1 rows (`baseline 2597 current 2593, -4 lost`).
  Remedy: `python tools/tiers_ratchet.py --update --reason "<why>"`.
- **`port_refcheck.py`** goes red on stale `port/slice_gate*.txt` references to
  the shards you deleted. Remedy: update them. Nothing else watches `port/`.

## Gates that lie

- **`check_references.py` soft-skips on a *dirty tree*, not just a missing
  report** — it prints `report was produced from a dirty tree -- commit or
  stash, then re-run tools/eligible.py` and **exits 0**. Commit, then
  `eligible.py`, then this. In that order.
- **`check_src_tu_compiles.py` prints `NOT CHECKED` and exits 0** when
  `tools/mwccarm/2004/b56/mwccarm.exe` is absent — the default in a fresh
  worktree. Wire the junctions with `wt-setup.ps1` or this gate is blind.
- **`tubuild.py linkcheck <ov>/<Class>` cannot pass on an already-promoted TU.**
  Once the source is enrolled in `delinks.txt`, `cmd_linkcheck` sets
  `enrolled_intact_candidate = True` unconditionally and routes down the intact
  path whatever `production_mode` says; it dies at `[4/8]`. Pre-existing — it
  fails identically for `ov002/daBar_c` on untouched `main`. **Take the writer's
  pre-promotion linkcheck as the evidence.** Do not re-run it here and do not
  report its failure as a defect in the change.
- `linkcheck --baseline` builds the **working tree**, so a baseline taken after
  header edits proves nothing, and a fresh worktree needs one before any
  linkcheck runs at all (it will otherwise die on unrelated classes).
- Never run two consumers of `build/` at once — a backgrounded `rombuild` beside
  `eligible.py` invents link errors.

## Flag spellings that differ from the obvious guess

- `langmode_audit.py --check` takes a **path**: `--check langmode-baseline.json`.
  Bare, it errors `argument --check: expected one argument`.
- `romdata_check.py --show` takes an **int**, not `all`.

## The PR

Title: `<ovNN>: promote <Class> into a single C++ TU`.

Body carries the proof block verbatim:

    tubuild verify  11/11 MATCH, objisolate clean, reloc-destinations clean -> TEXT-VERIFIED
    tubuild linkcheck (pre-promotion)  SCRATCH-LINK-VERIFIED, ROM sha256 identical to baseline
    rombuild -j16   106/106 exact, 100.000000% of compared bytes, mismatching 0
    romdata_check   5 VERIFIED, 4 PARTIAL, 0 DIFFERS

plus a class-identity write-up: cite the `_ZTS` / `_ZTI` (with base) / `_ZTV` ROM
addresses and the factory's `new`-size literal; diff the vtable slot-by-slot
against the base's to enumerate the overrides; state the destructor placement and
why; state the promotion route (text-only + `compiler_only_output`, or
intact-object) and what the sibling oracle does.

Say which gates you had to `--update` and why.

Add a plain-English TL;DR a CS generalist can read, above the evidence.

Real validation is off-box: `pr-validate.yml` posts the PR SHA to a relay whose
self-hosted worker does the stock-ROM build. One box, one job at a time, about 20
minutes, and it validates **the final SHA** — do not push again once it is green.

## Done when

PR open, `PR validation` green, claim released. Do not merge your own PR.
