typedef short s16;
typedef struct Vec3 { int x, y, z; } Vec3;
typedef struct Mtx43 { int w[12]; } Mtx43;
typedef struct dBgCh_Gnd { char filler[0x44]; int clsnY; char rest[0x8]; } dBgCh_Gnd;

extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *self);
extern s16 func_02010844(void *unused, void *v, s16 angle);
extern void Vec3_Asr(Vec3 *d, Vec3 *s, int sh);
extern void Matrix4x3_FromTranslation(Mtx43 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(Mtx43 *m, s16 angY);
extern void Matrix4x3_ApplyInPlaceToRotationX(Mtx43 *m, s16 angX);
extern void Matrix4x3_ApplyInPlaceToRotationZ(Mtx43 *m, s16 angZ);
extern void _ZN9ModelBase12ApplyOpacityEj(void *self, unsigned int op, int z);
extern void MulMat4x3Mat4x3(void *dst, void *a, void *b);
extern void Vec3_LslInPlace(void *v, int sh);
extern void _ZN9dBgCh_GndC1Ev(dBgCh_Gnd *self);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(dBgCh_Gnd *self, Vec3 *pos, void *actor);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(dBgCh_Gnd *self);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *c, void *sm, void *mtx, int rad, int h, unsigned int flags);
extern void _ZN9dBgCh_GndD1Ev(dBgCh_Gnd *self);

extern Mtx43 data_020a0e68;

void func_ov060_0211577c(char *c)
{
    char pad[8];
    Mtx43 saved;
    Vec3 pos;
    Vec3 v;
    Vec3 v2;
    dBgCh_Gnd rc;
    int zero;

    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x14c) == 0)
        goto skip_angles;
    if (*(int *)(c + 0x410) != 0)
        goto skip_angles;
    *(s16 *)(c + 0x8c) = func_02010844(c, c + 0x3bc, *(s16 *)(c + 0x8e));
    *(s16 *)(c + 0x90) = func_02010844(c, c + 0x3bc, (s16)(*(s16 *)(c + 0x8e) - 0x4000));
skip_angles:
    Vec3_Asr(&v, (Vec3 *)(c + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(s16 *)(c + 0x8e));
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(s16 *)(c + 0x8c));
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, *(s16 *)(c + 0x90));
    *(Mtx43 *)(c + 0xf0) = data_020a0e68;
    _ZN9ModelBase12ApplyOpacityEj(c + 0xd4, (unsigned char)((int)*(unsigned char *)(c + 0x41c) >> 3), 1);
    saved = data_020a0e68;
    zero = 0;
    *(int *)(c + 0x3e0) = zero;
    *(int *)(c + 0x3e4) = zero;
    *(int *)(c + 0x3e8) = zero;
    MulMat4x3Mat4x3(*(char **)(c + 0xe8) + 0x90, &data_020a0e68, &data_020a0e68);
    *(int *)(c + 0x3e0) = data_020a0e68.w[9];
    *(int *)(c + 0x3e4) = data_020a0e68.w[10];
    *(int *)(c + 0x3e8) = data_020a0e68.w[11];
    Vec3_LslInPlace(c + 0x3e0, 3);
    *(int *)(c + 0x3e4) = *(int *)(c + 0x60);
    *(int *)(c + 0x3d4) = zero;
    *(int *)(c + 0x3d8) = zero;
    *(int *)(c + 0x3dc) = zero;
    data_020a0e68 = saved;
    MulMat4x3Mat4x3(*(char **)(c + 0xe8) + 0x120, &data_020a0e68, &data_020a0e68);
    *(int *)(c + 0x3d4) = data_020a0e68.w[9];
    *(int *)(c + 0x3d8) = data_020a0e68.w[10];
    *(int *)(c + 0x3dc) = data_020a0e68.w[11];
    Vec3_LslInPlace(c + 0x3d4, 3);
    *(int *)(c + 0x3d8) = *(int *)(c + 0x60);
    if (*(unsigned char *)(c + 0x426) == 0)
        return;
    _ZN9dBgCh_GndC1Ev(&rc);
    pos.x = *(int *)(c + 0x5c);
    pos.y = *(int *)(c + 0x60);
    pos.z = *(int *)(c + 0x64);
    pos.y = pos.y + 0x32000;
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rc, &pos, 0);
    if (_ZN9dBgCh_Gnd10DetectClsnEv(&rc))
        pos.y = rc.clsnY;
    else
        pos.y = *(int *)(c + 0x60);
    Vec3_Asr(&v2, &pos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v2.x, v2.y, v2.z);
    *(Mtx43 *)(c + 0x330) = data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c + 0x308, c + 0x330, 0x140000, 0x64000, 0xf);
    _ZN9dBgCh_GndD1Ev(&rc);
}
