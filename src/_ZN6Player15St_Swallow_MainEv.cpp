//cpp
// @symbol _ZN6Player15St_Swallow_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Animation.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern unsigned int _ZNK6Player14GetBodyModelIDEjb(void* c, unsigned int a, int b);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, void* v);
extern int func_ov002_020ceaf4(char* c);
extern int _ZN6Player12FinishedAnimEv(char* c);
extern void _ZN6Player11ChangeStateERNS_5StateE(char* c, void* st);
extern void func_ov002_020bedd4(char* c);

extern int data_0209ee90[];
extern int data_ov002_0211067c;
extern int data_ov002_0211013c;
}

int Player::St_Swallow_Main()
{
    void* p360;

    if (_ZNK9Animation12WillHitFrameEi((char*)(*(void**)(((char*)this) + (_ZNK6Player14GetBodyModelIDEjb(((char*)this), mParam & 0xff, 0) << 2) + 0xdc)) + 0x50, 3)) {
        func_ov002_020d71a0(((char*)this));
        goto L65ec;
    }
    if (_ZNK9Animation12WillHitFrameEi((char*)(*(void**)(((char*)this) + (_ZNK6Player14GetBodyModelIDEjb(((char*)this), mParam & 0xff, 0) << 2) + 0xdc)) + 0x50, 7) == 0) {
        goto L65ec;
    }

    p360 = *(void**)((char*)&mObjInMouth);
    if (p360 == 0) goto L65e4;
    {
        int b = (*(unsigned short*)((char*)p360 + 0xc) == 0xbf);
        if (b == 0) goto L653c;
    }
    *(short*)((char*)p360 + 0x8c) = mAngX;
    *(short*)((char*)p360 + 0x8e) = mAngleY;
    *(short*)((char*)p360 + 0x90) = mAngZ;
    func_ov002_020d5ab4(*(void**)((char*)&mObjInMouth));
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(0, 0x100, ((char*)this) + 0x74);
    goto L65e4;

L653c:
    (*(void (**)(void*, char*))(*(char**)p360 + 0x4c))(p360, ((char*)this));
    {
        void* q = *(void**)((char*)&mObjInMouth);
        if ((*(int (**)(void*))(*(char**)q + 0x48))(q) == 1) goto L65e4;
    }
    if ((unsigned short)(mStateFlags & 0x1000)) goto L65e4;
    {
        unsigned char flag = 0;
        int b_c2 = (*(unsigned short*)((char*)*(void**)((char*)&mObjInMouth) + 0xc) == 0xc2);
        if (b_c2 || *(int*)((char*)data_0209ee90 + 0x238) != 0) {
            flag |= 1;
        }
        *(unsigned char*)(((long long)(int)((char*)&mEggParams))) |= (flag + 1);
    }
    func_ov002_020d6368(((char*)this));
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(0, 0x100, ((char*)this) + 0x74);

L65e4:
    *(void**)((char*)&mObjInMouth) = 0;
L65ec:
    if (mIsUnderwater != 0) {
        *(int*)(((long long)(int)((char*)&mVertSpeed))) += func_ov002_020ceaf4(((char*)this));
    }
    if (_ZN6Player12FinishedAnimEv(((char*)this))) {
        if (mIsUnderwater != 0) {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), &data_ov002_0211067c);
        } else {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), &data_ov002_0211013c);
        }
    }
    func_ov002_020bedd4(((char*)this));
    return 1;
}
