//cpp
typedef struct { int x, y, z; } Vector3;
extern "C" void *_ZN5Actor13ClosestPlayerEv(void *self);
extern "C" int func_ov032_02111350(void *c);
extern "C" int func_ov032_02111254(void *c);
extern "C" void Matrix4x3_FromRotationY(void *m, short angle);
extern "C" void Matrix4x3_ApplyInPlaceToRotationX(void *m, short angX);
extern "C" void MulVec3Mat4x3(void *in, void *m, void *out);
extern "C" int _ZN9Animation8FinishedEv(void *self);
extern "C" int _Z14ApproachLinearRsss(short *p, short target, short step);
extern "C" int AngleDiff(short a, short b);
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca, int a, int fix, unsigned int b);
extern "C" void func_ov032_02111ff4(void *c, void *p);
extern "C" short Vec3_VertAngle(void *a, void *b);

extern int data_020a0e68;
extern void *data_ov032_02113a50[];
extern void *data_ov032_02113a8c;
extern int data_0209f32c;

extern "C" int func_ov032_02111620(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    unsigned char *r5;

    r5 = (unsigned char *)_ZN5Actor13ClosestPlayerEv(c);
    if (r5 == 0)
        return 1;

    if (func_ov032_02111350(c) == 1)
        goto zeroblock;
    if (func_ov032_02111254(c) != 0)
        goto matrixblock;
zeroblock:
    *(int *)(c + 0x98) = 0;
    *(int *)(c + 0xa4) = 0;
    *(int *)(c + 0xa8) = 0;
    *(int *)(c + 0xac) = 0;
    goto afterblock;
matrixblock:
    {
        int in[3];
        int out[3];
        in[2] = 0;
        in[2] = 0x14000;
        in[0] = 0;
        in[1] = 0;
        out[0] = 0;
        out[1] = 0;
        out[2] = 0;
        Matrix4x3_FromRotationY(&data_020a0e68, *(short *)(c + 0x8e));
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(short *)(c + 0x8c));
        MulVec3Mat4x3(in, &data_020a0e68, out);
        *(int *)(c + 0xa8) = out[1];
    }
afterblock: ;

    if (*(int *)(c + 0x424) == 0) {
        if (_ZN9Animation8FinishedEv(c + 0x39c) == 0)
            goto player_path;
    }

    _Z14ApproachLinearRsss((short *)(c + 0x92), 0, 0x200);
    *(int *)(c + 0x424) = 1;
    *(unsigned char *)(c + 0x428) = 0;
    if (AngleDiff(*(short *)(c + 0x92), 0) < 0x200) {
        *(short *)(c + 0x92) = 0;
        *(short *)(c + 0x400 + 0x2a) = 0x64;
        *(short *)(c + 0x400 + 0x30) = *(short *)(c + 0x8e);
        *(int *)(c + 0xb0) = 3;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x34c, data_ov032_02113a50[1], 0, 0x1000, 0);
        func_ov032_02111ff4(c, &data_ov032_02113a8c);
    }
    goto end;

player_path:
    {
        int vec[3];
        unsigned int n;
        int *p = (int *)(((int)r5 + 0x5c) & 0xFFFFFFFFFFFFFFFFLL);
        vec[0] = p[0];
        vec[1] = p[1];
        vec[2] = p[2];
        if (data_0209f32c > *(int *)(c + 0x60)) {
            _Z14ApproachLinearRsss((short *)(c + 0x92), 0, 0x200);
        } else {
            _Z14ApproachLinearRsss((short *)(c + 0x92), Vec3_VertAngle(c + 0x5c, vec), 0x200);
        }
        n = (unsigned int)(*(int *)(c + 0x3a4) << 4) >> 0x10;
        if (n > 0x14 && n < 0x3c)
            *(unsigned char *)(c + 0x428) = 1;
        else
            *(unsigned char *)(c + 0x428) = 0;
    }
end:
    return 1;
}
