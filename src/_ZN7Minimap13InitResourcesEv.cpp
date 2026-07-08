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

extern u8 MAP_TILE_ARR_SIZE;
extern u16* MAP_TILE_ARR_PTR;
extern u8 CURRENT_GAMEMODE;
extern u8 CURR_PLAYER_ID;
extern void* PLAYER_ARR[];
extern signed char LEVEL_ID;
extern signed char CUR_MINIMAP_ID;
extern u8 BOTTOM_SCREEN_RELATED;
extern s32 SAVE_DATA[];
extern s32 RUNNING_KUPPA_SCRIPT;
extern u8 EXIT_COURSE_VS_MODE;

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

    p = MAP_TILE_ARR_PTR;
    for (i = 0; i < MAP_TILE_ARR_SIZE; i++) {
        if (*p != 0) {
            int f3 = LoadFile(*p);
            DecompressLZ16(f3, (char*)_ZN3G2S12GetBG3ScrPtrEv() - (i << 11));
            Deallocate((void*)f3);
        }
        p++;
    }

    {
        int b = (CURRENT_GAMEMODE == 1);
        if (!b) {
            _ZN7Minimap19UpdateLevelSpecificEv();
        }
    }

    CUR_MINIMAP_ID = (signed char)GetMinimapID(PLAYER_ARR[CURR_PLAYER_ID], -1);

    if (CUR_MINIMAP_ID >= 0) {
        if ((SublevelToLevel(LEVEL_ID) == 0x1d && LEVEL_ID != 1 && LEVEL_ID != 0x33 && LEVEL_ID != 3)
            || SublevelToLevel(LEVEL_ID) == 4
            || (SublevelToLevel(LEVEL_ID) == 0x13 && LEVEL_ID == 0x2e)
            || SublevelToLevel(LEVEL_ID) == -1)
        {
            *(volatile u16*)0x400100e = (u16)(((0x1f - CUR_MINIMAP_ID) << 8) | ((*(volatile u16*)0x400100e & 0x43) | 0x4010));
            *(s32*)(c + 0x1d8) = 0x100;
            *(s32*)(c + 0x1dc) = 0x80;

            if (SublevelToLevel(LEVEL_ID) == 4) {
                *(s32*)(c + 0x1e0) = 0x258000;
                *(s32*)(c + 0x1e4) = 0;
                *(s32*)(c + 0x1e8) = 0x64000;
            } else if (SublevelToLevel(LEVEL_ID) == 0x1d && LEVEL_ID != 1 && LEVEL_ID != 0x33 && LEVEL_ID != 3) {
                *(s32*)(c + 0x1e0) = -0x2bc000;
                *(s32*)(c + 0x1e4) = 0;
                *(s32*)(c + 0x1e8) = -0x2bc000;
            } else {
                SublevelToLevel(LEVEL_ID);
                *(s32*)(c + 0x1e0) = 0;
                *(s32*)(c + 0x1e4) = 0;
                *(s32*)(c + 0x1e8) = 0;
            }
            *(u8*)(c + 0x251) = 1;
        } else {
            *(volatile u16*)0x400100e = (u16)(((0x1f - CUR_MINIMAP_ID) << 8) | ((*(volatile u16*)0x400100e & 0x43) | 0x10));
            *(s32*)(c + 0x1d8) = 0x80;
            *(s32*)(c + 0x1dc) = 0x40;
            *(s32*)(c + 0x1e0) = 0;
            *(s32*)(c + 0x1e4) = 0;
            *(s32*)(c + 0x1e8) = 0;
            *(u8*)(c + 0x251) = 2;
        }
        BOTTOM_SCREEN_RELATED |= 8;
    } else {
        BOTTOM_SCREEN_RELATED &= ~8;
    }

    *(s32*)(c + 0x214) = GetMinimapScale(CUR_MINIMAP_ID);

    {
        int b1 = (CURRENT_GAMEMODE == 0);
        if (b1) {
            if (!(SAVE_DATA[2] & 0x80)) {
                int b3 = (RUNNING_KUPPA_SCRIPT != 0);
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

    EXIT_COURSE_VS_MODE = 0;
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
