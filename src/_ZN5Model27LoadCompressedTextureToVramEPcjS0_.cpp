//cpp
// @symbol _ZN5Model27LoadCompressedTextureToVramEPcjS0_
/* recovered: Model::LoadCompressedTextureToVram, the compressed-texture upload.
 *
 * It uploads two blocks - the compressed texel data at `src` for `size` bytes,
 * then the palette-index block at `dst` for half that - and bumps the two VRAM
 * cursors past them. It RETURNS the pre-bump texel cursor, the offset this
 * texture just landed at, exactly as its sibling LoadTextureToVram does, and
 * include/Model.h has always declared it `static u32`.
 *
 * That return is the whole match. The draft this replaces was typed `void`, and
 * with the value dead at the exit mwccarm reused the loaded register in place
 * (add r4,r4,r5) and batched both adds before both stores. Keeping the pre-bump
 * value live to the return forces the cartridge's shape at +0x88: ldr r0,[r2] /
 * ldr r3,[r1] / add r4,r0,r5 / str r4,[r2] / add r2,r3,r5,lsr#1 / str r2,[r1],
 * with the sum in a fresh r4 because r0 has to survive to the exit. The old
 * header called those five words an allocator build delta from PR #957; they
 * were a missing return value, and every caller in the tree already knew it -
 * func_ov075_0211aa94 and func_ov080_02125630 both store the result, and
 * port/hal/gx_upload_bridge.cpp re-reads data_020a4bc8 before the call to
 * reconstruct by hand what the function was already returning.
 *
 * The `volatile` on the two cursor bumps went with it. It was pinning
 * load-add-store twice where the cartridge emits load, load, add, str, add, str,
 * so it cost more than it bought.
 */
#include "Model.h"
extern "C" extern u32 data_020a4be8;
extern "C" extern u32 data_020a4bc8;
extern "C" extern u32 data_020a4be4;
extern "C" extern u32 data_020a4be0;
extern "C" void Crash();

struct GX {
    static void BeginLoadTex();                           /* _ZN2GX12BeginLoadTexEv */
    static void LoadTex(const void *src, u32 offset, u32 size);
    static void EndLoadTex();
};

#pragma opt_common_subs off
u32 Model::LoadCompressedTextureToVram(char *src, u32 size, char *dst)
{
    u32 vramOffset;

    if ((data_020a4be8 - data_020a4bc8) < size)
        Crash();
    GX::BeginLoadTex();
    GX::LoadTex(src, data_020a4bc8, size);
    data_020a4be4 += size;
    GX::LoadTex(dst, data_020a4be0, size >> 1);
    data_020a4be4 += size >> 1;
    GX::EndLoadTex();
    vramOffset = data_020a4bc8;
    data_020a4bc8 += size;
    data_020a4be0 += size >> 1;
    return vramOffset;
}
