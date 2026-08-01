#include "types.h"
/* _ZN5Actor11UntrackStarERa at 0x0200ff60
 * Actor::UntrackStar(s8& starID)
 * If starID >= 0, calls SetStarMarker to clear it, then sets starID = -1.
 */
struct Actor;

extern void SetStarMarker(s32 id, struct Actor *starMarker, u8 type); /* 0x0202a660 */

void _ZN5Actor11UntrackStarERa(struct Actor *thiz, s8 *starID)
{
    s8 id;
    id = *starID;
    if (id < 0)
        return;
    SetStarMarker((s32)id, (struct Actor *)0, 2);
    *starID = -1;
}
