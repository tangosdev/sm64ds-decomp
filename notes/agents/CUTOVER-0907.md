# The v1 to v2 cutover, and what the inventory found

Measured 2026-09-07 against `origin/main` at `f327f7b6`. This supersedes the
operational parts of [IN-FLIGHT.md](IN-FLIGHT.md), which was written the same day
*before* the upgrade and which asks you not to trust its own numbers. Three of its
claims are corrected below.

Like that file, this one starts rotting immediately. The queue is the live record;
this is the reasoning behind it.

## The cutover happened

- **Workflow commit: `f327f7b6`** — every restarted session pins this. It is the
  merge of #2396 and it is what `main` carries.
- **Queue initialized** on `refs/heads/agents/coordination`, session `fleet-0907`.
  That branch holds `state.json`. It is metadata; never merge or check it out.
- **Stop condition met.** `refs/claims/*` was empty, `classqueue.py list` reported
  no live claims, the tangos.dev range service reported zero claims project-wide,
  and no `mwccarm`/`tubuild`/`rombuild` process was running. None of that is proof
  on its own — the protocol is right that silence is not a stop — so the two live
  agent sessions were confirmed idle by a human before initialization.

Six tasks are registered: [#2409](https://github.com/tangosdev/sm64ds-decomp/issues/2409) (the gate fix) and the five
classes of the stopped wave — [#2410](https://github.com/tangosdev/sm64ds-decomp/issues/2410) `daMky_c`,
[#2411](https://github.com/tangosdev/sm64ds-decomp/issues/2411) `da1up_c`, [#2412](https://github.com/tangosdev/sm64ds-decomp/issues/2412) `daGmch_c`,
[#2413](https://github.com/tangosdev/sm64ds-decomp/issues/2413) `daBmb_c`, [#2414](https://github.com/tangosdev/sm64ds-decomp/issues/2414) `daMip_c`.

## Three corrections

**1. The gate defect does not stop all five.** #2400 says one validator line stops
the whole wave. It stops one PR. `daMky_c`'s private validation *passed* — 686
symbols exact, no lost-symbol line — and its red gate is `check_rename_ledger`,
eight historical rows still asserting the coined spelling. Only `da1up_c` has a
confirmed `rom_data_regressions` failure; `daGmch_c`'s validation was still queued
when this was written and its predicted failure is a prediction, not a result.

**2. "The validator restores `tools/` from the base commit" is out of date.** That
was retired on 2026-08-30 by `fa5aa1f65`, and `plan-cpp-conversion-queue.md` says
so. The conclusion survives for a different reason: the *base* ROM report is built
at `baseSha` and may be served from a per-SHA cache, so the base side of the
comparison would still carry no addresses. The gate fix still has to land alone
and first — cite the cache, not the restore.

**3. #2392 is not that fix.** It relaxes the function-coverage denominator so a
symbol-table SPLIT can pass, which is what #2360 needs. Its diff contains no
`rom_data_regressions`. Every other unmerged `validator/*` and `tools/*` branch was
checked; the rename-awareness work is unstarted.

## Counting branches: the method matters more than the number

309 worktrees, 148 unmerged remote branches, 61 local `cpp/*` branches whose
commits are on no remote.

A first pass called all 61 unbanked. That was wrong, and wrong in an expensive
direction: it diffed whole path sets including `converted-baseline.json`,
`attribution.json` and the overlay `delinks.txt`, which drift on every branch
because `main` moved. Re-measured against **the class's own files** — its actor
source, its `tu_manifest.d` entry, its header:

| verdict | count | meaning |
|---|---|---|
| LANDED | 14 | content on `main`; the local commits are redundant |
| UNBANKED-DELTA | 8 | file exists on `main` but differs |
| UNBANKED-NEW | 39 | adds class files `main` does not have |

`dScMgTeresa_c`, `Goomboss` and `Player` all read as unbanked under the loose test
and are byte-identical to `main` under the strict one. **Diff the class files, not
the aggregates.** The aggregates are the integration lane's problem.

**Correction, later the same day: the UNBANKED-NEW column above is still wrong, and
the strict test was not strict enough.** It asked whether a class file exists at its
branch path on `main`. PR #2270 had moved actor sources to `src/game/actors/` and
renamed their stems to `d_a_<snake>`, so a *move* reads as an *absence* — and one
case renamed the stem as well, from a class-named `daEyBm_c` source to a
`d_a_ey_bm` one, which no directory-swap heuristic finds. Eight tasks were registered off
those rows and all eight were already banked; every one was cancelled. A parallel
inventory of the `tu/*` lane hit the third variant of the same mistake, where class
and manifest RENAMES made 54 of 79 already-promoted classes read as unlanded.

The rule that covers all three: **key the comparison on something `main` cannot
move** — blob hashes across every plausible path, or `(module, ROM address)` for a
symbol, with the module included because overlays reuse addresses. And note that the
files a promotion DELETES being absent from `main` is proof it landed, not evidence
of missing work; reading that backwards is what inflated the count.

## Do not delete

- **`cpp/promote-da-obj-lava`** — 55 class files absent from `main`, PR #2231 closed
  unmerged. Twelve of them carry inline `~Class() {}` destructor bodies that exist
  nowhere else: `daObjC0Water_c`, `daObjC1Peach_c`, `daObjClock_c`, `daObjFire_c`,
  `daObjHeart_c`, `daObjKm2_Ami_Bou_c`, `daObjKm2_Nobiru_c`, `daObjKm2_Ukishima_c`,
  `daObjLava_c`, `daObjTtWater_c`, `daObj_volcanoCannon_c`, `daRFlag_c`. Five more
  classes exist on `main` *without* the inline form. IN-FLIGHT.md warned that two
  older branches carry irreplaceable inline-dtor flips; auditing that set found
  **one**, this one. The second was not located. Do not assume it was imaginary.
- **`cpp/lifecycle-ownership-16`** — the deepest of a six-branch stack (6 ⊂ 8 ⊂ 10 ⊂
  12 ⊂ 14 ⊂ 16). 34 of its files are absent from `main`. Keep 16; the rest are
  subsets, but verify before retiring any of them.
- **`cpp/next-migration`** and **`cpp/enemy-subclasses-2`** — their merged PRs
  (#1708, #1387) covered only early commits. The later ones never landed.

## Two commits were reachable from nothing

`d2ae80fa` ("Refresh C++ TU project state") and `c28a236f` ("Clarify dead-reference
baseline updates") sat in detached worktrees on no branch, local or remote — one
garbage collection from gone. They are now `rescue/actord-vfy-d2ae80fa` and
`rescue/tool-gates-review-c28a236f` on `origin`.

Four branches held commits on no remote ref and were pushed:
`verify/carpet-toolvfy-0830` (15), `chore/profile-management` (2),
the `profile-lifecycle-crosswalk` docs branch (1). The fourth, `constitution-lane`, had
**diverged** — the remote carried 105 commits the local lacked, so a force push
would have destroyed them. Its local tip went to
`rescue/constitution-lane-local-b4dc998c` instead.

One match had landed without its credit. `func_ov006_0212a764` is on `main` and the
delinks route to it, but the winning attempt and its provenance row were sitting
uncommitted in a working tree. Recovered in #2408.

**Only six uncommitted things in the whole fleet were unique.** The rest of the
dirty state is regenerable tool output or stale copies of what already landed.

## The three unadopted lanes, now inventoried

All three were scouted after the cutover. Two of them are finished work, not backlog.

**The profile-reconstruction campaign is complete and merging any of it is a
regression.** All 53 branches are landed; 46 have a merged PR and none has an open
one. Across 51,631 symbol deltas there is not one case where a branch holds a real
name and `main` holds a placeholder. Merging 50 of the 53 would revert between 261
and 781 ROM RTTI names, including 56 coined-name reversions such as `Exit` for
`daChScene_c` and `StarDoor` for `daStarGate_c`. **The `wave24` lead recorded above is
refuted and was inverted:** its commit is accurate only about that branch's own merge,
which discarded 99 campaign renames inside the branch and then re-applied them there.
`main` never had them to lose, and carries all 99 today. Its substantive content
landed via #2235.

**The `tu/*` promotion lane is mostly finished too.** 54 of the 79 classes are already
promoted on `main` under renamed ids and moved paths. Only 25 are genuinely open and
only 6 contested, each with a recommended lineage. No manifest falsely claims
`promoted` — 146 checked, zero failures. `tu/promote-batch-2` remains the sole carrier
of an inline-destructor census, now 17 classes rather than the 43 first recorded, and
stays on the do-not-delete list. Two earlier entries are settled and no longer needed:
`main` has `~dPathLiftActor_c(){}`, and `WDW_Water` promoted without the inline form.

**The port lane is the one with real unbanked work, and one branch in it is
irreplaceable.** `port-linux-fwd-sweep` is contained in no other ref and is the only
Linux/GCC/SDL2 host lane: 20 files exist nowhere else, among them a Linux
host-platform HAL source, an MSVC-compatibility header, a 155-entry `--defsym`
alias list and twelve WSL harness scripts. The modern `l3` chain
is Windows-only. 25 other port branches are strict git ancestors of a surviving tip
and are safe to retire. Two premises this file inherited were also wrong: the `l1`,
`l2`, `l3` names are a DAG rather than a line, and the older `port-*` branches conflict
at 19-37 markers, not the 100-226 first reported — that larger figure belongs to
`port-0.2.7-hotfix` and the deep chain. The `l3-recon-c` port branch merges today's `main`
with zero conflicts and is the natural lane trunk.

**#2360 cannot land until #2392 does.** Confirmed from the validator's own message and
`tools/validate_merge.py:665-667`: #2360 splits a symbol, which changes the coverage
denominator, and #2392 is the relaxation that admits a split.

## What the upgrade actually fixed

IN-FLIGHT.md listed five gaps. The queue closes the first, second and fourth: a
released lease keeps its task, outputs and evidence rather than erasing them;
ownership is a durable record instead of a ref that vanishes; and a finished stage
publishes an immutable SHA to a named next owner, so nothing stops one step short.

The third (`prior_work()` matching on substring, so `BobOmb` collides with
`KingBobOmb`) is moot — v2 reserves declared resources instead of guessing from
names. The fifth is a scheduling choice, not a tool defect: **land scout facts as
soon as the scout finishes.** They cost the most to produce, carry no byte risk,
and every one of the 2026-09-06 scout branches sat unmerged while its facts were
already on `main` under another commit.
