/* HOST COPY of src/func_ov032_02112164.cpp -- BUBBA's Render (id 228, ov032,
 * level 24's big fish, RTTI 12daBakubaku_c). Run rel0215, lane cast-sweep2.
 *
 * WHY A HOST COPY: the ModelAnim slot-5 collision, the Koopa / Whomp / Fish /
 * MontyMole case (port/unmatched/Koopa_Render.cpp, and the fault written down
 * in port/unmatched/W19_Slot5_Renders.cpp). The matched TU dispatches BUBBA's
 * ModelAnim at +0x34c through a LOCAL six-virtual shadow:
 *
 *     struct Obj { virtual void m0(); ... virtual void Target(int); };
 *     Obj *o = (Obj *)(c + 0x34c); o->Target(0);
 *
 * so `Target` is the ROM's slot 5. ROM disasm at 0x02112190:
 *     add r0, r0, #0x34c      the ModelAnim
 *     ldr r2, [r0]            its vptr
 *     mov r1, #0              the scale argument, NULL
 *     ldr r2, [r2, #0x14]     slot 5
 *     blx r2
 * The host _ZTV9ModelAnim is MSVC-ordered (one destructor slot where Itanium
 * spends two), so ROM slot 5 lands on Virtual18, which takes two arguments
 * where the shadow passes one. The remedy is the qualified ModelAnim::Render,
 * exactly as Koopa_Render.cpp spells it -- and BUBBA passes a NULL scale, which
 * ModelAnim::Render reads as "no scale override".
 *
 * The matched src TU stays in src/ as the byte proof and is dropped from
 * port/slice_sweep2_ov032.txt.
 *
 * The engine-hide test is the same 0x40000 flag at +0xb0 every other Render in
 * the tree reads, and it comes first, before the draw.
 *
 * PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Koopa/Whomp case.
 */
#include "ModelAnim.h"

/* PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Koopa/Whomp case. */
extern "C" int func_ov032_02112164(void *selfv)
{
    char *c = (char *)selfv;

    if ((*(unsigned int *)(c + 0xb0) & 0x40000) != 0)
        return 1;

    /* ((Obj *)&mModelAnim)->Target(0) -- ROM slot 5, ModelAnim::Render(NULL) */
    ((ModelAnim *)(c + 0x34c))->ModelAnim::Render((const Vector3 *)0);
    return 1;
}
