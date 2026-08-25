//cpp
#include "Player.h"
#include "types.h"
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN6Player7SetAnimEji5Fix12IiEj(void *, unsigned int animID, int a, int b, unsigned int c);


extern "C" {
    void func_ov002_020c2f64(void* c);
    int func_ov002_020e0a64(char* c);
    void func_ov002_020e25d4(char* c);
    int func_ov002_020e2c84(char* c);
    void func_ov002_020bf9d4(char* c);
}

extern int data_ov002_0211019c;
extern int data_ov002_021101b4;
extern int data_ov002_021101e4;
extern int data_ov002_0211052c;
extern int data_ov002_02110724;
extern int data_ov002_0211007c;
extern int data_ov002_0211055c;
extern int data_ov002_02110454;
extern unsigned char data_020a0e40[1];
extern char data_0209f4a0[];

#pragma opt_common_subs off
int Player::St_Land_Init()
{
    char* c = (char*)this;
    int r4;

    func_ov002_020c2f64(c);
    r4 = *(int*)(c + 0x374);

    if (mJumpedFromQuicksand == 0) {
        int v = mSurfaceType;
        if (v >= 6 && v <= 9) {
            int s = mSinkDepth;
            if (s < 0x1e000) {
                mSinkDepth = 0x1e000;
            }
        }
    }

    int r1;
    if (r4 == (int)&data_ov002_0211019c) {
        r1 = func_ov002_020e0a64(c);
    } else if (r4 == (int)&data_ov002_021101b4) {
        r1 = 0x55;
    } else if (r4 == (int)&data_ov002_021101e4) {
        r1 = 0x4e;
    } else if (r4 == (int)&data_ov002_0211052c) {
        mPrevAngleY = mAngleY;
        mHorzSpeed = 0;
        r1 = 0x2b;
    } else if (r4 == (int)&data_ov002_02110724 || r4 == (int)&data_ov002_0211007c) {
        r1 = 0x51;
    } else if (r4 == (int)&data_ov002_0211055c) {
        r1 = 0x1b;
    } else if (r4 == (int)&data_ov002_02110454) {
        r1 = 0x60;
    } else {
        r1 = 0x52;
    }

    _ZN6Player7SetAnimEji5Fix12IiEj(this, r1, 0x40000000, 0x1000, 0);

    int idx = ((unsigned char*)&data_020a0e40)[0];
    short sval = *(short*)((char*)&data_0209f4a0 + idx * 0x18);
    if (sval == 0 && (IsAnim(0x2b) || IsAnim(0x4c))) {
        func_ov002_020e25d4(c);
    }
    mStateStep = 0;
    if (r4 == (int)&data_ov002_0211019c && mJumpComboStage == 2) {
        mStateStep = 1;
    }

    *(short*)(c + 0x6a4) = 2;
    if (func_ov002_020e2c84(c) != 2 && mIsInShallowWater == 0) {
        func_ov002_020bf9d4(c);
    }

    return 1;
}
