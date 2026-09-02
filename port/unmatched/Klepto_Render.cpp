/* HOST COPY of src/_ZN6Klepto6RenderEv.cpp -- the BlendModelAnim slot-5
 * collision, the same trap hal/blend_vtable.cpp's header and
 * unmatched/ModelAnim_Renders.cpp spell out for the ModelAnim family.
 *
 * hal/blend_vtable.cpp fills _ZTV14BlendModelAnim in MSVC order (dtor 0,
 * DoSetFile 1, UpdateVerts 2, Virtual10 3, Render 4, Virtual18 5), because MSVC
 * spends ONE vtable slot on the destructor where Itanium spends two. Its own
 * header says the consequence out loud: "if a shadow-class TU ever dispatches
 * BlendModelAnim::Render it will land on blend_virtual18 and read its scale off
 * the stack." src/_ZN6Klepto6RenderEv.cpp is exactly such a TU -- it dispatches
 * through a LOCAL SHADOW CLASS with six virtuals
 *
 *     struct Base { virtual void v0(); ... virtual void v4(); virtual void m(int); };
 *     Base *b = &((Derived *)this)->base; b->m(0);
 *
 * so its "slot 5" is the ROM's Render, while the host array's slot 5 is
 * Virtual18, which takes TWO arguments where the shadow passes one. Compiled
 * from src it read a scale off the stack and handed Model::Virtual10 garbage:
 * MEASURED c0000005 in Model::Virtual10 (via BlendModelAnim::Virtual18) at the
 * frame the cross-fade weight reaches 1.0 and the pose stops routing through
 * func_0204531c -- the same fault the Butterfly reading records, one class over.
 * The first Klepto seat left Render a face onto the src body and hit exactly
 * this; the abort stub over the e17c main handler used to kill the run before
 * the fade completed, hiding it, so it surfaced the moment that handler became
 * an inert return.
 *
 * The remedy is the ModelAnim_Renders / Whomp recipe: host the one TU and spell
 * the dispatch as the qualified method the ROM means. Base is at this+0x334
 * (Derived's pad), and m(0) is Render(0) -- Render with a null scale. Nothing
 * here changes what code runs; it only puts the call on the right slot.
 */
#include "BlendModelAnim.h"

extern "C" int _ZN6Klepto6RenderEv(void *selfv)
{
    ((BlendModelAnim *)((char *)selfv + 0x334))
        ->BlendModelAnim::Render((const Vector3 *)0);
    return 1;
}
