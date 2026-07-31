//cpp
// @symbol _ZN13UpDownLiftBbh8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "UpDownLiftBbh.h"
struct Plat;
typedef void (Plat::*PMF)();
extern "C" PMF data_ov095_02137910[];

extern "C" void* _ZN5Actor13ClosestPlayerEv(void* c);
extern "C" void _ZN8Platform21UpdateModelPosAndRotYEv(void* c);
extern "C" int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void* c, int a, int b);
extern "C" void _ZN8Platform19UpdateClsnPosAndRotEv(void* c);
extern "C" int _ZN6Player7IsInAirEv(void* c);

typedef unsigned char u8;
typedef unsigned short u16;

int UpDownLiftBbh::Behavior()
{
    int old;
    *(void**)((char*)&unk_324) = _ZN5Actor13ClosestPlayerEv(((char*)this));
    old = mState;
    (((Plat*)((char*)this))->*data_ov095_02137910[old])();
    *(u16*)(((int)((char*)this) + 0x344) & 0xFFFFFFFFFFFFFFFF) += 1;
    if (old != mState) *(u16*)(((char*)&unk_300) + 0x44) = 0;
    _ZN8Platform21UpdateModelPosAndRotYEv(((char*)this));
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0, 0) != 0)
        _ZN8Platform19UpdateClsnPosAndRotEv(((char*)this));
    if (mState == 0 || (unsigned)(mState - 3) <= 1) {
        void* pl = *(void**)((char*)&unk_320);
        if (pl != 0) {
            if (_ZN6Player7IsInAirEv(pl) == 0) {
                if (unk_348 == 0) {
                    unk_320 = 0;
                    unk_347 = 1;
                }
            }
        }
    }
    unk_348 = 0;
    if (*(void**)((char*)&unk_324) != 0)
        unk_330 = *(int*)(*(char**)((char*)&unk_324) + 0x60);
    return 1;
}
