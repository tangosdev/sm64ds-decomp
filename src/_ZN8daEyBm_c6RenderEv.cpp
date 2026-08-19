//cpp
// @symbol _ZN14MrI_Projectile6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "MrI_Projectile.h"
extern "C" {
int _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
    unsigned int a, unsigned int b, int c, int d, int e, const void* f);
}

int MrI_Projectile::Render()
{
    unk_328 = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
        unk_328, 0x46, mPosX, mPosY, mPosZ, 0);
    unk_32c = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
        unk_32c, 0x47, mPosX, mPosY, mPosZ, 0);
    return 1;
}
