//cpp
// @symbol _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs
/* recovered: named members + shared header, real C++ method */
#include "dActor_c.h"

extern "C" int RandomIntInternal(int *seed);
extern int data_0209e650[];

/* Stays a mangled free definition: the real signature carries Fix12<int> and
   wall 6az (notes/mwccarm-codegen.md) homes class-typed by-value parameters.
   The declaration in dActor_c.h is the real one and callers may use it. */
extern "C" void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(
    dActor_c *self, const Vector3 *pos, int count, int spread, short angle)
{
    dActor_c *actor;
    int ang;
    int prev;
    int i;
    u32 r;
    short ang0;
    const Vector3_16 *rot;
    short deathId;

    ang0 = angle;
    rot = 0;
    deathId = -1;
    prev = 0xff;

    if (count > 1) {
        if (spread < 0x4000)
            spread = 0x4000;
    }

    for (i = 0; i < count; i++) {
        actor = dActor_c::Spawn(0x120, 2, *pos, rot, self->mAreaId, deathId);

        if (actor != 0) {
            do {
                ang = (int)(((u32)RandomIntInternal(data_0209e650) >> 16) << 27) >> 16;
            } while (ang == prev);

            r = (u32)RandomIntInternal(data_0209e650);
            r >>= 16;

            actor->mPrevAngleX = 0;
            spread = (u32)((u32)spread * ((r % 50) + 100)) / 100;
            prev = ang;
            actor->mPrevAngleY = (short)(ang0 + ang);
            actor->mPrevAngleZ = 0;
            actor->mHorzSpeed = spread;
        }
    }
}
