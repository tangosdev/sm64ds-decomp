/* HOST COPY of src/func_ov036_02112378.cpp -- FLYING_CARPET's (130, ov036)
 * Render, the ModelAnim slot-5 collision one more time.
 *
 * The matched TU dispatches through a LOCAL SIX-VIRTUAL ROM-order shadow
 * (`struct Base { v0..v4; virtual void m(int); }; struct Derived { char
 * pad[0x428]; unsigned short fld; char pad2[...]; Base base; };  b->m(0);`)
 * over the object at +0x450, and FlyingCarpet's member there is a ModelAnim,
 * not a Model: FlyingCarpet_Spawn.cpp constructs it with _ZN9ModelAnimC1Ev at
 * c+0x450 and func_ov036_02112158 (its D1) destroys it with _ZN9ModelAnimD1Ev
 * at the same offset. Its three plain 0x50-stride Models live at +0x320 and
 * are not touched here.
 *
 * So the shadow's "slot 5" is the ROM's ModelAnim::Render, and the host
 * _ZTV9ModelAnim's slot 5 is Virtual18 -- a two-argument method reached with
 * the shadow's one argument, the Whomp/Fish/BabyPenguin case documented in
 * port/unmatched/ModelAnim_Renders.cpp. _ZTV5Model is dual-filled and would
 * have served this correctly; _ZTV9ModelAnim cannot be, because Virtual18
 * really occupies that slot.
 *
 * NOT MEASURED AS A CRASH HERE, and this file does not claim it was: the
 * collision is predicted from the member's own constructor/destructor pair and
 * from the eleven prior instances of the identical shape, and the TU is hosted
 * before the first boot rather than after a fault. The matched src stays
 * byte-locked in src/ as proof and is dropped from port/slice_ov036cast.txt.
 *
 * Control flow is the matched source line for line, and the ROM listing at
 * 0x02112378 confirms both halves:
 *   02112380  add r1, r0, #0x400 ; ldrh r1, [r1, #0x28]   -> u16 at +0x428
 *   02112388  cmp r1, #0x5a ; bhs ...                     -> the < 0x5a gate
 *   02112390  ands r1, r1, #1 ; ...ne -> return 1         -> the odd-frame skip
 *   021123a4  add r0, r0, #0x450 ; ldr r2,[r0] ; ldr r2,[r2,#0x14] ; blx r2
 *                                                          -> slot 5 (0x14/4)
 * PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Fish case.
 */
#include "ModelAnim.h"

extern "C" {

int func_ov036_02112378(void *selfv)
{
    char *c = (char *)selfv;
    unsigned short f = *(unsigned short *)(c + 0x428);
    if (f < 0x5a && (f & 1))
        return 1;
    /* ((Base *)&mModelAnim)->m(0) -- ROM slot 5, spelled qualified */
    ((ModelAnim *)(c + 0x450))->ModelAnim::Render(0);
    return 1;
}

}  /* extern "C" */
