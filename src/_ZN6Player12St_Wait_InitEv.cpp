//cpp
// @symbol _ZN6Player12St_Wait_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void* c, unsigned int a, int b, int d, unsigned int e);
extern int _ZN6Player9GetHealthEv(void* c);

extern unsigned char data_0209f2d8;
extern int data_0209caa0[];
}

int Player::St_Wait_Init()
{
    unsigned char f2d8;
    int b0;

    unk_6e6 = 0;
    f2d8 = data_0209f2d8;
    b0 = (f2d8 == 1);
    if (b0 != 0) goto L80;
    if ((data_0209caa0[2] & 0x80) == 0) goto L50;
    {
        int b1 = (f2d8 == 2);
        if (b1 == 0) goto L80;
    }
L50:
    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0xb3, 0, 0x1000, 0);
    mStateStep = 0;
    return 1;

L80:
    if (mIsMega == 0) goto Ld8;
    if (b0 != 0) goto Lbc;
    if (data_0209caa0[2] & 0x80) goto Lbc;
    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0xa0, 0, 0x1000, 0);
Lbc:
    mStateTimer = 0x384;
    return 1;

Ld8:
    if (unk_650 == 0x80000000) goto L13c;
    if (mPosY >= unk_650 - 0x64000) goto L13c;
    if (mIsMetal != 0) goto L13c;
    if (unk_6fb != 0) goto L13c;
    mStateStep = 0xa;
    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 5, 0, 0x1000, 0);
    return 1;

L13c:
    mStateStep = 0;
    if (mSinkDepth > 0x32000) {
        _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x7d, 0, 0x1000, 0);
    } else if (_ZN6Player9GetHealthEv(((char*)this)) <= 2) {
        mStateStep = 7;
        _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0xe, 0x40000000, 0x1000, 0);
    } else {
        _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x47, 0, 0x1000, 0);
    }
    mStateTimer = 0x384;
    return 1;
}
