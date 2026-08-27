//cpp
#include "Butterfly.h"
typedef short s16;
struct Mtx { int w[12]; };
typedef void (Butterfly::*ButterflyState)();

extern "C" {
void Vec3_Asr(void* d, void* s, int sh);
void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(void* m, s16 ang);
void _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(void* a, void* clsn);
void _ZN9Animation7AdvanceEv(void* a);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void* a, void* sm, void* m, int r, int h, unsigned int f);
}

extern struct Mtx data_020a0e68;
extern s16 data_02082214[];
extern "C" ButterflyState data_ov100_02148628[];

int Butterfly::Behavior()
{
    char* c = (char*)this;
    (this->*data_ov100_02148628[mState])();

    if (*(unsigned char*)(c + 0x3f0) != 0) {
        int spd = *(int*)(c + 0x3e0);
        *(int*)(c + 0x80) = spd;
        *(int*)(c + 0x84) = spd;
        *(int*)(c + 0x88) = spd;

        {
            int s = *(int*)(c + 0x98);
            int idx = (*(unsigned short*)(c + 0x92) >> 4) << 1;
            long long p;
            p = (long long)(-(int)data_02082214[idx]) * s;
            *(int*)(c + 0xa8) = (int)((p + 0x800) >> 0xc);
            p = (long long)(int)data_02082214[idx + 1] * s;
            int horiz = (int)((p + 0x800) >> 0xc);
            {
                int idx2 = (*(unsigned short*)(c + 0x94) >> 4) << 1;
                p = (long long)horiz * (int)data_02082214[idx2];
                *(int*)(c + 0xa4) = (int)((p + 0x800) >> 0xc);
            }
            {
                int idx3 = ((*(unsigned short*)(c + 0x94) >> 4) << 1) + 1;
                p = (long long)horiz * (int)data_02082214[idx3];
                *(int*)(c + 0xac) = (int)((p + 0x800) >> 0xc);
            }
        }
        _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(c, 0);
        (*(int*)(((int)c + 0x3e8)))++;
    }

    if (*(int*)(c + 0x3e4) != 4) {
        int t[3];
        Vec3_Asr(t, c + 0x5c, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, t[0], t[1], t[2]);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(s16*)(c + 0x8e));
        if (*(unsigned char*)(c + 0x3f1) != 0) {
            *(struct Mtx*)(c + 0xf0) = data_020a0e68;
            _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
                c, c + 0x188, c + 0xf0, 0x14000, 0x12c000, 0xf);
            _ZN9Animation7AdvanceEv(c + 0x124);
        } else {
            *(struct Mtx*)(c + 0x154) = data_020a0e68;
            _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
                c, c + 0x1b0, c + 0x154, 0x64000, 0x12c000, 0xf);
        }
    }
    return 1;
}
