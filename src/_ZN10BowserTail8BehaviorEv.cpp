//cpp
// @symbol _ZN10BowserTail8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header
 *
 * The tail follows Bowser: find him by the uniqueID stashed in unk_108, then park
 * this actor 0x8c units out from his position along his facing angle.
 * data_02082214 is a sin/cos table indexed by angle>>4, two shorts per entry.
 *
 * The one-line `struct Actor { static Actor* FindWithID(unsigned int); };` stand-in
 * this file used to carry is gone -- Actor.h already declared FindWithID, so it was
 * never needed.
 *
 * THE POINTER BUMP AND THE volatile ARE LOAD-BEARING, both measured. Reading
 * Bowser's fields the obvious way -- `bowser->mPrevAngleY`, `bowser->mPosX` and so
 * on, which the real Actor now makes possible -- compiles and does not reproduce
 * the ROM. The bump to +0x5c and the three loads off it are what the original
 * source did, and the offsets are Actor's: 0x94 is mPrevAngleY, 0x5c..0x64 are
 * mPosX/mPosY/mPosZ.
 */
#include "decl_common.h"
#include "BowserTail.h"

extern short data_02082214[];

int BowserTail::Behavior()
{
    Actor* a = Actor::FindWithID(unk_108);
    if (!a) return 1;

    int ang = *(short*)((char*)a + 0x94);        /* Actor::mPrevAngleY */
    a = (Actor*)((int)a + 0x5c);                 /* Actor::mPosX */
    int x = *(int*)a;
    volatile int v[3];
    v[0] = x;
    v[1] = ((int*)a)[1];                         /* mPosY */
    v[2] = ((int*)a)[2];                         /* mPosZ */
    int j = 2 * (((unsigned short)(short)(ang + 0x8000)) >> 4);
    mPosX = (short)data_02082214[j] * 0x8c + x;
    mPosY = v[1];
    mPosZ = (short)data_02082214[j + 1] * 0x8c + v[2];

    func_ov060_02115b84(((char*)this));
    return 1;
}
