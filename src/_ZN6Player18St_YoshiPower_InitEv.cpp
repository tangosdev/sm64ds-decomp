//cpp
// @symbol _ZN6Player18St_YoshiPower_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
/* Player::St_YoshiPower_Init at 0x020d7ed0 (ov002), size 0x200
 * Matched byte-for-byte with mwccarm 1.2/sp2p3
 * flags: -O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
extern "C" {
extern void _ZN6Player7SetAnimEji5Fix12IiEj(char*, unsigned int, int, int, unsigned int);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int, unsigned int, void*);
extern void _ZN12CylinderClsn5ClearEv(void*);
extern void func_ov002_020ed63c(void*, int);
}

int Player::St_YoshiPower_Init()
{
    *(unsigned short*)(((int)((char*)this) + 0x6ce) & 0xFFFFFFFFFFFFFFFFLL) |= 0x200;
    unk_6ee = 0;
    mEggParams = 0;
    if (unk_6be != 0) {
        unk_6f4 = 2;
    }
    if (mHorzSpeed >= 0x1c000) {
        mHorzSpeed = 0x1c000;
    }
    if (**(int**)((char*)&mHeldObjQueue) == 0) {
        if (unk_6f4 != 0) {
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x6d, 0x40000000, 0x1000, 0);
            mStateStep = 5;
            mStateTimer = 0x1e;
            *(unsigned char*)(((int)((char*)this) + 0x6f4) & 0xFFFFFFFFFFFFFFFFLL) -= 1;
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(0, 0x101, ((char*)this)+0x74);
        } else if (*(void**)((char*)&mObjInMouth) == 0) {
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x6c, 0x40000000, 0x1000, 0);
            if (mUseAltBodyModel != 0) {
                func_ov002_020d71a0(((char*)this));
            }
            mStateStep = 0;
            mStateArg = 0;
            _ZN12CylinderClsn5ClearEv((char*)&mAttackClsn);
            func_ov002_020d708c(((char*)this));
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(0, 0xfc, ((char*)this)+0x74);
        } else {
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x6d, 0x40000000, 0x1000, 0);
            mStateStep = 4;
        }
    } else {
        int i;
        _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x30, 0x40000000, 0x1000, 0);
        mStateStep = 6;
        *(void**)((char*)&mHeldObj) = **(void***)((char*)&mHeldObjQueue);
        *(int*)(((int)*(void**)((char*)&mHeldObj) + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) |= 0x100;
        func_ov002_020ed63c(*(void**)((char*)&mHeldObj), 1);
        (*(int**)((char*)&mHeldObjQueue))[0] = 0;
        for (i = 1; i < 5; i++) {
            int* p = *(int**)((char*)&mHeldObjQueue);
            int v = p[i];
            if (v != 0) {
                (p + i)[-1] = v;
                (*(int**)((char*)&mHeldObjQueue))[i] = 0;
            }
        }
    }
    return 1;
}
