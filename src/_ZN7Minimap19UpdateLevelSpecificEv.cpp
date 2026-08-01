//cpp
#include "types.h"
extern "C" {
extern s8 data_0209f2f8;
extern u8 data_0209f220;
extern u32 data_0209caa0[];
int SublevelToLevel(int i);
int IsStarCollected(int level, int star);
}

namespace G2S {
    void* GetBG3CharPtr();
}

struct Event {
    static int GetBit(unsigned int bit);
};

struct Minimap {
    static void UpdateLevelSpecific();
};

void Minimap::UpdateLevelSpecific()
{
    int state = data_0209f2f8;
    switch (state) {
    case 7: {
        u16* p;
        u16 tile;
        int i;
        if (!IsStarCollected(SublevelToLevel(state), 1)) return;
        if (data_0209f220 < 2) return;
        p = (u16*)((char*)G2S::GetBG3CharPtr() - 0x734);
        tile = 0x39c;
        for (i = 0; i < 4; i++) {
            p[0] = tile;
            p[1] = tile + 1;
            p[2] = tile + 2;
            p[3] = tile + 3;
            p += 0x10;
            tile += 0x20;
        }
        break;
    }
    case 8: {
        u16 tile;
        int i;
        u16* p;
        if (!IsStarCollected(SublevelToLevel(state), 1)) return;
        if (data_0209f220 < 2) return;
        p = (u16*)((char*)G2S::GetBG3CharPtr() - 0x6ea);
        tile = 0x35c;
        for (i = 0; i < 6; i++) {
            p[0] = tile;
            p[1] = tile + 1;
            p[2] = tile + 2;
            p[3] = tile + 3;
            p += 0x10;
            tile += 0x20;
        }
        break;
    }
    case 0x12: {
        u16 tile;
        int i;
        u16* p;
        if (!(data_0209caa0[1] & 0x204)) return;
        p = (u16*)((char*)G2S::GetBG3CharPtr() - 0x748);
        tile = 0x35d;
        for (i = 0; i < 6; i++) {
            p[0] = tile;
            p[1] = tile + 1;
            p[2] = tile + 2;
            p += 0x10;
            tile += 0x20;
        }
        break;
    }
    case 1: {
        u16 tile;
        int i;
        u16* p;
        if (!(data_0209caa0[2] & 0x80000)) return;
        p = (u16*)((char*)G2S::GetBG3CharPtr() - 0x73a);
        tile = 0x396;
        for (i = 0; i < 4; i++) {
            p[0] = tile;
            p[1] = tile + 1;
            p[2] = tile + 2;
            p[3] = tile + 3;
            p[4] = tile + 4;
            p[5] = tile + 5;
            p[6] = tile + 6;
            p[7] = tile + 7;
            p[8] = tile + 8;
            p[9] = tile + 9;
            p += 0x10;
            tile += 0x20;
        }
        break;
    }
    case 4: {
        u16 tile;
        int i;
        u16* p;
        if (!(data_0209caa0[2] & 0x80000)) return;
        p = (u16*)((char*)G2S::GetBG3CharPtr() - 0x1ca4);
        tile = 0x38e;
        for (i = 0; i < 4; i++) {
            p[0] = tile;
            p[1] = tile + 1;
            p[2] = tile + 2;
            p[3] = tile + 3;
            p[4] = tile + 4;
            p[5] = tile + 5;
            p[6] = tile + 6;
            p[7] = tile + 7;
            p[8] = tile + 8;
            p[9] = tile + 9;
            p[10] = tile + 10;
            p[11] = tile + 11;
            p[12] = tile + 12;
            p[13] = tile + 13;
            p[14] = tile + 14;
            p[15] = tile + 15;
            p[16] = tile + 16;
            p[17] = tile + 17;
            p += 0x20;
            tile += 0x20;
        }
        break;
    }
    case 0x10: {
        u16 tile;
        int i;
        u16* p;
        if (!Event::GetBit(0xe)) return;
        p = (u16*)((char*)G2S::GetBG3CharPtr() - 0x736);
        tile = 0x3de;
        for (i = 0; i < 2; i++) {
            p[0] = tile;
            p[1] = tile + 1;
            p += 0x10;
            tile += 0x20;
        }
        break;
    }
    case 0x18: {
        u16 tile;
        int i;
        u16* p;
        if (!Event::GetBit(0xe)) return;
        p = (u16*)((char*)G2S::GetBG3CharPtr() - 0x752);
        tile = 0x3be;
        for (i = 0; i < 3; i++) {
            p[0] = tile;
            p[1] = tile + 1;
            p += 0x10;
            tile += 0x20;
        }
        break;
    }
    case 0x19: {
        u16* p;
        if (!Event::GetBit(0xe)) return;
        p = (u16*)(((int)G2S::GetBG3CharPtr() - 0x712) & 0xFFFFFFFFFFFFFFFFULL);
        p[0] = 0x3fe;
        p[1] = 0x3ff;
        break;
    }
    }
}
