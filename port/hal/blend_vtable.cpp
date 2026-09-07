// BlendModelAnim's vtable storage and runtime fill.
//
// BlendModelAnim is ModelAnim with a cross-fade: blendWeight ramps 0 -> 0x1000
// by blendStep per Advance, and while it is below 1.0 UpdateVerts and Virtual10
// route the pose through func_0204531c instead of UpdateVertsUsingBones. The
// twelve matched TUs are in slice_gate24.txt; this file carries only the two
// tables the constructor installs and the fill that makes dispatch through them
// land somewhere real.
//
// SLOT NUMBERING -- the trap this whole file exists for. BlendModelAnim
// declares the destructor first and then DoSetFile, UpdateVerts, Virtual10,
// Render, Virtual18. MSVC spends ONE vtable slot on the destructor where
// Itanium spends two (D1 and D0), so the two numberings are:
//
//     method       ROM/Itanium   MSVC
//     ~dtor            0, 1        0
//     DoSetFile          2         1
//     UpdateVerts        3         2
//     Virtual10          4         3
//     Render             5         4
//     Virtual18          6         5
//
// The array below is filled in MSVC order, because the TUs that dispatch
// through it are compiled by MSVC from BlendModelAnim.h. This is the same
// choice hal/cxxname_bridge.cpp makes for _ZTV9ModelAnim and _ZTV10ModelAnim2,
// and it carries the same restriction: NO DUAL-FILL. Render's ROM slot (5) is
// Virtual18's MSVC slot, and Virtual18 takes two arguments where Render takes
// one, so a TU that dispatches Render through a LOCAL SHADOW CLASS (which
// counts in ROM numbering) cannot be served by this array. Butterfly, Fish and
// QuestionBlock already walked into exactly that for ModelAnim and had to be
// hand-written into port/unmatched/ModelAnim_Renders.cpp; if a shadow-class TU
// ever dispatches BlendModelAnim::Render it will land on blend_virtual18 and
// read its scale off the stack, so trap-by-Virtual18 will name it.
//
// The destructor slot is the same no-op the ModelAnim family uses. MSVC's dtor
// slot is a scalar-deleting destructor taking a hidden flag argument, which the
// matched D0/D1 (real functions, and linked -- they are in the slice) do not
// have; nothing in the port deletes a BlendModelAnim through a base pointer, so
// the slot stays a no-op rather than inventing an ABI. Direct calls to
// _ZN14BlendModelAnimD1Ev still run the real body, including the operator
// delete of unk_6c.
//
// The destructor chain the slice pulls in (BlendModelAnim D1 -> ModelAnim D2
// -> Model/Animation D2 -> ModelBase D2) bottoms out in src/_ZdlPv.cpp, which
// dispatches Heap::_Deallocate as an MSVC METHOD. That face normally lives in
// hal/cxx_aliases.cpp:298-300, which the gate-7 stack does not link (it faces
// dozens of symbols this target has no use for). The same three lines are
// repeated below so gate 24 stands on its own. A TARGET THAT LINKS BOTH THIS
// FILE AND cxx_aliases.cpp must not take it twice, so it is behind
// PORT_CXX_ALIASES_LINKED. Gate 32 is the first build that links both:
// KING_BOB_OMB owns a BlendModelAnim and he spawns on Bob-omb Battlefield,
// which is the moment the note above said would come.
#include "BlendModelAnim.h"

#ifndef PORT_CXX_ALIASES_LINKED
extern "C" void _ZN4Heap10DeallocateEPv(void *self, void *ptr);
struct Heap { void _Deallocate(void *ptr); };
void Heap::_Deallocate(void *ptr) { _ZN4Heap10DeallocateEPv(this, ptr); }
#endif

static void __fastcall blend_dtor(void *, void *) {}
static void __fastcall blend_dosetfile(void *self, void *, char *file, int a,
                                       int b)
{ ((BlendModelAnim *)self)->BlendModelAnim::DoSetFile(file, a, b); }
static void __fastcall blend_updateverts(void *self, void *)
{ ((BlendModelAnim *)self)->BlendModelAnim::UpdateVerts(); }
static void __fastcall blend_virtual10(void *self, void *, void *m)
{ ((BlendModelAnim *)self)->BlendModelAnim::Virtual10(*(Matrix4x3 *)m); }
static void __fastcall blend_render(void *self, void *, const void *s)
{ ((BlendModelAnim *)self)->BlendModelAnim::Render((const Vector3 *)s); }
static void __fastcall blend_virtual18(void *self, void *, unsigned m,
                                       const void *s)
{ ((BlendModelAnim *)self)->BlendModelAnim::Virtual18(m, (const Vector3 *)s); }

extern "C" {
/* installed by _ZN14BlendModelAnimC1Ev: the primary at +0 and the
   multiple-inheritance thunk table the Animation base gets at +0x50 */
void *_ZTV14BlendModelAnim[10];
void *VTable_Animation_BlendModelAnimThunk[8];

void hal_fill_blendmodelanim_vtable(void)
{
    /* ROM NUMBERING, as of the destructor respelling in include/ModelBase.h:
       slots 0 and 1 are the ROM's D1/D0 pair (seeded no-op here,
       hal/model_dtor_seat.cpp seats the matched bodies), and everything below
       sits where the ROM's own table holds it. Was MSVC-numbered, one slot
       early from DoSetFile on, because MSVC folded the destructor pair. */
    _ZTV14BlendModelAnim[0] = (void *)blend_dtor;
    _ZTV14BlendModelAnim[1] = (void *)blend_dtor;
    _ZTV14BlendModelAnim[2] = (void *)blend_dosetfile;
    _ZTV14BlendModelAnim[3] = (void *)blend_updateverts;
    _ZTV14BlendModelAnim[4] = (void *)blend_virtual10;
    _ZTV14BlendModelAnim[5] = (void *)blend_render;
    _ZTV14BlendModelAnim[6] = (void *)blend_virtual18;
    /* the Animation-base secondary table only ever destructs */
    VTable_Animation_BlendModelAnimThunk[0] = (void *)blend_dtor;
    VTable_Animation_BlendModelAnimThunk[1] = (void *)blend_dtor;
}
}
