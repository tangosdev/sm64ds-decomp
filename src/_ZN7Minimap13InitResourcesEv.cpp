//cpp
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

extern "C" int _ZN7Minimap13InitResourcesEv(char *c)
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
            *(s32*)(c + 0x1d8) = 0x100;
            *(s32*)(c + 0x1dc) = 0x80;

            if (SublevelToLevel(data_0209f2f8) == 4) {
                *(s32*)(c + 0x1e0) = 0x258000;
                *(s32*)(c + 0x1e4) = 0;
                *(s32*)(c + 0x1e8) = 0x64000;
            } else if (SublevelToLevel(data_0209f2f8) == 0x1d && data_0209f2f8 != 1 && data_0209f2f8 != 0x33 && data_0209f2f8 != 3) {
                *(s32*)(c + 0x1e0) = -0x2bc000;
                *(s32*)(c + 0x1e4) = 0;
                *(s32*)(c + 0x1e8) = -0x2bc000;
            } else {
                SublevelToLevel(data_0209f2f8);
                *(s32*)(c + 0x1e0) = 0;
                *(s32*)(c + 0x1e4) = 0;
                *(s32*)(c + 0x1e8) = 0;
            }
            *(u8*)(c + 0x251) = 1;
        } else {
            *(volatile u16*)0x400100e = (u16)(((0x1f - data_ov002_02111148) << 8) | ((*(volatile u16*)0x400100e & 0x43) | 0x10));
            *(s32*)(c + 0x1d8) = 0x80;
            *(s32*)(c + 0x1dc) = 0x40;
            *(s32*)(c + 0x1e0) = 0;
            *(s32*)(c + 0x1e4) = 0;
            *(s32*)(c + 0x1e8) = 0;
            *(u8*)(c + 0x251) = 2;
        }
        data_0209d454 |= 8;
    } else {
        data_0209d454 &= ~8;
    }

    *(s32*)(c + 0x214) = GetMinimapScale(data_ov002_02111148);

    {
        int b1 = (data_0209f2d8 == 0);
        if (b1) {
            if (!(data_0209caa0[2] & 0x80)) {
                int b3 = (data_0209fc48 != 0);
                if (!b3) {
                    *(s32*)(c + 0x218) = (*(s32*)(c + 0x214)) << 1;
                    *(u16*)(c + 0x21c) = 0;
                    *(u8*)(c + 0x255) = 1;
                    goto unk218_done;
                }
            }
        }
        *(s32*)(c + 0x218) = *(s32*)(c + 0x214);
        *(u8*)(c + 0x255) = 0;
    unk218_done:;
    }

    data_ov002_02111150 = 0;
    *(s32*)(c + 0x50) = 0x1000;
    *(s32*)(c + 0x54) = 0;
    *(s32*)(c + 0x58) = 0;
    *(s32*)(c + 0x5c) = 0x1000;
    *(s32*)(c + 0x200) = 0x1000;
    *(s32*)(c + 0x204) = 0;
    *(s32*)(c + 0x208) = 0;
    *(s32*)(c + 0x20c) = 0x1000;
    *(s32*)(c + 0x1ec) = 0;
    *(s32*)(c + 0x210) = 0x1000;
    *(s32*)(c + 0x90) = 0;
    *(s32*)(c + 0x94) = 0;
    *(s32*)(c + 0x98) = 0;
    *(s32*)(c + 0x9c) = 0;

    *(s32*)(c + 0x1d4) = func_0202a958();
    *(s32*)(c + 0x1d4) = ((*(s32*)(c + 0x1d8)) << 12) / (*(s32*)(c + 0x1d4)) / 10;

    return 1;
}
