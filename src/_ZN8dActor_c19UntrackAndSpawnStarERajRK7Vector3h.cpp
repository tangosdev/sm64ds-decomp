//cpp
// @symbol _ZN5Actor19UntrackAndSpawnStarERajRK7Vector3h
/* Actor::UntrackAndSpawnStar(s8 &trackStarID, u32 starID,
 *                            const Vector3 &spawnPos, u8 howToSpawnStar)
 * at 0x0200ff14.
 *
 * Retires this actor's star marker and puts the star itself in the world:
 * actor 0xb2, spawned at spawnPos into this actor's own area. The spawn
 * parameter packs howToSpawnStar into the bits above the star index.
 *
 * A member outright -- r0 is kept in r6 across the UntrackStar call for the
 * sole purpose of loading mAreaId from it afterwards (`ldrsb r1, [r6, #0xcc]`,
 * signed, agreeing with the s8 declared at that offset).
 *
 * THE FOURTH PARAMETER IS 8-BIT, AND THE OLD SYMBOL NAME SAID OTHERWISE.
 * The ROM reads it `ldrb r0, [sp, #0x18]` -- only its low byte ever reaches
 * the `orr`. A u32 parameter cannot produce that load: writing the narrowing
 * as a cast instead, `(u8)howToSpawnStar`, compiles to `ldr` followed by
 * `and r0, r0, #0xff`, which is one instruction too many and lengthens the
 * function to 0x50. Declared u8, the load is `ldrb` and all 0x4c bytes
 * reproduce.
 *
 * So the parameter is u8 and the symbol ends `h`, not `j`. The old spelling
 * was an imported name whose parameter types nothing had ever checked -- a
 * caller cannot observe an over-wide integer parameter, because AAPCS widens
 * it either way, so migrating the definition is the first moment the claim
 * became falsifiable. Renamed here, in symbols.txt, in delinks.txt and in the
 * twelve callers that spell it.
 */
#include "Actor.h"

Actor *Actor::UntrackAndSpawnStar(s8 &trackStarID, u32 starID,
                                  const Vector3 &spawnPos, u8 howToSpawnStar)
{
    UntrackStar(trackStarID);

    return Spawn(0xb2, starID | (howToSpawnStar << 4), spawnPos, 0,
                 mAreaId, -1);
}
