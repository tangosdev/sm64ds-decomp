/* HOST COPY of src/func_ov006_020c1f4c.cpp -- the per-frame matrix update of
 * the ov006 sub-object dScMgSlot3_c holds at this+0x4f38, reached from vtable
 * slot 9 (Render) through func_ov006_0210b648 -> func_ov006_020c201c.
 * Run mg11, lane MUG.
 *
 * THE LOCAL SHADOW CLASS AGAIN, AND THIS TIME ON THE SLOT hal/blend_vtable.cpp
 * PREDICTED WOULD BREAK NEXT. That file's own header says it out loud:
 *
 *     "a TU that dispatches Render through a LOCAL SHADOW CLASS (which counts
 *      in ROM numbering) cannot be served by this array"
 *
 * and lists the two numberings that make it true. MSVC spends ONE vtable slot
 * on the destructor where Itanium spends two:
 *
 *     method       ROM/Itanium   MSVC
 *     ~dtor            0, 1        0
 *     DoSetFile          2         1
 *     UpdateVerts        3         2
 *     Virtual10          4         3
 *     Render             5         4
 *     Virtual18          6         5
 *
 * src/func_ov006_020c1f4c.cpp spells the object at +0x10 as a local
 * `struct Vtbl` of FOUR pure virtuals and calls `v3()`, which is the ROM's own
 * numbering: byte +0xc of an Itanium vtable is the fourth word, and on
 * BlendModelAnim that word is UpdateVerts. Read straight off
 * extracted/overlays/overlay_0006.bin at base 0x020bfec0:
 *
 *     020c1f98  add  r0, r4, #0x10       the BlendModelAnim at +0x10
 *     020c1f9c  ldr  r1, [r0]            its vptr
 *     020c1fa0  ldr  r1, [r1, #0xc]      ROM slot 3 -> UpdateVerts
 *     020c1fa4  blx  r1
 *
 * and _ZTV14BlendModelAnim at 0x0208e94c reads, word by word,
 * {D1 0x02016690, D0 0x02016644, DoSetFile 0x02016604, UpdateVerts 0x02016578,
 * Virtual10 0x02016518, Render 0x020164e4, Virtual18 0x020164b0}, so byte 0xc
 * is UpdateVerts and nothing else.
 *
 * ON THE HOST that byte is the FOURTH MSVC word, which is
 * Virtual10(Matrix4x3 &mat). The shadow class passes no argument, so the
 * reference arrives as whatever was in the slot.
 *
 * MEASURED, scene 365, this tree, before this file existed:
 *
 *   FAULT code c0000005 at +0x000bea7c accessing ffffec00
 *     walker node 307FAFF4 actor 307FAFBC id 0x16d
 *     slot3_render+0xc -> func_ov006_0210b648+0x23
 *       -> func_ov006_020c201c+0xed
 *       -> func_ov006_020c1f4c+0x62
 *       -> BlendModelAnim::Virtual10+0x42
 *       -> Model::Virtual10+0xc,  eax=ffffec00
 *
 * BANKED AS A REPRODUCTION: runs/mg11/out/MUG/fault_c0000005_shadowclass_
 * playlog.log and its faultmap symbolization. The first boot's playlog was
 * rotated out of the build directory before it was banked, so the fault was
 * re-created by reverting the one line below to src's shadow dispatch in a
 * scratch build and restoring it afterwards. The module-relative bases moved
 * between the two builds; the functions, the offsets into them, the fault
 * code, the faulting address and the actor id are identical.
 *
 * which is the MgMemory2_ModelRender_020c1804 chain one slot over, ending in
 * the same Model::Virtual10.
 *
 * WHICH CLASS THE OBJECT IS, established from the CONSTRUCTOR rather than
 * guessed from the call, per that file's rule. func_ov006_020c221c builds this
 * sub-object and runs `_ZN14BlendModelAnimC1Ev(t + 0x10)` -- the disassembly is
 * in unmatched/MgSlot3_SubObjCtor.cpp, which host-copies it for a different
 * defect -- so +0x10 is a BlendModelAnim and its UpdateVerts is MSVC slot 2.
 *
 * THE DELTA FROM src IS ONE LINE. `((Vtbl*)(c+0x10))->v3()` becomes
 * `((BlendModelAnim *)(c + 0x10))->UpdateVerts()`, which is a virtual call on
 * the real class through the real header, so the host resolves the slot the
 * same way the ROM's `ldr r1,[r1,#0xc]` does. Everything else -- the
 * Matrix4x3_FromTranslation on data_020a0e68 from +0x8c/+0x90/+0x94, the Y
 * rotation by +0x9a, the copy into +0x2c, the read-back of the pointed-to
 * object's matrix at +0x120 through the pointer at +0x24, the X rotation by
 * +0xa4, the Z rotation by +0xa2 and the write-back -- is src's, unchanged.
 *
 * IT DEFINES func_ov006_020c1f4c, so src/func_ov006_020c1f4c.cpp is OUT of
 * port/slice_mug.txt -- listing both would be an LNK2005. That is the third
 * linked function this seat pays.
 *
 * ONE CALLER AND ONE OWNER. config/arm9/overlays/ov006/relocs.txt carries
 * exactly one arm_call to 0x020c1f4c, from 0x020c2128 inside
 * func_ov006_020c201c, whose own only caller is 0x0210b668 inside
 * dScMgSlot3_c's Render. The whole 0x020c1eb4..0x020c221c sub-object family is
 * reached from this class and no other, so nothing outside this lane is
 * affected and no canonical-copy question arises the way it did for
 * func_ov006_020c2b8c at the mg9 merge.
 *
 * THE DECOMP-SIDE FIX IS ROUTED, NOT TAKEN: the src TU should spell the
 * BlendModelAnim rather than a four-virtual shadow, and whether it still builds
 * byte-identically under mwccarm is a byte-gated-tree question this lane does
 * not touch. Same routing the flower, door and memory2 shadow repairs got.
 */

#include "common.h"
#include "BlendModelAnim.h"

typedef short s16;

extern "C" {
void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *m, s16 a);
void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *m, s16 a);
void Matrix4x3_ApplyInPlaceToRotationZ(Matrix4x3 *m, s16 a);
extern Matrix4x3 data_020a0e68;
}

struct Sub { char pad[0x120]; Matrix4x3 m; };

extern "C" void func_ov006_020c1f4c(char *c)
{
    Matrix4x3_FromTranslation(&data_020a0e68, *(int *)(c + 0x8c),
                              *(int *)(c + 0x90), *(int *)(c + 0x94));
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(s16 *)(c + 0x9a));
    *(Matrix4x3 *)(c + 0x2c) = data_020a0e68;
    ((BlendModelAnim *)(c + 0x10))->UpdateVerts();     /* DELTA: was v3() */
    data_020a0e68 = (*(Sub **)(c + 0x24))->m;
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(s16 *)(c + 0xa4));
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, *(s16 *)(c + 0xa2));
    (*(Sub **)(c + 0x24))->m = data_020a0e68;
}
