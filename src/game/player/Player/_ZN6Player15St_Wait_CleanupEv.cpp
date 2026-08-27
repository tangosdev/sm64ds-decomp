//cpp
// @symbol _ZN6Player15St_Wait_CleanupEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern unsigned char data_0209f2d8;
extern void* data_0209f318;
extern int data_0209caa0[];
}

int Player::St_Wait_Cleanup()
{
    void* r4 = data_0209f318;
    (*(unsigned int*)(((int)r4 + 0x154))) &= ~0x2000;
    mSleepStage = 0;
    do {
        unsigned char v = data_0209f2d8;
        int b0 = (v == 1);
        if (b0 == 0) {
            if ((data_0209caa0[2] & 0x80) != 0) {
                int b1 = (v == 2);
                if (b1 == 0) break;
            }
            if (unk_727 == 0xf) unk_743 = 0;
            unk_727 = 0;
            unk_728 = 0;
        }
    } while (0);
    return 1;
}
