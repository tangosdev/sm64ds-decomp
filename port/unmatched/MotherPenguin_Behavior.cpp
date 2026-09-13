/* HOST COPY of src/_ZN7SkiLift8BehaviorEv.cpp -- the MSVC dtor-slot-shift
 * seam (sm64ds-port-msvc-dtor-slot-shift), applied to a raw shadow-vtable
 * index call instead of a real vtable fill.
 *
 * CLASS IDENTITY NOTE: despite the mangled name _ZN7SkiLift8BehaviorEv,
 * this method belongs to MOTHER_PENGUIN (257) -- see
 * MotherPenguin_InitResources.cpp's header and port/slice_gate191.txt.
 *
 * THE BUG. The matched src dispatches ModelAnim::UpdateVerts through a
 * RAW SHADOW VTABLE INDEX: `struct Sub { virtual m0,m1,m2,m3; };
 * ((Sub*)&mModelAnim)->m3()`. On the ROM, ModelAnim's own vtable is
 * Itanium-shaped (TWO destructor slots: D1@0, D0@1), so a raw index-3 load
 * lands on DoSetFile@2... UpdateVerts@3 (confirmed against ROM disassembly
 * via match.py: the compiled call is `add r0,r4,#0xd4; bl Advance` for the
 * FIRST Advance, meaning the m3() call is a SEPARATE, later dispatch whose
 * index-3 slot is UpdateVerts in the ROM's own two-destructor-slot
 * numbering -- see include/ModelAnim.h's documented vtable, slot 3).
 *
 * The port's hosted _ZTV9ModelAnim (hal/cxxname_bridge.cpp) is filled in
 * MSVC's OWN vtable order instead (ONE folded destructor slot: dtor@0,
 * DoSetFile@1, UpdateVerts@2, Virtual10@3, ...) -- correct for every OTHER
 * caller that dispatches through a properly-typed `ModelAnim*` (MSVC emits
 * the same folded numbering at those call sites too, so the array and its
 * callers agree). This ONE caller's raw index-3 read, though, lands on
 * Virtual10 (MSVC slot 3) instead of UpdateVerts (MSVC slot 2, ROM slot 3)
 * -- a garbage/uninitialized function pointer one gate 191 does not fill
 * (Virtual10's own slot IS filled, with ma2_virtual10, but m3() is called
 * with ZERO extra arguments while Virtual10 takes a Matrix4x3& -- the ABI
 * itself does not line up either way), and the call faults.
 *
 * THE FIX. Dispatch UpdateVerts through the real, properly-typed method
 * call instead of the raw shadow index -- the ma2_updateverts treatment
 * (hal/cxxname_bridge.cpp) applied to a host copy instead of a fresh
 * vtable fill. Matched source line for line otherwise.
 * src/_ZN7SkiLift8BehaviorEv.cpp stays byte-locked and untouched, dropped
 * from slice_gate191.txt in favour of this file.
 *
 * THE SECOND BUG, and it is the bigger one (gate mpg). The matched src's
 * FIRST call is spelled `_ZN13RacingPenguin16OnPendingDestroyEv()`. That
 * symbol is ov019's own 4-byte `bx lr`
 * (config/arm9/overlays/ov019/delinks.txt:106-108, 0x0211235c..0x02112360).
 * The ROM's own instructions here are
 *     02112480  push {r4,lr}
 *     02112484  mov  r4, r0
 *     02112488  bl   0x0211235c
 * and ov018's own relocation table says where that branch goes:
 *     from:0x02112488 kind:arm_call to:0x0211235c module:overlay(18)
 * -- OVERLAY 18, whose 0x0211235c is func_ov018_0211235c, the 0x3c-byte
 * TICK DISPATCHER of MotherPenguin's own state table (it reads the
 * pointer-to-member record at self+0x370 PLUS 8 and calls through it; see
 * port/unmatched/MotherPenguin_AfterClsnSeat.cpp for the whole table).
 * ov018 and ov019 share load base 0x021111a0 and are never co-resident, so
 * both symbols legitimately sit at that address in their own modules and the
 * decomp picked the wrong one -- the same symscope-crossing family as
 * PMFB7's HootTheOwl finding. The consequence in the port was total: the
 * mother penguin's per-frame state tick never ran, because the port linked
 * ov019's empty body and called that instead.
 *
 * Fixed here by calling func_ov018_0211235c, which is what the ROM's own
 * relocation names. The matched src stays byte-locked and untouched (on ARM
 * it assembles to the same `bl 0x0211235c` either way, which is exactly why
 * no byte gate ever caught this); the crossing is reported decomp-side.
 */
#include "SkiLift.h"

extern "C" {
extern void _ZN9Animation7AdvanceEv(void*);
extern void _ZN12CylinderClsn5ClearEv(void*);
extern void _ZN12CylinderClsn6UpdateEv(void*);
extern int func_ov018_02111d28(void*);
extern void func_ov018_0211235c(void*);   /* ov018's own TICK DISPATCHER */
}

int SkiLift::Behavior()
{
  func_ov018_0211235c((char*)this);
  _ZN9Animation7AdvanceEv((char*)(Animation *)&mModelAnim);
  _ZN9Animation7AdvanceEv((char*)&mTextureSequence);
  _ZN12CylinderClsn5ClearEv((char*)&mMovingCylinderClsn);
  _ZN12CylinderClsn6UpdateEv((char*)&mMovingCylinderClsn);
  mModelAnim.ModelAnim::UpdateVerts();
  func_ov018_02111d28(((char*)this));
  return 1;
}
