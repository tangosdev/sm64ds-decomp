/* HOST COPY of src/func_ov006_020e777c.cpp -- the THREE-ModelAnim draw at the
 * sub-object's +0x84, reached from vtable slot 9 through
 * src/func_ov006_020e9d1c.cpp and unmatched/Mg3DEsp_ModelAnimRender_020e7b44.cpp.
 * Run mg9, lane PSY.
 *
 * THE SAME DEFECT AS ITS CALLER, ONE FRAME DOWN, AND THIS ONE WAS MEASURED AS A
 * FAULT.  unmatched/Mg3DEsp_ModelAnimRender_020e7b44.cpp repairs the shadow-class
 * Render dispatch on the ModelAnim at the sub-object's +0xc, and says in its own
 * header that it was found by reading rather than by crashing.  The first
 * RENDERED run of scene 389 then crashed here instead:
 *
 *     walk_window crash, code c0000005, access 00000000 at 00000000
 *       psy_render (slot 9)
 *         -> func_ov006_020e9d1c+0xa5
 *           -> func_ov006_020e777c+0x7a
 *             -> ModelAnim::Virtual18+0xe
 *               -> ModelAnim::Virtual10+0x25
 *                 -> Model::Virtual10+0xc,  eax = 0
 *
 * which is the Y3D chain unmatched/MgFlower_ModelRender_020c3bf4.cpp and
 * unmatched/MgMemory2_ModelRender_020c1804.cpp both end in, and it is exactly
 * what hal/cxxname_bridge.cpp's ModelAnim2 block predicts:
 *
 *     No dual-fill here: Render's ROM slot (5) is Virtual18's MSVC slot, so
 *     shadow-TU Render dispatch cannot be served by the same array --
 *     trap-by-Virtual18 will name it if such a TU ever appears.
 *
 * TWO SUCH TUs APPEARED IN ONE CLASS.  A ONE-AT-A-TIME FIX WOULD HAVE FOUND
 * THIS ONE ONLY BY CRASHING AGAIN, so after the fault this lane swept the
 * class's whole code block, 0x020e7660..0x020ea280, for the ROM's own tell --
 * `ldr rV,[rObj]` followed by `ldr rV,[rV,#N]` followed by a `blx rV` -- word by
 * word, so a literal pool cannot stop the scan the way a straight-line
 * disassembly does.  NINE dispatches, and the census is the whole answer:
 *
 *   0x020e77f0  slot  5  func_ov006_020e777c  ModelAnim x3   <- this file
 *   0x020e7bcc  slot  5  func_ov006_020e7b44  ModelAnim      <- the sibling
 *   0x020e9dac  slot  5  func_ov006_020e9d1c  Model          SAFE, dual fill
 *   0x020e9dd0  slot  5  func_ov006_020e9d1c  Model          SAFE, dual fill
 *   0x020e9a3c  slot 18  func_ov006_020e989c  SELF           SAFE, ROM table
 *   0x020ea118  slot 18  func_ov006_020e9e70  SELF           SAFE, ROM table
 *   (three more reads at slot bytes 0xb4/0xb8 in func_ov006_020e989c carry no
 *    blx at all -- they are struct fields behind a pointer, not dispatches)
 *
 * So this class has exactly TWO broken sites and both are now host copies, and
 * that is a measurement rather than a hope.
 *
 * ---- WHICH CLASS THE THREE OBJECTS ARE, FROM THE CONSTRUCTOR --------------
 *
 * src/func_ov006_020e7aac.c builds them:
 *
 *     func_020733a8(this,         3, 0x64, _ZN9ModelAnimC1Ev,       _ZN9ModelAnimD1Ev);
 *     func_020733a8(this + 0x12c, 3, 0x14, _ZN15MaterialChangerC1Ev, _ZN15MaterialChangerD1Ev);
 *
 * an array of THREE ModelAnims at stride 0x64 and three MaterialChangers at
 * stride 0x14, which is exactly the two cursors this loop walks.  Plain
 * ModelAnim -- not ModelAnim2 and not BlendModelAnim.
 *
 * ---- THE ROM, AND THE ARGUMENT ------------------------------------------
 *
 *     020e77a0  mov r6,#0 / mov r8,r6           r8 is the zero
 *     020e77a8  add r0,r7,r6,lsl #2
 *     020e77ac  ldr r0,[r0,#0x168] / cmp r0,#0 / beq   the per-slot gate
 *     020e77b8  the twelve-word Matrix4x3 copy into r5+0x1c, as three ldm/stm
 *     020e77d8  mov r0,r4 / add r1,r5,#8 / bl 0x02015788   MaterialChanger::Update
 *     020e77e4  mov r0,r5
 *     020e77e8  mov r1,r8                        the scale argument, NULL
 *     020e77ec  ldr r2,[r0] / ldr r2,[r2,#0x14] / blx r2
 *
 * so the call is Render(this, NULL) and src's `((Obj*)r5)->m(zero)` with
 * `zero = (void*)0` agrees.
 *
 * ---- THE DELTA FROM src, line by line ------------------------------------
 *
 *   1. the local `struct Obj` shadow is gone and its `m(zero)` call is
 *      `((ModelAnim *)r5)->Render(0)`
 *
 * Nothing else.  THE DISPATCH STAYS VIRTUAL rather than qualified, for
 * MgFlower_ModelRender_020c3bf4.cpp's reason: the ROM's is
 * (ldr vptr / ldr [vptr,#0x14] / blx) and a qualified call would silently
 * devirtualise a subclass override.  The Matrix4x3_FromTranslation with its
 * 0x8c000 / 0x80000 / 0x40000, the +0x168 gate, the matrix copy into +0x1c, the
 * MaterialChanger::Update over the ModelComponents at +8, and the three-step
 * loop with its 0x64 and 0x14 strides are src's, unchanged.
 *
 * IT DEFINES func_ov006_020e777c, so src/func_ov006_020e777c.cpp is OUT of
 * port/slice_psy.txt -- listing both would be an LNK2005.
 *
 * THE DECOMP-SIDE FIX IS NOT ROUTED, because there is nothing wrong with the
 * decomp: the shadow class produces the ROM's own instruction sequence. The
 * defect is the HOST's.
 */

#include "common.h"
#include "ModelAnim.h"

struct ModelComponents;
struct MaterialChanger { void Update(ModelComponents &); };

extern "C" {
extern Matrix4x3 data_020a0e68;
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void func_ov006_020e777c(char *c);
}

// PORT_HOST_ABI: mwcc virtual-shadow dispatch (ROM vtable numbering vs MSVC's)
void func_ov006_020e777c(char *c)
{
    int i;
    char *r5;
    char *r4;
    Matrix4x3_FromTranslation(&data_020a0e68, 0x8c000, 0x80000, 0x40000);
    r5 = c;
    r4 = c + 0x12c;
    i = 0;
    do {
        if (*(int *)(c + i * 4 + 0x168) != 0) {
            *(Matrix4x3 *)(r5 + 0x1c) = data_020a0e68;
            ((MaterialChanger *)r4)->Update(*(ModelComponents *)(r5 + 8));
            ((ModelAnim *)r5)->Render(0);                /* DELTA 1 */
        }
        i++;
        r5 += 0x64;
        r4 += 0x14;
    } while (i < 3);
}
