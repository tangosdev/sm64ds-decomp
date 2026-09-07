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

**Stronger, and this is the part the ordering above understates: `eligible.py`
must be the LAST command before `check_references`.** On a rebase the ledger, the
regenerated note and the attribution commit each land *after* the rebase and each
one restales the report. `check_references` hard-fails on a stale report — it
does not soft-skip — so re-run `eligible.py` immediately before it, every time,
not merely once after the rebase.

    # 1. bytes
    python tools/tubuild.py verify <ov>/<Class>
    #    ^ this CAN write to the manifest: it may add a partial_isolation block
    #      that is degenerate post-promotion (contributionEquivalent "0/22",
    #      state "derived"). Revert it if it appears -- it does not always. The
    #      guess that it depends on status: promoted is refuted: daObjFallBlock_c
    #      had that status and verify still wrote nothing. The follow-up guess --
    #      that it writes only when no partial_isolation block exists yet -- is
    #      ALSO refuted: dScMgMemory2_c has no such block and verify wrote
    #      nothing across 5 runs. No correlate is known. Run `git status` after
    #      every verify; do not predict it.
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
    python tools/queue_audit.py --check
    #    ^ RED after any promotion until you regenerate the queue with --write:
    #      your class's row still says `promoted no` with the old shard_count.
    #      It needs build/rtti.json AND a build/tu_map.json NEWER than config/,
    #      and refuses with a bare regeneration hint otherwise -- so run
    #      `python tools/rtti_extract.py` and
    #      `python tools/tu_map.py --out build/tu_map.json` first. It is not
    #      wired into any workflow, so nothing else will catch a stale queue.
    python tools/cpp_tu_state.py --check-note
    #    ^ --check-note is safe on a tree whose dirt is OUTSIDE the authority
    #      inputs: measured with an unrelated unstaged change, it printed
    #      `cpp-tu state note is current`, exit 0. It is NOT unconditionally
    #      safe -- an unstaged edit to src, src_tu, include or config makes it
    #      exit 2 with `authority inputs have unstaged/untracked changes`,
    #      naming the file. Stage the edit and re-run; do NOT git stash, which
    #      is shared across every worktree of this clone.
    #      It reports stale only when the note file itself differs from a
    #      regeneration. The dirty-tree refusal below belongs to --write-note
    #      (and the one that hard-fails on an unstaged tree is
    #      check_references.py, a different tool).
    #      --write-note EXITS 2 on a dirty tree, which is exactly the state the
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
  rows resist and why, rather than writing it off as inherent. **`0 changed` is
  a target, not a merge bar** — the landed `dScMgHanachan_c` promotion (#2309)
  measures `43 changed, 0 lost`, exit 1, at its own commit. Judge on `lost`.

  **It is NOT the same computation as the validator's credit line on a
  many-to-one fold, and this file used to imply that it was.** Measured on the
  52-member ov006/`dScMgMemory2_c` promotion: locally `0 changed, 0 lost`, at the
  validator **`11 changed`**. The two disagree about which side is *before* — for
  `dScMgMemory2_c_classInit` the local `--json` gave `before=tangosdev,
  after=bmanus2` while the validator reported `bmanus2 -> tangosdev`, exactly
  inverted. So do not build overrides from the local tool's "was" column and
  expect the validator to agree. It still answers the question that gates a
  merge: without those overrides the local tool measured **38 lost**, and `lost`
  fails a merge while `changed` is explicitly not a blocker.

  **A second, independent mechanism for the same divergence, measured on the
  301-member ov002/`Player` fold: a symbol with NO override row at all.** Local
  reported `0 changed`; the validator reported **18 changed**, because for those
  18 the resolver fell back to the *promotion commit's* author — moving them off
  `tangosdev` (8), `ruspecial` (6) and `lunavyqo` (4). The repair is to restore
  them from the validator's own "Before" column, after which the re-run is
  `0 changed`. Two different causes, one rule: **a local green here does not
  predict the gate on a many-to-one fold.**

  **`prepush_attribution --json` takes a PATH argument**, not a bare flag. `--json`
  alone errors `expected one argument`; this file used to imply it prints to stdout.

  **The mechanism this file never named, which cost a builder real time:** the
  overrides live in `attribution.json` at the repo **root**, in the `overrides`
  dict, keyed `src/<tu>.cpp#<symbol>`. `prepush_attribution --json` prints the
  rows verbatim, so you can lift the keys straight out of it. Two traps: an
  override has **no effect until it is committed**, and a key must match the
  file's real extension — `main` carries a `..._c14RoundShowCardsEv.c` key for a
  file that is `.cpp`, and that single mis-spelled key kept one member reporting
  CREDIT CHANGED after the fold. Add the correctly-spelled key; never prune. Run the check, commit what it asks for,
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
supporting invariant is worth re-deriving rather than trusting, and it *has*
moved — **six times in a single day of promotions**: 13,201/136, then 9,998/142,
10,181/138, 10,005/142, 9,663/145, 9,656/145, all measured by different builders
within hours of each other. Any number printed here is stale before it is read,
because promotions collapse blocks. The invariant itself holds — 0 blocks and 0
manifests carry more than one `.text` run — which is why a licensed claim cannot
span a hole. **Re-derive the counts at your own base; never quote them.**

**Never read a gate's verdict through a pipe.** `python tools/source_coverage.py | tail -6;
echo $?` reports **`tail`'s** exit status, not the tool's. Measured: a builder
ran `source_coverage` that way, the tool printed its full failure banner into
the truncated tail, `$?` came back 0, and they recorded it as green. Redirect to
a file and echo `$?` *before* any pipe:
`python tools/source_coverage.py > /tmp/x.log 2>&1; echo $?; tail -6 /tmp/x.log`. Two related
parsing traps: `gh pr checks` output is **TAB**-separated, so `awk '{print $2}'`
on `PR validation<TAB>pending` yields `validation` and a wait-loop exits
immediately looking settled — use `awk -F'\t'`. **It also exits 8 while any check
is still pending**, so a loop that treats non-zero as failure reads a merely
queued validator as a hard failure: judge on the pending/pass/fail column, never
on `$?`. And `classqueue.py claim
--worktree` via a shell **eats backslashes**, recording
`C:tmpsm64ds-bomroom-build`, so pass the path with forward slashes.

**Diff the declared type and `extern` set against what the shipped members
actually reference.** A twice-narrowed TU carries preamble residue for the
functions it dropped — `dScMgHanachan_c` shipped three unreferenced shadow
structs and about thirty externs for functions outside its range, one of them
carrying a comment asserting a stride requirement belonging to a function above
the upper edge. Prose checking alone does not catch dead *declarations*.

**All-zero `romdata_check` counts are the correct result** for a TU that does not
own its key function. The proof-block example shows nonzero counts; do not read
zeros as a failed run.

**They are NOT evidence for an empty `compiler_only_output`, and this file used
to say they were.** `romdata_check` reports on *data* symbols. ov066/`Eyerok`
measures 0/0/0/0 with a non-empty **two-row** `compiler_only_output`, because
both rows are functions. Read all-zero as "no data rows", never as "no rows".

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

**And "prose fields" understates the blast radius.** On a measured negative
control the failing run also rewrote the entire `droppedSections` list
(`[9,10,80,81,...]` -> `[5,6,7,8,...]`) alongside `functions_matched: 48 -> 45`.
That list is not prose — it is the compiler-only policy's section index list.
A negative control you run for five seconds can leave a wrong licensing list in
a manifest you then commit.

**Byte match alone is never enough.** Every relocated word is a wildcard in
`match.compare`. Require all three: byte compare, `objisolate` (relocation type
and addend), and `reloc_audit` (destination identity).

**And all four of those together still do not prove the TU LINKS.** Measured on
`ov006/dScMgPanel_c`: 71/71 MATCH, objisolate clean, reloc-destinations clean,
`[4b]` object audit `order_ok True, {'LICENSED': 71}, 0 refusals` — and the
object carried **41 undefined symbols that do not exist anywhere in the image**,
because a declaration got the wrong linkage. Every byte gate is happy with a
relocation against a name nobody defines; only the linker is not. **Add one
cheap check to the sweep**: read the compiled object's `.symtab` and list the
`SHN_UNDEF` entries (a ~20-line ELF walk over
`build/tu/<ov>-<Class>/<Class>.o`). Any `_Z…`-mangled name whose demangling is a
plain C ROM symbol — `_Z8LoadFilei`, `_Z19func_ov006_021063a0Pv` — is a linkage
bug, and it costs seconds where finding it through a failed `rombuild` costs an
hour. Legitimate `_ZN…` / `_ZT…` names are not what you are looking for.

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

## Catching up to a moved `main` — the trap no gate catches

**First, decide which operation you are allowed.** If the branch has already been
pushed, you **merge** `origin/main` into it — a rebase makes the next push a
non-fast-forward against the remote's copy of the same commits, and repairing
that costs an add/add conflict on the promoted `.cpp`. Rebase (or the
reset-and-cherry-pick above) is only for a branch that has never left this
machine.

Either way **the two ratchet files behave differently and only one of them tells
you**:

- `config/converted-baseline.json` **conflicts loudly — but only when the landed
  promotion actually changed the CONVERTED set.** It is regenerated whole per
  promotion, so an overlapping change collides, and that is the safe one. The
  guarantee is conditional and the condition is easy to miss: a promotion whose
  absorbed shards were all *below* CONVERTED tier banks nothing, leaving the file
  byte-identical and unable to collide. Measured — `dScMgRoulette_c` (#2312), a
  40-member promotion, changed **neither** ledger file and put zero rows in the
  exceptions file. Do not infer "no conflict, therefore no promotion landed".
- `config/converted-backslide-exceptions.jsonl` **auto-merges silently and
  reintroduces stale rows** — under a merge exactly as under a cherry-pick, since
  it is the union driver doing it, not the replay. Measured here: a cherry-pick
  re-added five
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

**In a merge train the conflict set is bigger than one file, and every builder
hit the same three.** Across six promotions rebased onto each other in one
afternoon, the recurring `UU` set was `notes/cpp-tu-current-state.md`,
**`attribution.json`** and, on the larger folds, **`config/converted-baseline.json`**.
The last two are sorted, keyed **sets**, so resolve them as **unions**, line-wise,
and never re-serialise: a reorder conflicts with every other open PR in the train.
The audit to run afterwards is `resolved == exact union of both parents` with
0 missing, 0 invented, 0 value conflicts and 0 duplicate keys — three builders ran
it and all three came out exact (2,307 = 2,277 + 2,267 on one; 2,340 = 2,307 + 33
on the next). For `converted-baseline.json`, check the count *and* the swap: one
fold showed 2,681 held constant with 21 shard paths replaced by 21
`<Class>.cpp#symbol` rows.

**And `converted-baseline.json` is the dangerous one precisely because it often
merges silently.** On the 52-member `dScMgMemory2_c` fold it auto-merged with no
conflict at all — run the count-and-swap audit anyway. **The swap does not have
to balance**: that fold went 4 shard paths out against 3 member rows in, net −1,
because `D0` legitimately fails the readability criteria in merged form. An
unbalanced swap is normal, and the rows in `converted-backslide-exceptions.jsonl`
are exactly what banks the difference — `tiers_ratchet --check` then passes at the
lower count. Assert the shape (every removal a plain shard path, every addition a
TU member row, main's other rows untouched), not the arithmetic.

**Expect `main` to move under you far more than this file used to claim.** It
said twice is normal and six is the record; during a single afternoon's train it
moved **seven** times, four of them inside one builder's run — once between a
final gate sweep and its push, and once mid-command through the shared `.git`.
Every base-relative gate (`source_coverage`, `prepush_attribution`,
`cpp_tu_state --check-note`) goes red with *other people's* work when that
happens; the symptom is a gate screaming about an overlay you never touched.

**The mechanism, and the check that survives it: sibling worktrees share one ref
store, so `origin/main` can advance between your `git fetch` and your
`git rebase origin/main`.** The SHA you verified is not necessarily the SHA you
rebased onto — one builder confirmed `5604d114a` and landed on `ce2cf87c1` minutes
later, because another agent's fetch advanced the shared ref. A pre-flight
`git log -1 origin/main` therefore proves nothing. Verify **after** the fact:
`git merge-base HEAD origin/main` equal to `HEAD~N`, and
`git log --oneline HEAD..origin/main` empty.

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
(`dBgActor_c`, #2269). **Twice is the old rate: a later `dScMgBomroom_c` build
saw `main` move six times**, one of them a same-overlay promotion that conflicted
on both `attribution.json` and `notes/cpp-tu-current-state.md`. Re-fetch
immediately before opening the PR, not only at the start.

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
  fails identically for `ov002/daBar_c` on untouched `main`, and a second
  control taken on the `dScMgPanel_c` run reproduces it on
  `ov006/dScMgTeresa_c`, an already-promoted text-only TU that run never
  touched: `FAIL src/actors/dScMgTeresa_c.cpp: isolate: intact TU preparation
  refused: ov006/dScMgTeresa_c: intact production requires one .text claim and
  at least one non-text claim`. The exact chain is
  `tubuild.py:4613` (`enrolled_intact_candidate = True` for any entry already in
  `delinks.txt` that is neither `partial` nor `partitioned`) ->
  `tubuild.py:4777` (`intact_override[entry["source"]] = entry`) ->
  `tu_production.py:41`, which refuses a manifest with no non-text claim. A
  text-only promotion has `data: []`, `bss: []` and no `production_mode` key at
  all, so it can never satisfy it. `rombuild.intact_tu_policies` skips entries
  without `production_mode: "intact-object"`, which is why the normal build does
  not take this path. **Never add a `production_mode` or a fake non-text claim
  to make it go away** — `rombuild` would have to eat the lie later.
  Do not report its failure as a defect in the change.

  **But do NOT take the writer's recorded linkcheck as proof of the link.** This
  file used to say to, and on `ov006/dScMgPanel_c` that was wrong: the manifest
  recorded `result: scratch-link-verified` with `phases.link: true`, and the
  source as shipped **did not link at all** — 41 mangled undefined symbols, and
  mwldarm aborting with 22 `Undefined`. **The link proof you can still run is
  `rombuild.py` itself.** Its BASELINE CONTROL is a full scratch delink, lcf,
  whole-tree `mwccarm`, `mwldarm` link and ROM build of the **working tree**, and
  a text-only promoted TU is compiled and linked inside it like any other
  enrolled source (only `production_mode: intact-object` sources are demoted to
  ROM gap bytes). So `rombuild` reaching `module fidelity: 106/106 exact` *is*
  the link evidence for a text-only promotion, and a fresh worktree that has to
  bootstrap that control will surface the failure whether you asked for it or
  not. In `dScMgPanel_c`'s case the whole diagnosis came out of
  `could not bootstrap strict stock control`, which reads like a stale-baseline
  complaint and is not one — read the `LINK FAILED` block above it.
  **But do not leave the `[4b/8]` gate unverified just because `linkcheck`
  refuses.** The object audit behind it is reachable in about a minute without a
  linkcheck at all: `_compile_tu(entry)` -> `apply_compiler_only_policy` ->
  `audit_tu_object` -> `object_audit_refusals`. That gives you
  `emittedTextOrderIsRomAscending`, the LICENSED count and the refusal list
  directly. Run it, and run it a second time with the suspect line removed as a
  negative control — on `dScMgMemory2_c` that turned "the pragma is
  load-bearing" from the writer's claim into the builder's measurement
  (`order_ok True, {'LICENSED': 52}` with `#pragma defer_codegen off`;
  `order_ok False -> object-audit-refused` with only that line deleted).

- **`source_coverage --check` and `prepush_attribution` are BASE-RELATIVE, and a
  stale base makes both scream about other people's work.** A `main` that moved
  mid-build produced `REGRESSION: 4,672 B stopped being built from source`
  naming five files in ov002/ov005/ov034 with nothing to do with the class, and
  a `CREDIT LOST` for a different class entirely. Both read exactly like real
  regressions. Re-fetch and **merge** `origin/main` before believing either;
  after the merge both were clean. **Merge, do not rebase** — a rebase of a
  branch that is already on the remote makes the next push a non-fast-forward
  against the remote's copy of the same commits, and repairing that costs an
  add/add conflict on the promoted `.cpp` itself. The same applies to `git ls-tree origin/main` — worktrees
  share one `.git`, so another agent's fetch moves `origin/main` under you, which
  is enough to make an existing file look deleted.

- `linkcheck --baseline` is needed **only if you actually run a `linkcheck`** —
  which this role is told not to do. Running it anyway costs a full scratch
  delink and link for nothing, and `rombuild.py` runs its own baseline control
  regardless. When you do need it: it builds the **working tree**, and it
  fingerprints `trackedConfigArm9Sha256` — so **any `config/` edit invalidates
  it, a `delinks.txt` splice included**, not just a header edit. That splice is
  the most common edit in a promotion. The symptom does not point at you: a
  stale baseline surfaces as `[4/8]` failures on *unrelated* TUs —
  `daObjRcCarpet_c` and `daPropeller_Heyho_c` are the two that show up —
  reported as baseline-fingerprint drift. Retake the baseline after the splice
  before believing any unrelated-TU failure, and a fresh worktree must take one
  before any linkcheck or it dies on unrelated classes.
- **The `tools.test_<gate>` invocation below does not work from the repo root
  for every gate, and the module you want may not exist at all.** `python -m
  unittest tools.test_classqueue` dies with `ModuleNotFoundError: No module
  named 'classqueue'`, because the test does a bare `import classqueue`; run it
  as `cd tools && python -m unittest test_classqueue` (3 tests, OK). And there
  is no `test_langmode_audit` module under `tools/` whatsoever, nor a
  `test_queue_audit` for the queue auditor -- a missing module is a gap in
  coverage, not a regression you introduced. (Spelled without the directory
  prefix on purpose: this gate reads a `tools/...` token in prose as a path
  claim and goes red on it.)
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
- **Do not trust `romdata_check`'s `romExtent` field alone** — see
  `notes/agents/roles/writer.md`'s romExtent trap passage (the
  `88`/`0x58`-vs-`0x88` measurement is canonical there; this file only ever
  restated it). Short version: `emitted`/`bytes` with `blindWords: 0` are the
  truthful fields, and the field is class-specific-unreliable, not universally
  wrong — corroborate as above.
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
(`tools/rtti_vtables.py --own <Class>` — **pass the cartridge's RTTI spelling,
not the coined name**: `--own Goomboss` prints `no vtable for Goomboss`, which
reads as "no vtable exists", while `--own daKuriKing_c` prints the 31 slots.
`writer.md` gained this caveat in #2321; this file did not contain the word
"coined" at all); state the destructor placement and
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

**These four N's are not one number, and reading them as a family invites a
wrong PR body.** On a many-to-one fold the ranges-left count is exactly **one
higher** than the byte-verified function delta, because the TU's own new range
re-enters the set as the shard ranges leave: the 52-member `dScMgMemory2_c`
promotion reports 52 ranges left against -51 functions.

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
