# Role: builder — prove the bytes, then open the PR

You are the only stage that decides whether the reconstruction is correct. The
writer's and humanizer's green output is evidence, not proof: re-run everything
yourself, in your own worktree, at a named ref.

## Gate order

The order matters and is not the order you would guess. `tiers_ratchet --update`
edits tracked files, and `check_references.py` refuses any eligibility report
that does not match `HEAD` — so the ratchet and its commit must come **before**
`eligible.py`, not after it. Run them the other way round and you get
`report describes 7558d4ca8a9e, HEAD is 99567e1a082b` and have to redo both.

    # 1. bytes
    python tools/tubuild.py verify <ov>/<Class>
    #    ^ this WRITES to the manifest: it adds a partial_isolation block that
    #      is degenerate post-promotion (contributionEquivalent "0/22",
    #      state "derived"). Revert it. Neither the writer's commit nor the
    #      landed dBgActor_c promotion carries one.
    python tools/rombuild.py -j 6 --no-rom
    python tools/romdata_check.py --files src/actors/<Class>.cpp
    python tools/tu_order_check.py <ov>/<Class>      # takes positional ids

    # 2. ratchet, then COMMIT — before anything reads an eligibility report
    python tools/tiers_ratchet.py --check
    python tools/tiers_ratchet.py --update --reason "<why>"
    git add -A && git commit

    # 3. everything that reads HEAD or a report
    python tools/eligible.py -j 6
    python tools/check_references.py --against origin/main
    python tools/cpp_tu_compat.py --require-ready
    python tools/check_src_tu_compiles.py --quiet

    # 4. the rest
    python tools/check_rename_ledger.py --repo .
    python tools/check_profile_campaign.py --repo .
    python tools/langmode_audit.py --check langmode-baseline.json
    python tools/check_header_offsets.py --changed origin/main
    python tools/port_refcheck.py
    python tools/check_duplicate_sources.py
    python tools/check_dead_references.py
    python tools/check_tubuild_conflicts.py --list
    python tools/layout_check.py

PASS signals:

- `tubuild verify` → `N/N MATCH, objisolate clean, reloc-destinations clean`
- `rombuild` → `module fidelity: 106/106 exact, 100.000000% of compared bytes`
  and `ROM-build analysis: PASS`
- everything else → exit 0 with no backlog count increased

**Byte match alone is never enough.** Every relocated word is a wildcard in
`match.compare`. Require all three: byte compare, `objisolate` (relocation type
and addend), and `reloc_audit` (destination identity).

**`romdata_check`'s per-symbol verdicts are not reachable from the CLI.**
`--show` only slices `report["differing"]` and `--json` carries counts. To prove
a specific symbol's identity — which is how the cross-module RTTI claim was
settled — import the module and call `check_object()` directly.

## What actually goes red on a promotion

- **`tiers_ratchet --check`** goes red by construction: folding N CONVERTED
  shards into one file loses rows. Remedy below — and read it, because the
  obvious remedy double-banks.
- **`check_dead_references.py`** goes red whenever prose named a shard you
  deleted. This is the common one: it fired on `notes/bgobject-provenance.md`
  naming seven. Remedy: remove the literal token, not just the sentence around
  it — the gate reads bare `src/` tokens.
- **`port_refcheck.py`** has now come back **green** on three consecutive
  promotions (402 checked, 0 stale). Check it, but do not expect it to fire or
  go looking for a remedy it does not need.

### `tiers_ratchet --update --reason` does not dedupe

If the writer already banked rows and you then take `main`'s copy of the ledger
files — which the rebase rule requires — a second `--update --reason` **appends
duplicate rows for the same paths** rather than replacing them. Nothing warns
you. The working recipe is to restore both files first and update exactly once:

    git checkout origin/main -- config/converted-backslide-exceptions.jsonl                                 config/converted-baseline.json
    python tools/tiers_ratchet.py --update --reason "<why>"

## Your branch

The writer's worktree still has `cpp/<Class>-tu` checked out, so you cannot use
that branch name. Cut `cpp/<Class>-tu-build` from it and open the PR from there.

## Rebasing onto a moved `main` — the trap no gate catches

A class branch cut before `main` moved must be rebased, and **the two ratchet
files behave differently and only one of them tells you**:

- `config/converted-baseline.json` **conflicts loudly**. It is regenerated whole
  per promotion, so any overlap collides. That is the safe one.
- `config/converted-backslide-exceptions.jsonl` **auto-merges silently and
  reintroduces stale rows.** Measured here: a cherry-pick re-added five
  `ShipWing` rows naming the former actor-directory location for
  `d_a_obj_rc_hane.cpp`; `main` had since moved it under `src/game/actors/`.
  Nothing flagged it. No gate reads those paths — which is also why this file
  writes that filename bare rather than repo-rooted: quoting the dead path in
  full would fail `check_dead_references`, the gate this paragraph is about.

**Restore BOTH files to `main`'s version and re-run `tiers_ratchet --update`.**
Never resolve either by hand and never let the merge resolve them for you — the
tool regenerates them correctly from the current tree, and a merged result is
wrong in a way that survives every gate.

Keep `main`'s version of `notes/data/c-cpp-classification.tsv` and
`notes/data/tu-merge-candidates.json` too; those carry `main`'s own changes.

**`converted-baseline.json` is a dict, not a list.** Any "did I clobber main's
rows?" set-diff has to unwrap `["converted"]`; comparing top-level keys silently
reports no difference.

**Re-fetch `main` and re-verify more than once.** It moved twice during a single
`daPgDfdr_c` build, and the second move landed *that class's own direct base*
(`dBgActor_c`, #2269). Re-fetch immediately before opening the PR, not only at
the start.

**Re-check the base immediately before you push.** The validator test-merges
against the *exact current base* and rejects with `test merge conflicts with
exact base <sha>`. `converted-baseline.json` is rewritten by every promotion
**and** by the automated `[skip ci]` progress refresh, so an hour-old rebase is
already stale. Getting this wrong costs a full validation cycle.

## Gates that lie

- **`check_references.py` HARD-FAILS on a dirty tree or a stale report.** It
  prints `report was produced from a dirty tree -- commit or stash, then re-run
  tools/eligible.py` and **exits 1** (`sys.exit(str)` at
  `check_references.py:159`; `:155` does the same for a report describing a
  different commit). Do not read either as a soft pass. The *only* soft skip is
  when no eligibility report exists at all.
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
- `linkcheck --baseline` is needed **only if you actually run a `linkcheck`** —
  which this role is told not to do. Running it anyway costs a full scratch
  delink and link for nothing, and `rombuild.py` runs its own baseline control
  regardless. When you do need it: it builds the **working tree**, so a baseline
  taken after header edits proves nothing, and a fresh worktree must take one
  before any linkcheck or it dies on unrelated classes.
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

Those figures are **one class's example, not a target.** Paste your own.

plus a class-identity write-up: cite the `_ZTS` / `_ZTI` (with base) / `_ZTV` ROM
addresses and the factory's `new`-size literal; diff the vtable slot-by-slot
against the base's to enumerate the overrides; state the destructor placement and
why; state the promotion route (text-only + `compiler_only_output`, or
intact-object) and what the sibling oracle does.

Say which gates you had to `--update` and why.

**If the class is abstract there is no factory and no `new`-size literal.**
`daOts_c` has neither — nothing in the image allocates one, and its vtable
carries pure-virtual zero words. Say so instead of inventing a number.

**Check what you actually pushed.** The pre-push hook creates an attribution
commit and does *not* push it, so `git push` can report success and leave that
commit sitting locally. Run `git log origin/<branch>..HEAD` afterwards; if it is
non-empty, push again. A PR missing its lineage commit looks complete.

Two validator lines are expected on a promotion and are not losses. `Contributor
credit: 0 added, N changed, 0 lost` is the hook restamping authorship, and
`N address range(s) left the byte-verified set` is the counter reading a
many-to-one fold — the same run reports `Relocation check: N checked; N
VERIFIED`. Explain both in the PR body so the reviewer need not rediscover them.

Add a plain-English TL;DR a CS generalist can read, above the evidence.

Real validation is off-box: `pr-validate.yml` posts the PR SHA to a relay whose
self-hosted worker does the stock-ROM build. One box, one job at a time, about 20
minutes, and it validates **the final SHA** — do not push again once it is green.

## Done when

PR open, `PR validation` green, claim released. Do not merge your own PR.
