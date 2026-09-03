# Lane SHADOW-A: the arm9 stand-ins (LINKSCOPE lane 5)

Branch `port/shadow-replace-a`, worktree `C:/tmp/shadow-a`, base `3d048eb4e`
(the cons tip, `port-mount-noseat-cluster`, after shadow-B landed there). The
thirteen retirements were first cut on `f8e154e33` and then rebased onto
`3d048eb4e`; every conflict was the same shape (cons commit fb1af5d41 had
added a PORT_HOST_ABI tag to the host copies this lane deletes, and shadow-B
had added two hostgen tables next to this lane's additions), and each was
resolved by taking the deletion or keeping both additions. Scope: the 13
non-Player arm9 stand-in retirements LINKSCOPE.md section 3a names for lane
5, expected +14. Nothing under `src/` or `include/` was edited; no overlay
stand-in was touched (that is shadow-B's lane); `port/tools/hostgen.py`
received table entries and one new table, no restructuring, and the camera
rows reuse the CALL_STATE_FN table shadow-B landed rather than a parallel
entry (section 5).

Branch tip: this file is the last commit, on top of the CALL_STATE_FN move (33a682dab). Not merged, not pushed.

## 1. Numbers

Both readings from `python port/tools/linkage.py --queue`. The before column
is the cons tip's own build (`build/port/walk_window.map` in
`sm64ds-wt/cons`, built three minutes after 3d048eb4e landed, read with the
tool's positional root argument and saved here as
`build/linkage_before_3d048.txt`); the after column is this worktree's
rebased build, `build/linkage_rebased.txt`.

| | before (3d048eb4e) | after | net |
|---|---|---|---|
| matched TUs in src/ | 11327 | 11327 | |
| linked into walk_window | 9284 (82.0%) | 9297 (82.1%) | +13 |
| SHADOWS, repo-wide | 18 | 16 | -2 |
| documented host-ABI exceptions | 533 | 522 | -11 |
| FACES | 262 | 263 | +1 |
| MSVC-name shadows | 12 | 12 | |

The split between the SHADOWS and the exceptions columns is fb1af5d41's
doing: that cons commit tagged every host copy this lane deletes with
PORT_HOST_ABI except two, so eleven of the thirteen came out of the
documented-exceptions bucket and only Scene::AfterCleanupResources and
Animation::Advance were still counted as SHADOWS on the tip. The +1 FACE is
Animation::Advance: hal/player_bridges.cpp defines the C-name bridge
`_ZN9Animation7AdvanceEv` into the method (the same shape as its
Animation::Finished neighbour, which was already a FACE), and now that the
matched TU's `?Advance@Animation@@QAEXXZ` is in the map the tool sees both.
On the pre-rebase base the same thirteen read 9242 to 9255, 149 SHADOWS to
138, 262 FACES flat (`build/linkage_before.txt`, `build/linkage_g234.txt`).
LINKSCOPE projected +14 for the lane; the one row not taken is
HUD::InitResources, refused below with a wall the scope did not see.

Battery: `python port/tools/battery.py --skip-build` on the guarded build
with TEMP/TMP under `build/tmp`, three runs. `build/battery1.log` after the
first four retirements and `build/battery2.log` after all thirteen, both on
the pre-rebase base, both ALL GREEN (14/14 smoke binaries, every level and
scene selftest ok with the two standing decomp-owned skips on levels 27 and
45, default title boot ok, ptr_audit 0, shipping config built and its
selftest rc=0). `build/battery_rebased.log` on the rebased tip:
ALL GREEN (14/14 smoke binaries ok, 50 level and 34 scene selftests ok with the same two decomp-owned skips on levels 27 and 45, default title boot ok at 300 frames, linkage 9297 (82.1%), ptr_audit 0 unhosted code pointers, shipping config built in 59s and its selftest rc=0).

Every retired TU was re-verified with `tools/match.py --version 2004/b56`
(strict relocs on by default) before its host body was deleted, and again
on the rebased tree (`build/reverify_rebased.txt`, addresses and sizes from
config/arm9/symbols.txt): 13 of 13 MATCHING both times, and `git diff
f8e154e33 3d048eb4e -- src/<the thirteen>` is empty, so the second reading
is the same bytes. HUD::InitResources (ov002, 0x020fda04) is matched too;
it was refused for a different reason.

## 2. Vocabulary

LINKMG.md's: REPLACED means the matched src TU is in the binary and the host
body is gone; retire plain is the src line on a slice and nothing else;
hostgen patch is a `port/tools/hostgen.py` table entry, exact-string and
hard-errored if the source ever moves; seam is a small host entry that stays
and forwards into the matched body; refuse is a documented wall.

## 3. The thirteen, one commit each

| # | symbol | mechanism | what actually happened |
|---|---|---|---|
| 1 | `_ZN10FaderColorD1Ev` | retire plain | hal/intro_script_records.cpp's no-op sat under a stale "no matched src TU" banner. src/engine/fader/_ZN10FaderColorD1Ev.c is matched; it joins slice_intro.txt. The registrar only stores the pointer and the port never runs global destructors, so nothing observable changes. |
| 2 | `_ZN5Scene21AfterCleanupResourcesEj` | hostgen --extern-data (existing) | The TU's `unsigned char data_02092660;` is a strong definition to MSVC's C++ front end and the byte is .dsstate owned by hal/scene_boot.cpp. --extern-data was already the port's answer (Player::InitResources); the symbol joins HOSTABI_RETIRE_SYMS. |
| 3 | `NumStars` | retire plain | The host copy widened the course id because src/NumStars.c declared IsStarCollected(s8,..) while the callee defined int. src/IsStarCollected.c takes s8 today, so the callee extends the byte itself under cdecl and the matched TU links as written. |
| 4 | `CollectStar` | hostgen ARG_WIDTH (new table, one entry) | The WRITE half: caller declares s8, body takes int, and `data_0209cab4[a] \|= 1 << b` reads three stale bytes under cdecl (measured fault). The entry retypes the one parameter in the definition. GATE35_SYMS. |
| 5 | `_ZN9Animation7AdvanceEv` | hostgen DS_DIV (existing) | `% len` with len 0 on the VS cap; ITCM divider answers 0, x86 faults. One ds_imod entry; a small ANIM_ADVANCE_GEN custom command replaces the host copy in the gate-7 and gate-10 loops. |
| 6 | `func_02016ff4` | hostgen VIRTUAL_CALL (existing) + 1-line seam | Shadow-class slot 2 (ROM numbering, two dtor slots) is DoSetFile; MSVC folds the dtor to one slot so it landed on UpdateVerts. Resolved at the caller to host slot 1 through the __fastcall thunks. The shrink-to-fit tail stays behind port_model_shrink_enabled (off by default, SM64DS_MODEL_SHRINK=1), which the host copy also carried; documented in the table as a seam, not as part of the fold. GATE35_SYMS. |
| 7 | `func_0204335c` | hostgen VIRTUAL_CALL (existing) | Init Process: three static {slot,1} records (0x02099ebc/ec4/e94 = slots 0/1/2) into ActorBase::Process, whose host globals are zeroed. The entry inlines Process's own control flow over the slots. The gate-10 filter keeps the plain src line out. |
| 8 | `func_02043288` | hostgen VIRTUAL_CALL (existing) | Behaviour Process, slots 6/7/8, same entry shape. The render and cleanup wrappers keep their host copy (slot-5 Virtual18 ruling, not this lane's). |
| 9 | `func_02008550` | hostgen MG_PMF_CALL (existing) | 39 seeding assignments copy word 0 of hal/ptr_tables.cpp's {host handler, 0} records at the host stride (MSVC's PMF for the complete `Obj` is one code word, static_assert'ed while preparing); only the call line changes, from __thiscall PMF to cdecl with the receiver first. GATE13_SYMS. |
| 10 | `func_0200cae4` | hostgen CALL_STATE_FN (shadow-B's table) | onEnter dispatch, word 0 of the State; the call becomes hal_call_camera_state_fn over the same word. First cut as an MG_PMF_CALL entry; moved into CALL_STATE_FN after the rebase, since that table is exactly this shape (a mwcc PMF whose code word is a DS address, swapped for a seam call on the word), and the table's file-scope declaration block now names the camera seam next to hal_call_state_fn. The emitted TU differs only in its extern lines. |
| 11 | `func_0200ca50` | hostgen CALL_STATE_FN (shadow-B's table) | main dispatch, word 2 (`obj + 8`), same move as row 10. The host copy's extra null check on the State pointer is not carried; the ROM does not have it. |
| 12 | `_ZN3G3i7LookAt_EPK7Vector3S2_S2_bP9Matrix4x3` | hostgen MMIO_PTR (existing) | The scope said the rule needed extending to register-bound pointers; it already covers them (the func_0204488c case) and rewrites all 13 sites. The TU is a real `namespace G3i` function, so two /alternatename lines in hal/cxx_aliases.cpp point Camera::Render's static-member spelling and the six Itanium-name callers' spelling (Camera_UpdateMatrices and five overlay TUs) at MSVC's namespace mangling. |
| 13 | `_ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3` | hostgen MMIO_PTR (existing) + MMIO_EXTERN seam | 21 rewrites from the existing passes. The DIVCNT 64/32 write, the deliberate divergence the scope says must survive, is an MMIO_EXTERN entry anchored on the first DIV_NUMER store, exactly where the host copy had it, hard-errored if the anchor moves. |

The three deliberate divergences the scope names are untouched:
SaveData::ReadDataFromCart (hal/scene_boot.cpp, still returns 2),
G3i_PerspectiveW's DIVCNT line (now carried by hostgen, see row 13), and
func_02038324's guard (port/unmatched/func_02038324_hostcopy.cpp, still there).

The two env-gated instruments that lived in the G3i host copies
(`SM64DS_LOOKAT_LOG`, `SM64DS_PERSP_LOG`) went with them. Nothing else in the
tree read them.

## 4. Refused: HUD::InitResources (`_ZN3HUD13InitResourcesEv`)

LINKSCOPE row: "seven file-scope globals defined instead of declared; matched
source otherwise verbatim". The first half is true and hostgen now handles it
(EXTERN_DATA learned `void*` for `void* data_0209f394[];`; the emitted file
declares all ten). The second half is not, and the host copy's own banner says
why:

1. The port's `LoadFile` (hal/level_boot.cpp) is SharedFilePtr-backed and hands
   back DECODED bytes. The matched TU calls `LoadFile` then `DecompressLZ16`
   on the result, the ROM's contract (func_0201818c returns the file still
   compressed). Linked as written it would LZ-decode an already-decoded buffer
   into VRAM. The host copy substitutes the raw loader `func_0201817c`, which
   is the port's own ROM-faithful spelling of that call shape, at every one of
   its ~25 call sites.
2. Ten of the HUD's handles are not in this extraction's catalog
   (build/assets/handles.tsv: 0xa003/a007/a00a/a00e, 0xac03, 0xb00e,
   0x8000..0x8003, 0x980f; the ARCHIVE/cee.narc members). The raw loader
   returns null for them and the matched body has no null path, so a plain
   retirement faults on the first level boot inside DecompressLZ16. The host
   copy skips absent loads and says so once.

Neither is an ABI wall, which is why the row was scoped as hostgen, but
carrying (1) as ~25 exact-string patches is a host copy spelled inside the
tool, and (2) is an extraction gap, not a port mechanism. Retire this one when
the extraction is complete (then (1) is a single `#define LoadFile
func_0201817c`-shaped entry, or better a LoadFile seam that returns raw bytes
for this call shape) plus a `?GetHealth@Player@@QAEEXZ` alias for the TU's
`u8 GetHealth()` shadow declaration. The HUD host copy still stands, so the
map is unchanged for it; it is still a SHADOW in the queue.

## 5. hostgen.py, what changed and how it merges

Additions only against 3d048eb4e, no line of shadow-B's tables changed
except CALL_STATE_FN's declaration string (one line became two):

- `EXTERN_DATA`: `void\**` in the type alternation. Checked with old vs new
  hostgen over every src file with --extern-data: the only outputs that change
  are HUD::InitResources' one line and `func_ov006_020fa7b8` (host-copied,
  storage owned by the ov006 mount; the change is a help to the day that row
  retires, not a hazard).
- `DS_DIV`: one entry (Animation::Advance).
- `MMIO_EXTERN`: one entry (PerspectiveW_'s DIVCNT seam).
- `VIRTUAL_CALL`: three entries (func_02016ff4, func_0204335c, func_02043288).
- `MG_PMF_CALL`: one entry (func_02008550; its records are host function
  pointers, not DS addresses, so it does not belong in CALL_STATE_FN).
- `CALL_STATE_FN` (shadow-B's table, landed on the tip in b31d8a7cf): two
  entries (func_0200cae4, func_0200ca50) and a second line in
  CALL_STATE_FN_DECL for hal_call_camera_state_fn. Shadow-B's
  EXTERN_C_DATA table was looked at for row 2 and does not apply: it moves
  declarations inside an extern "C" block, while Scene::AfterCleanupResources'
  problem is a definition that has to become a declaration, which is what
  the pre-existing --extern-data flag does.
- `ARG_WIDTH`: a NEW table with one entry (CollectStar), its `arg_width_patch`
  helper, and one call line in `emit()` after shadow-B's `call_state_fn_patch`
  line (the rebase put the two lane's `emit()` lines next to each other; they
  are independent).

After the rebase every symbol named in any hostgen table (42) was emitted
once with the rebased tool and no patch hard-errored; the two camera TUs
were emitted before and after the CALL_STATE_FN move and diffed, and only
the extern lines at the top changed.

## 6. CMake and slices

- `HOSTABI_RETIRE_SYMS` += Scene::AfterCleanupResources.
- `GATE35_SYMS` += CollectStar, func_02016ff4, func_0204335c, func_02043288.
- `GATE13_SYMS` += func_0200cae4, func_0200ca50, func_02008550, the G3i pair.
- `ANIM_ADVANCE_GEN`: one custom command, appended where the two loops used
  to append the host copy.
- `LEVELBOOT_HOST_SOURCES`, `CAMERA_HOST_SOURCES`, `SLICE_PTR_SOURCES` and the
  three walk_window-family target lines lose the deleted files.
- slice rows: slice_intro.txt gains the FaderColorD1 line; slice_gate10.txt
  gains src/NumStars.c; slice_scene1/gate13/gate15/gate16/ptrsweep comments
  say where each TU comes from now. Existing skip filters for src lines that
  hostgen now generates (func_0204335c, func_0200cae4, func_02008550,
  CollectStar, Animation::Advance) stay in place, with their comments updated.
- Four comments in hal/camera_states.cpp, hal/ptr_tables.cpp,
  hal/level_boot.cpp and unmatched/func_0204322c_hostcopy.cpp that named the
  deleted files by path were refreshed. Lane records elsewhere (arm9_frontier,
  ov007_seat, scene_boot_map) are left as written; they describe their day.

## 7. Not proven, and gaps

- The battery is liveness (selftests, boots, ptr_audit, the shipping build),
  not a raster comparison. Row 12's alias retarget and row 13's seam were
  exercised by every level selftest (Camera::Render runs both every frame)
  and the default title boot, and the frame BMPs were written, but nobody
  compared pixels.
- Rows 7 and 8 change which object file the per-frame actor spine is compiled
  from; the control flow is Process's verbatim and every level selftest ran
  through it, but there is no unit test of the wrappers themselves.
- Row 11 drops a host-only null check. The ROM has none; if a State pointer
  is ever null on the host the fault will now be at the src line, which is
  the honest place for it.
- HUD::InitResources is the one row of the 14 not taken (section 4).
- `func_02038324_hostcopy.cpp`, the KEEP row, was not touched and is still
  tagged as a fix, not an ABI exception; it stays in the queue's documented
  bucket as before.

## 8. Commands

    git -C C:/tmp/shadow-a reset --hard f8e154e33        # reuse, per the brief
    # ... the thirteen commits, then:
    git rebase 3d048eb4e                                 # 11 conflicts, all tag-vs-delete or both-add
    python port/tools/linkage.py C:/Users/bmanu/Documents/sm64ds-wt/cons --queue > build/linkage_before_3d048.txt
    python build/reverify.py                             # 13 of 13 MATCHING on the rebased src
    export TMP=C:/tmp/shadow-a/build/tmp TEMP=C:/tmp/shadow-a/build/tmp
    cmd //c "port\build-port.cmd"                         # baseline, rc=0
    python port/tools/linkage.py --queue > build/linkage_before.txt
    python tools/match.py --c src/<tu> --func <sym> --addr <a> --size <n> \
        --version 2004/b56 --brief                        # 13 of 13 MATCHING
    # edits per section 3, one commit each
    cmd //c "port\build-port.cmd"                         # rc=0, all guards
    python port/tools/battery.py --skip-build             # build/battery1.log, battery2.log
    python port/tools/linkage.py --queue > build/linkage_g234.txt
    python port/tools/linkage.py --queue > build/linkage_rebased.txt   # after the rebase build
