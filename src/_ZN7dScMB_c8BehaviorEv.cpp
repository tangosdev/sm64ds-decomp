//cpp
// @symbol _ZN7dScMB_c8BehaviorEv
/* recovered: named members + real C++ method */
/* dScMB_c::Behavior() -- vtable slot 6. An eight-step state machine on
 * unk_060 (each step falls through to the next once its wait clears), with
 * unk_064 counting frames until a 0x1518-frame timeout gives up. */
#include "dScMB_c.h"
#include "decl_common.h"

extern "C" {
void DecompressLZ16(void *src, int dst);
int func_0201a244(int a0, int a1, int a2, int a3, int a4);
void func_020308b4(void);
void _ZN8dScene_c14StartSceneFadeEjjt(u32 a, u32 b, u16 c);
void func_02012790(int x);

extern u8 data_0209d45c;
extern u8 data_0209d454;
}

s32 dScMB_c::Behavior()
{
    switch (unk_060) {
    case 0:
        if (func_0203d8fc() == 0) break;
        DecompressLZ16(data_0208a0e4[func_0200f0bc()], 0x6400000);
        DecompressLZ16(data_0208a0e4[func_0200f0bc()], 0x6600000);
        data_0209d45c = 0x10;
        data_0209d454 = 0x10;
        *(volatile u32 *)0x4000000 = (*(volatile u32 *)0x4000000 & ~0x1f00) | 0x1000;
        *(volatile u32 *)0x4001000 = (*(volatile u32 *)0x4001000 & ~0x1f00) | 0x1000;
        unk_060++;
        break;

    case 1:
        {
            int r = func_0201a1bc();
            *(int *)&data_020a0c64 = r;
            if (r == 0) break;
        }
        unk_060++;
        /* fallthrough */
    case 2:
        unk_060++;
        /* fallthrough */
    case 3:
        unk_060++;
        /* fallthrough */
    case 4:
        func_0201a244((int)&func_02034fbc, 0, 0xf, 0, 0x1000);
        unk_060++;
        /* fallthrough */
    case 5:
        if (func_0201a1bc() == 0) break;
        unk_060++;
        /* fallthrough */
    case 6:
        if (func_0203d7b8() != 0) break;
        func_0200f220();
        func_0200f13c();
        func_0203d930();
        func_020308b4();
        unk_060++;
        /* fallthrough */
    case 7:
        if (func_020308a8() == 0) break;
        _ZN8dScene_c14StartSceneFadeEjjt(6, 0, 0x7fff);
        func_02012790(0x11f);
        unk_060++;
        break;
    }

    if (unk_060 < 8) {
        unk_064++;
        if (unk_064 >= 0x1518) {
            data_0209fc54 = 1;
        }
    }

    return 1;
}
