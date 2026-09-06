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
    #      only when no partial_isolation block exists yet -- a second promotion
    #      (daDgr_c, daDkk_c: block already present, verify wrote nothing) is
    #      consistent with that. Check, do not assume.
    python tools/rombuild.py -j 6
    Get-FileHash build/sm64ds.nds -Algorithm SHA256
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
    python tools/cpp_tu_state.py --check-note
    #    ^ --write-note EXITS 2 on a dirty tree, which is exactly the state the
    #      rebase recipe leaves you in, and its message tells you to STASH --
    #      which this file forbids (shared across worktrees; unstages
    #      deletions). Use `git add -A`, then re-run.
    #      notes/cpp-tu-current-state.md is generated and goes stale on every
    #      promotion. Regenerate with --write-note and commit it in the same
    #      PR; a stale note on
    #      main is how the queue starts lying about what is already promoted.
    python tools/check_tubuild_conflicts.py --list
    python tools/layout_check.py
    python tools/source_coverage.py --check --base origin/main
    python tools/prepush_attribution.py --base origin/main --head HEAD

PASS signals:

- `tubuild verify` → `N/N MATCH, objisolate clean, reloc-destinations clean`
- `rombuild` → `module fidelity: 106/106 exact, 100.000000% of compared bytes`
  and `ROM-build analysis: PASS`; its stock profile must report 0 mod source
  replacements and 0 ROM-gap fallbacks. **There is no retail-SHA oracle — do not
  look for one.** The built ROM differs from the repo's own `sm64.nds` in ~54 KB
  across ~7,000 ranges, all outside the compared modules, and the manifest's
  `matchesStockRom` means "identical to this tree's last `build/sm64ds.nds`",
  not "identical to retail". The real checks are `module fidelity: 106/106
  exact, 100.000000%` plus equality with the run's own source-independent stock
  baseline control
- `source_coverage` → `0 B` handed back to the cartridge
- **`rombuild`'s BASELINE CONTROL legitimately prints
  `dsd check symbols --fail FAIL`** — 9 pre-existing errors, present on untouched
  `main`. It is absent from the PASS list above and it is not your defect, but a
  builder grepping the log for `FAIL` will stop on it. Compare the error count
  against the control run and report `0 new`
- `prepush_attribution` → **no symbol *lost*.** Do **not** hold out for
  `0 changed`: a promotion folds N shards into one file, and the counter credits
  only the delinks range's *first* symbol and reclassifies the rest as
  "claimed" — see the many-to-one fold artifact below, which lists the lines
  that are expected and are not losses. **But `changed` is not noise by
  construction, and this file used to say it was.** With correct `path#symbol`
  overrides committed, the validator reports `0 changed` and the four "expected"
  fold lines collapse to one (`N address range(s) left the byte-verified set`).
  Measured on `ov006/dScMgRoulette_c`: `32 consolidated with credit intact,
  0 changed, 0 lost`. So aim for `0 changed`; if you cannot reach it, say which
  rows resist and why, rather than writing it off as inherent. Run the check, commit what it asks for,
  report the numbers, and move on — reconciling credit beyond that is a stated
  non-goal in this repo and has consumed whole sessions before.
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

**Verify a partial's structural claim by parsing `delinks.txt` for `.text`
coverage gaps** across the whole `tu_map` run. This is the highest-value check
available on a partial and it settles the question in one pass: on
`dScMgHanachan_c` it produced 39 `.text` blocks with exactly one gap — the
sourceless hole — and the exact held-out shard count as a by-product. The
supporting invariant is worth re-deriving rather than trusting: **0 of 13,201
delink blocks and 0 of 136 TU manifests carry more than one `.text` run**, which
is why a licensed claim cannot span a hole.

**Diff the declared type and `extern` set against what the shipped members
actually reference.** A twice-narrowed TU carries preamble residue for the
functions it dropped — `dScMgHanachan_c` shipped three unreferenced shadow
structs and about thirty externs for functions outside its range, one of them
carrying a comment asserting a stride requirement belonging to a function above
the upper edge. Prose checking alone does not catch dead *declarations*.

**All-zero `romdata_check` counts are the correct result** for a TU that does not
own its key function, and are positive evidence for an empty
`compiler_only_output`. The proof-block example shows nonzero counts; do not read
zeros as a failed run.

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

**`match.py`'s API is not shaped the way that instruction implies.**
`extract_func` returns a `(bytes, relocs)` **pair**, and the signature is
`target_bytes(addr, size, bin_path: pathlib.Path, base: int)` — passing a module
label like `"ov006"` raises
`AttributeError: 'str' object has no attribute 'read_bytes'`. For an overlay pass
`pathlib.Path("extracted/dsd/arm9_overlays/ov006.bin")` with base `0x020bfec0`.

**`romdata_check`'s per-symbol verdicts are not reachable from the CLI.**
`--show` only slices `report["differing"]` and `--json` carries counts. To prove
a specific symbol's identity — which is how the cross-module RTTI claim was
settled — import the module and call `check_object()` directly. **The records DO carry an address** — this file used to say they carry `module`
but no address field. The keys are exactly `symbol, module, addr, bytes,
emitted, romExtent, blindWords, verdict`. The verdict key is `verdict`; printing
`status`/`result`/`state` gives `None`. The signature is
`check_object(obj_path, rel, names=None)` — passing a rom index positionally
raises `TypeError: got multiple values for argument 'names'`. **Pass
`names=romdata_check.name_index()`**: without it `module` comes back `None`,
which defeats the cross-module proof the call is for. The records carry `module`
but **no address field**, so addresses still need checking against `symbols.txt`
separately. That file's format is `NAME kind:<k> addr:0x<hex> ...` — one
space-separated field list per line, **not** `NAME = 0x...`. A grep written for
the `=` spelling matches nothing and reads as "symbol absent", which is the
wrong conclusion in the exact place it matters.

## Two manifest defects no gate catches

**Nothing checks a manifest's `legacy_source` paths against the tree.** Measured
on `config/tu_manifest.d/ov006/dScMgMemory2_c.json`, landed on `main`: ordinal
30 recorded the shard `_ZN14dScMgMemory2_c14RoundShowCardsEv` with a **`.c`**
extension where both the file and its `delinks.txt` entry are **`.cpp`**. (Both
spellings are written bare here rather than repo-rooted: quoting the dead one in
full would fail `check_dead_references`, which is a gate this very paragraph
would otherwise trip.) `linkcheck` refuses before doing any work
— `manifest names legacy source ..., which is not a delinks entry inside the
span` — so that class's recorded `status: link-verified` was **not
reproducible**. One bad row out of 52. When you touch a manifest, audit every
`legacy_source` against the tree; when you find one, say how many rows you
checked, not just the one you fixed.

**`linkcheck`'s report JSON and the committed manifests use different key names
for the same audit.** The live report writes `objectAudit.orderOk` /
`nonLicensed`; manifests on `main` record `emittedTextOrderIsRomAscending` /
`nonLicensedSymbols`. A script that copies one into the other silently drops the
fields — and a dropped emission-order field reads as "not audited", not as an
error.

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

**But do not use `merge-tree` to decide whether a PR will merge.** For
`config/converted-backslide-exceptions.jsonl` it is exactly the tool that says
**clean** where GitHub says **CONFLICTING**, because the file is declared
`merge=union` in `.gitattributes` and GitHub ignores the driver. Use it to
inspect content; use `gh pr view <n> --json mergeable` to decide mergeability.
Every local compose prediction made with `merge-tree` in this pipeline has
under-reported conflicts.

**Dedupe by whole record, never by path.** Rows are `{path, reason}` and one
path legitimately recurs under different reasons — `main` carries 66 such rows
from past promotions. Only byte-identical repetition is the defect. A builder
who deduped by path would delete real history.

## Check for a shallow clone before trusting ANY attribution output

    git rev-parse --is-shallow-repository        # must print false

**A shallow clone silently corrupts every attribution answer, and the wrong
answer is permanent.** `first_matchers()` / `match_finishers()` replay `src/`
history, so a truncated clone makes every lineage start at the newest commit
that touched the file — which is usually the automated refresh, i.e.
`github-actions[bot]`. `prepush_attribution` then reports dozens "lost", and
banking its suggestion writes override rows that are **highest priority and
never pruned**.

Measured 2026-09-05: this repo's primary checkout was shallow at **1,325 of
4,743 commits** for a whole session. A builder banked 40 override rows from that
output before catching it; the merge validator's "Before" column showed it would
have stripped functions from three named human contributors and mislabelled 24
more. After `git fetch --unshallow`, the recomputed owner set agreed with all 25
owners the validator printed, with **0 disagreements**.

`attribution.json` currently carries **968 of 2,036 override rows crediting
`github-actions[bot]`**, which is what this defect looks like at scale. CI is not
the source — the workflows that read history correctly set `fetch-depth: 0`.
Local agent runs are.

**An attribution override has no effect until it is committed.** With all rows
present in the working tree the gate still reported `0 consolidated, 71 lost` —
unchanged. After `git commit`, the identical rows gave `71 consolidated, 0
changed, 0 lost`. A builder who edits, re-runs, and sees no movement will
reasonably conclude overrides are the wrong mechanism. Commit, then re-run.

**So: never bank an attribution override from a shallow clone.** If
`--is-shallow-repository` prints true, run `git fetch --unshallow` and recompute
before writing anything. Worktrees share the parent clone's `.git`, so one
shallow clone makes every worktree cut from it shallow too.

## Two PowerShell traps that read as other people's bugs

- **`git commit -F @'...'@` does not fail at parse time.** PowerShell passes the
  here-string *body* as a filename, and git dies with `...: No such file or
  directory` — which reads like a git problem, not a shell one. Write the
  message to a class-unique scratch file and pass `-F <path>`.
- **No PowerShell pipeline form gives live progress from a backgrounded build.**
  `Select-Object -Last N`, `Tee-Object -FilePath` and `Out-File` all buffer
  identically; the log file stays at 0 bytes until the process exits. A
  backgrounded `linkcheck` cannot be watched at all — poll `build/tu/<id>/`
  directory mtimes instead.
- `jq` is **not** on PATH in the Bash tool here; only `gh`'s built-in `--jq`.
  A monitor built on `jq` fails silently with `command not found` and produces
  no events at all, which looks like the thing you are watching never finished.

## Do not use `git stash` in this repo

Two hazards compound. The stash is **shared across every worktree** here, so a
stash pushed in one worktree is visible — and poppable — in another. And a stash
round-trip **unstages staged deletions**: push then pop, and a 40-file deletion
set comes back as unstaged, where the next `git commit` silently omits it. A
writer caught this once mid-promotion; a promotion is exactly the change shaped
to lose that way, since deleting the shards is most of the diff. If you must
shelve work, copy files aside or commit to a scratch branch, and run
`git status` before every commit rather than trusting what you staged earlier.

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

- `config/converted-baseline.json` **conflicts loudly — but only when the landed
  promotion actually changed the CONVERTED set.** It is regenerated whole per
  promotion, so an overlapping change collides, and that is the safe one. The
  guarantee is conditional and the condition is easy to miss: a promotion whose
  absorbed shards were all *below* CONVERTED tier banks nothing, leaving the file
  byte-identical and unable to collide. Measured — `dScMgRoulette_c` (#2312), a
  40-member promotion, changed **neither** ledger file and put zero rows in the
  exceptions file. Do not infer "no conflict, therefore no promotion landed".
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

**These two rules look contradictory — here is the reconciliation.** Above, "if
`--check` passes, skip `--update`". Here, "restore both files and re-run
`--update`". Both are right; the rebase case needs the *damage detection*, not a
second banking. Do it non-destructively: copy the post-cherry-pick files aside,
`git checkout <newbase> --` both, run `--update` once, then
`git diff --no-index` the regenerated files against your copies. Identical means
the auto-merge was correct — `git checkout HEAD --` both and commit nothing.
Different means the silent merge lost or duplicated rows, and the regenerated
version is the one to keep.

**The recipe is missing its last step, and the omission is dangerous.**
`--update` *appends* to the exceptions file, so by the time you have diffed and
decided "my copy is the one to keep", the tool has **already overwritten the
working file**. Restoring your copy is a separate, explicit action —
`git checkout HEAD --` it, or copy it back. Deciding is not restoring.

**And `--check` goes red on a narrower trigger than stated.** Measured: red after
restoring **only** `config/converted-baseline.json`. The exceptions file plays no
part in the backslide verdict at all, so do not restore it "to make the check
pass" — that only risks the rows.

**Committing the regenerated exceptions file destroys the writers' reasons.**
Rows carry whatever string you pass to `--reason`, so regenerating from a clean
base and committing the result replaces N informative per-class reasons with N
identical generic ones. Nothing reads those strings, so no gate goes red and no
reviewer diff makes it obvious — it is silent data loss. This is the second half
of the non-destructive recipe above: when the regenerated file differs from your
copy *only in the `reason` strings*, your copy is the one to keep.

**A third file conflicts on a rebase and this section used to omit it:
`notes/cpp-tu-current-state.md`.** It is generated, every promotion rewrites it,
and on one measured rebase it was the *only* `UU` conflict while both ledger
files merged silently. Same rule as the ledgers — regenerate, never resolve:
`git checkout <newbase> --` it, finish the merge, then
`python tools/cpp_tu_state.py --write-note`.

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
- **`pytest tools/` is not a CI signal.** The workflows invoke targeted
  `python -m unittest tools.test_<gate>` modules only, so a green `tools` check
  does not mean the suite passes. Two tests fail on untouched `origin/main`
  (`test_dtor_members.py::test_the_frozen_census_reproduces` and
  `test_opnew_sizes.py::test_only_two_live_classes_are_genuinely_headerless`).
  Do not adopt those as your regression.
- **PowerShell `| Select-Object -Last N` buffers everything**, so a backgrounded
  `rombuild` writes a zero-byte output file until it exits. You cannot watch
  progress that way.
- Never run two consumers of `build/` at once — a backgrounded `rombuild` beside
  `eligible.py` invents link errors.

## Tools the write-up needs, and how they mislead

- **`opnew_sizes.py` prints no per-class row** — read `build/opnew_sizes.json`.
- **`check_references` and `check_dead_references` emit "run `--update` to bank
  it" nudges that are NOT failures.** Do not bank them from a class PR; they are
  someone else's baseline to shrink. The same goes for any *shrinkable baseline*
  line: a clean promotion legitimately removes dead references, so the nudge
  fires on healthy PRs. `config/unresolved-baseline.json` is maintained by the
  automated `[skip ci]` refresh — a class PR that touches it is adding noise the
  next refresh will overwrite.
- **`opnew_sizes.py` and `rtti_vtables.py` both need `build/rtti.json`**, which
  only `tools/rtti_extract.py` writes. Without it they die on a bare
  `FileNotFoundError` naming no remedy. Run the extractor first.
- **Corroborate every slot count; do not take one tool's word.**
  `rtti_vtables.py` used to over-report (34 against a real 32 on
  `daObjCtMecha03_c`, tail words *below* the overlay's load range), but it now
  trims following-table tails and has since agreed with a direct read. Keep
  corroborating; drop the assumption that it is wrong.
- **`verify` does not print a `_ZTV` section size.** I claimed it does, in this
  file and in launch prompts — it prints the MATCH table, byte comparison,
  objisolate, emission order and the result, and nothing else. Do not go looking.
- **`romdata_check`'s `romExtent` field carries the WRONG short extent** — `88`
  decimal, i.e. the false `0x58`, on the very class where the real answer is
  `0x88`. The truthful fields are **`emitted`** and **`bytes`**, with
  `blindWords: 0`. Following the extent field confirms the trap instead of
  catching it. **This is class-specific, not universal** — on
  `dScMgRoulette_c` the field reads `144`, which is correct and equal to
  `emitted`/`bytes`. So it is unreliable rather than always-wrong: never take it
  alone, and corroborate as above.
- **Print scalar fields only from a `check_object()` record.** Its `src` key is a
  dict keyed by *tuples* covering every symbol in the module, so `json.dumps`
  raises `TypeError: keys must be str...` and printing the record raw dumps about
  20 MB.
- **`check_object()` alone does nothing.** The per-symbol verdict recipe is:
  build `name_index()` and `rom_data_index()`, compile with `_compile(rel,
  tmpdir)`, *then* call `check_object()`. It is worth the trouble — it is how a
  cross-module RTTI home gets proved from the other direction
  (`_ZTI10dBgActor_c ov002 VERIFIED 12 bytes`).
- **Write scratch files under a class-unique name.** The session scratchpad is
  shared across concurrent pipeline agents; a sibling builder overwrote a
  `pr-body.md` mid-run. Use `pr-body-<Class>.md`.

**The command blocks in this file are POSIX.** The primary shell here is
PowerShell, where `git commit -F - <<MSG` is a parse error. Write the message to
a class-unique scratch file and pass `-F <path>` — the same rule this file
already gives for `pr-body-<Class>.md`.

**`tu_order_check`'s `EXTRA [...] the extras above need a compiler-only policy`
is a PASS**, not a warning, when the manifest carries the matching row.

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

**Check what you actually pushed.** The feature-branch pre-push hook does not run
the attribution gate and never creates commits. Run `prepush_attribution.py`
yourself, commit every required `path#symbol` mapping, then run
`git log origin/<branch>..HEAD`; if it is non-empty, push again. **Also re-read
the remote head after pushing** with `gh pr view --json headRefOid`. Another
pipeline agent can push to your branch, which a local extras check cannot see;
it happened twice on one PR, including after a force-push removed a repair. A PR
missing its lineage commit looks complete while silently losing contributor
credit.

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
