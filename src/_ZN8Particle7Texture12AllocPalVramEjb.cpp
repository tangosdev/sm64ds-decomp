//cpp
// @symbol _ZN8Particle7Texture12AllocPalVramEjb
#include "Particle__Texture.h"

extern u32 data_0209ee84;
extern u32 data_0209ee8c;

u32 Particle::Texture::AllocPalVram(u32 size, bool fromLowAddress)
{
    if (fromLowAddress) {
        u32 alignedSize = (size + 7) & ~7;
        u32 address = data_0209ee84;
        data_0209ee84 = address + alignedSize;
        return address;
    }

    u32 alignedSize = (size + 15) & ~15;
    u32 address = data_0209ee8c - alignedSize;
    data_0209ee8c = address;
    return address;
}
