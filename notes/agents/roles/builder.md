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
    #    ^ this CAN write to the manifest: it may add a partial_isolation block
    #      that is degenerate post-promotion (contributionEquivalent "0/22",
    #      state "derived"). Revert it if it appears -- it does not always. The
    #      guess that it depends on status: promoted is refuted: daObjFallBlock_c
    #      had that status and verify still wrote nothing. It more likely writes
    #      only when no partial_isolation block exists yet. Check, do not assume.
    python tools/rombuild.py -j 6 --no-rom
    python tools/romdata_check.py --files src/actors/<Class>.cpp
    python tools/tu_order_check.py <ov>/<Class>      # takes positional ids

    # 2. ratchet, then COMMIT — before anything reads an eligibility report
    python tools/tiers_ratchet.py --check
    #    ^ if this PASSES, do NOT --update. The writer already banked, and
    #      --update appends rather than replaces, so running it anyway commits
    #      exactly the double-banking defect described below. Audit instead:
    #      count byte-identical whole records, and set-diff
    #      converted-baseline.json["converted"] against the base.
    python tools/tiers_ratchet.py --update --reason "<why>"   # only if --check FAILED
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

**Re-read the manifest prose against what actually shipped.** This is the single
largest class of defect found so far, and no gate looks at it. When a writer
narrows a range mid-run, the rationale written for the *earlier* plan stays
behind and can assert the opposite of what shipped. On `daObjFallBlock_c`, four
places still described the superseded 12-function state: `boundary_evidence`
claimed the set was "left whole at 12/12 **rather than** cut to the orderable
10" — contradicting the entry directly below it — a note concluded **"PROMOTION
IS REFUSED"**, the header warned "IT COSTS THE TU PROMOTION" beside a path the
file no longer occupies, and the shipped `.cpp` was still headed *"SHADOW
translation unit — NOT ENROLLED, NOT CANONICAL. This file contributes nothing to
the ROM build."* All byte gates were green throughout.

**Check every `compiler_only_output` canonical address against the owning
module's own `symbols.txt`**, and scan the tree to confirm each is defined
exactly once. Cross-module homes are normal, not a smell — but a wrong one is
invisible to the byte gates. Corroborate from a second direction where you can:
the vtable's typeinfo word relocates to the `_ZTI` address, and `check_object()`
gives a per-symbol VERIFIED.

**`verify` writes to the manifest when it FAILS, and no gate reads what it
wrote.** This file warned only about a degenerate `partial_isolation` block. A
*failing* run also rewrites the prose fields — an experimental run with a pragma
removed flipped `functions_matched: 9 -> 8` and
`every_declared_function_bytes_match: PASS -> FAIL` in the committed manifest.
Every other tool treats those as prose, so a poisoned manifest ships silently.
**Run `git status` after every `verify`, especially one you expected to fail.**

**Byte match alone is never enough.** Every relocated word is a wildcard in
`match.compare`. Require all three: byte compare, `objisolate` (relocation type
and addend), and `reloc_audit` (destination identity).

**`romdata_check`'s per-symbol verdicts are not reachable from the CLI.**
`--show` only slices `report["differing"]` and `--json` carries counts. To prove
a specific symbol's identity — which is how the cross-module RTTI claim was
settled — import the module and call `check_object()` directly. **Pass
`names=romdata_check.name_index()`**: without it `module` comes back `None`,
which defeats the cross-module proof the call is for. The records carry `module`
but **no address field**, so addresses still need checking against `symbols.txt`
separately.

## What actually goes red on a promotion

- **`tiers_ratchet --check`** goes red only when the rows are **not already
  banked** — after a rebase that restored the base's copies of the ledger files.
  It is *not* red by construction, which is how this file used to put it: the
  writer normally banks correctly and `--check` then **passes** (measured on
  `daObjCtMecha03_c`, `daObjFallBlock_c` and `daDsnBase_c`, all PASS 2674/2674).
  **If it passes, skip `--update` entirely** — running it anyway appends
  duplicate rows on an unmoved base, which is the same defect as the rebase
  case below and not only a rebase hazard.
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

    git checkout <the-new-base> -- config/converted-backslide-exceptions.jsonl                                    config/converted-baseline.json
    python tools/tiers_ratchet.py --update --reason "<why>"

**`<the-new-base>` is whatever you rebased onto — not always `origin/main`.**
When you are stacked on a sibling class branch, `git checkout origin/main --`
**permanently destroys the sibling's rows**: `write_baseline` regenerates the
whole `converted` set from the tree so the loud file self-heals, but
`append_exceptions` opens the exceptions file `"a"` and only ever appends, so
what you drop there never comes back. That asymmetry is also why the silently
auto-merging file is the dangerous one and the loudly conflicting one is safe.

**Detect the damage, do not just apply the remedy.** `git merge-tree
--write-tree <base> <head>` shows you the silent auto-merge before it lands, and
an identical-whole-record count over the exceptions file shows you double-banking.

**Dedupe by whole record, never by path.** Rows are `{path, reason}` and one
path legitimately recurs under different reasons — `main` carries 66 such rows
from past promotions. Only byte-identical repetition is the defect. A builder
who deduped by path would delete real history.

## Your branch

The writer's worktree still has `cpp/<Class>-tu` checked out, so you cannot use
that branch name. Cut `cpp/<Class>-tu-build` from it and open the PR from there.

## Rebasing when the branch you compose with has not landed

Your class branch may have to stack on a sibling's branch rather than on `main`.
Two things follow:

- **`git rebase --onto <newbase> origin/main` replays too much.** A class branch
  cut from `agents/class-pipeline` shares that branch's commits with its
  siblings, and `origin/main..HEAD` includes all of them — so they collide
  add/add on the role files and `tools/classqueue.py`, which a later shared
  commit already edited on the base. `git merge-base` reports `origin/main` and
  gives you no warning. Instead: `git reset --hard <newbase>`, then cherry-pick
  only the class's own commits.
- **Retarget the PR too**: `gh pr edit <n> --base <sibling-branch>`. Without it
  the PR's diff silently claims the sibling's work as its own.

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

**Trust `gh pr view --json mergeable` over a local merge test for these two
files.** `config/converted-backslide-exceptions.jsonl` is declared `merge=union`
in `.gitattributes`, which that file itself warns GitHub ignores. So
`git merge-tree` honours the driver and reports **clean** while GitHub reports
**CONFLICTING** on the very same pair. A green PR can flip to CONFLICTING with
nobody pushing anything, purely because the base moved — and a local compose test
will not have predicted it.

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

## Tools the write-up needs, and how they mislead

- **`opnew_sizes.py` prints no per-class row** — read `build/opnew_sizes.json`.
- **`check_references` and `check_dead_references` emit "run `--update` to bank
  it" nudges that are NOT failures.** Do not bank them from a class PR; they are
  someone else's baseline to shrink.
- **`opnew_sizes.py` and `rtti_vtables.py` both need `build/rtti.json`**, which
  only `tools/rtti_extract.py` writes. Without it they die on a bare
  `FileNotFoundError` naming no remedy. Run the extractor first.
- **`rtti_vtables.py`'s slot count is not authoritative** — the known `_ZTV`
  extent overrun. It reported 34 slots for `daObjCtMecha03_c` against a real 32;
  the tail two words sat *below* the overlay's own load range. Corroborate
  against `romdata_check`'s verified byte count (128 = 32 x 4) or the `_ZTV`
  section size `verify` prints.
- **`check_object()` alone does nothing.** The per-symbol verdict recipe is:
  build `name_index()` and `rom_data_index()`, compile with `_compile(rel,
  tmpdir)`, *then* call `check_object()`. It is worth the trouble — it is how a
  cross-module RTTI home gets proved from the other direction
  (`_ZTI10dBgActor_c ov002 VERIFIED 12 bytes`).
- **Write scratch files under a class-unique name.** The session scratchpad is
  shared across concurrent pipeline agents; a sibling builder overwrote a
  `pr-body.md` mid-run. Use `pr-body-<Class>.md`.

## Flag spellings that differ from the obvious guess

- `langmode_audit.py --check` takes a **path**: `--check langmode-baseline.json`.
  Bare, it errors `argument --check: expected one argument`.
- `romdata_check.py --show` takes an **int**, not `all`.

## The PR

Title: `<ovNN>: promote <Class> into a single C++ TU`.

Body carries the proof block verbatim:

    tubuild verify  11/11 MATCH, objisolate clean, reloc-destinations clean -> TEXT-VERIFIED
    tubuild linkcheck (pre-promotion)  SCRATCH-LINK-VERIFIED, ROM sha256 identical to baseline
    rombuild -j6    106/106 exact, 100.000000% of compared bytes, mismatching 0
    romdata_check   5 VERIFIED, 4 PARTIAL, 0 DIFFERS

Those figures are **one class's example, not a target.** Paste your own.

plus a class-identity write-up: cite the `_ZTS` / `_ZTI` (with base) / `_ZTV` ROM
addresses and the factory's `new`-size literal (`tools/opnew_sizes.py`); diff the
vtable slot-by-slot against the base's to enumerate the overrides
(`tools/rtti_vtables.py --own <Class>`); state the destructor placement and
why; state the promotion route (text-only + `compiler_only_output`, or
intact-object) and what the sibling oracle does.

Say which gates you had to `--update` and why.

**If the class is abstract there is no factory and no `new`-size literal.**
`daOts_c` has neither — nothing in the image allocates one, and its vtable
carries pure-virtual zero words. Say so instead of inventing a number.

There is a third case between the two: an **abstract base whose descendants are
concrete**. `daObjFallBlock_c` allocates nothing itself, but its four
descendants' factories each `mov r0, #0x34c` — corroborating the header's
`0x34c` span from four independent overlays. That is stronger evidence than a
single factory, so look for it before writing "no size available".

**Check what you actually pushed.** The pre-push hook *can* create an attribution
commit without pushing it, so `git push` may report success and leave that commit
sitting locally. It does not always — one measured run had the hook fire with no
commit created. Run the check regardless; drop the certainty, keep the habit. Run `git log origin/<branch>..HEAD` afterwards; if it is
non-empty, push again. **That check is not sufficient on its own** — compare
`gh pr view --json headRefOid` too. Another pipeline agent can push *to your
branch*, which a local extras check cannot see; it happened twice on one PR,
including after a force-push removed it. A PR missing its lineage commit looks complete.

Several validator lines are expected on a promotion and are not losses. All are
the same many-to-one fold artifact: the counter credits only the delinks range's
**first** symbol and reclassifies the rest as "claimed".

    Contributor credit: 0 added, N changed, 0 lost
    N address range(s) left the byte-verified set
    N more function(s) now claim a match that nothing compiles
    Byte-verified functions -N, code bytes -N,NNN

The byte figure reconciles exactly — on `daObjCtMecha03_c`, -1,236 is the eight
functions' 1,312 bytes minus D1's `0x4c`. The same run reports `Relocation
check: N checked; N VERIFIED`. Explain all four in the PR body, or the next
builder chases the ones you left out.

Add a plain-English TL;DR a CS generalist can read, above the evidence.

Real validation is off-box: `pr-validate.yml` posts the PR SHA to a relay whose
self-hosted worker does the stock-ROM build. One box, one job at a time, about 20
minutes, and it validates **the final SHA** — do not push again once it is green.

## Done when

PR open, `PR validation` green, claim released **if one exists** — a class the
writer already released, or that was never claimed, has no `refs/claims/*` ref
and nothing to release. `python tools/classqueue.py status <Class>` answers this
read-only. Do not merge your own PR.
