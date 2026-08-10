//cpp
// @symbol _ZN9ShipWater8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "ShipWater.h"
extern "C" {
extern char* _ZN5Actor15FindWithActorIDEjPS_(unsigned int id, char* prev);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int cc, void* v, unsigned int e);
extern int _ZN9Animation7AdvanceEv(char* t);
extern int _ZN8Platform21UpdateModelPosAndRotYEv(char* t);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(char* t);
}

int ShipWater::Behavior()
{
    if (unk_338 == 0) {
        int ok = 1;
        char* p = _ZN5Actor15FindWithActorIDEjPS_(0xd, 0);
        while (p != 0) {
            int state = *(int*)(p+0x16c);
            int cond = 1;
            if (state != 1) {
                if (state != 2) cond = 0;
            }
            if (cond == 0) ok = 0;
            p = _ZN5Actor15FindWithActorIDEjPS_(0xd, p);
        }
        if (ok != 0) unk_338 = 1;
    } else {
        int d = unk_334 - mPosY;
        if (d < 0) d = -d;
        if (d < 0x92e000) {
            int* q;
            unk_33c = _ZN5Sound8PlayLongEjjjRK7Vector3s(unk_33c, 3, 0x96, ((char*)this)+0x74, 0);
            q = (int*)((char*)&mPosY);
            *q -= 0x5000;
        }
    }
    unk_32c = 0x1000;
    _ZN9Animation7AdvanceEv((char*)&mTextureTransformer);
    _ZN8Platform21UpdateModelPosAndRotYEv(((char*)this));
    _ZN8Platform19UpdateClsnPosAndRotEv(((char*)this));
    return 1;
}
