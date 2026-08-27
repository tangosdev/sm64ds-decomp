//cpp
// @symbol _ZN5Model17LoadTextureToVramEPcj
#include "Model.h"
extern "C" extern u32 data_020a4be4;

struct GX {
    static void BeginLoadTex();                           /* _ZN2GX12BeginLoadTexEv */
    static void LoadTex(const void *src, u32 offset, u32 size);
    static void EndLoadTex();
};

u32 Model::LoadTextureToVram(char *texData, u32 size)
{
    u32 vramOffset = GetVramOffset(size);
    GX::BeginLoadTex();
    GX::LoadTex(texData, vramOffset, size);
    data_020a4be4 += size;
    GX::EndLoadTex();
    return vramOffset;
}
