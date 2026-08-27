//cpp
// @symbol _ZN8dActor_c18ClosestWithActorIDEj
/* dActor_c::ClosestWithActorID(u32 actorID) at 0x020108c4.
 *
 * Walks every live actor carrying `actorID` and returns whichever is nearest
 * to this one, skipping this actor itself. Null when the list holds nobody
 * else with that ID.
 *
 * A member: `this` is stashed in r9 at entry and used twice -- once as the
 * identity to skip (`cmp r7, sb`), and once for the position every distance is
 * measured from (`add r4, sb, #0x5c`, hoisted out of the loop).
 *
 * The sentinel is 0x7fffffff, materialised as `mvn r5, #0x80000000`, and the
 * comparison is a signed `cmp`/`movlt` pair -- a distance is a Fix12i, not an
 * unsigned magnitude.
 */
#include "dActor_c.h"

extern "C" Fix12i Vec3_Dist(const Vector3 *a, const Vector3 *b); /* 0x0203cfdc */

dActor_c *dActor_c::ClosestWithActorID(u32 actorID)
{
    dActor_c *actor = 0;
    dActor_c *closest = 0;
    Fix12i closestDist = 0x7fffffff;

    for (;;) {
        actor = FindWithActorID(actorID, actor);
        if (!actor)
            break;

        if (actor != this) {
            Fix12i dist = Vec3_Dist((const Vector3 *)&mPosX,
                                    (const Vector3 *)&actor->mPosX);
            if (dist < closestDist) {
                closestDist = dist;
                closest = actor;
            }
        }
    }
    return closest;
}
