//cpp
// @symbol _ZN4Coin16CleanupResourcesEv
/* Coin::CleanupResources -- vtable slot 3. Releases the files this coin's own
 * type loaded, gives up its star-tracking slot, decrements the live-coin count
 * on the puzzle manager that spawned it, and -- unless it is disappearing on a
 * timer -- puts up the collection sparkle a little above itself. */
#include "Coin.h"

typedef int Fix12i;

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(char *p);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int a, Fix12i x, Fix12i y, Fix12i z);
extern char data_ov002_0210d9a8;
extern char *data_ov002_020ff06c[];
extern char *data_ov002_020ff060[];
}

s32 Coin::CleanupResources()
{
    dActor_c *o;
    int b = (int)(actorID == 0x121);
    if (b != 0) _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210d9a8);
    if (mCoinType == 2) {
        _ZN13SharedFilePtr7ReleaseEv(data_ov002_020ff06c[mCoinType]);
        _ZN13SharedFilePtr7ReleaseEv(data_ov002_020ff060[mCoinType]);
    }
    UntrackStar(mTrackStarID);
    o = dActor_c::FindWithID(mPuzzleManagerID);
    if (o != 0) {
        int b2 = (int)(o->actorID == 0x4f);
        if (b2 != 0) {
            /* +0xd6 is the puzzle manager's own live-coin count; no header
               describes that class yet, so the offset stays raw. */
            if (*(unsigned char *)((char *)o + 0xd6) != 0) {
                unsigned char *p = (unsigned char *)((int)o + 0xd6);
                *p = *p - 1;
            }
        }
    }
    /* THE ROM READS THIS FIELD UNSIGNED (`ldrh`, not `ldrsh`), and the legacy C
       form spelled it `*(unsigned short*)(c + 0x3a8)`. include/Coin.h types it
       s16, so the cast here is what keeps the load faithful; the header may
       simply have the sign wrong, but changing it is a separate question from
       this migration. Dropping the cast is the one word this function misses. */
    if ((u16)mDisappearTimer != 0) return 1;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd2, mPosX, mPosY + 0x28000, mPosZ);
    return 1;
}
