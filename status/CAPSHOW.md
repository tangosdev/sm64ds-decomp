# Lane CAPSHOW -- DELIVERY

The wing, metal and vanish caps now draw in the online VS arenas, and a
player can pick one up. The cap-visibility manager the cartridge runs from
Stage::Render is hosted as the ROM's own code, and the two host-side faults
that seating it exposed are closed with the ROM's own semantics.

* Worktree `C:\tmp\capshow`, branch `port/capshow`, rebased onto the cons tip
  `9cf4eb8b8`
* Branch tip: see the report; this file is the last commit
* Nothing under `src/` or `include/` was touched. No slice_gate*.txt was
  touched either.

---

## 1. What was wrong

status/VSMAP.md measured it ("THE CAPS DO NOT DRAW"): every CAP (actor 269,
class WaterfallMist) in every VS arena spawned, sat on the behaviour list at
the ROM's coordinates, ran its render slot every frame, and drew nothing.

The chain, each link a line in matched source:

1. `src/_ZN13WaterfallMist6RenderEv.cpp:14` returns while `unk_3ff == 1`.
2. `src/_ZN13WaterfallMist8BehaviorEv.cpp:41` writes that 1 every frame while
   bit 1 of the byte at +0x3eb is clear. That byte is +0x1b of the registry
   node InitResources builds at +0x3d0 through `func_ov001_020ab228`, which
   zeroes it: a cap starts hidden.
3. The only writer of that bit in the tree is `func_ov001_020aa420`, the VS
   cap manager. Its only caller is `func_ov001_020aaf40`. Its only caller is
   `Stage::Render` (`src/_ZN5Stage6RenderEv.cpp:125`).
4. The port hosts `Stage::Render` in `port/tests/walk_window.cpp`, and
   `port/stage_lifecycle_map.txt:777-783` listed `func_ov001_020aaf40` among
   its eleven unresolved callees.

So the manager never ran, and no cap was ever told to show.

## 2. The change

| file | what |
|---|---|
| `port/slice_cap.txt` | NEW manifest. Eleven matched TUs: `func_ov001_020aaf40` (the Stage::Render callee, mode test inside), `func_ov001_020aa420` (the VS manager) with `func_ov001_020aa79c` and `func_ov001_020aa7b8`, the single-console pair `func_ov001_020aadac` / `func_ov001_020aaa54` with `func_ov001_020aa858`, `func_ov001_020aa960`, `func_ov001_020aa6e4`, and the two arm9 leaves `func_0202a8e0` (sound-slot allocator) and `func_02029600` (minimap-area test). The head of the file carries the derivation, the record census that says three of six caps show, and the ROM offsets of every function. |
| `port/CMakeLists.txt` | Reads slice_cap.txt into `SLICE_CAP_SOURCES` on the three targets that host Stage::Render (`smoke_player`, `walk_window`, `walk_window_hires`). Refuses the manifest if it ever relists one of the four chain functions slice_gate32/33 already compile (`func_ov001_020aa6b0`, `_020aa6cc`, `_020ab110`, `_020ab228`), which would be LNK2005 three thousand objects later. Substitutes two host copies inside existing slice loops (sections 3 and 4): `unmatched/Animation_AdvanceDivGuard.cpp` for `src/_ZN9Animation7AdvanceEv.cpp` in the slice_gate7 and slice_gate10 loops, and `unmatched/WaterfallMist_Behavior_HostSites.cpp` for `src/_ZN13WaterfallMist8BehaviorEv.cpp` in the slice_gate33 loop. |
| `port/tests/walk_window.cpp` | One call: `func_ov001_020aaf40()` at Stage::Render:125's position, immediately after `CylinderClsn::Process`, UNCONDITIONAL like the ROM's (the `data_0209f2d8 == 1` test is inside the callee; guarding here on VS would host a function the cartridge does not have). Gated on `boot_spawns` like the line above it, because the registries are filled by the actors' own InitResources. `SM64DS_NO_CAP_MANAGER=1` restores the pre-seat behaviour on this binary. |
| `port/unmatched/Animation_AdvanceDivGuard.cpp` | NEW. Host copy of `Animation::Advance` with the ARM divider's `n % 0 = 0`. Section 3. |
| `port/unmatched/WaterfallMist_Behavior_HostSites.cpp` | NEW. Host copy of `WaterfallMist::Behavior` with its one shadow virtual call spelled at the ROM's target, `ModelAnim::UpdateVerts`. Section 4. |
| `port/hal/actor_classes_bob_world.cpp` | The `SM64DS_CAP_PROBE` render probe prints one line per cap per VERDICT CHANGE, keyed on the actor, instead of the first eight calls. The manager runs in Stage::Render and its flag is read by the next frame's Behavior, so frame 1 is always "returns early" for every cap and an eight-line probe could only ever witness the broken state. (+0x10, the word it first keyed on, is not a per-actor id: every VS cap reads 0x20000 there.) |
| `port/tools/vs_shots.py` | Sets `SM64DS_CAP_PROBE`, reports each candidate's own `[cap]` verdict and the arena's draw count, prefers a candidate whose cap the manager turned on over any camera angle, and takes `--cand` to boot named record indices only. |

No new mount. The three ov001 tables the manager reads
(`data_ov001_020ad628`, `_020ad630`, `_020ad634`) were already on
`port/ov001_syms.txt:54` and ov001 is already in `PORT_ACTOR_OVERLAYS`, so the
mount VSMAP costed for this fix was already paid.

### Every hosted body is the ROM's

All eleven manifest files were byte-verified against the cartridge under the
pinned compiler with strict relocation checking before anything was wired:

    python tools/match.py --c src/<f> --func <f> --addr <a> --size <s> \
        --bin extracted/overlays/overlay_0001.bin --base 0x020aa420 --module ov001

    func_ov001_020aa420.c   MATCHING VERSIONS: 2004/b56
    func_ov001_020aa6e4.cpp MATCHING VERSIONS: 2004/b56
    func_ov001_020aa79c.c   MATCHING VERSIONS: 2004/b56
    func_ov001_020aa7b8.cpp MATCHING VERSIONS: 2004/b56
    func_ov001_020aa858.cpp MATCHING VERSIONS: 2004/b56
    func_ov001_020aa960.cpp MATCHING VERSIONS: 2004/b56
    func_ov001_020aaa54.cpp MATCHING VERSIONS: 2004/b56
    func_ov001_020aadac.c   MATCHING VERSIONS: 2004/b56
    func_ov001_020aaf40.c   MATCHING VERSIONS: 2004/b56
    func_02029600.c         MATCHING VERSIONS: 2004/b56   (arm9, arm9_dec.bin)
    func_0202a8e0.c         MATCHING VERSIONS: 2004/b56   (arm9, arm9_dec.bin)

None carries a NONMATCHING banner or an asm block. The two host copies below
are the matched TUs with exactly one expression changed each, and each banner
says which and why. Nothing in this lane is a guessed body; the inferred-stub
ratchet ran on every build and the baseline did not move.

### Three of six, and which three

Every arena places six CAP records and the DS shows three. That is what the
manager computes, not a choice this port made (`slice_cap.txt` section 2):
each cap type has a priority-1 record (`0x1004` wing, `0x1104` metal, `0x1204`
vanish) and a priority-0 alternate (`0x0004`, `0x0104`, `0x0204`), the manager
turns on the highest-priority node per type, and the alternate stays hidden
until that type's cap is taken. Map 3 (level 29) places the low trio five
times over; the same rule hides all of them.

## 3. Fault one: Animation::Advance divides by zero the first frame a cap shows

The first run of the seated manager on VS map 2 faulted on frame 0:

    FAULT code c0000094 at +0x00104d7e accessing 00000000
      walker node 30038BD0 actor 30038BA8 id 0x10d
      regs eax=00001000 ecx=30038ef8 edx=00000000 ...

+0x104d7e is `Animation::Advance` +0x1e (walk_window.map), actor 0x10d is the
CAP, eax is the animation speed (0x1000) and edx the frame count (0). The VS
cap's ModelAnim carries a MODEL AND NO ANIMATION: `WaterfallMist::InitResources`
seats only the BMD for mType 4 (`ModelBase::SetFile` at +0x300, no
`Animation::LoadFile`, no `SetAnim`) and sets `unk_35c` (the speed) to 0x1000,
and `WaterfallMist::Behavior:81` calls `Animation::Advance(this + 0x350)` on
every frame the cap is not hidden. Until this lane every VS cap was hidden and
Behavior returned before the call, so the `(currFrame + speed + len) % len`
with `len == 0` had never been reached on the host.

What the cartridge does with it: mwccarm compiles that modulus as
`bl 0x01ffabe4` (`config/arm9/relocs.txt`, from 0x02015c64, module itcm), the
ITCM signed divide that returns the quotient in r0 and the remainder in r1.
Disassembled (`tools/disasm.py extracted/dsd/arm9/itcm.bin --offset 0x2be4
--base 0x01ff8000`):

    01FFABE4  eor ip, r0, r1        ; sign of the quotient
    ...
    01FFABF8  cmp r1, #0            ; r0 = |n|, r1 = |d| by here
    01FFAC00  beq 0x1ffaddc         ; ZERO DIVISOR: straight to the epilogue
    ...
    01FFADDC  ands r3, ip, #0x80000000
    01FFADE0  rsbne r0, r0, #0      ; quotient sign
    01FFADE4  ands r3, ip, #1
    01FFADE8  rsbne r1, r1, #0      ; remainder sign
    01FFADEC  bx lr

So on the DS `n / 0 = n` and `n % 0 = 0`, Advance parks `currFrame` at 0 and
carries on, and nothing reads the frame because the ModelAnim's file pointer
(+0x360) is null and the pose is never rebuilt. x86 `idiv` faults instead.

`port/unmatched/Animation_AdvanceDivGuard.cpp` is `src/_ZN9Animation7AdvanceEv.cpp`
with that one modulus routed through `d ? n % d : 0`, tagged PORT_HOST_ABI,
the shape `port/unmatched/VS_PathDivGuard.cpp` already uses for the PathLift
quotient sites. The src line is skipped inside the two CMake slice loops that
list it; `slice_gate7.txt` and `slice_gate10.txt` are untouched.

A note for another lane, recorded and not acted on: the port's own
`__aeabi_idiv` host body (`port/hal/actor_vtables.cpp:556`) answers
`n / 0 = 0`, and the ROM routine above answers `n / 0 = n`. No site this lane
touched takes the quotient of a zero divisor, so nothing here depends on it.

## 4. Fault two: picking a cap up returned into the actor list

With the caps showing, the orbiting player of `vs_shots.py` walked into one
on two of the first twelve boots (map 2 record 2, map 1 record 1), and the
tick after the pickup faulted:

    FAULT code c0000005 at +0x2fc387b0 accessing 300387b0
      walker node 30038BD0 actor 30038BA8 id 0x10d
      regs eax=00000001 ecx=30038cb8 edx=00000000 ...

eip is a HEAP address (`dd 300387b0` reads 30038bd0, the walker node: it is
a pointer inside the actor list), eax is Behavior's return value and ecx is
the cap's CylinderClsn (+0x110), the last thing Behavior touches. So Behavior
finished and its `ret` popped the wrong word. cdb pinned which callee
(`build/tmp/cdb_trace.txt`: a breakpoint at every call-return site in
`WaterfallMist::Behavior` printing esp when edi is the collected cap):

    every frame before the pickup:
      E 1af1dc  R5963a0 1af1d4  R5963a3 1af1d0  R5963af 1af1cc
      R5963d3 1af1d4  R5963de 1af1cc  R59644c 1af1d0  R596452 1af1cc
      X 1af1dc 4f6023            <- ret pops the dispatcher's return, fine
    the frame after the pickup:
      E 1af1dc  R5963a0 1af1d4  R5963a3 1af1d0  R5963af 1af1cc
      R5963c7 1af1d0            <- func_020167a4 ran for the first time
      R5963d3 1af1d8            <- after call [eax+0Ch]: FOUR BYTES HIGH
      R5963de 1af1d0  R59644c 1af1d4  R596452 1af1d0
      (Access violation)

`src/_ZN13WaterfallMist8BehaviorEv.cpp:83-86`: once the cap's ModelAnim
carries a file (+0x360 != 0, which `func_ov002_020b74d0`'s `SetAnim` gives it
on the frame after pickup), Behavior calls `func_020167a4(c + 0x300)` and then
`((VObj *)(c + 0x300))->v03()`, a four-slot shadow's +0x0c. The ROM's
`_ZTV9ModelAnim` (0x0208e980) is `D1, D0, DoSetFile, UpdateVerts, Virtual10,
Render, Virtual18`, so +0x0c is `ModelAnim::UpdateVerts()`, no arguments. The
port fills `_ZTV9ModelAnim` in MSVC numbering with ONE destructor slot
(`hal/actor_classes_intro.cpp:440`: `[0] dtor, [1] DoSetFile, [2] UpdateVerts,
[3] Virtual10, ...`), so the shadow's +0x0c lands on `Virtual10(Matrix4x3 &)`,
a one-argument `__thiscall` whose `ret 4` pops a word the caller never
pushed. MSVC compiled Behavior without a frame pointer (`push esi / push edi
... pop edi / pop esi / ret`), so nothing repairs esp and its own `ret` takes
the saved-edi slot, which holds the list pointer.

`port/unmatched/WaterfallMist_Behavior_HostSites.cpp` is the matched TU with
that one call spelled `((ModelAnim *)(c + 0x300))->ModelAnim::UpdateVerts()`,
the `Lakitu_HostSites.cpp` treatment (the object is the cap's own ModelAnim,
constructed as one, so the qualified call and the ROM's virtual dispatch reach
the same body). `diff -w` against the src shows the added include and that
one statement, nothing else. The src line is skipped inside the slice_gate33
loop; `slice_gate33.txt` is untouched.

Why nobody hit this before: no VS cap could be picked up while none showed,
and in single-console play the ground cap is the lost-cap respawn, which this
tree has no boot for.

## 5. How it was proven

### The pictures

`python port/tools/vs_shots.py --map <m> --class CAP --cand <records>` on the
final binary, muted and minimised, the game's own camera (`cam_mode` stays
CAM_DS). `--cand` names the three priority-1 records on each arena; the
picker now prefers a cap whose own probe verdict is "draws".

| file | what the run measured |
|---|---|
| `status_shots/map2_level43_cap_f205.png` | VS map 2, level 43, wing cap record 2 at (341,356,-1106). rc 0. `this cap draws; 3 of 6 caps on the arena draw`. Frame 205, 7.9 degrees off axis, 1598 units from the eye. In frame: the red cap on the grass by the round shadow, a star in its bubble, the local Yoshi. VSMAP's picture of this exact frame showed the shadow and nothing on it. |
| `status_shots/map1_level51_cap_f166.png` | VS map 1, level 51, wing cap record 1 at (6048,449,414). rc 0. `this cap draws; 2 of 2 caps on the arena draw` (only two caps reached the render slot in this boot; the rest were culled). Frame 166, 8.1 degrees, 1188 units. In frame: the red cap hovering beside the local Yoshi's head, a tree, the fence. Sixteen frames later the orbit walks him into it and the pickup runs clean (section 4). |
| `status_shots/map4_level42_cap_f339.png` | VS map 4, level 42, wing cap record 2 at (-300,0,-300). rc 0. `this cap draws; 3 of 5 caps on the arena draw` (five distinct probe keys: records 2 and 5 share the position). Frame 339, 8.0 degrees, 1728 units. In frame: the red cap floating next to the local Yoshi at the pole. |

The pictures are ROM-derived imagery and stay out of git, like VSMAP's; the
paths are where the runs leave them.

### The probe, per arena, final binary

One line per cap per verdict change (`SM64DS_CAP_PROBE=1`), the run of map 2
record 0:

    [cap] uid 805540840 pos(-100,845,-1900)  model 2 unk_400 01 unk_3ff 01 flags3eb 00 -> RETURNS EARLY   (frame 1, before the manager's flag is read)
    [cap] uid 805538728 pos(341,356,-1106)   model 0 unk_400 01 unk_3ff 00 flags3eb 02 -> draws
    [cap] uid 805540840 pos(-100,859,-1900)  model 2 unk_400 01 unk_3ff 00 flags3eb 02 -> draws
    [cap] uid 805539784 pos(-900,651,-1300)  model 1 unk_400 01 unk_3ff 00 flags3eb 02 -> draws
    [cap] uid 805535560 pos(3700,94,300)     model 2 unk_400 00 unk_3ff 01 flags3eb 08 -> RETURNS EARLY
    [cap] uid 805537672 pos(-2614,158,2208)  model 0 unk_400 00 unk_3ff 01 flags3eb 08 -> RETURNS EARLY
    [cap] uid 805536616 pos(297,100,3606)    model 1 unk_400 00 unk_3ff 01 flags3eb 08 -> RETURNS EARLY

Three draw, one per type (model 0, 1, 2), all with `unk_400 01` (the
priority-1 records) and `flags3eb 02` (the manager's bit). Three stay hidden
with `unk_400 00` and `flags3eb 08`. The showing caps' y has risen (845 to
859, 582 to 651): they are alive and bobbing. Map 4 reads the same
(records 0x1004/0x1104/0x1204 draw, the three low records hidden) and map 1
reads the same for whichever caps the camera had in frame.

Before this lane the same probe read `unk_3ff 01 flags3eb 00 -> RETURNS
EARLY` for every cap on every arena, forever (status/VSMAP.md, "THE CAPS DO
NOT DRAW").

### The pickup

Map 2 record 0's boot (`build/vs_shots/map1_CAP_0/run.log`): the orbit walks
the player into the wing cap at frame 147, he enters `St_NoControl`
(`st=020c976c`, the hat-change state), the cap's own Behavior runs the ROM's
collected state (`func_ov002_020b76ec` then `_020b74d0`, animation seated,
destroyed when it finishes) and the run exits rc 0 at frame 470 with no
quarantine line. The same boot faulted before section 4's copy. Map 1 record
1 (pickup at frame 182) likewise: rc 3221225477 before, rc 0 after.

### The census

`python port/tools/vs_arena_census.py --frames 420`, one quiet boot per
arena, manager build: `VERDICT: PASS`. Actor 269 rom / spawn / alive:
map 1 `6 / 6 / 6`, map 2 `6 / 6 / 6`, map 3 `15 / 15 / 15`, map 4
`6 / 6 / 6`. Every other class row is identical to VSMAP's; the manager
changes what draws, not what spawns.

### The four-window proof

`python port/tools/vs4_proof.py --map 1 --frames 900` on the final binary:
`VERDICT: ALL GREEN`, every rung PASS. Four windows exit rc 0, all four seat
a four-player world on the same round, four distinct Yoshis in four palette
rows, every window reads every other window's pad, every slot in the same
place in all four windows (worst delta 0), MATCH OVER in all four with the
same scores, and `dhdiff` reports NO DIVERGENCE on all 401 common frames for
every pair. The per-window census is `[census] 37 spawned (10 classes), 0
skipped` in all four, VSMAP's number. The manager running in every window
did not desynchronise anything: it reads the registries the actors built and
the players' cap state, all of which are lockstep state.

### The battery

`python port/tools/battery.py --skip-build`, from the worktree root with
`TEMP`/`TMP` at `C:\tmp\capshow\build\tmp`, exit 0, zero FAIL lines, run three
times on this lane:

* on the manager build (section 2, before the Advance copy): `battery: ALL GREEN`
* on the manager + Advance build: `battery: ALL GREEN`
* on the final binary (manager + both copies + the probe re-key): `battery: ALL GREEN`

Every scene selftest ok, the default boot reaches the title clean, all fifty
level selftests ok (the two standing SKIP_CLASS rows, level 27's
TTC_MOVING_BEAM and level 45's GOOMBOSS, are the decomp's and unchanged).
The manager runs on every one of those level boots too (it is unconditional),
takes the single-console arm, finds three empty registries on a level with no
cap and does nothing, which is the ROM's behaviour on those levels.

### Commands, verbatim

    git worktree: C:\tmp\capshow (junctioned tools\mwccarm and build\assets from cons per decomp-worktree)
    git stash push -u ... ; git rebase 9cf4eb8b8 ; git stash pop
    python tools/match.py ... (eleven runs, section 2)
    set TMP=C:\tmp\capshow\build\tmp & set TEMP=C:\tmp\capshow\build\tmp
    port\build-port.cmd                                  (four builds, all exit 0, every guard OK)
    python port/tools/battery.py --skip-build            (three runs, section 5)
    python port/tools/vs_arena_census.py --frames 420    (VERDICT: PASS)
    python port/tools/vs_shots.py --map 1 --class CAP --cand 0,2
    python port/tools/vs_shots.py --map 3 --class CAP --cand 0,1,2
    python port/tools/vs_shots.py --map 0 --class CAP --cand 1,2,3
    python port/tools/vs4_proof.py --map 1 --frames 900  (VERDICT: ALL GREEN)

## 6. What is NOT proven

* **The single-console arm ran only on levels with no cap.** `func_ov001_020aaf40`
  takes the `data_0209f2d8 != 1` arm on every level selftest and finds empty
  registries. A single-player level that registers a cap (the lost-cap
  respawn, or a level 4/17-type WaterfallMist record) would exercise
  `func_ov001_020aadac` / `_020aaa54` for the first time; those are matched
  TUs and byte-verified, but no boot in this tree reaches them with data.
* **Map 3 (level 29) was censused, not photographed.** Its priority-1 caps sit
  on a ledge at y=6000 and no `vs_shots` candidate for it is defined. The
  probe logic is arena-independent and the census row is right, but nobody
  looked at that arena.
* **The pickup was watched through the probes, not the picture.** The player
  entering the no-control state, the cap's collected state running and the
  clean exit are measured; what the swap looks like on screen (the Yoshi
  wearing the cap, the wing) was not captured, because the shot tool's frame
  picker aims at the cap, not the player.
* **Respawn after a pickup.** The manager's priority-0 alternate should turn on
  once its type's cap is held (`slice_cap.txt` section 2). The pickup boot
  ends 320 frames later with the alternate still hidden and a new cap at the
  taken cap's position; whether that is the ROM's timing or a port difference
  was not measured, because the 470-frame window was sized for the picture.
* **Sixteen-player and long matches.** 470 and 900 frames, four windows.
* **Two host copies carry the port's calling-convention and divider facts, not
  the ROM's bytes.** Both are the matched TU with one expression changed and
  both banners say which; the diffs are one line each. They are the right
  shape for this tree (VS_PathDivGuard, Lakitu_HostSites), and they are still
  copies. If `_ZTV9ModelAnim` is ever refilled in ROM numbering, the Behavior
  copy becomes unnecessary and the slice_gate33 skip should go with it.
