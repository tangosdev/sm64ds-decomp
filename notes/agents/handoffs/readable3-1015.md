# Handoff: three-class readability sweep, 2026-10-15

This commit completes two producer readability passes across the rolling-log
base (`daObjMaruta_c`), racing penguin (`daPgRcer_c`) and Whomp/King Whomp
(`daBtn_c`). It does not complete their reconstruction or independent review.

## Identity and scope

- Branch: `cpp/readable3-1015`; producer/coordinator: `codex-readable3-1015`.
- Source base and workflow pin: `40dfd83ba134be0600730b58f3261f04441632d8`.
- Worktree: `C:/tmp/sm64ds-readable3-1015`, isolated from the primary checkout.
- Tasks: `d_a_obj_maruta-readable1015`, `d_a_pg_rcer-readable1015`,
  `d_a_btn-readable1015`; producer stage `readability`, next stage `verify`.
- Issues: [rolling log #3870](https://github.com/tangosdev/sm64ds-decomp/issues/3870),
  [penguin #3871](https://github.com/tangosdev/sm64ds-decomp/issues/3871),
  [Whomp #3194](https://github.com/tangosdev/sm64ds-decomp/issues/3194).
- Only the three production sources and these evidence notes change. Shared
  headers, object layouts, manifests, symbol spellings and attribution are unchanged.

## Source review

The first pass replaced established raw field accesses and ABI spellings. The
second tested vector locals, pointer arithmetic, redundant casts and stale
compiler-barrier comments. Across the three classes, 22 function bodies changed.

The rolling log now uses its existing angular-velocity and timer members,
ordinary increments and signed division, a `Vector3` displacement local, and a
typed `dActor_c*` contact boundary. Its contact position remains a vector view of
the actor's scalar fields; direct individual field loads increased that function
from 204 to 224 bytes. Testing simpler flag/actor tests together changed Behavior
from 864 to 840 bytes, so those existing tests remain.

The penguin no longer needs raw angle/action-step macros. Path, dialog and
collision-normal locals use `Vector3`; collision-result accessors, angle/speed
approach functions and looping sound use native interfaces. The approach
prototypes were checked against `src/ApproachAngle.cpp`, including the `int`
return of `ApproachLinear(int&, int, int)`. Player position views avoid integer
pointer arithmetic; replacing them with separate member loads shrank the race
function from 988 to 980 bytes, so the grouped view stays.

Whomp's `BtnVec` stand-in is removed: the affected locals, including the retained
volatile copies, use `Vector3`. Animation comparisons compare pointers directly,
and the return-vector helper uses ordinary output-array indexing. Its player
survey keeps a borrowed vector view: separate scalar reads shrank 212 to 208
bytes. All original function names are retained.

No semantic field, helper, state or asset names were invented. Existing header
members supply names and types; identical generated accesses confirm their
layout at these sites. This is not a new claim about original source identifiers.
The experiments measure particular rewrites, not proof that no other spelling
could match.

## Byte and production evidence

[The experiment record](../../experiments/readable3-1015.json) pins source blobs,
raw/prepared object hashes, changed functions and retained experiments. Commands
and full local reports are under ignored `build/readable3/`; private receipts,
ROM/compiler inputs and object files are not committed.

Pinned compiler: `2004/b56`, `rombuild.CFLAGS` with `-lang c++`. Every function's
bytes and relocation records are identical to the base. The penguin's complete
raw object is identical too. For Maruta and Whomp, introducing native vector
locals moves the already-emitted empty `Vector3` destructor relative to other
sections. Excluding that unchanged duplicate, the ordered allocated sections
and their relocation records are identical. Both objects pass the existing
production compiler-only policy; no policy was widened.

- Standard `prepush_linkcheck.py --files` over all three sources: exit 0,
  76 VERIFIED plus 36 blind relocation words in Whomp's initializer.
- Fresh complete objects prepared with the unchanged production isolation or
  intact-object policy, then checked using `linkcheck.linkcheck` for every
  enrolled symbol: **77/77 VERIFIED, zero differences, zero blind words**.
  This includes the initializer after its local symbols receive their existing
  production names. The raw initializer warning alone is not acceptance.
- Whomp's intact-object preparation validates its owned `.text`, `.init`,
  `.ctor` and `.bss` claims. Maruta and penguin remain text-only promotions.
- Declaration-agreement ratchet: no new disagreements or local header
  redeclarations. Port check: 267 references resolve. `git diff --check` passes.
- `python tools/rombuild.py -j16 --no-rom`: exit 0; **106/106 modules exact**,
  11,334 source-built functions reproduce, all 331 owned data claims reproduce,
  zero new symbol errors. The build reused 2,749 objects and compiled three.
  Each final prepared object is byte-identical to its actual production input.
  The stock control and candidate retain nine existing symbol-check errors;
  the data inventory retains one differing `_ZTV10dCcAcPos_c` from unchanged
  destructor sources, 1,519 partial symbols and 21 unnamed symbols.

The legacy `tubuild.py verify ov079/daBtn_c --no-write` entry point returned 1:
36 text functions match with clean isolation/relocation destinations, but it
classifies the intact initializer and local BSS as extras. It does not apply the
production intact-object ownership path. That refusal is retained here; the
fresh production preparation and ordinary ROM build are the applicable proofs.

## Remaining reconstruction and next action

- **R3-LAYOUT-01:** Whomp's bone accesses at `+0x1a/+0x1c/+0x1e` remain inside
  `BMD_Bone` padding. The observed usage does not establish a complete bone
  interface; leave these unnamed until the producers and consumers agree.
- **R3-ASSET-01:** `SharedFilePtr` has no shared field layout. The loaded-file
  views (`LOADED` and `BtnFileRef`) remain local; no guessed fields propagate.
- **R3-ABI-01:** Scalar Fix12 calls, some legacy free helpers, Whomp's matrix
  block-copy and return-vector views remain. No broad interface completion is claimed.
- **R3-LIFECYCLE-01:** Existing factory/destructor and metadata ownership is
  unchanged. The rolling-log base is abstract; the leaves still own factories.
  Maruta/penguin metadata promotion is outside this readability sweep.
- **R3-REVIEW-01:** Independent review and acceptance remain pending. The next
  verifier must review this exact candidate and the source base above, retain
  these findings and issue #3194's unresolved scope, and repeat relevant checks.

No candidate NDS packaging or gameplay test was run. PR CI and any composition
with a newer main are separate checks; local byte proof does not close these
classes' reconstruction issues.
