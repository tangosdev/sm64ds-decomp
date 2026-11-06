//cpp
#include "OAM.h"

extern "C" {

typedef volatile unsigned int vu32;

typedef struct UnkObj UnkObj;
typedef struct UnkVt {
    void *pad0;
    void *pad4;
    int (*func8)(UnkObj *);
} UnkVt;
struct UnkObj {
    UnkVt *vt;
};

extern UnkObj *data_0209d4a8;
extern unsigned char gMainScreenLayers;
extern unsigned char gSubScreenLayers;
extern short gBg0HOffset;
extern short gBg0VOffset;
extern short gBg1HOffset;
extern short gBg1VOffset;
extern short gBg2HOffset;
extern short gBg2VOffset;
extern short gBg3HOffset;
extern short gBg3VOffset;
extern short gSubBg0HOffset;
extern short gSubBg0VOffset;
extern short gSubBg1HOffset;
extern short gSubBg1VOffset;
extern short gSubBg2HOffset;
extern short gSubBg2VOffset;
extern short gSubBg3HOffset;
extern short gSubBg3VOffset;

void func_02055454(int v);

/* The VBlank IRQ's display commit (IRQ2.cpp). The scene's GraphCallback2
   (data_0209d4a8) can skip it by returning 0. Otherwise flush and load
   OAM, put the layer enables (DISPCNT bits 8-12) from gMainScreenLayers on
   the main screen and gSubScreenLayers on the sub screen, and write both
   screens' BG scroll registers from their shadow words (each H/V pair is
   set by its SetBgNOffset or SetSubBgNOffset). With BG0 in 3D
   mode (DISPCNT bit 3) its horizontal offset goes through
   G3X_SetHOffset (func_02055454) instead. */
void func_02019144(void)
{
    UnkObj *p = data_0209d4a8;
    if (p != 0) {
        if (p->vt->func8(p) == 0)
            return;
    }
    OAM::Flush();
    OAM::Load();
    *(vu32 *)0x4000000 = (*(vu32 *)0x4000000 & ~0x1f00) | (gMainScreenLayers << 8);
    *(vu32 *)0x4001000 = (*(vu32 *)0x4001000 & ~0x1f00) | (gSubScreenLayers << 8);
    if (!(*(vu32 *)0x4000000 & 8)) {
        *(vu32 *)0x4000010 = (gBg0HOffset & 0x1ff) | (0x1ff0000 & (gBg0VOffset << 16));
    } else {
        func_02055454(gBg0HOffset);
    }
    *(vu32 *)0x4000014 = (gBg1HOffset & 0x1ff) | (0x1ff0000 & (gBg1VOffset << 16));
    *(vu32 *)0x4000018 = (gBg2HOffset & 0x1ff) | (0x1ff0000 & (gBg2VOffset << 16));
    *(vu32 *)0x400001c = (gBg3HOffset & 0x1ff) | (0x1ff0000 & (gBg3VOffset << 16));
    *(vu32 *)0x4001000;
    *(vu32 *)0x4001010 = (gSubBg0HOffset & 0x1ff) | (0x1ff0000 & (gSubBg0VOffset << 16));
    *(vu32 *)0x4001014 = (gSubBg1HOffset & 0x1ff) | (0x1ff0000 & (gSubBg1VOffset << 16));
    *(vu32 *)0x4001018 = (gSubBg2HOffset & 0x1ff) | (0x1ff0000 & (gSubBg2VOffset << 16));
    *(vu32 *)0x400101c = (gSubBg3HOffset & 0x1ff) | (0x1ff0000 & (gSubBg3VOffset << 16));
}
}
