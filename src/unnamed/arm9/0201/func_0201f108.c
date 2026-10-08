#include "types.h"
// func_0201f108 - fills BG0 screen buffer with sequential tile indices 0..0x3ff
extern u16* _ZN3G2S12GetBG0ScrPtrEv(void);

void func_0201f108(void) {
    u16* scr = _ZN3G2S12GetBG0ScrPtrEv();
    int i = 0;
    do {
        scr[i] = (u16)i;
        i++;
    } while (i < 0x400);
}