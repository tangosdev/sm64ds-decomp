//cpp
#include "types.h"
// @symbol _ZN14UnknownVsEntry8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "UnknownVsEntry.h"
extern "C" {
extern void func_ov075_021152d4(void* self);
extern int _ZN9Animation7AdvanceEv(void* a);
}
extern u8 data_0209fc5c;

int UnknownVsEntry::Behavior()
{
    if (mSuspended == 0) {
        int i;
        char* base;
        char* p;
        u8* g;
        int fp;
        int z;
        char* ee;
        g = &data_0209fc5c;
        p = ((char*)this);
        base = ((char*)this) + 0x920;
        fp = 1;
        i = 0;
        z = 0;
        ee = ((char*)this); ee += 0xe80;
        for (; i < 4; i++, base += 0x158, p += 0x158, g += 1) {
            func_ov075_02114cd8(base);
            if (*(u8*)(p + 0xa75)) {
                int* q = (int*)(int)(base + 0x118);
                int v[3];
                v[0] = q[0]; v[1] = q[1]; v[2] = q[2];
                func_ov075_0211ab38(ee, v);
            }
            if (*g) {
                if (func_ov075_021148f0(base) == 0)
                    fp = z;
            }
        }

        if (mState == 1 && fp != 0) {
            int j = 0;
            u8* g2 = &data_0209fc5c;
            char* b2 = ((char*)this) + 0x920;
            for (; j < 4; j++) {
                if (*g2) func_ov075_02114894(b2);
                g2 += 1; b2 += 0x158;
            }
            mState = 2;
        }
        if (mState != 0) {
            if (func_ov075_02114ac4(((char*)this) + 0x920 + mFocusedPlayer * 0x158, ((char*)this) + 0xf34, ((char*)this) + 0xf28) != 0)
                func_ov075_021152d4(((char*)this));
        }
        if (mAnimActive) {
            int r = func_0203da9c();
            func_ov075_021151b4(((char*)this), r);
            /* 0x90c is +0x50 inside the ModelAnim at 0x8bc -- its Animation base,
               which the cartridge's own ~UnknownVsEntry proves is there
               (tools/dtor_members.py). Advance is non-virtual, so this is the same
               direct bl to _ZN9Animation7AdvanceEv with this adjusted by +0x50. */
            mModelAnim.Advance();
        }
        func_ov075_0211b418((char*)&unk_e80);
    }
    return 1;
}
