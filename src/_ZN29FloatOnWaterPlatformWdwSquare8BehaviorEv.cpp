//cpp
#include "types.h"
// @symbol _ZN29FloatOnWaterPlatformWdwSquare8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "FloatOnWaterPlatformWdwSquare.h"
extern void func_02012694(int a, void* p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, void* clsn);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void* self);
extern int _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(void* self, int a, int b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* self);

int FloatOnWaterPlatformWdwSquare::Behavior()
{
    switch (((u8*)this)[0x327]) {
    case 0:
        if (((u8*)this)[0x326] != 0) {
            func_02012694(0x17b, ((u8*)this) + 0x74);
            {
                u8* p = (u8*)(((int)((u8*)this) + 0x327));
                *p = *p + 1;
            }
        }
        unk_098 = 0xa000;
        break;
    case 1:
    case 3:
        _ZN5Actor9UpdatePosEP12CylinderClsn(((u8*)this), 0);
        {
            int* p = (int*)(((int)((u8*)this) + 0x320));
            *p = *p + unk_098;
        }
        if (unk_320 >= 0x177000) {
            unk_320 = 0;
            {
                u8* p = (u8*)(((int)((u8*)this) + 0x327));
                *p = *p + 1;
            }
            {
                u8* base = (u8*)(((int)((u8*)this) + 0x300));
                *(u16*)(base + 0x24) = 0;
            }
        }
        break;
    case 2:
    case 4:
        if (*(u16*)((u8*)(((unsigned)((u8*)this) + 0x300)) + 0x24) >= 0x3c) {
            {
                s16* p = (s16*)(((int)((u8*)this) + 0x94));
                *p = *p + 0x8000;
            }
            if (((u8*)this)[0x327] == 4) {
                ((u8*)this)[0x327] = 0;
            } else {
                {
                    u8* p = (u8*)(((int)((u8*)this) + 0x327));
                    *p = *p + 1;
                }
                func_02012694(0x17b, ((u8*)this) + 0x74);
            }
            {
                u8* b2 = (u8*)(((int)((u8*)this) + 0x300));
                *(u16*)(b2 + 0x24) = 0;
            }
        } else {
            u16* p = (u16*)(((int)((u8*)this) + 0x324));
            *p = *p + 1;
        }
        break;
    }

    _ZN8Platform21UpdateModelPosAndRotYEv(((u8*)this));
    if (_ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(((u8*)this), 0x100000, 0) != 0) {
        _ZN8Platform19UpdateClsnPosAndRotEv(((u8*)this));
    }
    ((u8*)this)[0x326] = 0;
    return 1;
}
