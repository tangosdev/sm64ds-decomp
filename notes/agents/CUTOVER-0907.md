# The v1 to v2 cutover, and what the inventory found

Measured 2026-09-07 against `origin/main` at `f327f7b6`, then re-measured the same
day against `cd7e45c91` after the approved merge train landed. This supersedes the
operational parts of [IN-FLIGHT.md](IN-FLIGHT.md), which was written the same day
*before* the upgrade and which asks you not to trust its own numbers. Three of its
claims are corrected below.

Like that file, this one starts rotting immediately. The queue is the live record;
this is the reasoning behind it. It has already rotted once and been repaired: the
branch table in this file has now been wrong twice, and the section that explains
why is the part worth reading.

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

**1. The gate defect did not stop all five — and it is now fixed.** #2400 claimed
one validator line stopped the whole wave. It stopped one PR. #2400 was closed as
superseded rather than landed, because that headline would have entered the tree as
tracked misinformation, and it was already false on measurement when it was written.
`daMky_c`'s private validation *passed* — 686 symbols exact, no lost-symbol line —
and its red gate was `check_rename_ledger`, eight historical rows still asserting
the retired coined spelling. Only `da1up_c` had a confirmed `rom_data_regressions`
failure; `daGmch_c`'s was a prediction at the time, and was confirmed later.

**#2425 is the fix, and it is on `main` as `1c93d2663`.** `rom_data_regressions`
now anchors on `(module, address, bytes)` instead of the symbol name, so a rename
reads as a rename rather than a loss. Re-running the gate for `daGmch_c` against a
base that includes the fix leaves the known-defect bucket **empty**.

What #2400 measured is worth keeping now that the PR itself is gone. The stopped
wave folded 184 shards into 5 TUs and turned 110 members into real C++ methods:
`daMip_c` converted 32 of 32, and `daGmch_c` 36 of 37 — the 37th being the factory,
free by design. On every member that did not convert, the wall was **naming or
scope. Never codegen.**

**2. "The validator restores `tools/` from the base commit" is out of date.** That
was retired on 2026-08-30 by `fa5aa1f65`, and `plan-cpp-conversion-queue.md` says
so. The conclusion survived for a different reason: the *base* ROM report is built
at `baseSha` and may be served from a per-SHA cache, so the base side of the
comparison would still carry no addresses. The gate fix did land alone and first,
as #2425 — if the question comes up again, cite the cache, not the restore.

**3. #2392 is not that fix.** It relaxes the function-coverage denominator so a
symbol-table SPLIT can pass, which is what #2360 needs. Its diff contains no
`rom_data_regressions`. Every other unmerged `validator/*` and `tools/*` branch was
checked; the rename-awareness work was unstarted when this was written. It has since
been written and merged as #2425, which is a different PR from #2392.

## Counting branches: the method matters more than the number

309 worktrees, 148 unmerged remote branches, 61 local `cpp/*` branches whose
commits are on no remote.

A first pass called all 61 unbanked. That was wrong, and wrong in an expensive
direction: it diffed whole path sets including `converted-baseline.json`,
`attribution.json` and the overlay `delinks.txt`, which drift on every branch
because `main` moved. A second pass compared **the class's own files** — its actor
source, its `tu_manifest.d` entry, its header — and read 14 landed, 8 differing, and
39 adding class files `main` does not have.

`dScMgTeresa_c`, `Goomboss` and `Player` all read as unbanked under the loose test
and are byte-identical to `main` under the strict one. **Diff the class files, not
the aggregates.** The aggregates are the integration lane's problem.

**Correction, later the same day: that 39 is still wrong, and the strict test was
not strict enough.** It asked whether a class file exists at its branch path on
`main`. PR #2270 had moved actor sources to `src/game/actors/` and renamed their
stems to `d_a_<snake>`, so a *move* reads as an *absence* — and one case renamed the
stem as well, from a class-named `daEyBm_c` source to a `d_a_ey_bm` one, which no
directory-swap heuristic finds. Eight tasks were registered off
those rows and all eight were already banked; every one was cancelled. A parallel
inventory of the `tu/*` lane hit the third variant of the same mistake, where class
and manifest RENAMES made 54 of 79 already-promoted classes read as unlanded.

The rule that covers all three: **key the comparison on something `main` cannot
move** — blob hashes across every plausible path, or `(module, ROM address)` for a
symbol, with the module included because overlays reuse addresses. And note that the
files a promotion DELETES being absent from `main` is proof it landed, not evidence
of missing work; reading that backwards is what inflated the count.

### The third pass, which executed that rule instead of stating it

Every rename `main` took from the oldest merge-base to its tip — 927 of them — was
collected into one map and applied as **both** a path map and a text substitution
before diffing, and every comparison was keyed on blob hashes and ROM addresses
rather than on paths. This is the row to use. The two above are kept only because
the way they failed is the lesson.

| verdict | count | meaning |
|---|---|---|
| LANDED | 22 | content is on `main`; nothing on the branch that `main` lacks |
| MAIN-AHEAD | 37 | content landed AND `main` has since evolved it — restacking REVERTS |
| GENUINELY-UNBANKED | 2 | real work not on `main` in any form |

**59 of the 61 are fully banked.** Read the table as 22 + 37, not as 22: MAIN-AHEAD
is banked work too, and the only thing separating it from LANDED is that `main` has
moved further. 55 of the 59 landed through their own merged PR.
Four landed by another route and so look PR-less if you only check the branch:
`cpp/daObjFloatBoard_c-tu`, whose #2276 was closed and whose content went in through
#2294; `cpp/dabrq-sinit-resource-tu`, landed as `201fa394d` and `893200df3` plus
#2270; `cpp/dascre-restack-2143`, through its parent #2143 and then #2270; and
`cpp/profile-reconstruction-wave24`, through #2241 (wave 27) and #2270. **A branch
with no merged PR of its own is not evidence of unbanked work.**

The corroboration is stronger than any file diff, because it is keyed on the
cartridge rather than on anything a repository can rename. Across all 61 branches
there are **30,820** ROM addresses carrying a real, non-placeholder symbol name on
both the branch and `main`, and in every one of those the disagreement is a
*spelling*, never coverage. Only **16 name/address pairs, on 8 distinct addresses
across 5 branches**, have a real name on a branch where `main` still carries a
`func_*`/`data_*` placeholder — two of them belong to the lava promotion below, two
are data-file labels on `cpp/ov063-rtti-names`, and the remaining four are the same
two pairs of vtable-interior labels repeated across three intact-TU branches.

**No branch holds ROM coverage `main` lacks.** That is what vindicates the rule
above: the 39 "unbanked" rows were the rename map, not work.

## The 37 that would revert `main`

No earlier revision of this file had this column, and it is the one with teeth.
MAIN-AHEAD does not mean "fine, merge it later". It means the branch's content is
*already on `main`* and `main` has since improved it, so restacking the branch
un-does the improvement. Every one of the 37 is marked RETIRE for that reason, and
the damage would pass the byte gates, because a name is not a byte. Five causes
recur.

**Profile-symbol reconstruction.** The campaign renamed each actor's factory from
the coined `<Class>_Spawn` to the ROM-shaped `<Class>_classInit`, and its profile
global from `<Class>_SpawnInfo` to `g_profile_<ID>`. Any branch cut before its
overlay's wave still spells the old pair, and restacking writes the coined spelling
back over the cartridge's own. `cpp/daeybm-genuine-tu`,
`cpp/c0switch-rtti-rename-snake-tu`, `cpp/flyguy-genuine-tu`, `cpp/hs-billboard`,
`cpp/ov063-rtti-names`, `cpp/kpa2bg`, `cpp/obj-ks-water`,
`cpp/dabrq-sinit-resource-tu`, `cpp/dascre-restack-2143`,
`cpp/profile-campaign-tail` and waves 6, 8, 9, 10 and 30 each carry at least one.
The worst is `cpp/rtti-snake-tu-2`: at **18 ov002 addresses** `main` now holds the
cartridge's own `daChScene_c` and `daChRoom_c`, and the branch holds the coined
`Exit` and `VirtualDoor`.

**classInit absorption.** Where `main` later folded the factory into the promoted TU
as its last ordinal, the branch's manifest declares one function FEWER than
`main`'s, and the missing ordinal is always the class's own `_classInit`. Measured,
branch against `main`: `cpp/rc-buranko` 7 against 8, `cpp/bk-rotebar` 6 against 7,
`cpp/cv-shutter` 5 against 6, `cpp/obj-rc-hane` 7 against 8, `cpp/td-water` 6
against 7, `cpp/rc-guruguru` 6 against 7.

**Member loss.** `cpp/flyguy-genuine-tu`, `cpp/kaitendai-intact-tu`,
`cpp/km1-dorifu-intact-tu` and `cpp/kuruma-intact-tu` each carry a 13-member
propeller-heyho actor source, where `main`'s is 25 members — the same twelve members
lost four times over. `cpp/minigame-tu-4`, wave 24 and wave 38 turn between 16 and
18 real `dScMgMCarlo2_c` and `dMgMCarlo2CardObj_c` method names back into
`func_ov006_*` placeholders.

**Destructor levers.** `main` gives both `daObjMarioCap_c` and `daDgr_c` an inline
`virtual ~Class() {}` in the class body. That is not a style choice — the
declaration form decides which destructor variants mwccarm emits, and each was taken
against a measured D1/D0 address justification. Wave 25 re-declares the cap
destructor out of line, and wave 29 does the same to `daDgr_c` while also dropping
`main`'s richer Wiggler field decode.

**Corrected prose, and one corrected declaration.** `cpp/Eyerok-tu-build` would
restore a `0x4b0 -> 0x4ac` size claim that `main` re-measured and explicitly
retracted; `main`'s comment now records the size as UNCHANGED at `0x4b0` and says so
in as many words. `cpp/Player-tu-build` would drop a `char*` local and the codegen
comment explaining why the int-typed parameter costs a literal-pool load instead of
a folded offset. Sharpest of the three is `cpp/dScMgSound_c-tu-build`: `main`
corrected one shared forward declaration from a K&R `extern int f()` to
`void f(void*)`, justified against the cartridge's own bytes in six lines of
comment, and the branch restores the wrong spelling. No gate compares a shared
declaration's return type against the ROM, so that one would land in silence.

## Do not delete

Re-measured with the rename-aware method above. Of the four `cpp/*` entries an
earlier revision of this section listed, **only one survives.**
`cpp/lifecycle-ownership-16`, `cpp/next-migration` and `cpp/enemy-subclasses-2` all
report banked once renames and path moves are mapped; their "files absent from
`main`" counts were the same artefact this file has now produced three times, and
the `#1708`/`#1387` reading — merged PR covers only the early commits, so the later
ones never landed — is exactly what a rename map dissolves.

Reported banked is not the same as proven banked one branch at a time, so the
standing caution stands unchanged: **no branch here gets deleted until its content
is demonstrably on `main`, keyed on blob hash or `(module, ROM address)`.** Retire,
verify, then delete — in that order, and never the last two together.

- **`cpp/promote-da-obj-lava`** @ `9622b484a`, PR #2231 CLOSED unmerged. 17 of its
  21 promoted-TU manifests are absent from `main`, covering **135 functions**. All
  17 declare `production_mode: intact-object` and record `functions_matched ==
  functions_declared`; 15 have every stated criterion PASS and 16 record
  `matchesStockRom: true`. The two exceptions are disclosed in the manifests
  themselves, not hidden — `daObjWlPolelift_c` records a FAIL-BY-DESIGN on eight
  unlicensed section/symbols, and `daObjKm2_Ami_Bou_c` waives or leaves un-run five
  of its thirteen criteria. The PR was never rejected on substance: the validator's
  only complaint was a test-merge conflict in `attribution.json`. Twelve of its
  classes also carry inline `~Class() {}` destructor bodies that exist nowhere else
  — `daObjC0Water_c`, `daObjC1Peach_c`, `daObjClock_c`, `daObjFire_c`,
  `daObjHeart_c`, `daObjKm2_Ami_Bou_c`, `daObjKm2_Nobiru_c`, `daObjKm2_Ukishima_c`,
  `daObjLava_c`, `daObjTtWater_c`, `daObj_volcanoCannon_c`, `daRFlag_c` — verified
  still absent from `main` today. IN-FLIGHT.md warned that two older branches carry
  irreplaceable inline-dtor flips; auditing that set found **one**, this one. The
  second was not located. Do not assume it was imaginary. The branch nonetheless
  carries the profile-symbol regression described above, so it must be
  **re-derived on `main`'s `_classInit`/`g_profile_*` names, never restacked as
  it stands.** A rescue is under way, re-measuring first.
- **`cpp/kpa3bg`** @ `1064a7730`, no PR was ever opened. This is a **naming pass,
  not new coverage.** `main` already names every ov060 address the branch touches,
  already has `daKpa3Bg_c_classInit` and `g_profile_KOOPA3BG`, and already carries
  an auto-generated `daKpa3Bg_c` header. The branch is also *behind*: it would
  rename six method addresses to `daKpa3Bg_c::*` that `main` has since settled as
  `SpikeBomb::*`. Re-derive the `daKpa3Bg_c` half on top of `main`; do not restack.
- **`port-linux-fwd-sweep`** — unchanged by this re-measurement and still the only
  Linux host lane. See the port-lane paragraph below for what is on it.

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
uncommitted in a working tree. Recovered in #2408, which was then closed as a
duplicate of #2399 — the credit is banked either way.

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
with zero conflicts and is the natural lane trunk. One operational note for whoever
takes the lane next: the port stack needed a restack at **every** step of the merge
train, because squash-merging a base detaches the branch stacked above it. Budget for
that; it will recur on any stacked lane.

**#2392 has landed, so #2360's dependency is cleared.** The dependency was real and
confirmed from the validator's own message and `tools/validate_merge.py:665-667`:
#2360 splits a symbol, which changes the coverage denominator, and #2392 is the
relaxation that admits a split. #2392 is now on `main` as `cde63e0df`; #2360 is
still open on its own merits.

## The merge train landed

Everything the approved train carried is on `main`, in this order: #2392
(`cde63e0df`), #2425 (`1c93d2663`), #2372 (`2cf25da41`), #2399 (`7097a3922`), #2373
(`ac9106002`) and #2380 (`cd7e45c91`, which is `main`'s tip as this was written).
#2408 was closed as a duplicate of #2399 — the `func_ov006_0212a764` attribution it
carried is banked either way. Treat any sentence above about these PRs as history,
not as a plan.

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
