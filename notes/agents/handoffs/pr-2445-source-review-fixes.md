# Handoff: pr-2445-source-review-fixes, September 10 continuation

This commit resumes `938dd60b7a10e8c88cf5abb4ff9db11f2448895c` in the existing
`pr-2445-source-review-fixes` task. Session `codex-r2445-0910` read the offered
verification, preserved SNM-01 as open through rework, and claimed the returned
`fix` producer stage before editing. Worktree: `C:/tmp/sm64ds-r2445-0910`.
The same session also claimed `pr2445-ledger-repair-0910` at the same input,
which separately reserves `symbols/actor_renames.tsv`. Current queue operations
use the tools in `C:/tmp/sm64ds-review-coord-0910`.
The queue retains the historical workflow pin and active review-policy pin.

The four changes use interfaces and fields already declared by the real class
hierarchy:

- `Behavior` calls `mCylinder.Clear()` and `mCylinder.Update()` directly.
- `InitState2` uses `mVertAccel`, `mTerminalVelocity`, `mFlags`, `mPathNode`,
  `mHorzSpeed` and `mStateValue`.
- `InitState4` and `InitState5` use `mFlags` and `mStateValue`.

The constants, operations and return values are preserved. There is no header,
symbol, manifest policy or ownership change. The source has 630 lines. The
`tiers.py` RAW_OFFSET expression finds 143 matches, versus 150 in the accepted
input; this limited regex census is not a completion assessment.

## Findings and remaining ownership

SNM-01 is now fixed: under the separately claimed ledger reservation, both
`symbols/actor_renames.tsv` rows at ov072 `0x0211fcb0` received the exact
SetState/Ei/int-versus-enum disclosure specified in the preserved handoff below.
Only those two why cells changed; all other columns, rows and ordering were
preserved. The existing declaration, definition and manifest disclosures remain.
SNM-02 through SNM-05 remain fixed in their original bounded scopes.

This is partial reconstruction under
https://github.com/tangosdev/sm64ds-decomp/issues/2476. Larger state bodies still
use raw offsets and external mangled calls; the matrix-copy adapter also remains.
They are unfinished reconstruction, not established compiler constraints. The
class retains its compiler-built lifecycle and owned text/data. The issue stays
open, and this source producer cannot provide independent source acceptance.

## Fresh proof for this source

Every isolated alternative and their combination passed all 29 manifest symbols
under `2004/b56`, using `build_pin.flags_for` including `-Cpp_exceptions off`.
There was no failed alternative in this bounded set. Exact sources, patches,
objects and individual results are retained under `build/typed-probes/`.

The final source was compiled separately. `build/final-proof.py` uses the existing
production object-isolation and TU policy functions without modifying them:

| Check | Result |
| --- | --- |
| Exact per-symbol linkcheck | 29/29 VERIFIED, 3,892 bytes, exact sizes, zero blind bytes and empty diffs |
| Complete owned data | 188 bytes exact at ov072 `0x02122780..0x0212283c`; all 36 relocations resolve correctly |
| Complete emitted-object audit | 33 LICENSED outputs, no unlicensed sections or refusals; ROM function order preserved |
| Raw emitted metadata | 6 VERIFIED, 4 PARTIAL, zero DIFFERS or UNNAMED; partial string extents remain partial |
| Port references | 423 checked, zero stale |
| Header-offset helper | 15 fields through `0x38c`, zero mismatches; it does not cover the PMF/scalar tail |
| Rename-ledger checker | 2,055 mangled/vtable rows agree; 1,532 coined rows outside coverage; SNM-01 is closed by direct inspection of the two added disclosures |
| Production ROM | exit 0; 11,192/11,192 functions and 26/26 data claims reproduce; 106/106 modules exact; 3 BSS claims cover 172 NOBITS bytes |

The packed 16,777,216-byte ROM has the retail SHA256
`d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`.
The strict scratch baseline still reports nine pre-existing symbol-check errors;
production reports zero new symbol errors. That nested baseline check remains
red. The global emitted-data census has 706 verified, 224 partial, four differing
and 387 unnamed records; this is separate from the exact owned-data/ROM gates.

Final tested source git blob: `4535ffc43944a30161246ec2ba2477fdc97ed830`.
Final tested source SHA256: `5ed6a6cac15e2e7612f2b55d3d0cbac79971ad64059fab0610111bcb32e5b02e`.
Reports are retained in `build/final-proof/`, `build/production-rom.json` and
`build/production-data.json`. Build outputs and private receipts remain ignored.
Current-main composition needs fresh independent source acceptance and final
private validation; these producer results do not authorize merging.

## Preserved September 8 handoff

Everything below describes the historical `938dd60` checkpoint, including its
then-current source counts, prior ownership and previously measured proof.
The continuation above supersedes its current-work claims without erasing them.

# Handoff: pr-2445-source-review-fixes

This continuation resumes the existing Snowman promotion. The queue records its
immutable output SHA. This remains a partial reconstruction checkpoint: SNM-01
has two required ledger disclosures pending legitimate ownership. It is not a
whole-class pass or integration acceptance.

## Identity and ownership

- PR: https://github.com/tangosdev/sm64ds-decomp/pull/2445.
  Task `pr-2445-source-review-fixes`, stage `fix`, producer session
  `codex-humanizer-snmbdy-20260908-02`; supporting coordinator
  `codex-humanizer-integrator-20260908`.
- Branch `cpp/humanizer-snmbdy-0908`, wired worktree
  `C:/tmp/sm64ds-humanizer-snmbdy-0908`. Fresh input is the preserved checkpoint
  `c42d5b3fff1232631eb06f80cfbc5820f8f9591e`, returned through independent verifier
  rework. Original adopted PR head is
  `19921838ecf6acaf689a4dbc3bb4fd05e4d0effc`; actual PR merge base is
  `690637e41a7a302eace631f6065ab1c4fe37326e`. No classes were restarted and no
  history was rebased or reset.
- Predecessor task `snmbdy-ov072-0907b`. Inherited source contributors include
  `prod-snmbdy-0907` and `int-snmbdy-0907b-b303`; the prior correction producer
  was `codex-humanizer-snmbdy-20260908-01`. This producer cannot verify its own
  work. Workflow remains `f327f7b6460e157153eb7fc0749dbbe60dd854f1`; supported
  queue tools are `6ef1dfb896e6074e83d2aa1a03cc3b701d07dcae`.
- Reservation: class source/header, class manifest, this handoff and the original
  [producer handoff](prod-snmbdy-0907.md); ov072 text
  `[0x0211f000,0x0211ff34)` (29 functions, 3,892 bytes) and owned data
  `[0x02122780,0x0212283c)` (188 bytes). This rework changes only source, the
  profile manifest note and this handoff. Shared headers, ledger, attribution,
  compiler flags, symbols, ranges and production policies are unchanged.
- Local evidence: `build/pr2445-rework-probes-20260908/` and
  `build/pr2445-rework-gates-20260908/` in this worktree. The primary ignored
  `build/fleet-restart-20260907/review-watch/` holds the exact probe/gate runners,
  original reviews, rework census, pending ledger patch and revised PR-body draft.
  Private receipts remain ignored and are never published.

The inherited review findings are tracked in
https://github.com/tangosdev/sm64ds-decomp/issues/2476, owned by the humanizer and
integrator support coordinator through @andrewboudreau. The public review at
https://github.com/tangosdev/sm64ds-decomp/pull/2445#pullrequestreview-5145352638
and the older complete-source review were both read; the first checkpoint's
failure did not discard either set of findings.

## Finding dispositions and actual changes

- **SNM-01 remains open in the ledger.** The previous checkpoint qualified
  `SetState(int)` in its declaration, definition comment and manifest. Table
  indexing and call sites do not establish the original English name, the `Ei`
  spelling or original int versus enum. The two remaining ledger why cells are
  listed below; this task does not own their file and does not claim this finding
  fully fixed.
- **SNM-02 fixed, with current evidence separated from history.** The original
  handoff remains explicitly historical. Its measurements and the first
  correction's results remain pinned to their actual inputs. Current source
  census and fresh proof below replace any assumption of executable-token
  identity with the adopted PR: this rework intentionally changes C++ source.
- **SNM-03 fixed in source and manifest.** The descriptor halfwords at +0x04 and
  +0x06 are behavior/render priorities, as read by `fBase_c::fBase_c()` in
  `src/_ZN7fBase_cC2Ev.cpp`. `dActor_c::dActor_c()` in
  `src/_ZN8dActor_cC2Ev.cpp` reads flags at +0x08 and sends words at
  +0x0c/+0x10/+0x14/+0x18 to SetRanges as clip offset Y, clip radius, clip
  distance and far distance. The BIG_SNOWMAN_BODY registry entry corroborates
  priorities 274/133 and ranges 0x82000/0x82000/0x01000000/0x01000000.
  `SnmBdyProfile` names now describe those consumers. The signed halfwords,
  Fix12i/u32 types, member order, initializers, factory symbol/relocation and
  0x1c extent are unchanged. These observations do not recover original field
  spellings or signedness. The old groupFlags/executeOrder/drawOrder schema was
  false even though the resulting bytes happened to match.
- **SNM-04 fixed for both requested bounded methods.** `InitState1` now writes
  `mSubstate = 0`, `mStateTimer = 0x15`, `mStateValue = 1`.
  `InitState3` writes `mSubstate = 0`, `mStateValue = 3`. Both retain `return 1`.
  The named members already existed at +0x3a2/+0x3a0/+0x394 with the observed
  byte/halfword/word widths; no header or fabricated return was needed.
- **SNM-05 fixed.** The fabricated local complete one-method `struct Player`
  and its unsupported header-conflict explanation are removed. The TU includes
  the actual `Player.h`, whose `Player : dActor_c` declares the same StartTalk
  member. This direct inclusion passes; no compiler obstacle is claimed.

All four source hypotheses were first compiled separately from exact input
`c42d5b3fff1232631eb06f80cfbc5820f8f9591e` in ignored probe files. The control and
all four hypotheses each passed every one of the 29 exact function symbols under
`2004/b56`, not only the edited methods. Their combination also passed 29/29,
zero blind bytes, empty diffs and exact requested sizes. No failed hypothesis was
encountered in this bounded set. The applied combined source only closes the
extra blank line before `#include "Player.h"`; final source was compiled again.

| Probe from c42d5b3 | Exact change | Result |
| --- | --- | --- |
| control | No source change | 29/29 strict |
| profile | Rename only descriptor fields and correct its comment | 29/29 strict |
| player-header | Replace local Player definition/comment with Player.h | 29/29 strict |
| initstate1 | Replace only its three raw scalar writes with named members | 29/29 strict |
| initstate3 | Replace only its two raw scalar writes with named members | 29/29 strict |
| combined | Apply the four disjoint changes together | 29/29 strict |

Each `<probe>.cpp`, `<probe>.patch`, `<probe>.o` and `<probe>.json` is retained in
`build/pr2445-rework-probes-20260908/`. `summary.json` records full source,
object and patch SHA256, compiler flags and all exact linkcheck records. The
patches are reproduced by taking only the corresponding source hunks between
c42d5b3 and this commit, preserving every other source/header blob from c42d5b3.

## Pending ledger dependency

`symbols/actor_renames.tsv` remains reserved by paused
`issue-2411-humanizer-2` at its preserved 848da2bb repair. It was not edited.
The coordinator must arrange legitimate ownership and include these exact two
why-cell corrections before accepting the full provenance finding:

| Module/address | From | To |
| --- | --- | --- |
| ov072 / 0x0211fcb0 | `func_ov072_0211fcb0` | `_ZN12daBgSnmBdy_c8SetStateEi` |
| ov072 / 0x0211fcb0 | `_ZN11SnowmanBody8SetStateEi` | `_ZN12daBgSnmBdy_c8SetStateEi` |

Append this suffix to each existing why cell, preserving all other columns, rows
and ordering: `; SetState and the Ei int parameter spelling are inferred; table
indexing and call sites do not establish the original type or rule out an enum`.
The exact before/after patch remains in the primary ignored review bundle.
No global refresh, symbol rename or credit baseline update is part of this task.

## Remaining reconstruction scope

This is an intact-object production promotion with partial readable source.
There are 26 out-of-line native member definitions, a registry factory and the
inline destructor's D1/D0 pair: 29 function symbols. These native definitions
predate this repair; the repair removes a counterfeit class and five raw scalar
writes, rather than claiming new method ownership. Genuine existing progress
includes compiler-owned construction, ordinary `new`, typed subobjects, vptr
and metadata ownership.

Current source has 636 lines and its unchanged header 129. The exact `tiers.py`
RAW_OFFSET regex now finds 150 occurrences on 113 lines in 16 of 26 member
bodies, versus 154 on 117 lines in 18 bodies at the adopted input. That regex
misses some raw accesses (including c[index]); it is a defined census, not a
complete count or a completion score. Matrix word-copy adapters, external
mangled ABI calls, raw accesses in larger state methods, one unknown field and
padding remain. They are explicit unfinished work under issue2476, not a newly
asserted compiler wall or an independent acceptance of their necessity.

The class still has size 0x3a8, and its Model, ShadowModel, dCcAc_c, dBgCh_Actr
and PathPtr members remain at +0xd4/+0x124/+0x14c/+0x180/+0x380. The header-offset
helper only sees 15 fields through 0x38c, stopping at the StateFunc typedef;
it does not check the PMF/scalar tail. Function bytes, the size assertion and
intact-object data each provide their separately bounded evidence.

ROM class identity still comes from the RTTI string at ov072 0x0212278c,
typeinfo at 0x02122780 and direct dActor_c base pointer 0x0208e390. Private names,
profile/factory identifiers and filename remain reconstructed. The TU owns its
188-byte data band, including full vtable storage with ABI preamble, RTTI/name
and profile. It has no static initializer; the existing PMF initializer is
unchanged. All 29 symbol attribution overrides and retired-stem credit mapping
are preserved.

## Fresh producer proof

All checks below used the frozen source/header/manifest identified in
`build/pr2445-rework-gates-20260908/identity.json`. Final commit-to-tested-blob
identity is recorded in the ignored review bundle. The runners did not edit any
tracked verification records or policies; the TU gate uses an ignored full
manifest copy. No source changed after these gates began.

- Exact pinned compilation of the complete TU followed by production object
  isolation and explicit `linkcheck` for every requested manifest symbol:
  **29/29 VERIFIED**, exact sizes, zero blind bytes and empty diffs. One fresh
  `2004/b56` object supplies all symbols. `strict.json` records source/object
  hashes and every result. The class header has one consumer, this TU; no header
  was edited. The new Player include is covered by this complete source proof.
- `python tools/tubuild.py --manifest
  build/pr2445-rework-gates-20260908/tu_manifest.gate.json linkcheck
  ov072/daBgSnmBdy_c -j8`: exit 0, **SCRATCH-DATA-VERIFIED**. All 3,892 text bytes
  and 188 owned data bytes reproduce from one object, 36 data relocations resolve
  correctly, all 33 symbols are licensed, and there are no unlicensed sections
  or collisions. All 106 modules are exact and the actual scratch ROM packs to
  the stock SHA256 below. See `intact-tu.log` and `intact-tu.report.json`.
  The nested `dsd check symbols --fail` remains red on nine pre-existing errors:
  zero new, zero resolved against the preserved strict stock control. That nested
  check is not represented as passing.
- `python tools/rombuild.py -j8 --report-json <production-rom.json>
  --data-json <production-data.json>`: exit 0, normal production includes all
  enrolled candidates, 11,192/11,192 functions reproduce, 26/26 owned data claims
  reproduce, three BSS claims cover 172 NOBITS bytes, and **106/106 modules are
  exact**. This builds the final 16,777,216-byte production ROM, not only a stock
  control or a `--no-rom` module result. SHA256:
  `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`.
- `python tools/romdata_check.py --files src/actors/daBgSnmBdy_c.cpp --json
  <class-romdata.json>`: exit 0, 6 VERIFIED symbols
  (196 bytes), 4 PARTIAL records
  (36 equal bytes), zero DIFFERS/UNNAMED. Partial extents are
  still partial; the complete owned 188-byte band has its separate exact proof.
- The global informational emitted-data census retains four DIFFERS outside
  this class's source closure: arm9 `_ZTV10dCcAcPos_c`, ov002
  `_ZTIN8daDemo_c10anmModel_cE` and `_ZTIN8daDemo_c13simpleModel_cE`, and ov084
  `_ZTV7daKrb_c`. Their sources and the data baseline were not changed. This
  helper result is distinct from the exact production ownership/module gates.
- Header-offset, port-reference and rename-ledger checks exit 0. The offset
  checker covers only 15 fields through 0x38c; its tail limitation remains.
  Port checks 423 references with zero stale entries. The ledger checker
  compares 2,055 mangled/vtable rows, with 1,532 coined rows outside that check;
  a passing syntax/identity check does not fix the pending why-cell prose.

Post-commit attribution and scoped prepush results are recorded separately.
Bare TU-basename NO-SYM output is not function proof; explicit 29-function and
complete intact-object evidence above supplies that coverage. A producer pass
does not dispose of SNM-01 or substitute for independent source review.

## Preserved historical limits

The earlier checkpoint c42d5b3 had unchanged executable tokens against 19921838;
that statement applies only to that checkpoint. Its full handoff and measurements
remain in git at that exact commit, with ignored `build/pr2445-proof-20260908/`
and `build/pr2445-proof-20260908-control/` evidence intact. The first TU attempt
failed for missing local RcCarpet/Propeller control; the subsequent ordinary ROM
build generated the genuine stock control and the TU gate then passed. Those
raw results were not overwritten or represented as this rework's tests.

The original producer handoff preserves a limited `tubuild verify` refusal for
six inherited externalized RTTI records. No new baseline or ownership exception
is introduced to hide it. Fresh complete intact-object text/data and production
ROM gates have their own results and limits. Bare basename NO-SYM checks are
never substituted for explicit manifest-function proof.

The published private validator's prior pass belongs to PR head 19921838; it
has not tested this new candidate or a future merge composition. A producer
queue offer comes before a different-session independent review; passing that
stage must precede PR/integration publication. Ledger completion and any merge
composition require their own legitimate ownership and final proof.
