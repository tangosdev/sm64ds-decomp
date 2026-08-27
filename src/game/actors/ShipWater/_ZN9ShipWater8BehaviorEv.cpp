//cpp
// @symbol _ZN9ShipWater8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "ShipWater.h"
extern "C" {
extern char* _ZN8dActor_c15FindWithActorIDEjPS_(unsigned int id, char* prev);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int cc, void* v, unsigned int e);
extern int _ZN9Animation7AdvanceEv(char* t);
extern int _ZN10dBgActor_c21UpdateModelPosAndRotYEv(char* t);
extern int _ZN10dBgActor_c19UpdateClsnPosAndRotEv(char* t);
}

int ShipWater::Behavior()
{
    if (mChestsOpen == 0) {
        int ok = 1;
        char* p = _ZN8dActor_c15FindWithActorIDEjPS_(0xd, 0);
        while (p != 0) {
            int state = *(int*)(p+0x16c);
            int cond = 1;
            if (state != 1) {
                if (state != 2) cond = 0;
            }
            if (cond == 0) ok = 0;
            p = _ZN8dActor_c15FindWithActorIDEjPS_(0xd, p);
        }
        if (ok != 0) mChestsOpen = 1;
    } else {
        int d = mOriginalPosY - mPosY;
        if (d < 0) d = -d;
        if (d < 0x92e000) {
            int* q;
            mSoundID = _ZN5Sound8PlayLongEjjjRK7Vector3s(mSoundID, 3, 0x96, ((char*)this)+0x74, 0);
            q = (int*)((char*)&mPosY);
            *q -= 0x5000;
        }
    }
    (*(s32 *)((char *)&mTextureTransformer + 0xc)) = 0x1000;
    _ZN9Animation7AdvanceEv((char*)&(*(u8 *)&mTextureTransformer));
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char*)this));
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char*)this));
    return 1;
}
