// PORT_HOST_ABI. HOST COPY of src/func_ov006_02107d80.cpp -- the roulette
// wheel's DRAW, and the only code in dScMgRoulette_c that submits either of its
// two 3D models. Run mg11, lane RLT (actor id 0x17f, scene 383).
//
// ---- THE SAME LOCAL-SHADOW-CLASS DEFECT AS ITS SIBLING, AT A DIFFERENT SLOT
//
// port/unmatched/MgRoulette_WheelDispatch.cpp repairs ROM slot 3
// (Model::UpdateVerts) in func_ov006_02107db8. This is ROM slot 5, in the body
// vtable slot 9 (Render) calls on the same object, and it is a SEPARATE FILE
// for the reason port/mg_fanout_costs.txt section 15 gives about census lines:
// the two failures look nothing alike and only one of them is on the draw path.
// A run that ticks the wheel and never draws it reports exactly the same
// dispatch counts as one that draws it.
//
// WHAT THE ROM DOES, out of extracted/overlays/overlay_0006.bin at base
// 0x020bfec0 (size 0x38 = 14 words: 14 instructions, no literal pool):
//
//   02107d80  push {r4, lr}
//   02107d84  mov  r4,r0
//   02107d88  add  r0,r4,#0x10        the first Model
//   02107d8c  ldr  r2,[r0]            its vptr
//   02107d90  mov  r1,#0              scale = NULL
//   02107d94  ldr  r2,[r2,#0x14]      ROM slot 5
//   02107d98  blx  r2
//   02107d9c  add  r0,r4,#0x60        the second Model
//   02107da0  ldr  r2,[r0]
//   02107da4  mov  r1,#0
//   02107da8  ldr  r2,[r2,#0x14]
//   02107dac  blx  r2
//   02107db0  pop  {r4, lr} / bx lr
//
// Byte +0x14 is the SIXTH word of an Itanium vtable, and include/Model.h reads
// _ZTV5Model out of the ROM as (0 D1, 1 D0, 2 DoSetFile, 3 UpdateVerts,
// 4 Virtual10, 5 Render), so ROM slot 5 is Model::Render(const Vector3 *scale)
// and the `mov r1,#0` is that scale being NULL.
//
// src models the object with a local `struct Sub` of six virtuals and calls the
// sixth, `m(0)`, which is the ROM's own numbering. On the host that byte is the
// SIXTH MSVC word, and MSVC folds D1 and D0 into one slot, so the host's sixth
// word is one PAST Render -- off the end of Model's vtable entirely, into
// whatever the linker put next. port/mg_fanout_costs.txt section 13 states the
// test this file applies: a shadow over an object whose vptr is the MOUNTED ROM
// TABLE is correct, because the port's fills key on ROM words and leave the
// table in ROM word order; a shadow over a HOST C++ vtable -- Model, ModelAnim,
// BlendModelAnim, built by hal/cxxname_bridge.cpp in MSVC order -- is wrong.
// These two objects are host Models: MgMushroomRoulette_Spawn constructs them
// with _ZN5ModelC1Ev at +0x530c+0x10 and +0x530c+0x60, and vtable slot 0's
// func_ov006_021085c0 hands them NitroFS files 0x21d and 0x21e through
// ModelBase::SetFile.
//
// WHICH CLASS THE OBJECTS ARE IS ESTABLISHED FROM THE CONSTRUCTOR, not guessed
// from the call -- the discipline
// port/unmatched/MgMemory2_ShadowSlot_020c06dc.cpp records. The factory calls
// _ZN5ModelC1Ev (0x02016d58) on both, so both are exactly Model.
//
// THE DISPATCH STAYS VIRTUAL rather than qualified, for
// MgFlower_ModelRender_020c3bf4.cpp's reason: the ROM's is
// (ldr vptr / ldr [vptr,#0x14] / blx). Here the objects are Models by
// construction so both resolve the same way; keeping the call virtual matches
// the ROM's shape and costs nothing.
//
// WHAT CHANGED FROM src, so it can be checked line by line: the local
// `struct Sub` shadow and its two `m(0)` calls are replaced by two virtual
// Model::Render(0) calls. There is nothing else in the function.
//
// IT DEFINES func_ov006_02107d80, so src/func_ov006_02107d80.cpp is OUT of
// port/slice_rlt.txt -- listing both would be an LNK2005. The cost is the
// counterfactual slice line, not a regression: this TU was in no slice on the
// base, and the measured headline goes 7121 -> 7162 with nothing falling.
// port/unmatched/MgRoulette_WheelDispatch.cpp carries the arithmetic.
//
// NOTHING IS OWED ON THE DECOMP SIDE: the shadow class produces the ROM's own
// instruction sequence under mwccarm, and the defect is entirely the host's
// vtable shape.

#include "common.h"
#include "Model.h"

extern "C" void func_ov006_02107d80(void *self);

// PORT_HOST_ABI: mwcc virtual-shadow dispatch (ROM vtable numbering vs MSVC's).
extern "C" void func_ov006_02107d80(void *self)
{
    char *c = (char *)self;
    ((Model *)(c + 0x10))->Render(0);
    ((Model *)(c + 0x60))->Render(0);
}
