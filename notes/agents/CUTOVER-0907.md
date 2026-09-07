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

## Not adopted yet

Registered nothing for these; they need their own inventory before anyone claims
them.

- **The port lane.** 30 unmerged branches, exactly one PR (#2360) between them. The
  `l1` → `l2` → `l3` names are a genuine ancestor chain, not siblings:
  `intro-cutscene` → `l1-*` → `l2-openfault` → `link100` → `l3-*` → `l3-recon-c`.
  Eleven older top-level `port-*` branches from 2026-08-04 conflict heavily with
  current `main` (100 to 226 markers) and are effectively a rewrite.
- **The `tu/*` promotion lane.** Eight branches, none with a PR, and they overlap
  hard: `tu/promote-batch-2` holds 76 manifests and shares 39 with
  `tu/restack-inline-dtors` and 41 with `tu/legacy-source-extensions`. These are
  three different treatments of one actor wave. Pick one lineage per class before
  promoting anything; they cannot all apply.
- **The profile-reconstruction campaign.** ~24 branches. One commit on
  `cpp/profile-reconstruction-wave24` says it re-applies renames that a `main`
  promotion reverted — check that before discarding the cluster.
- **Older `cpp/*` work**, per the do-not-delete list above.

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
