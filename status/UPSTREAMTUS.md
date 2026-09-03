# Lane UPSTREAM-MATCHED -- daBakubaku_c, daObjEmmLog_c, daBgSnwmn_c re-checked against upstream #2198-2202

Upstream (tangosdev/sm64ds-decomp `main`) merged PRs #2197-#2200 and #2202
promoting three classes to matched TUs: daBgSnwmn_c (ov072, SNOWMAN 272,
#2197 + the #2199 fix), daBakubaku_c (ov032, BUBBA 228, #2198 + the #2202
fix) and daObjEmmLog_c (ov052, BOBBING_LOG_EMM 174, #2200). (#2201 in the
same run, ov018/daPgMthr_c, is a different class and out of scope.) Note the
task brief's "(ov032)" for daBgSnwmn_c is off -- the class and its PRs are
ov072; SNOWMAN and BUBBA are different overlays.

* Worktree `C:/tmp/upstream-tus`, branch `port/upstream-matched`, base cons
  `1192d9cee`. Two content commits (`6d51e4819`, `872c832e4`), this status
  commit on top. Not merged, not pushed. Never touched: cons.

## 1. Why the upstream diffs do not apply here

This fork's lineage (merge-base `7b2f913fe` against `origin/main`) predates
a foundational decomp-side class-hierarchy recovery: `include/dActor_c.h`,
`dBgActor_c.h`, `dBgW_KcMbg.h` and `tools/tubuild.py`/`tu_map.py`/
`rtti_extract.py` all exist on `origin/main` and do not exist here, and were
never deleted here -- they simply postdate this fork's divergence point.
Checking out upstream's promoted TUs verbatim (`git show origin/main:src/
actors/d_a_bakubaku.cpp`, etc.) fails to compile: `include/daObjEmmLog_c.h`
pulls `dBgActor_c.h` -> `dActor_c.h` -> `dBase_c.h`/`fListNode.h` and more,
none of which resolve. Importing that whole prerequisite hierarchy would be
a different, much larger effort outside this task's scope (and outside
"bring only the src/ and include/ changes for the three classes").

The substance is reachable without it. This fork's own established idiom
for exactly this class family already exists and already works: flat
`u8 pad_NNN[...]` structs plus free-standing matched files that spell the
ROM's own mangled names directly with `extern "C"`, sharing small
`include/decl_*.h` prototype headers instead of a real inheritance chain
(`src/_ZN14SquarePathLiftD1Ev.c`, `src/_ZN10DonutBlockD1Ev.c`, and
`SquarePathLift.h`/`.cpp`'s `SquarePathLift::InitResources()` real-method
form, all already in this tree and already linked). That idiom is what the
fixes below use.

## 2. Per class

**daBakubaku_c (BUBBA, 228, ov032): already fine, nothing to do.** All 24
of its own functions (`config/arm9/overlays/ov032/delinks.txt`,
0x021111a0..0x021124a8) already carry `complete` and spot-checked D1
(`func_ov032_021111a0`), D0 (`func_ov032_021111f0`) and the factory
(`func_ov032_0211244c`) all report `MATCHING VERSIONS: 2004/b56` under
`tools/match.py --strict-relocs`. Port side: `port/hal/actor_classes_ov032.cpp`
seats BUBBA off its own ROM addresses already (`hal_fill_bubba_vtable`),
riding 37 of 41 ov032 functions straight from `src/`; Behavior, Render,
the state-enter setter and one chase-gate body are DELIBERATE host copies
in `port/unmatched/Bubba_*.cpp` for reasons independent of TU-promotion
status (the ModelAnim slot-5 dispatch collision for Render, an mwcc
pointer-to-member-on-incomplete-class wall for the state machine, an
r0-passthrough ABI seam for the chase gate -- all documented in
`port/slice_sweep2_ov032.txt`). `linkage.py --queue` carries no SHADOWS row
for Bakubaku/Bubba. Refused, with reason: there is no stand-in to retire
and no gap to fill.

**daObjEmmLog_c (BOBBING_LOG_EMM, 174, ov052) and daBgSnwmn_c (SNOWMAN,
272, ov072): D0 fixed, both classes now fully complete and matched.** Both
had a botched auto-generated D0 stub (`src/func_ov052_021111e4.c`,
`src/func_ov072_02120874.c`), mislabelled "OnYoshiTryEat" in both cases,
whose first instruction stored `VT0` (daObjEmmLog_c's D0 stored `VT0` then
`VT1`) -- `decl_common.h`'s generic `int VT0[20]` / `int VT1[20]`
shared-header placeholder (`port/hal/actor_vtables.cpp`), not the class's
own table. This is not a compile failure -- checked against the pre-fix
source with `match.py`, it compiles and even reports `MATCHING VERSIONS`,
because `match.py` wildcards the literal-pool word a vtable store loads and
so cannot see which symbol filled the slot. It is a real defect on the
port side though: no per-file `COMPILE_DEFINITIONS` override in
`port/CMakeLists.txt` ever redirected `VT0`/`VT1` for either file (the
pattern dozens of other files in this tree use), so both D0 bodies were
silently storing a 20-int dummy array's address over the object's own
vtable during the deleting-destructor path. That undone recovery is also
why neither file, nor its sibling D1, nor (for ov072) the factory carried
`complete` in `config/arm9/overlays/ov0{52,72}/delinks.txt`: dsd was
supplying all of them straight from the ROM bytes instead of building them
from source.

Fixed: both D0 bodies now store the real tables (`_ZTV13daObjEmmLog_c` /
`_ZTV10dBgActor_c`, `_ZTV11daBgSnwmn_c`) in the exact store-own-then-
delegate shape their sibling D1s already proved. Marked `complete`:
D1 + D0 for both classes, plus (ov072) the factory `func_ov072_02120c00`,
which was itself correct but had never been marked. All 15 functions across
the two classes verified individually with `tools/match.py --module ov052`
/ `--module ov072 --strict-relocs --version 2004/b56`: `MATCHING VERSIONS:
2004/b56` on every one, reloc destinations included.
`port/tools/inferred_stub_guard.py` independently confirms the daObjEmmLog_c
fix: its own build-time sweep reports `DEBT PAID: func_ov052_021111e4`
(left the guessed-vtable-body set), and `port/tools/inferred_stub_debt.txt`
is tightened to match, per the guard's own instruction.

No port/hal changes for either class: every symbol name src/ exposes is
unchanged (`func_ov052_02111xxx`, `func_ov072_0212xxxx`), so
`port/hal/actor_classes_ov052.cpp`'s BOBBING_LOG_EMM/SQUARE_PATH_LIFT fill
and `port/hal/actor_classes_ov072.cpp`'s SNOWMAN fill -- including the
documented Prepare-short-1 host copy for SNOWMAN's own InitResources
(`port/unmatched/Snowman_InitResources.cpp`, an ABI seam unrelated to TU-
promotion status) -- are untouched and still link against the same names.
`linkage.py --queue` carries no SHADOWS row for either class.

## 3. Proof, per spawning level (SM64DS_FAULTS_FATAL=1, 600 frames, walk_window.exe)

All three: `SM64DS_WINDOW_SELFTEST=600 SM64DS_FAULTS_FATAL=1
SM64DS_ACTOR_PROBE=1`, `rc=0`, zero `[quarantine]`/`UNHOSTED`/`[FAULT]`
lines, spawn confirmed in BOTH the start and end census blocks (not merely
a boot-time census).

* **Level 10** (`proof_l10.log`) -- SNOWMAN x1, SNOWMAN_HEAD x1 census +
  `[pos]` at frame 1; both still present in the frame-600 census. No
  `[actor] SNOWMAN` render-probe line: the probe caps at 32 distinct class
  names for the process lifetime (`port_actor_render_probe`,
  `hal/actor_classes.cpp`) and level 10's 38-class census exceeds that, so
  the cap -- not a render failure -- explains the silence; the census/pos/
  zero-fault evidence stands on its own.
* **Level 24** (`proof_l24.log`) -- BUBBA x2, HUGE_COVER x1, HUGE_WATER x1
  census + `[pos]`; one `[actor] HUGE_WATER ...` render-probe line lands
  (same 32-class cap reached before BUBBA's turn).
* **Level 44** (`proof_l44.log`) -- BOBBING_LOG_EMM x8, SQUARE_PATH_LIFT x8
  census + `[pos]`, AND both get their own render-probe line: `[actor]
  BOBBING_LOG_EMM model 30027710 file 300D1DD0 transforms 300D1DA0 mat.t
  (215,610,-363) scene` and `[actor] SQUARE_PATH_LIFT model 30028EB8 file
  300C9CEC transforms 300C9CBC mat.t (-174,500,75) scene` -- non-null model/
  file/transforms, real draw-space coordinates.

## 4. Build, linkage, battery

`port/build-port.cmd` (guards: closestplayer_guard, inferred_stub_guard,
closure --selftest, facegen --selftest, mapdiff --selftest, and the rest of
the pipeline) -- clean, `EXIT CODE: 0`, `build/tmp/build_uptus.log`.

`port/tools/linkage.py`: matched TUs in src/ 11328, linked into
walk_window 9487 (83.7%), SHADOWS 19 + 12 MSVC-name = 31 total, none
naming Bakubaku/Bubba, daBgSnwmn_c/Snowman or daObjEmmLog_c/BobbingLog/
SquarePathLift. Unchanged before/after this lane's two commits: neither
fix adds, removes or renames a linked symbol -- both touch only the
CONTENT of two already-in-the-link files (the D0 stub's own vtable-store
target), so no TU joins or leaves `walk_window.map`; the delta this lane
makes is entirely on the ROM-side `complete`/match.py axis (5 addresses
newly source-verified) and the one port-side correctness fix (the D0
vtable-store target).

`python port/tools/battery.py --skip-build`: ALL GREEN. 50 mounted levels (levels 31 and 41 not mounted, as usual) + 34 hosted scenes each ran under SM64DS_FAULTS_FATAL, level 10/24/44 among them; default boot reaches the title (300 frames clean); linkage 9487 (83.7%); ptr_audit 0 unhosted code pointers; shipping config (PORT_ROM_CLEAN, static CRT) build + selftest ok, rc=0. Only skips are the pre-existing decomp-owned level 27 (TTC_MOVING_BEAM, `_ZN14TtcMovingCubeA8BehaviorEv` has no matched body) and level 45 (GOOMBOSS, `func_ov074_02121380` has no matched body) -- both unrelated to this lane. Levels 10, 24 and 44, this lane's three live edges, all ran bare and unskipped.

## 5. Gaps

* Upstream's literal promoted TUs (real C++ inheritance from dActor_c/
  dBgActor_c, one merged .cpp per class) were not brought in; the three
  classes are matched-and-complete in this fork's own flat idiom instead,
  which upstream itself used for these exact classes before its own
  dActor_c-hierarchy recovery landed. Re-running this lane after this
  fork picks up that hierarchy (or after cons does) could still do the
  literal merge-into-one-TU + real-class-method rename upstream did; it
  was not attempted here as it needs headers this fork does not have.
  `config/converted-baseline.json`, `config/converted-backslide-exceptions.jsonl`
  and `config/tu_manifest.d/` are similarly upstream-only bookkeeping this
  fork's tree does not carry at all (not gitignored, never added) and
  nothing here reads them (`tools/tubuild.py` does not exist in this tree
  either) -- left untouched, out of scope.
* daBakubaku_c was not merged into one TU (its 24 functions stay as
  individual matched files, as they already were): it was already fully
  complete and matched, so a merge would be pure reorganisation with no
  functional or verification benefit, and was not worth the risk of
  disturbing 24 already-working files.
* Render probes for SNOWMAN and BUBBA did not print (32-class cap, see
  above) -- census + position + zero-fault-across-600-frames is the proof
  substitute used, consistent with how the render probe is documented to
  behave (`hal/actor_classes.cpp:895-917`).
