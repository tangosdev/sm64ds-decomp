#include "types.h"
/* Both angle bumps read +0x8e through a const view: `+= 0x8000` makes mwcc
   CSE the c+0x8e address into r2 (add r2,r0,#0x8e / ldrsh r1,[r2] /
   strh r1,[r2]); the cartridge folds it (ldrsh r1,[r0,#0x8e] / strh
   r1,[r0,#0x8e]). Same lever as dScMgPachinko2_c::OnYoshiTryEat. */
void func_ov002_020c70ac(char* c) {
    if (*(u16*)(c+0x6a4) != 0) {
        *(int*)(c+0xa8) = 0;
        *(int*)(c+0x98) = 0;
        return;
    }
    *(u16*)(c+0x6a6) = 0;
    *(u8*)(c+0x6e5) = 0;
    *(int*)(c+0x9c) = -0x4000;
    switch (*(u8*)(c+0x6e3)) {
    case 12:
        *(s16*)(c+0x8e) = *(const s16*)(c+0x8e) + 0x8000;
    case 11:
    case 18:
        *(int*)(c+0x98) = 0x20000;
        *(u16*)(c+0x6a6) = 6;
        break;
    case 9:
        *(s16*)(c+0x8e) = *(const s16*)(c+0x8e) + 0x8000;
    case 8:
    case 16:
        *(int*)(c+0xa8) = 0x40000;
        *(int*)(c+0x98) = 0x18000;
        break;
    case 13:
        *(int*)(c+0xa8) = 0x36000;
        *(int*)(c+0x98) = 0xa000;
        *(u8*)(c+0x70c) = 0;
        break;
    case 17:
        *(int*)(c+0xa8) = 0x3c000;
        *(int*)(c+0x98) = 0xa000;
        *(u8*)(c+0x70c) = 0;
        break;
    }
}
