/* HOST COPY of src/func_ov006_020cafb4.cpp -- the per-frame model draw of ONE
 * dMgTrmpln3DMario_c element (one of the four bouncing characters).
 * Run mg11, lane TTI.
 *
 * ---- 1. THE DEFECT IS THE LOCAL SHADOW CLASS, AGAIN ----------------------
 *
 * The same one port/unmatched/MgFlower_ModelRender_020c3bf4.cpp,
 * MgMemory2_ModelRender_020c1804.cpp and Door_Render.cpp already carry:
 * hal/cxxname_bridge.cpp fills the host model vtables in MSVC order, where the
 * destructor costs ONE slot and Itanium's costs two.
 *
 *   _ZTV5Model      dtor 0, DoSetFile 1, UpdateVerts 2, Virtual10 3, Render 4
 *   _ZTV9ModelAnim  dtor 0, DoSetFile 1, UpdateVerts 2, Virtual10 3, Render 4,
 *                   Virtual18 5
 *
 * src/func_ov006_020cafb4.cpp models the object at +0x6c with a local six-pure-
 * virtual `struct C` and calls its SIXTH virtual:
 *
 *     struct C { virtual void a0(); ... virtual void f(void*); };
 *     ((C *)(o + 0x6c))->f((void *)data_ov006_0212e038);
 *
 * That is the ROM's own numbering -- BYTE +0x14 of an Itanium vtable is the
 * sixth word, and on both model classes that word is Render.  Read straight off
 * extracted/overlays/overlay_0006.bin at base 0x020bfec0:
 *
 *   020cafb4  push {r4,lr}
 *   020cafb8  sub  sp,sp,#4
 *   020cafbc  ldr  r2,[r0,#0x6c]!      ; advance to +0x6c AND load the vptr
 *   020cafc0  ldr  r1,[pc,#0x10]       ; = data_ov006_0212e038
 *   020cafc4  ldr  r2,[r2,#0x14]       ; ROM slot 5 -> ModelAnim::Render
 *   020cafc8  blx  r2
 *
 * On the host that same sixth word is ModelAnim::Virtual18, so the shadow's
 * one-argument call arrives at a two-argument function with the scale left as
 * stack litter.
 *
 * ---- 2. WHAT IT MEASURED HERE, WHICH IS NOT WHAT IT MEASURED FOR 390 -----
 *
 * Scene 384, 60 frames, SM64DS_FAULTS_FATAL=1, before this file existed:
 *
 *   FAULT code c0000005 at +0x303ff244 accessing 307ff244
 *     walker node 307FA270 actor 307FA238 id 0x180
 *     stack[10] +0x000a6088   func_ov006_021212fc  (slot 9, Render)
 *
 * +0xa6088 is the RETURN address of the `call _func_ov006_020cd270` at
 * +0x163 in the compiled Render body (dumpbin /disasm:nobytes over
 * func_ov006_021212fc.c.obj), and func_ov006_020cd270's own last loop is
 *
 *     for (i = 0; i < data_ov006_02140594; i++)
 *         func_ov006_020cafb4(data_ov006_02140550 + i * 0xd0);
 *
 * -- the four element records, at the 0xd0 stride this class's factory built
 * them with.  The faulting eip is INSIDE the actor object rather than at zero,
 * which is the run law's tell for a calling-convention or slot mismatch rather
 * than a null dereference, and it is why this repair was made from the
 * disassembly rather than from a guess about which call in Render was the one.
 *
 * THE CONTROL IS THE SAME RUN WITH SM64DS_SCENE_SLOT9=0: rc 0, 60 frames, init
 * 1, behavior 13, state-reset 1, slot34 44, 13 dispatches routed to chain link
 * 0, 0 UNHANDLED, all four fill tables 0 raw words left.  Everything except the
 * draw was already clean, which is what localises the fault to this call.
 *
 * ---- 3. WHAT CHANGED, LINE BY LINE --------------------------------------
 *
 * The local `struct C` shadow and its `f(...)` call become a virtual
 * ModelAnim::Render(...).  Nothing else: the +0x6c offset and the
 * data_ov006_0212e038 argument are the matched source's, unchanged.
 *
 * THE DISPATCH STAYS VIRTUAL rather than qualified, for the reason
 * MgFlower_ModelRender_020c3bf4.cpp gives: the ROM's is (ldr vptr /
 * ldr [vptr,#0x14] / blx), and a qualified call would silently devirtualise a
 * subclass override.  The object at element+0x6c is constructed by
 * _ZN9ModelAnimC1Ev in the element constructor (see
 * unmatched/MgTrmpln3DMario_ModuleFix.cpp), so it is a ModelAnim by
 * construction -- but the ROM still dispatches, and so does this.
 *
 * IT TAKES THE SAME SYMBOL NAME, so src/func_ov006_020cafb4.cpp is out of
 * port/slice_tti.txt.  The body is dMgTrmpln3DMario_c's, which is THIS class's
 * element type alone (only this class's factory calls the element constructor
 * that installs data_ov006_0213b2c4), so no sibling lane collides on it.
 */

#include "ModelAnim.h"

extern "C" {
extern int data_ov006_0212e038[];
void func_ov006_020cafb4(char *o);
}

// PORT_HOST_ABI: mwcc virtual-shadow dispatch (ROM vtable numbering vs MSVC's).
void func_ov006_020cafb4(char *o)
{
    ((ModelAnim *)(o + 0x6c))->Render((const Vector3 *)data_ov006_0212e038);
}
