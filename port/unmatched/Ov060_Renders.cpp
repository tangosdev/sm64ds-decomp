/* HOST COPY of src/_ZN6Bowser6RenderEv.cpp -- the ModelAnim slot-5 collision,
 * once more. run linkw wave 6, lane w6-A.
 *
 * port/unmatched/ModelAnim_Renders.cpp carries the long version of this
 * reading: hal/cxxname_bridge.cpp fills _ZTV9ModelAnim in MSVC order (dtor 0,
 * DoSetFile 1, UpdateVerts 2, Virtual10 3, Render 4, Virtual18 5) because MSVC
 * spends ONE slot on the destructor where Itanium spends two, and ModelAnim
 * -- unlike Model -- cannot be dual-filled, because Virtual18 already occupies
 * the slot Render would need. A matched TU that dispatches through a LOCAL
 * six-virtual shadow class counts in the ROM's numbering, so its "slot 5" is
 * Render and the host array's slot 5 is Virtual18, which takes two arguments
 * where the shadow passes one and hands Model::Virtual10 a stack word.
 *
 * src/_ZN6Bowser6RenderEv.cpp is exactly that shape:
 *
 *     struct Obj { virtual void v0(); ... virtual void m(void *); };
 *     ((Obj *)((char *)&mModelAnim))->m((char *)&mScaleX);
 *
 * with mModelAnim at +0xd4 (Bowser_Spawn constructs a ModelAnim there) and the
 * scale Vector3 at +0x80. Bowser is the FIFTEENTH class in this collision
 * class; the fourteenth was Boo, landed in wave 5.
 *
 * SPIKE BOMB and BOWSER SKY PLATFORM are NOT in this file on purpose: their
 * shadow dispatch lands on a plain Model at +0xd4, and _ZTV5Model IS
 * dual-filled (slot 4 and slot 5 both Render) for precisely this case. Their
 * Renders stay matched src. BOWSER SHOCKWAVES (281) has the same collision on
 * TWO ModelAnims (+0xd4 and +0x174) and is not hosted this lane -- it is not
 * registered, so nothing dispatches it.
 *
 * The body below is the matched source's control flow line for line; only the
 * shadow dispatch is respelled as the qualified method the ROM means.
 */
#include "ModelAnim.h"

extern "C" {
void _ZN9Animation7AdvanceEv(void *a);
void _ZN15TextureSequence6UpdateER15ModelComponents(void *seq, void *comp);

/* PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Fish case. */
int _ZN6Bowser6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    if (*(unsigned char *)(c + 0x41c) < 8)
        return 1;
    _ZN9Animation7AdvanceEv(c + 0x138);
    _ZN15TextureSequence6UpdateER15ModelComponents(c + 0x138, c + 0xdc);
    /* ((Obj *)&mModelAnim)->m(&mScaleX) -- the ROM slot-5 Render, qualified */
    ((ModelAnim *)(c + 0xd4))->ModelAnim::Render((const Vector3 *)(c + 0x80));
    return 1;
}

/* ---- BOWSER SHOCKWAVES (281) -- lane SEAT-OV002-RUNTIME ---------------------
   HOST COPY of src/_ZN16BowserShockwaves6RenderEv.cpp: the same ModelAnim
   slot-5 collision the Bowser body above documents, on TWO ModelAnims (+0xd4
   and +0x174, both constructed by BowserShockwaves_Spawn). The matched TU
   dispatches `((Obj *)(c+0xd4))->m(0)` and `((Obj *)(c+0x174))->m(v)` with
   v = {0x800,0x800,0x800} through a local six-virtual shadow; those are the
   ROM's ModelAnim::Render(nullptr) and ModelAnim::Render(&v), spelled
   qualified. The six animation Updates before them are the matched source's
   own calls in its own order (TextureSequence x2, MaterialChanger x2,
   TextureTransformer x2, each against the ModelComponents at +0xdc / +0x17c). */
int _ZN15MaterialChanger6UpdateER15ModelComponents(void *mc, void *comp);
int _ZN18TextureTransformer6UpdateER15ModelComponents(void *tt, void *comp);

/* PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch on two ModelAnims. */
int _ZN16BowserShockwaves6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    _ZN15TextureSequence6UpdateER15ModelComponents(c + 0x138, c + 0xdc);
    _ZN15TextureSequence6UpdateER15ModelComponents(c + 0x1d8, c + 0x17c);
    _ZN15MaterialChanger6UpdateER15ModelComponents(c + 0x14c, c + 0xdc);
    _ZN15MaterialChanger6UpdateER15ModelComponents(c + 0x1ec, c + 0x17c);
    _ZN18TextureTransformer6UpdateER15ModelComponents(c + 0x160, c + 0xdc);
    _ZN18TextureTransformer6UpdateER15ModelComponents(c + 0x200, c + 0x17c);
    /* ((Obj *)(c+0xd4))->m(0) -- the ROM slot-5 Render, qualified */
    ((ModelAnim *)(c + 0xd4))->ModelAnim::Render((const Vector3 *)0);
    {
        Vector3 v;
        v.x = 0x800; v.y = 0x800; v.z = 0x800;
        /* ((Obj *)(c+0x174))->m(v) */
        ((ModelAnim *)(c + 0x174))->ModelAnim::Render(&v);
    }
    return 1;
}
}
