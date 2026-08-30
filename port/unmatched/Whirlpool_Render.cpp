/* HOST COPY of src/_ZN9Submarine6RenderEv.cpp -- the WHIRLPOOL's Render
 * (id 246, daWater_Tatumaki_c, ov026, vtable slot 9).
 *
 * THE NAME IS dsd's, NOT THE CLASS'S: every `_ZN9Submarine*` TU in ov026
 * belongs to the whirlpool. See hal/actor_classes_ov026.cpp's header.
 *
 * THE ModelAnim SLOT-5 COLLISION -- the Whomp/Butterfly/Fish/QuestionBlock/
 * BabyPenguin case, walked into a sixth time. The matched src dispatches
 * through a LOCAL SIX-VIRTUAL ROM-ORDER SHADOW:
 *
 *     struct Sub { virtual void v0(); ... virtual void m(int); };
 *     struct Base { char pad[0x114]; Sub sub; };
 *     Sub *b = &((Base *)this)->sub; b->m(0);
 *
 * Six virtuals means its "slot 5" is the ROM's numbering, and for THIS class
 * the object at +0x114 is a ModelAnim (Whirlpool_Spawn constructs
 * _ZN9ModelAnimC1Ev there, and the whirlpool's InitResources calls
 * ModelAnim::SetAnim on it). hal/cxxname_bridge.cpp fills _ZTV9ModelAnim in
 * MSVC order and puts ma2_virtual18 in slot 5, and says in its own words why
 * it cannot dual-fill the way _ZTV5Model does: "Render's ROM slot (5) is
 * Virtual18's MSVC slot". Virtual18's trampoline pops TWO stack words where
 * this shadow call pushes one, so the skewed caller rets into its own spilled
 * frame -- the measured Butterfly/Whomp failure.
 *
 * The three OTHER shadow-Render bodies in this overlay (func_ov026_02111308,
 * _021117a8 and _02111998, the polelift, the shutter and the real Submarine)
 * dispatch the identical idiom at +0xd4, where every one of those classes
 * constructs a plain Model -- and _ZTV5Model[5] IS dual-filled, so those three
 * stay in the slice. Only this one moves.
 *
 * The body below is the matched source's control flow line for line; only the
 * dispatch is spelled as the qualified method the ROM means.
 */
#include "ModelAnim.h"

extern "C" {

void _ZN18TextureTransformer6UpdateER15ModelComponents(void *tt, void *comp);

/* PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Fish case. */
int _ZN9Submarine6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    /* mTextureTransformer at +0x178, the ModelAnim's ModelComponents at +0x11c
       (mModelAnim +0x114, its data +0x8) -- both straight out of the matched
       source's own expressions. */
    _ZN18TextureTransformer6UpdateER15ModelComponents(c + 0x178, c + 0x11c);
    /* ((Sub *)&mModelAnim)->m(0) -- the ROM slot-5 Render, spelled qualified */
    ((ModelAnim *)(c + 0x114))->ModelAnim::Render(0);
    return 1;
}

}
