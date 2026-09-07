//cpp
// @symbol _ZN8dActor_c15IsPlayerInRangeE5Fix12IiES1_S1_i
/* recovered: named members + shared header, real C++ method */
#include "dActor_c.h"
#include "Player.h"

extern "C" int Vec3_Dist(const Vector3 *a, const Vector3 *b);

/* Stays a mangled free definition: the real signature carries Fix12<int> and
   wall 6az (notes/mwccarm-codegen.md) homes class-typed by-value parameters.
   The declaration in dActor_c.h is the real one and callers may use it. */
extern "C" int _ZN8dActor_c15IsPlayerInRangeE5Fix12IiES1_S1_i(
    dActor_c *self, int posX, int posY, int posZ, int maxDist)
{
    Vector3 pos;
    pos.x = posX;
    pos.y = posY;
    pos.z = posZ;
    Player *closest = self->ClosestPlayer();
    return Vec3_Dist(&pos, (const Vector3 *)&closest->mPosX) < (maxDist << 12);
}
