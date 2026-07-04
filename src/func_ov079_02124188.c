typedef struct { int x, y, z; } Vec3;
typedef struct { int m[12]; } Mtx43;

extern void Matrix4x3_FromRotationXYZExt(void *m, int x, int y, int z);
extern void Vec3_Asr(Vec3 *d, Vec3 *s, int sh);
extern void Matrix4x3_FromTranslation(Mtx43 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *mF, short angY);
extern void _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    void *self, void *sm, void *mtx, int a, int b, int c, unsigned int g);

extern short data_02082214[];
extern Mtx43 data_020a0e68;

#define FX12(a,b) (int)(((long long)(a) * (int)(b) + 0x800) >> 12)

void func_ov079_02124188(char *self)
{
    Vec3 pos;
    Vec3 asr;
    int shadowRad;
    int cosv;
    int sinComp;
    char *rotdesc;

    Matrix4x3_FromRotationXYZExt(self + 0x2e8,
        *(short *)(self + 0x8c), *(short *)(self + 0x8e), *(short *)(self + 0x90));

    *(int *)(self + 0x30c) = *(int *)(self + 0x5c) >> 3;
    *(int *)(self + 0x310) = (*(int *)(self + 0x60) + *(int *)(self + 0x410)) >> 3;
    *(int *)(self + 0x314) = *(int *)(self + 0x64) >> 3;

    rotdesc = *(char **)(self + 0x2dc);
    pos.x = *(int *)(self + 0x5c);
    pos.y = *(int *)(self + 0x60);
    pos.z = *(int *)(self + 0x64);

    *(short *)(self + 0x3e0) = (short)-*(unsigned short *)(rotdesc + 0x1c);
    *(short *)(self + 0x3e2) = *(unsigned short *)(rotdesc + 0x1a);
    *(short *)(self + 0x3e4) = (short)(*(unsigned short *)(rotdesc + 0x1e) - 0x4000);

    if (*(unsigned char *)(self + 0x414) != 0) {
        cosv = data_02082214[(*(unsigned short *)(self + 0x8c) >> 4) << 1];
        sinComp = cosv * 0x190;
        shadowRad = cosv * 0x1a9 + 0xc8000;
    } else {
        cosv = data_02082214[(*(unsigned short *)(self + 0x8c) >> 4) << 1];
        sinComp = cosv * 0xc8;
        shadowRad = cosv * 0xd2 + 0x64000;
    }

    pos.x += FX12(sinComp, data_02082214[(*(unsigned short *)(self + 0x8e) >> 4) << 1]);
    pos.z += FX12(sinComp, data_02082214[((*(unsigned short *)(self + 0x8e) >> 4) << 1) + 1]);

    Vec3_Asr(&asr, &pos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, asr.x, asr.y, asr.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68,
        (short)(*(short *)(self + 0x8e) + *(short *)(self + 0x3e2)));

    *(Mtx43 *)(self + 0x36c) = data_020a0e68;

    if (*(unsigned char *)(self + 0x404) == 0)
        return;

    if (*(unsigned char *)(self + 0x414) != 0) {
        _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
            self, self + 0x344, self + 0x36c, 0x1cc000, 0x190000, shadowRad, 0xf);
    } else {
        _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
            self, self + 0x344, self + 0x36c, 0xf0000, 0x190000, shadowRad, 0xf);
    }
    return;
}
