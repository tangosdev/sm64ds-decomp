/* HOST COPY of src/func_ov027_02111e00.cpp -- DA_PG_DFDR's (258, ov027)
 * Render, the ModelAnim slot-5 collision one more time.
 * Run rel0215 wave 2, lane cast-ov027.
 *
 * The matched TU dispatches through a LOCAL SIX-VIRTUAL ROM-order shadow
 * (`struct Sub { v0..v4; virtual void m(int); }; struct Base { char pad[0x320];
 * Sub sub; };  b->m(0);`) over the object at +0x320, and DA_PG_DFDR's member
 * there is a ModelAnim, not a Model: func_ov027_0211207c (its factory)
 * constructs it with _ZN9ModelAnimC1Ev at c+0x320, and both destructors --
 * func_ov027_021118c8 (D1) and func_ov027_02111924 (D0) -- destroy it with
 * _ZN9ModelAnimD1Ev at the same offset. Its plain Model lives at +0xd4 and is
 * not touched here.
 *
 * So the shadow's "slot 5" is the ROM's ModelAnim::Render, and the host
 * _ZTV9ModelAnim's slot 5 is Virtual18 -- a two-argument method reached with
 * the shadow's one argument, the Whomp/Fish/BabyPenguin case documented in
 * port/unmatched/ModelAnim_Renders.cpp and hit again by FLYING_CARPET in
 * port/unmatched/FlyingCarpet_Render.cpp. _ZTV5Model is dual-filled and would
 * have served this correctly; _ZTV9ModelAnim cannot be, because Virtual18
 * really occupies that slot.
 *
 * UNLIKE FLYING_CARPET'S, THIS ONE WAS MEASURED AS A CRASH, not predicted.
 * The lane's first seated boot of level 19 died on frame 2:
 *
 *   FAULT code c0000005 at +0x000faf9c accessing 00000000
 *     walker node 30039688 actor 30039650 id 0x102
 *     regs eax=00000000 ecx=30039978 edx=30075f2c ... esi=30039970 edi=30039650
 *
 * and port/tools/faultmap.py resolves the frame chain exactly through this
 * path: +0x000ecaa9 port_actor_process -> +0x001ce74a pd_render
 * (hal/actor_classes_ov027.cpp, slot 9) -> +0x001cfb1e func_ov027_02111e00
 * -> +0x000fafde ModelAnim::Virtual18 -> +0x000fae95 ModelAnim::Virtual10
 * -> +0x000faf9c Model::Virtual10, dereferencing null. esi = 0x30039970 is
 * actor 0x30039650 + 0x320, the ModelAnim, and ecx = +0x328 is what
 * Virtual18 took for its second argument -- the shadow's `0` never reached it.
 *
 * Control flow is the matched source line for line, and the ROM listing at
 * 0x02111e00 confirms both halves:
 *   02111e08  add r0, r4, #0x384                    -> the TextureSequence
 *   02111e0c  add r1, r4, #0x328                    -> its ModelComponents
 *   02111e10  bl  <TextureSequence::Update>
 *   02111e14  add r0, r4, #0x320                    -> the ModelAnim
 *   02111e18  ldr r2, [r0]                          -> its vptr
 *   02111e1c  mov r1, #0                            -> the one argument
 *   02111e20  ldr r2, [r2, #0x14]                   -> slot 5 (0x14/4)
 *   02111e24  blx r2
 *   02111e28  mov r0, #1                            -> return 1
 * PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Fish case.
 *
 * The matched src stays byte-locked in src/ as proof and is dropped from
 * port/slice_ov027cast.txt.
 */
#include "ModelAnim.h"

extern "C" {

void _ZN15TextureSequence6UpdateER15ModelComponents(void *ts, void *mc);

int func_ov027_02111e00(void *selfv)
{
    char *c = (char *)selfv;
    _ZN15TextureSequence6UpdateER15ModelComponents(c + 0x384, c + 0x328);
    /* ((Sub *)&mModelAnim)->m(0) -- ROM slot 5, spelled qualified */
    ((ModelAnim *)(c + 0x320))->ModelAnim::Render(0);
    return 1;
}

}  /* extern "C" */
