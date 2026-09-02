/* HOST COPY of src/_ZN13RacingPenguin6RenderEv.cpp (ov019 0x02112360, 0x34
 * bytes), run rel0215 wave 3 (lane w3-e).
 *
 * PORT_HOST_ABI: mwcc shadow-class dispatch of a ModelAnim's ROM slot 5, which
 * MSVC's ModelAnim table cannot serve.
 *
 * THE ROM BODY, disassembled out of extracted/overlays/overlay_0019.bin:
 *   02112368  ADD r0, r4, #0x138            the TextureSequence
 *   0211236c  ADD r1, r4, #0xdc             the ModelComponents
 *   02112370  BL  TextureSequence::Update
 *   02112374  ADD r0, r4, #0xd4             the ModelAnim
 *   02112378  LDR r2, [r0]                  its vptr
 *   0211237c  ADD r1, r4, #0x80             the Vector3
 *   02112380  LDR r2, [r2, #0x14]           SLOT 5
 *   02112384  BLX r2
 *   02112388  MOV r0, #1
 *
 * The matched source models that dispatch with a local six-virtual shadow and
 * calls its LAST member, which is index 5 in the ROM's own numbering (two
 * destructor slots, Itanium). That is correct against the ROM table and wrong
 * against the host one: hal/cxxname_bridge.cpp fills _ZTV9ModelAnim in MSVC
 * numbering -- dtor 0, DoSetFile 1, UpdateVerts 2, Virtual10 3, Render 4,
 * Virtual18 5 -- and its own comment says so in as many words: "No dual-fill
 * here: Render's ROM slot (5) is Virtual18's MSVC slot, so shadow-TU Render
 * dispatch cannot be served by the same array". _ZTV5Model IS dual-filled for
 * this shape, but +0xd4 on a RacingPenguin is a ModelAnim, not a Model: its
 * deleting destructor runs ModelAnim::~ModelAnim on that offset.
 *
 * So the dispatch is replaced with the direct call it resolves to on the ROM,
 * and nothing else about the body changes. The matched TU stays in src/ and is
 * deliberately NOT on port/slice_w3e_ov019.txt.
 */
#include "ModelAnim.h"

extern "C" {
void _ZN15TextureSequence6UpdateER15ModelComponents(void *self, void *mc);

// PORT_HOST_ABI: mwcc shadow-class dispatch of a ModelAnim's ROM slot 5, which MSVC's ModelAnim table cannot serve.
int _ZN13RacingPenguin6RenderEv(char *self)
{
    _ZN15TextureSequence6UpdateER15ModelComponents(self + 0x138, self + 0xdc);
    ((ModelAnim *)(self + 0xd4))->ModelAnim::Render((const Vector3 *)(self + 0x80));
    return 1;
}
}
