/* HOST COPY of src/_ZN7Wiggler6RenderEv.cpp -- the Wiggler's Render (id 248,
 * ov034, level 26). Run rel0215, lane cast-sweep2.
 *
 * WHY A HOST COPY: the ModelAnim slot-5 collision, the Koopa / Whomp / Fish /
 * MontyMole case (port/unmatched/Koopa_Render.cpp). The matched TU dispatches
 * through a LOCAL six-virtual shadow over an ARRAY of five ModelAnims:
 *
 *     struct Sub { virtual int g0(); ... virtual int g5(void *); };
 *     ((Sub *)p6)->g5(p4);
 *
 * so `g5` is the ROM's slot 5. ROM disasm 0x02112b1c:
 *     mov r0, r6 ; ldr r2,[r0] ; mov r1, r4 ; ldr r2,[r2,#0x14] ; blx r2
 * with r6 = this + 0x110 + i*0x64 (the i-th ModelAnim) and r4 = this + 0x408 +
 * i*0xc (the i-th scale Vector3; the +0x408 is the ROM's own literal pool word
 * at 0x02112b58). The host _ZTV9ModelAnim is MSVC-ordered -- one destructor
 * slot where Itanium spends two -- so ROM slot 5 lands on Virtual18, which
 * takes two arguments where the shadow passes one. The remedy is the qualified
 * ModelAnim::Render, exactly as Koopa_Render.cpp spells it.
 *
 * THE WIGGLER IS FIVE MODELS, NOT ONE. Wiggler_Spawn array-constructs five
 * ModelAnims at +0x110 (0x64 apart), five MaterialChangers at +0x304 and five
 * TextureSequences at +0x368 (0x14 apart) -- the head and the four body
 * segments. Render walks all five, updating each segment's TextureSequence
 * against that segment's ModelComponents (+8 into the ModelAnim) before drawing
 * it at its own scale.
 *
 * Everything else is the matched source statement for statement.
 * The matched src TU stays in src/ as the byte proof and is dropped from
 * port/slice_sweep2_ov034.txt.
 *
 * PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Koopa/Whomp case.
 */
#include "ModelAnim.h"

extern "C" int _ZN15TextureSequence6UpdateER15ModelComponents(void *seq, void *mc);

/* PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Koopa/Whomp case. */
extern "C" int _ZN7Wiggler6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    char *p6 = c + 0x110;   /* the five ModelAnims,      stride 0x64 */
    char *p5 = c + 0x368;   /* the five TextureSequences, stride 0x14 */
    char *p4 = c + 0x408;   /* the five scale Vector3s,   stride 0x0c */

    for (int i = 0; i < 5; i++) {
        _ZN15TextureSequence6UpdateER15ModelComponents(p5, p6 + 8);
        /* ((Sub *)p6)->g5(p4) -- ROM slot 5, ModelAnim::Render(scale) */
        ((ModelAnim *)p6)->ModelAnim::Render((const Vector3 *)p4);
        p6 += 0x64;
        p5 += 0x14;
        p4 += 0xc;
    }
    return 1;
}
