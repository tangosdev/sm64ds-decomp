# Lane STANDIN-SWEEP: honest stand-in inventory, and the cheap retirements

Branch `port/standin-sweep`, worktree `C:/tmp/standin-sweep`, base `3b605d0c4`
(the cons tip). Scope per the coordinator's brief: inventory every stand-in
surface in the live port tree, classify each with status/LINKMG.md's
vocabulary, execute the cheap ones (retire-plain and hostgen rows, smallest
first), and hand off a ranked DECOMPILE list. Additional exclusion, received
mid-lane: the arity family (cap_yoshi, bly_aimed, the Coin dispatch, MgBase)
and the two aritycheck baseline files are another brain's, inventory-only, no
execution. Not touched: port/hal/sub_actors.cpp, port/hal/player_bridges.cpp,
anything HUD-related, St_SwingPlayer_Main, port/hal/rollback.cpp.

No `src/` edits. No restructuring of hostgen.py -- every table entry below is
additive rows in existing tables, or none at all: this lane's landed work was
entirely comment-only tag binding, not a new hostgen mechanism.

## 1. The four counts, before and after

Both readings from `python port/tools/linkage.py --queue` in this worktree
(`build/linkage_before.txt` is the pre-edit tree via `git stash`, same built
`walk_window.map` both times -- these are pure documentation edits, no code or
link change, so the map does not need rebuilding between readings).

| | before | after | net |
|---|---|---|---|
| matched TUs in src/ | 11328 | 11328 | |
| linked into walk_window | 9488 (83.8%) | 9488 (83.8%) | |
| SHADOWS (undocumented) | 19 | 14 | -5 |
| documented host-ABI exceptions | 522 | 527 | +5 |
| MSVC-NAME SHADOWS | 12 | 12 | |
| FACES | 263 | 263 | |

`port/tools/objsrc_check.py`: HOSTGEN 215, HOST 0 (no host object's stem
collides with a matched TU's -- the linkage headline is not over-counting).

`port/tools/inferred_stub_guard.py`: clean, "no new guessed vtable body
seated," 398 seated / 2 baseline / 42 unadjudicated DEBT / 354 ROM-ADJUDICATED,
unchanged by this lane (that queue is decomp-side marker debt on src/ bodies
guessed from vtable slot identity -- a different axis from this sweep, not a
host stand-in, and out of scope; see section 5).

Build: `cmd /c port\build-port.cmd` from a fresh worktree, all guards green
(closestplayer_guard, inferred_stub_guard, closure/facegen/mapdiff/vtablerows
selftests, gxband_guard, tailjump_guard), 33059-step ninja, rc=0.

Battery: `python port/tools/battery.py --skip-build` (`build/battery1.log`) --
**ALL GREEN**. 18/18 smoke binaries ok, 50 level selftests ok (levels 27 and
45 with their two standing decomp-owned skips, TTC_MOVING_BEAM and
GOOMBOSS -- no matched body for either, unrelated to this lane), 34 scene
selftests ok including every minigame scene 361-390, default title boot ok
(300 frames clean), linkage 9488 (83.8%, matches the readings above),
ptr_audit 0 unhosted code pointers, shipping config (PORT_ROM_CLEAN, static
CRT) built in 614s and its selftest rc=0 (589,878-byte BMP written,
liveness only).

## 2. The inventory, by class

### (a) SHADOW-marked bodies -- linkage.py's SHADOWS bucket

19 undocumented before this lane, 14 after. Full before/after lists in
`build/linkage_before.txt` / `build/linkage_after.txt`. The 5 retired out of
SHADOWS (all comment-only tag bindings, see section 3):

| symbol | file | class |
|---|---|---|
| `func_ov085_02129dbc` | Ov085_PrincessPeach_Declshapes.cpp | hostgen-adjacent (MSVC front-end refusal C2733) |
| `func_ov085_02129ebc` | Ov085_PrincessPeach_Declshapes.cpp | hostgen-adjacent (MSVC front-end refusal C4716) |
| `_ZN14QuestionSwitch6RenderEv` | ModelAnim_Renders.cpp | seam (ROM-order ModelAnim slot-5 dispatch) |
| `func_ov006_0211ba88` | MgSound_ShadowSlot35.cpp | seam (four measured ROM-vs-src defects, corrected) |
| `_ZN6Klepto6RenderEv` | Klepto_Render.cpp | seam (ROM-order BlendModelAnim slot-5 dispatch) |

The remaining 14 SHADOWS, left standing and why:

| symbol | file | classification |
|---|---|---|
| `func_ov002_020f2dd4`, `_020f37a0`, `_020f5990`, `_020f5dd8`, `_020f5f0c` | intro_kuppa_dispatch.cpp | refuse -- not read this lane, five-symbol dispatcher, needs its own reading |
| `LoadOverlay`, `UnloadArchives`, `func_02017e94` | scene_boot.cpp | refuse -- not read this lane, boot-path infrastructure |
| `_ZN9LakituBro13InitResourcesEv` | method_faces.cpp | refuse -- not read this lane |
| `MgTrampolineTerror_Spawn` | MgTrampolineTerror_Factory.cpp | refuse -- not read this lane |
| `func_02038324` | func_02038324_hostcopy.cpp | **deliberately kept undocumented** -- SHADOWA.md lane ruled this a genuine port-side bounds-guard fix (24-slot collider registry, empty-sentinel probe), not an ABI exception, and specifically ruled it must stay visible in the SHADOWS queue rather than be tagged out. Not touched, per that ruling. |
| `_ZN7Minimap8BehaviorEv` | Minimap_Behavior.cpp | **refuse -- the VS16 mod**. Hosts sixteen player-icon slots where the ROM (and the matched src TU) hosts four; `port/hal/vs_width.h` documents this as the owner's standing ruling (online multiplayer is a host-layer opt-in mod, the DS-faithful local path stays default). The widened arrays are inert at four players -- same bytes, same behaviour -- and only diverge when a session actually runs more than four. This is infrastructure another lane (VS16.md/VS16HOST.md) owns; not retagged here to avoid colliding with that lane's conventions. Flagging: per Tango's standing policy this divergence should carry a documented gate of its own rather than ride as an unrecognized SHADOW -- worth a follow-up in the VS16 lane, not this one. |
| `ShutterBob_Spawn` | ShutterBob_Spawn_hostcopy.cpp | refuse -- not read this lane |
| `func_ov091_021339fc` | Ov091_HostSites.cpp | **DECOMPILE, decomp-side** -- see section 4. Not a port defect: the port body is a verified-correct propagation from origin/main; the local `src/func_ov091_021339fc.c` carries a stale NONMATCHING banner main has already disproved. |

### (b) MSVC-name shadows

12 before and after this lane (untouched; none of the 10 edited files carry
an MSVC-mangled face). Full list in `build/linkage_after.txt` under
"MSVC-NAME SHADOWS." All 12 are real C++ methods whose Itanium-named matched
TU is not linked -- BabyPenguin/Coffin/CrazedCrate/CutsceneObject(x2)/Eyerok/
Minimap::Render/SkiLift/Player::Behavior/SharedFilePtr/Scene::AfterInitResources.
Not read individually this lane; each needs the same per-symbol reading the
MSVC-name rows in SHADOWA.md give as precedent (register/ABI wall vs. genuine
retirement candidate).

### (c) Whole-function transcriptions enrolled in a slice manifest, in place of a matched src body

Methodology: `grep -oh "port/unmatched/.*\.\(cpp\|c\)" port/slice_*.txt`
excluding commented-out lines (233 raw hits collapsed to 37 actually-active
slice lines once comments are excluded -- most of the 233 are retirement
notes, not live enrollments) unioned with every `unmatched/*.cpp` path
CMakeLists.txt adds outside a comment (274 more), for 311 total port/unmatched
files actually built into some target. Each was scanned for a defined symbol
(balanced-paren definition detector, not a naive regex -- an early pass
matching `SYM(...) {` without paren balancing produced dozens of false
positives on ordinary call expressions) whose name also names a standalone
`src/` TU, then checked against `linkage.py`'s own `_reasons_in()` for a bound
PORT_HOST_ABI tag.

18 genuinely untagged symbols across 10 files, all fixed this lane (section
3). Five moved the SHADOWS count (they survive to the linked map); the other
thirteen are single-caller bodies MSVC inlined into their TU's dispatcher, so
their symbol never reaches `walk_window.map` at all and `linkage.py`'s
stem-join is structurally blind to them -- **this is the specific
under-counting the coordinator asked about**: a whole class of PMF-wall host
copies (MgSnowball, MgCurling2, MgMemory1, MgMemory2's field-PMF case,
MgWiggler) that are real, correct, wall-driven host copies, invisible to the
tool not because they're undocumented but because inlining erases their
map row. They are now documented to the same standard as their tagged
neighbours; the queue numbers can't reflect it because the queue can't see
them, and that gap is inherent to a map-symbol-based census, not a bug in
this fix.

Additional findings, not edited (correctly excluded per their own ruling or
already resolved):

- **Cap_StateDispatch.cpp / `_ZN6Player14InitMetalWarioEv`**: false positive.
  A genuine FACE (cdecl->thiscall forwarding bridge into the real matched
  method), not a stand-in. Not touched -- also inside the arity family's
  named exclusion zone by proximity, so left alone either way.
- **MgPachinko_Faces.cpp, MgSmartball_Faces.cpp**: alias/facegen-refusal
  material (`/alternatename` rows and hand-ruled aliases), not body
  transcriptions. `MgPachinko_Faces.cpp` carries the same period-not-colon
  `PORT_HOST_ABI.` banner LINKMG.md found across 14 files, but the file's
  content is FACES-bucket material, not SHADOWS work; left for the FACES
  bucket's own review rather than mis-tagged here.
- **Ov029_MethodFaces.cpp** (15 symbols in the combined-list scan): confirmed
  legitimate FACES -- explicit forwarding bridges into real C++ methods
  (`{ return ((X*)s)->Method(); }`), "the OPPOSITE of scaffolding" per
  linkage.py's own doc. Not a stand-in.
- **Ov007_DoodleCanvas_020b8fd4.cpp**: already self-resolved. This is a
  documented INTERIM transcription (not byte-matched, six-instruction
  register-tiebreak residual against the ROM, explicitly provisional) whose
  CMake guard (`DOODLE_INTERIM_SOURCES`) is conditioned on
  `src/func_ov007_020b8fd4.c` NOT existing. That src TU now exists and is the
  one actually linked (`build/port/walk_window.map:12177-12178` resolves
  `_func_ov007_020b8fd4` to `func_ov007_020b8fd4.c.obj`, not the port file).
  The port/unmatched file is dead source today -- present, uncompiled, safe
  to delete in a cleanup pass, not an active stand-in.
- **func_02038324_hostcopy.cpp**: see (a) above -- deliberately left
  undocumented by an earlier ruling.
- **Minimap_Behavior.cpp**: see (a) above -- the VS16 mod, owned elsewhere.

### (d) Inert-return or abort stubs seated in a live path

Searched for trivial `{ return CONST; }` bodies in port/hal and
port/unmatched (a balanced definition scan, not a grep on the word "stub" --
that word appears in 38 files, almost all unrelated defensive-assert or
documentation prose). Two hits, both already fully documented and correctly
classified; neither needs a change:

- **`_ZN8Platform20UpdateKillByMegaCharEsss5Fix12IiE`**
  (port/hal/megachar_stub.cpp): `return 0;` unconditionally. Explicitly and
  deliberately undocumented per its own banner ("THIS WAS NEVER A HOST-ABI
  EXCEPTION AND MUST NOT BE TAGGED AS ONE... a stub standing in for a matched
  TU that could link, which is the definition of a SHADOW"). Live only in the
  three narrow gate-9 harnesses (smoke_actor, smoke_savestate, smoke_persist)
  that link no Player gate / RaycastLine / ov002 body, where the real
  closure genuinely cannot be built; the main walk_window targets already
  link the real forwarding version via hal/wf_enemy_bridges.cpp. Classified:
  **refuse**, dependency-closure wall, correctly still counted (it's outside
  this lane's 311-file port/unmatched-in-build-target scope since it's not
  reached from a slice line or a literal CMakeLists path -- it's pulled in by
  the three narrow harness targets specifically).
- **`func_0203d974`** (port/hal/star_flow.cpp): `return 1;` unconditionally.
  Correctly PORT_HOST_ABI-tagged already ("src reads the unmapped
  console-type halfword at 0x027ffc40"). The DS-faithful `return 0` branch
  walks a sound heap the port intentionally never builds
  (hal/sdat/consumer.cpp skips it), which would null-deref on host; `1` is
  the port's own deliberate, documented, already-classified divergence.
  Classified: **refuse**, documented and correct as-is.

## 3. What was retired: the ten commits

All ten are pure PORT_HOST_ABI tag-binding fixes -- comment-only, no `src/`
edit, no code change, no link change, no hostgen.py table entries. Each binds
an already-written ruling to the specific definition `linkage.py`'s
`_reasons_in()` scans for, the identical mechanism LINKMG.md used for 43 of
its 46 rows ("a ruling made, written down, and never bound to the symbol the
queue asks about"). Smallest-first by symbol count, one commit each:

1. `ce3529659` Goomboss_HostSites.cpp -- 1 symbol
2. `5a1c6ff20` ModelAnim_Renders.cpp -- 1 symbol
3. `0c59e3c53` MgSound_ShadowSlot35.cpp -- 1 symbol (also fixed the
   period-not-colon banner LINKMG.md flagged as a repo-wide pattern)
4. `c6aecfebf` Klepto_Render.cpp -- 1 symbol (new tag, no prior banner)
5. `9c5ef1dfc` Ov085_PrincessPeach_Declshapes.cpp -- 2 symbols
6. `b523378ed` MgSnowball_StateDispatch.cpp -- 2 symbols
7. `60fd21736` MgMemory1_StateDispatch.cpp -- 2 symbols
8. `d0e115652` MgMemory2_FieldPmf.cpp -- 1 symbol
9. `08d1def6c` MgCurling2_StateDispatch.cpp -- 3 symbols
10. `54ebf0187` MgWiggler_StateDispatch.cpp -- 4 symbols

18 symbols, 10 files. No retire-plain or hostgen-patch rows were executed
this lane (see section 2c -- the remaining SHADOWS/gap rows found either
needed a reading this lane didn't have time for, are owned by another lane,
or are already correctly ruled to stay undocumented). Verified after every
group: full `port\build-port.cmd` (all guards green, rc=0),
`port/tools/linkage.py --queue`, `port/tools/objsrc_check.py`,
`port/tools/inferred_stub_guard.py`.

Build guarded with `TMP=TEMP=C:/tmp/standin-sweep/build/tmp` (the same
tailjump_guard-selftest-vs-antivirus interaction LINKMG.md and this
worktree's own memory note record -- not a tree defect).

## 4. The ARM r0 ride-through cross-check (CALLEE_SEAM)

`port/tools/hostgen.py`'s CALLEE_SEAM mechanism (added for HUD::InitResources,
lane HUD-INITRES) rewrites a callee NAME to a host seam via file-scope
`#define`, and its own comment states the disqualifier precisely: "a receiver
that exists only in a register with no in-scope expression naming it." The
REG_RIDE_ARG family (func_ov002_020e444c) is the sibling mechanism for when
the missing thing is an *argument* rather than a destination, and it is
eligible for exactly the same reason: the receiver (`c`) is the TU's own
named parameter, live and in scope at the call site.

`port/tools/tailjump_guard.py` tracks the other 33 live ARM-register-carried
frames (35 total minus the 2 it reports RETIRED: `func_ov007_020add3c` and
`func_ov007_020b6eb4`, both already converted to explicit-argument host
copies in `port/unmatched/Ov007_ImplicitR0_020add3c.cpp` and
`Ov007_BdPending_020b6eb4.cpp`). Cross-checked the remaining 33 against
CALLEE_SEAM/REG_RIDE_ARG eligibility: **none qualify**. All 33 are pure tail-
jump veneers -- `jmp` reusing the caller's own argument frame with NO named
receiver anywhere in the forwarder's own source (5 plain-call "A" frames that
aren't ride-throughs at all, 7 "C" and 21 "V" ride-through frames, several
folded duplicates onto `_ZN6Player17St_EndingFly_MainEv`). The value these
frames carry exists only in a CPU register between one `bl`/`jmp` and the
next; there is no C expression in the forwarder's own scope to name it, which
is exactly hostgen.py's own disqualifying condition. These are correctly
`tailjump_guard`-guarded rather than hostgen-retired, and the guard is the
right mechanism: it is a link-time byte scan for `E8`/`E9` opcodes confirming
the tail-jump shape survives, not a source transformation, because there is
no source-level fix available for a value that only ever lived in a register.

Conclusion: the CALLEE_SEAM/REG_RIDE_ARG family has already claimed every row
in this class that it CAN claim (the 2 retirements above are the full set to
date); the 33 remaining are a structurally different case (destination known,
argument unrecoverable in source) and correctly stay as guarded refuse rows,
not a queue this lane should try to shrink further.

## 5. Ranked DECOMPILE list

Only one row surfaced by this lane's specific cross-checks (section-2c's
311-file port/unmatched sweep, section 2d's stub sweep, and the
section-4 CALLEE_SEAM cross-check) that names ROM code without a *verified*
matched TU in this tree today:

| rank | symbol | address | size | module | note |
|---|---|---|---|---|---|
| 1 | `func_ov091_021339fc` | 0x021339fc | 0x270 | ov091 | **Not new decomp work** -- already solved on origin/main and proven MATCHING there (`tools/match.py`, 2004/b56, strict relocs, base 0x02130f00, evidence at `runs/rel0215/out/w3-f2/f021339fc_fidelity.txt`). This tree's `src/func_ov091_021339fc.c` carries a stale NONMATCHING banner with an unbuildable ARM `asm` hatch that main's version no longer has (main restructured to hoist two byte reads before the tests it needed the hatch to force-order). The fix is a straight port of main's already-proven restructuring into this tree's `src/`, not new decompilation. A decomp lane should sync it and delete `port/unmatched/Ov091_HostSites.cpp`'s section (3) once done; five Itanium-name bridges the two trees disagree about (listed in that file, lines 335-370) need carrying across at the same time, all by-address and already resolved in the port file's own commentary. |

This is a short list because this lane's methodology is a targeted
cross-check (slice/CMake enrollment, trivial-stub scan, ARM-ride-through
walls), not an exhaustive census of every ROM function without a matched
src TU -- that census is `port/arm9_frontier.txt` / the per-overlay `_seat.txt`
files' job, and is out of scope for a stand-in sweep. The four SHADOWS rows
this lane left as "refuse -- not read this lane" (intro_kuppa_dispatch.cpp's
five symbols, scene_boot.cpp's three, LakituBro::InitResources,
MgTrampolineTerror_Spawn, ShutterBob_Spawn) are the honest next reads for
whoever continues this lane; none were confirmed DECOMPILE candidates
because none were read closely enough this lane to say either way.

## 6. What remains and why (not done, in scope but not executed)

- The 14 remaining SHADOWS (section 2a) beyond the 5 this lane retired: 8 not
  read (listed above), 1 deliberately kept undocumented by an earlier ruling
  (func_02038324), 1 owned by the VS16 lane (Minimap_Behavior), 1 is
  decomp-side debt with a proven fix on main (func_ov091_021339fc, section 5),
  and the megachar/star_flow stub pair from (d) is correctly classified and
  needs nothing.
- The 12 MSVC-NAME SHADOWS (section 2b): not read individually this lane.
- No retire-plain or hostgen-patch (table-entry) rows were executed. Every
  genuine gap this lane found was a documentation/classification gap
  (SHADOW -> documented exception), not a case where the matched src TU could
  actually replace the host body outright. That is a real finding, not a
  shortfall: the easy retirements in this corner of the tree were mostly
  already taken by LINKMG/SHADOWA; what was left standing was rulings that
  existed in prose but were not bound to the symbol the tooling reads.
- The arity family (cap_yoshi, bly_aimed, Coin dispatch, MgBase) was not
  executed on per the coordinator's mid-lane instruction; inventoried only
  where it surfaced incidentally (Cap_StateDispatch.cpp, confirmed a FACE and
  left alone either way).
- Not merged, not pushed.

## 7. Commands

    git -C C:/Users/bmanu/Documents/sm64ds-wt/cons worktree add C:/tmp/standin-sweep -b port/standin-sweep 3b605d0c4
    # junctions (PowerShell New-Item -ItemType Junction): extracted, tools/mwccarm,
    # tools/bin, build/objcache, build/assets, all from the cons checkout
    export TMP=C:/tmp/standin-sweep/build/tmp TEMP=C:/tmp/standin-sweep/build/tmp
    cmd //c "port\build-port.cmd"                          # baseline, all guards, rc=0
    python port/tools/linkage.py --queue > build/linkage_before.txt   # via git stash
    # ten commits, section 3
    python port/tools/linkage.py --queue > build/linkage_after.txt
    python port/tools/objsrc_check.py
    python port/tools/inferred_stub_guard.py
    python port/tools/tailjump_guard.py --build-dir build/port --report > build/tailjump_report.txt
    python port/tools/battery.py --skip-build > build/battery1.log
