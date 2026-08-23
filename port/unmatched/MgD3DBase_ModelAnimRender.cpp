/* PORT_HOST_ABI.  TWO mwcc VIRTUAL-SHADOW DISPATCHES onto ModelAnim's ROM slot
 * 5, in the dScMgD3DBase_c family's render path.  Run mg11.
 *
 * THE FILE NAME IS NOT LANE-SCOPED, for port/unmatched/MgD3DBase_Slots.cpp's
 * reason: the four ids under dScMgD3DBase_c (0x174, 0x175, 0x180, 0x181) share
 * this render machinery, and more than one mg11 lane reaches it.  Compare
 * contents at merge rather than assuming a duplicate.
 *
 * ---- THE DEFECT, WHICH IS THE FLOWER'S ------------------------------------
 *
 * port/unmatched/MgFlower_ModelRender_020c3bf4.cpp states it in full and this
 * file does not restate the whole of it.  In short: the ROM dispatches
 *
 *     ldr r2,[r0]         the vptr
 *     ldr r2,[r2,#0x14]   ROM slot 5 -> ModelAnim::Render(Vector3 const *)
 *     blx r2
 *
 * and the matched sources spell that as a LOCAL SHADOW CLASS with six virtuals,
 * calling the sixth.  On the host that same byte offset is MSVC's sixth word,
 * which on ModelAnim is Virtual18(u32 mat, const Vector3 *scale) -- a different
 * function with a different first parameter.  include/ModelAnim.h:74 declares
 * the real one `virtual void Render(const Vector3 *scale)` at slot 5.
 *
 * ---- HOW IT PRESENTED HERE -----------------------------------------------
 *
 * It is INVISIBLE to SM64DS_SCENE_NO_RENDER=1 and it was invisible to this lane
 * for a second reason on top: until port/unmatched/MgD3DBase_Slots.cpp put the
 * receiver back into dScMgD3DBase_c slot 10, Actor::BeforeRender answered NO on
 * every frame and the framework skipped SLOT 9 -- so no render code ran to be
 * wrong.  (Slot 10 itself WAS dispatched, 1500 times on a 1500-frame rendered
 * boot, and so was slot 11; only slot 9 read zero.  An earlier version of this
 * header said all three read zero, which was a NO_RENDER census quoted in a
 * rendered comparison.)  With slot 10 repaired, a 300-frame RENDERED boot of
 * scene 385 faults immediately:
 *
 *     FAULT code c0000005 at +0x303fd460 accessing 307fd460
 *       regs ... ebp=004915f4   -> func_ov006_020caadc+0xf4
 *       stack[10] +0x00095cab   -> func_ov006_021231ac+0x11b  (vtable slot 9)
 *       stack[16] +0x0008e3ec   -> tte_render
 *       ... port_actor_render / port_scene_tick / port_scene_run
 *
 * and 0x307fd460 is the live scene object 0x307F8454 PLUS 0x500c -- the first
 * Trampoline-Mario record, i.e. the call jumped to its own operand.  That is
 * the signature of a receiver arriving on the stack where the callee expects a
 * register.
 *
 * ---- THE TWO SITES -------------------------------------------------------
 *
 * src/func_ov006_020c8e90.cpp, reached every rendered frame:
 *     struct C { virtual void a0()..a4(); virtual void f(void *); };
 *     ((C *)(o + 0x78))->f((void *)data_ov006_0212e02c);
 * The object is the Trampoline-Mario record's ModelAnim at +0x78 -- the same
 * +0x78 its constructor (src/_ZN6Player29TryExitCharacterDoorWithIntroEv.cpp,
 * misnamed) runs _ZN9ModelAnimC1Ev on.  data_ov006_0212e02c is four words of
 * 0x2000, a Vector3 scale of 2.0 in 20.12, which is what Render's parameter is.
 *
 * src/func_ov006_02122814.cpp, the same shape one object out:
 *     ((C *)c)->v5(c + 0x50);
 * with c's own +0x1c written from data_020a0e68 immediately before -- and
 * Model::mat4x3 is +0x1c into a Model, so `c` is a Model/ModelAnim by its own
 * preceding statement.  The argument c+0x50 is that object's scale vector.
 *
 * THE CLOSURE WAS SWEPT RATHER THAN WAITED ON, and the sweep has to be phrased
 * over the CLOSURE rather than over the slice or it cannot see its own answer.
 * Every TU declaring a local struct with virtuals was scanned for WHICH virtual
 * it calls and at what index.
 *
 *   over the 204 lines of port/slice_tte.txt          18 hits, NONE at index 5
 *     13 at index 4 (byte 0x10) on the Trampoline-Mario element's own five-slot
 *        table 0x0213b2e0, which hal/scene_mg_trampoline2.cpp fills, so those
 *        land on this port's own thunks
 *      1 at index 18 (byte 0x48), the scene's slot-18 dispatch from slot 0
 *      2 at index 0, 1 at index 1, 1 at index 2, on argument objects that are
 *        not ModelAnims
 *   plus the two HOST-COPIED files below   2 hits, both at INDEX 5 (byte 0x14)
 *
 * 18 + 2 = 20 over the closure. An earlier version of this paragraph said
 * "every TU in port/slice_tte.txt ... TWENTY hits", which is false twice over:
 * the slice answers 18, and src/func_ov006_020c8e90.cpp and
 * src/func_ov006_02122814.cpp stopped being slice lines the moment this file
 * took them. The two hits it was quoted for were outside the set it named.
 *
 * ---- WHAT DIVERGES FROM src, EXACTLY --------------------------------------
 *
 * The shadow class and its indexed call become a virtual ModelAnim::Render.
 * Every other statement, offset and constant is the matched source's, in the
 * source's order.  The dispatch stays VIRTUAL rather than qualified, for
 * MgFlower_ModelRender_020c3bf4.cpp's reason: the object is only known to be in
 * the ModelAnim family and a qualified call would devirtualise a subclass
 * override.  Both src TUs are OUT of port/slice_tte.txt: listing either beside
 * these definitions would be an LNK2005.
 *
 * THE DECOMP-SIDE FIX IS ROUTED, NOT TAKEN.  Nothing about either src is wrong
 * on ARM.
 */

#include "common.h"
#include "ModelAnim.h"

extern "C" {
extern int data_ov006_0212e02c[];
extern Matrix4x3 data_020a0e68;
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationZ(void *m, short angZ);
void func_ov006_020c8e90(char *o);
void func_ov006_02122814(char *c);
}

// PORT_HOST_ABI: mwcc virtual-shadow dispatch (ROM vtable numbering vs MSVC's).
void func_ov006_020c8e90(char *o)
{
    if (*(short *)(o + 0x6e) == 0)
        return;
    ((ModelAnim *)(o + 0x78))->Render((const Vector3 *)data_ov006_0212e02c);
}

// PORT_HOST_ABI: the same, on the object the three statements above it fill.
void func_ov006_02122814(char *c)
{
    Matrix4x3_FromTranslation(&data_020a0e68, *(int *)(c + 0x5c),
                              *(int *)(c + 0x60), *(int *)(c + 0x64));
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, *(short *)(c + 0x74));
    *(Matrix4x3 *)(c + 0x1c) = data_020a0e68;
    ((ModelAnim *)c)->Render((const Vector3 *)(c + 0x50));
}
