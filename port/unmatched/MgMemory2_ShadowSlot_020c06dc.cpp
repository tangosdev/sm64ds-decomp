/* HOST COPY of src/func_ov006_020c06dc.cpp -- the matrix set-up the model
 * sub-object runs immediately before it draws. Run mg6, lane MEM.
 *
 * THE SAME LOCAL-SHADOW-CLASS DEFECT as
 * port/unmatched/MgMemory2_ModelRender_020c1804.cpp, one call deeper and at a
 * DIFFERENT slot -- and this one does not merely read a wrong argument, it
 * UNBALANCES THE STACK. That is worth separating, because the two failures look
 * nothing alike in a fault dump and only one of them points at the function
 * that caused it.
 *
 * WHAT THE ROM DOES, out of extracted/overlays/overlay_0006.bin at base
 * 0x020bfec0:
 *
 *   020c06dc  stmdb sp!,{r4,r5,lr}
 *   020c06e0  sub   sp,sp,#4
 *   020c06e4  mov   r4,r0
 *   020c06e8  add   r0,r4,#0x18
 *   020c06ec  ldr   r1,[r0]          ; vptr of the BlendModelAnim at +0x18
 *   020c06f0  ldr   r1,[r1,#0xc]     ; ROM slot 3 -> UpdateVerts
 *   020c06f4  blx   r1
 *
 * src models that object with a local `struct Base` of four virtuals and calls
 * `->m()`, the fourth, which is the ROM's own numbering: BYTE +0xc of an
 * Itanium vtable is the fourth word, and on the model classes that word is
 * UpdateVerts (0 D1, 1 D0, 2 DoSetFile, 3 UpdateVerts, 4 Virtual10, 5 Render).
 *
 * On the host that byte is the FOURTH MSVC word, and MSVC folds D1 and D0 into
 * one slot, so the host's fourth word is Virtual10(Matrix4x3 &mat) -- one slot
 * along. TWO things then go wrong at once:
 *
 *   1. the wrong body runs, with a `Matrix4x3 &` that was never passed;
 *   2. Virtual10 is __thiscall with ONE stack parameter, so it RETURNS WITH
 *      `ret 4` and eats four bytes the caller never pushed.
 *
 * THE SECOND IS WHAT THE FAULT SHOWS, and it shows it two frames away from the
 * cause. Measured on scene 363, this tree, before this file existed:
 *
 *   FAULT code c0000005 at +0x303ffc04 accessing 307ffc04
 *     regs eax=00c89198 ecx=74c1e199 edx=00000000 ebx=307ffb28
 *          esi=001af38c edi=307ffbb4 ebp=0047374d
 *     stack[02] +0x000731c7  func_ov006_020c1804+0x1a7
 *
 * 0x307ffc04 is not code. It is the ARGUMENT func_ov006_020c1804 pushed for
 * func_ov006_020c07a0 (the sub-object base, this+0x4f38+0xdc): the four bytes
 * Virtual10 over-popped shifted every frame above it by one word, so
 * func_ov006_020c07a0's own `ret` took its caller's argument as a return
 * address and jumped into the DS heap. ebp reads 0047374d for the same reason.
 * NOTHING IN THAT DUMP NAMES func_ov006_020c06dc -- its frame is the one that
 * was consumed -- which is why this file's derivation is the ROM's
 * `ldr r1,[r1,#0xc]` and the MSVC vtable order, and not the stack trace.
 *
 * WHICH CLASS THE OBJECT IS, established from the CONSTRUCTOR rather than
 * guessed from the call: func_ov006_020c0a48 (host-copied in
 * unmatched/MgMemory2_SubObjCtor.cpp for an unrelated reason) runs
 * `_ZN14BlendModelAnimC1Ev(t + 0x18)`, so +0x18 is a BlendModelAnim, whose
 * UpdateVerts is MSVC slot 2 under include/BlendModelAnim.h.
 *
 * THE DISPATCH STAYS VIRTUAL rather than qualified, for
 * MgFlower_ModelRender_020c3bf4.cpp's reason: the ROM's is
 * (ldr vptr / ldr [vptr,#0xc] / blx) and a qualified call would silently
 * devirtualise a subclass override.
 *
 * WHY THE SIBLING SHADOW CLASS IN THIS SLICE IS *NOT* A DEFECT, because the
 * distinction is the whole rule and a sweep that flags every shadow class gets
 * it wrong. src/func_ov006_020c14bc.cpp models data_ov004_020beb68 -- the live
 * dScMgBase_c scene object -- with a 22-virtual shadow and calls `v21()`. That
 * object's vptr is the MOUNTED ROM TABLE, which hal/scene_mg_memory2.cpp fills
 * by ROM WORD and therefore leaves in ROM word order, so word 21 is the ROM's
 * slot 21 and the shadow's numbering is right. A shadow class over a HOST C++
 * vtable (Model, ModelAnim, BlendModelAnim, built by hal/cxxname_bridge.cpp in
 * MSVC order) is the one that is wrong. Same construct, opposite verdict, and
 * the test is which kind of table the object carries.
 *
 * WHAT CHANGED FROM src, so it can be checked line by line: the local
 * `struct Base` shadow and its `->m()` call are replaced by a virtual
 * BlendModelAnim::UpdateVerts(). Every other statement is the matched source's
 * -- the data_020a0e68 copy out of the model file at +0x2c +0x120, the X
 * rotation by the short at +0xf0, the copy back, the FromTranslation from
 * +0xc8/+0xcc/+0xd0, the Y rotation by the short at +0xea, and the store into
 * +0x34.
 *
 * IT DEFINES func_ov006_020c06dc, so src/func_ov006_020c06dc.cpp is OUT of
 * port/slice_mem.txt -- listing both would be an LNK2005. Nothing is owed on
 * the decomp side: the shadow class produces the ROM's own instruction
 * sequence, and the defect is entirely the host's vtable shape.
 */
#include "common.h"
#include "Model.h"
#include "ModelAnim.h"
#include "BlendModelAnim.h"

extern "C" {
void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *mF, short angX);
void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *mF, short angY);
extern Matrix4x3 data_020a0e68;
void func_ov006_020c06dc(char *thiz);
}

// PORT_HOST_ABI: mwcc virtual-shadow dispatch (ROM vtable numbering vs MSVC's).
void func_ov006_020c06dc(char *thiz)
{
    char *c = thiz;
    ((BlendModelAnim *)(c + 0x18))->UpdateVerts();      /* DELTA: was ->m() */
    {
        char *m = *(char **)(c + 0x2c);
        data_020a0e68 = *(Matrix4x3 *)(m + 0x120);
    }
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(short *)(c + 0xf0));
    {
        char *m = *(char **)(c + 0x2c);
        *(Matrix4x3 *)(m + 0x120) = data_020a0e68;
    }
    Matrix4x3_FromTranslation(&data_020a0e68, *(int *)(c + 0xc8),
                              *(int *)(c + 0xcc), *(int *)(c + 0xd0));
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short *)(c + 0xea));
    *(Matrix4x3 *)(c + 0x34) = data_020a0e68;
}
