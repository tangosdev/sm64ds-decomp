//cpp
// @symbol _ZN7Minimap13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "Minimap.h"
#pragma opt_strength_reduction off
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

extern "C" {
    int func_0202a980(void);
    int LoadFile(int handle);
    void* _ZN3G2S13GetBG3CharPtrEv(void);
    void DecompressLZ16(int src, void* dst);
    void Deallocate(void* ptr);
    int func_0202a96c(void);
    void _ZN3GXS18BeginLoadBGExtPlttEv(void);
    void _ZN3GXS13LoadBGExtPlttEPKvjj(const void* p, u32 a, u32 b);
    void _ZN3GXS16EndLoadBGExtPlttEv(void);
    void _ZN3GXS10LoadBGPlttEPKvjj(const void* p, u32 a, u32 b);
    void* _ZN3G2S12GetBG3ScrPtrEv(void);
    void _ZN7Minimap19UpdateLevelSpecificEv(void);
    int GetMinimapID(void* obj, int arg);
    int SublevelToLevel(int i);
    int GetMinimapScale(int idx);
    int func_0202a958(void);
}

extern u8 data_0209f2e8;
extern u16* data_0209f334;
extern u8 data_0209f2d8;
extern u8 data_0209f250;
extern void* data_0209f394[];
extern signed char data_0209f2f8;
extern signed char data_ov002_02111148;
extern u8 data_0209d454;
extern s32 data_0209caa0[];
extern s32 data_0209fc48;
extern u8 data_ov002_02111150;

int Minimap::InitResources()
{
    u16 *p;
    s32 i;

    *(volatile u16*)0x400100e = (*(volatile u16*)0x400100e & ~3) | 2;
    *(volatile u16*)0x400100e = (*(volatile u16*)0x400100e & 0x43) | 0x1f10;
    *(volatile u16*)0x400100e &= ~0x40;

    {
        int id1 = func_0202a980();
        if (id1 != 0) {
            int f1 = LoadFile(id1);
            DecompressLZ16(f1, _ZN3G2S13GetBG3CharPtrEv());
            Deallocate((void*)f1);
        }
    }

    {
        int id2 = func_0202a96c();
        if (id2 != 0) {
            int f2 = LoadFile(id2);
            _ZN3GXS18BeginLoadBGExtPlttEv();
            _ZN3GXS13LoadBGExtPlttEPKvjj((void*)f2, 0x6000, 0x200);
            _ZN3GXS16EndLoadBGExtPlttEv();
            _ZN3GXS10LoadBGPlttEPKvjj((void*)f2, 0, 2);
            Deallocate((void*)f2);
        }
    }

    p = data_0209f334;
    for (i = 0; i < data_0209f2e8; i++) {
        if (*p != 0) {
            int f3 = LoadFile(*p);
            DecompressLZ16(f3, (char*)_ZN3G2S12GetBG3ScrPtrEv() - (i << 11));
            Deallocate((void*)f3);
        }
        p++;
    }

    {
        int b = (data_0209f2d8 == 1);
        if (!b) {
            _ZN7Minimap19UpdateLevelSpecificEv();
        }
    }

    data_ov002_02111148 = (signed char)GetMinimapID(data_0209f394[data_0209f250], -1);

    if (data_ov002_02111148 >= 0) {
        if ((SublevelToLevel(data_0209f2f8) == 0x1d && data_0209f2f8 != 1 && data_0209f2f8 != 0x33 && data_0209f2f8 != 3)
            || SublevelToLevel(data_0209f2f8) == 4
            || (SublevelToLevel(data_0209f2f8) == 0x13 && data_0209f2f8 == 0x2e)
            || SublevelToLevel(data_0209f2f8) == -1)
        {
            *(volatile u16*)0x400100e = (u16)(((0x1f - data_ov002_02111148) << 8) | ((*(volatile u16*)0x400100e & 0x43) | 0x4010));
            unk_1d8 = 0x100;
            unk_1dc = 0x80;

            if (SublevelToLevel(data_0209f2f8) == 4) {
                unk_1e0 = 0x258000;
                unk_1e4 = 0;
                unk_1e8 = 0x64000;
            } else if (SublevelToLevel(data_0209f2f8) == 0x1d && data_0209f2f8 != 1 && data_0209f2f8 != 0x33 && data_0209f2f8 != 3) {
                unk_1e0 = -0x2bc000;
                unk_1e4 = 0;
                unk_1e8 = -0x2bc000;
            } else {
                SublevelToLevel(data_0209f2f8);
                unk_1e0 = 0;
                unk_1e4 = 0;
                unk_1e8 = 0;
            }
            unk_251 = 1;
        } else {
            *(volatile u16*)0x400100e = (u16)(((0x1f - data_ov002_02111148) << 8) | ((*(volatile u16*)0x400100e & 0x43) | 0x10));
            unk_1d8 = 0x80;
            unk_1dc = 0x40;
            unk_1e0 = 0;
            unk_1e4 = 0;
            unk_1e8 = 0;
            unk_251 = 2;
        }
        data_0209d454 |= 8;
    } else {
        data_0209d454 &= ~8;
    }

    unk_214 = GetMinimapScale(data_ov002_02111148);

    {
        int b1 = (data_0209f2d8 == 0);
        if (b1) {
            if (!(data_0209caa0[2] & 0x80)) {
                int b3 = (data_0209fc48 != 0);
                if (!b3) {
                    unk_218 = (unk_214) << 1;
                    unk_21c = 0;
                    unk_255 = 1;
                    goto unk218_done;
                }
            }
        }
        unk_218 = unk_214;
        unk_255 = 0;
    unk218_done:;
    }

    data_ov002_02111150 = 0;
    unk_050 = 0x1000;
    unk_054 = 0;
    unk_058 = 0;
    mPosX = 0x1000;
    unk_200 = 0x1000;
    unk_204 = 0;
    unk_208 = 0;
    unk_20c = 0x1000;
    unk_1ec = 0;
    unk_210 = 0x1000;
    unk_090 = 0;
    unk_094 = 0;
    unk_098 = 0;
    unk_09c = 0;

    unk_1d4 = func_0202a958();
    unk_1d4 = ((unk_1d8) << 12) / (unk_1d4) / 10;

    return 1;
}
