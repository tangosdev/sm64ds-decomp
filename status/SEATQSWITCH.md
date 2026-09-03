# Lane SEAT-QUESTIONSWITCH -- QUESTION_SWITCH (26, ov002) seated

LINKSCOPE.md lane 11: the big "?" switch. Its only placements are the test
map (level 0, x1) and habatake (level 31, x1, a level that does not boot), so
the live edge is level 0. Seated, byte-matched where the ROM lets it be,
proven alive and toggling under FAULTS_FATAL on level 0.

* Worktree `C:\tmp\seat-qswitch`, branch `port/seat-questionswitch`,
  REBASED (clean, no conflicts) onto cons `da58b6db0` (the all-green pushed
  base); seat commit `21cc7d4f2`. Not merged, not pushed. Handoff: C seated
  it, this lane rebased and re-proved on da58b6db0.
* Files: `port/hal/actor_classes_question_switch.cpp` (the 32-slot fill, the
  faces, the SM64DS_QS_PROBE tick probe), `port/slice_gate212.txt`, one
  registry row plus three declarations in `port/hal/actor_classes.inc`, the
  slice block and three source lines in `port/CMakeLists.txt`, one mount line
  in `port/ov002_syms.txt`, one host copy appended to
  `port/unmatched/ModelAnim_Renders.cpp`, one ruling line in
  `port/tools/inferred_stub_adjudicated.txt`. Never touched: cons, `src/`,
  `include/`.
* Build TEMP/TMP `C:\tmp\seat-qswitch\build\tmp`; the proof logs
  (`proof_l0c.log`, `proof_land.log`) and `battery.log` live there.
* linkage (port/tools/linkage.py on walk_window.map): 9416 (83.1%) on this
  branch rebased onto da58b6db0 (full clean build). +12 matched TUs enrolled
  via slice_gate212.txt (the +13 LINKSCOPE predicted minus Render, a host
  copy, see section 3).
* battery (2026-09-03, full clean rebuild then port/tools/battery.py on
  da58b6db0): ALL GREEN. 50 mounted levels + 34 hosted scenes each ran under
  SM64DS_FAULTS_FATAL=1, default boot reaches the title, linkage 9416,
  ptr_audit 0 unhosted code pointers, shipcfg (PORT_ROM_CLEAN static CRT)
  build + selftest ok. The only skips are the pre-existing decomp-owned
  level 27 (TTC_MOVING_BEAM) and level 45 (GOOMBOSS); level 0 -- this seat's
  live edge -- runs bare and unskipped.
* vtspan --seats (build/port/_abicheck_disasm, 950 hal objects): no
  wrong-body seats globally (1706 shared-arm9-body fills checked); the
  QuestionSwitch fill alone contributes 17 checked seats, every one at the
  ROM canonical slot, no regressions.
* FAULTS_FATAL liveness re-proven on this base (not merely a census): level
  0, 600 frames, rc 0, 0 fault/quarantine/UNHOSTED, 599 Behavior ticks. The
  idle run sits at rest (correct for an unpressed switch); the drop-on-switch
  run (SM64DS_SPAWN=1450,500,4050 SM64DS_SELFTEST_IDLE=1) shows real STATE
  CHANGE -- countdown 8->0, collider up->down, pressed bit 0->1, anim 0->4.

## 1. The class, re-derived on this tree

`_ZTV14QuestionSwitch`, ov002 0x02108e5c, RTTI daObjHatenaSwitch_c.
`python port/tools/vtspan.py . _ZTV14QuestionSwitch` -> 32 slots, Platform
subclass; tail, typeinfo, terminator, raw-run and next-symbol routes all
agree; all 32 words resolve. Own slots: 0 InitResources, 3 CleanupResources,
6 Behavior, 9 Render, 16 D1, 17 D0, 21 OnGroundPounded
(`func_ov002_020b4fc4`). Slot 12 stays ActorBase::OnPendingDestroy, slot 31
Platform::Kill. No pointer-to-member in Behavior, so no state table and no
state seat. Object 0x724 bytes: Platform base, MovingMeshCollider at 0x324
(raised mesh) and 0x4ec (pressed mesh), ModelAnim at 0x6b4.

`QuestionSwitch_SpawnInfo` ov002 0x02108e38, +4 halfword 26. The two CLPS
blocks InitResources hands MovingMeshCollider::SetFile, `data_ov002_0210d774`
(raised, 0x20 bytes) and `data_ov002_0210d8b4` (pressed, 0x20 bytes), are
mounted with it; the first link failed on d774, which until now was only the
boundary comment of the IceSheet block above it in ov002_syms.txt. The four
SharedFilePtr cells were already mounted and `__sinit_ov002_02100ec4` (gate
10) seeds them; the probe confirmed cell dd60 reads {000102bf, ptr} at render.

## 2. Slot 21, the one marker body (the inferred_stub_guard rule)

`src/func_ov002_020b4fc4.c` carries dsd's "recovered from vtable slot
identity" marker, so the guard refuses it unseated-by-default. Per the
SEATOV002 rule it was byte-matched FIRST:

    python tools/match.py --module ov002 --c src/func_ov002_020b4fc4.c
        --func func_ov002_020b4fc4 --addr 0x020b4fc4 --size 0xc
    +0x00 mov r1, #0 | +0x04 strb r1, [r0, #0x718] | +0x08 bx lr
    MATCHING VERSIONS: 2004/b56

and ruled REAL_DECOMP in `port/tools/inferred_stub_adjudicated.txt`. The body
zeroes the +0x718 countdown so a ground pound flips the switch on the frame
the landing edge arrives. `inferred_stub_guard OK: no new guessed vtable body
seated.` No handler needed an inert stand-in.

## 3. Render is a host copy (the ModelAnim slot-5 collision)

First proof run faulted c0000005 in Model::Virtual10 +0xc reading a null
matrix, through ModelAnim::Virtual18 -> ModelAnim::Virtual10, on the switch's
first drawn frame. `src/_ZN14QuestionSwitch6RenderEv.cpp` dispatches slot 5
of a six-virtual local shadow over the ModelAnim at 0x6b4: in ROM numbering
that is ModelAnim::Render(const Vector3 *scale = 0); in the host
`_ZTV9ModelAnim` (MSVC numbering, one destructor slot) slot 5 is
Virtual18(matrix, scale), and the 0 became the matrix. The QuestionBlock,
Butterfly, Fish and MontyMole case exactly. The one-line host copy
`_ZN14QuestionSwitch6RenderEv` is appended to
`port/unmatched/ModelAnim_Renders.cpp`, the matched TU stays off the slice,
and the fill's slot 9 calls the copy. That is the missing +1.

## 4. Proof (the Klepto lesson: not a census)

Both runs: `SM64DS_LEVEL=0 SM64DS_WINDOW_SELFTEST=600 SM64DS_FAULTS_FATAL=1
SM64DS_QS_PROBE=1`, walk_window.exe, rc 0, no `[quarantine]`, no
`UNHOSTED`, no FAULT line.

Alive (`proof_l0c.log`, plus SM64DS_ACTOR_PROBE=1):

    [census] ... +  26 x1   QUESTION_SWITCH          (both census blocks)
    [pos] QUESTION_SWITCH   (1450,0,4050)  [1]
    [actor] QUESTION_SWITCH   model 30035B04 file 300AA7EC transforms 300AA78C mat.t (181,0,506) scene
    [qsprobe] 30035450 pos (1450,0,4050) count 8 landed 0 talk 0 collider up pressed 0 anim 0   x599 ticks

Toggling (`proof_land.log`, plus `SM64DS_SPAWN=1450,500,4050
SM64DS_SELFTEST_IDLE=1`: the player is dropped 500 units over the switch and
lands on it at y 126.6 by frame 39):

    [qsprobe] ... count 8 landed 1 talk 0 collider up   pressed 0 anim 0   (the collider callback _567c set the edge)
    [qsprobe] ... count 7 .. 1 landed 1                                     (the 8-frame countdown)
    [qsprobe] ... count 0 landed 0 talk 1 collider down pressed 1 anim 1    (mesh swapped to the pressed KCL, data_0209caa0[1] bit 31 set, particles 0x6d..0x70, talk latch)
    [qsprobe] ... count 8 landed 0 talk 1 collider down pressed 1 anim 4    (press animation ran to its end and held)

A ground pound was not scripted (no selftest pounds); slot 21's effect is the
same edge with the countdown zeroed, and its body is 3 matched words.

## 5. Gaps

* The talk latch stays 1 for the rest of the idle run: after the press the
  Behavior walks Player::StartTalk / GetTalkState / ShowMessage(0x183) /
  HasFinishedTalking, and nothing in the idle selftest presses a button to
  dismiss a message, so it never reaches EndTalk. Not a fault (no decline, no
  UNHOSTED), not verified either; the message box's visibility was not
  checked here.
* Level 31 (habatake) still does not boot (LINKSCOPE lane 12), so the
  second placement stays unproven; the seat is not the blocker there.
* Render is the host copy of section 3, so the class is +12 not +13; the
  matched TU joins the QuestionBlock/Butterfly/Fish/MontyMole queue that
  waits on a ROM-order ModelAnim table.
* `[spawn-declined] actor 0xc0 not registered` appears once on level 0 in
  both proof runs, before the object table is walked; id 192 is not this
  class and the line is the registry's ordinary skip, not a fault.
* The before-linkage number is the cons tip's build (`4b54888e2`), two
  commits past this branch's base; neither of those commits enrolls a TU.
