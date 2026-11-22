#include "Ov007Touch.h"
#include "Ov007MenuMem.h"

extern char data_ov007_02103240[]; /* "touch_panel" */
extern void func_ov007_020c2024(Ov007Touch *t);

/* Builds the touch-panel record (include/Ov007Touch.h) from the heap at
 * data_ov007_02103240. */
Ov007Touch *func_ov007_020c2068(void) {
    Ov007Touch *t = func_ov007_020c3df4(data_ov007_02103240, sizeof(Ov007Touch));
    func_ov007_020c2024(t);
    return t;
}

/* Clears the touch record's counters, positions and flags; the samples are
 * left for func_ov007_020c1db0's first reading. */
void func_ov007_020c2024(Ov007Touch *t) {
    t->heldFrames = 0;
    t->releasedFrames = 0;
    t->countdown = 0;
    t->pressY = 0;
    t->pressX = t->pressY;
    t->releaseY = 0;
    t->releaseX = t->releaseY;
    t->dragY = 0;
    t->dragX = t->dragY;
    t->flags = 0;
    t->pad_40 = -1;
}

char data_ov007_02103240[] = {
    0x74, 0x6f, 0x75, 0x63, 0x68, 0x5f, 0x70, 0x61, 0x6e, 0x65,
    0x6c, 0x00, 0x70, 0x61, 0x64, 0x00, 0x32, 0x54, 0x76, 0x98,
};
