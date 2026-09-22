//cpp
// @symbol _ZN8Particle7Texture12AllocTexVramEjb
#include "Model.h"
#include "Particle__Texture.h"

extern u32 data_0209ee88;

u32 Particle::Texture::AllocTexVram(u32 size, bool isTexel4x4)
{
    if (isTexel4x4) {
        u32 address = data_0209ee88;
        data_0209ee88 = address + size;
        return address;
    }

    return Model::GetVramOffset(size);
}
