//cpp
// @symbol _ZN10BowserTail8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BowserTail.h"
struct Actor { static Actor* FindWithID(unsigned int); };
extern short data_02082214[];

int BowserTail::Behavior()
{
    Actor* a = Actor::FindWithID(unk_108);
    if (!a) return 1;

    int ang = *(short*)((char*)a + 0x94);
    a = (Actor*)((int)a + 0x5c);
    int x = *(int*)a;
    volatile int v[3];
    v[0] = x;
    v[1] = ((int*)a)[1];
    v[2] = ((int*)a)[2];
    int j = 2 * (((unsigned short)(short)(ang + 0x8000)) >> 4);
    mPosX = (short)data_02082214[j] * 0x8c + x;
    mPosY = v[1];
    mPosZ = (short)data_02082214[j + 1] * 0x8c + v[2];

    func_ov060_02115b84(((char*)this));
    return 1;
}
