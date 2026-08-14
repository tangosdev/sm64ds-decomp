/* HOST COPY of _ZN3MrI6RenderEv (ov071 0x021216b8, 0x34 bytes) -- the
 * ModelAnim slot-5 collision, one more class into the same wall the
 * Butterfly/Fish/QuestionBlock/Whomp/Scuttlebug copies in
 * port/unmatched/ModelAnim_Renders.cpp already document.
 *
 * WHY: src/_ZN3MrI6RenderEv.cpp dispatches through a LOCAL six-virtual shadow
 *
 *     struct Sub { virtual int g0(); ... virtual int g5(void*); };
 *     ((Sub*)((char*)&mModelAnim))->g5((char*)&mScaleX);
 *
 * so its "slot 5" is the ROM's ModelAnim::Render. hal/cxxname_bridge.cpp fills
 * _ZTV9ModelAnim in MSVC order, where slot 5 is Virtual18 -- a two-argument
 * method the shadow calls with one. That is the c0000005 the Butterfly gate
 * measured, and _ZTV9ModelAnim cannot be dual-filled because Virtual18 really
 * occupies the slot.
 *
 * The ROM body, read from extracted/overlays/overlay_0071.bin at base
 * 0x0211f000 (the base derivation is in unmatched/MrI_StateMains.cpp), is
 * thirteen instructions and does exactly two things:
 *
 *     021216c0  add r0, r4, #0x138      TextureSequence at +0x138
 *     021216c4  add r1, r4, #0xdc       ModelComponents  at +0xdc
 *     021216c8  bl  0x02015988          TextureSequence::Update(ModelComponents&)
 *     021216cc  add r0, r4, #0xd4       &mModelAnim
 *     021216d0  ldr r2, [r0]            its vptr
 *     021216d4  add r1, r4, #0x80       &mScaleX
 *     021216d8  ldr r2, [r2, #0x14]     vtable[5]  (0x14/4 = 5)
 *     021216dc  blx r2                  ROM slot 5 = ModelAnim::Render
 *     021216e0  mov r0, #1              returns 1
 *
 * The +0xdc argument is mModelAnim.data (mModelAnim sits at 0xd4, its
 * ModelComponents at +0x8), the same relationship the Whomp copy spells.
 * Only the dispatch is respelled as the qualified method the ROM means; the
 * control flow is the matched source's, line for line.
 *
 * HOST COPY, not matched src: src/_ZN3MrI6RenderEv.cpp is EXCLUDED from this
 * lane's slice. If _ZTV9ModelAnim ever gains a ROM-order view, this retires.
 */
#include "Model.h"
#include "ModelAnim.h"

extern "C" {
int _ZN15TextureSequence6UpdateER15ModelComponents(void *seq, void *comp);
int _ZN3MrI6RenderEv(void *selfv);
}

/* PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Fish case. */
extern "C" int _ZN3MrI6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    _ZN15TextureSequence6UpdateER15ModelComponents(c + 0x138, c + 0xdc);
    /* ((Sub *)&mModelAnim)->g5(&mScaleX) -- the ROM slot-5 Render */
    ((ModelAnim *)(c + 0xd4))->ModelAnim::Render((const Vector3 *)(c + 0x80));
    return 1;
}
