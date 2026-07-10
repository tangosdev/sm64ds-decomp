
typedef short s16;
typedef unsigned int u32;
typedef int Fix12i;
extern int data_0209f32c;
extern int data_020a0e68;
extern void *data_ov032_02113a50[];
extern void *data_ov032_02113a8c;
extern void Matrix4x3_FromRotationY(void *m, s16 angY);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, s16 angX);
extern void MulVec3Mat4x3(void *v, void *m, void *out);
extern u32 func_02022c80(u32, u32, Fix12i, Fix12i, Fix12i, const void *);
extern u32 func_02022d00(u32, u32, Fix12i, Fix12i, Fix12i, void *);
extern int func_ov032_02111350(char *c);
extern int func_ov032_02111254(void *c);
extern void _Z14ApproachLinearRsss(s16* p, s16 target, s16 step);
extern int _ZN9Animation8FinishedEv(void* a);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
extern int func_ov032_02111ff4(void*, void*);

int func_ov032_02111830(char *c)
{
    s16 speed;
    speed = 0x3000;
    if (data_0209f32c - 0x64000 > *(int*)(c + 0x60))
        speed = 0;
    if (*(int*)(c + 0x424) == 0) {
        if (data_0209f32c > *(int*)(c + 0x60)) {
            if (*(s16*)(c + 0x400 + 0x30) > 0)
                *(int*)(c + 0x424) = 1;
        }
    }
    if (*(int*)(c + 0x424) > 0 && *(int*)(c + 0x424) < 5) {
        int *p424 = (int*)(((int)c + 0x424) & 0xFFFFFFFFFFFFFFFFLL);
        (*p424)++;
        if (*(int*)(c + 0x424) == 4) {

            typedef struct { int x,y,z; } V3;
            V3 v[3];
            v[0].x = *(int*)(c + 0x5c);
            v[0].y = *(int*)(c + 0x60);
            v[0].z = *(int*)(c + 0x64);
            v[1].z = 0;
            v[1].x = 0; v[1].y = 0;
            v[2].x = 0; v[2].y = 0; v[2].z = 0;
            v[1].z = 0xa0000;
            Matrix4x3_FromRotationY(&data_020a0e68, *(s16*)(c + 0x8e));
            MulVec3Mat4x3(&v[1], &data_020a0e68, &v[2]);
            v[0].x += v[2].x;
            v[0].z += v[2].z;
            func_02022c80(0, 0x55, v[0].x, v[0].y, v[0].z, 0);
            *(int*)(c + 0x42c) = func_02022d00(*(int*)(c + 0x42c), 0x56, v[0].x, data_0209f32c, v[0].z, 0);
            v[0].y += 0x4b000;
            func_02022c80(0, 0x54, v[0].x, v[0].y, v[0].z, 0);

        }
    }

    if (func_ov032_02111350(c) == 1)
        goto zeroblock;
    if (func_ov032_02111254(c) != 0)
        goto matrixblock;
zeroblock:
    speed = 0;
    *(int*)(c + 0x98) = 0;
    *(int*)(c + 0xa4) = 0;
    *(int*)(c + 0xa8) = 0;
    *(int*)(c + 0xac) = 0;
    goto afterblock;
matrixblock:
    {
        int in2[3];
        int out2[3];
        in2[2] = 0;
        in2[2] = 0x14000;
        in2[0] = 0;
        in2[1] = 0;
        out2[0] = 0;
        out2[1] = 0;
        out2[2] = 0;
        Matrix4x3_FromRotationY(&data_020a0e68, *(s16*)(c + 0x8e));
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(s16*)(c + 0x8c));
        MulVec3Mat4x3(in2, &data_020a0e68, out2);
        *(int*)(c + 0xa8) = out2[1];
    }
afterblock: ;
    _Z14ApproachLinearRsss((s16*)(c + 0x430), speed, 0x200);
    _Z14ApproachLinearRsss((s16*)(c + 0x92), *(s16*)(c + 0x400 + 0x30), 0x200);
    if (_ZN9Animation8FinishedEv(c + 0x39c) != 0) {
        *(unsigned char*)(c + 0x428) = 0;
        if (speed == 0) {
            s16 a = *(s16*)(c + 0x92);
            if (a < 0) a = -a;
            if (a < 0x100) {
                *(s16*)(c + 0x400 + 0x2a) = 0x64;
                *(s16*)(c + 0x400 + 0x30) = *(s16*)(c + 0x8e);
                *(int*)(c + 0xb0) = 3;
                *(int*)(((int)c + 0x128) & 0xFFFFFFFFFFFFFFFFLL) &= ~2;
                *(int*)(((int)c + 0x168) & 0xFFFFFFFFFFFFFFFFLL) &= ~2;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x34c, data_ov032_02113a50[1], 0, 0x1000, 0);
                func_ov032_02111ff4(c, &data_ov032_02113a8c);
                return 1;
            }
        }
    } else {
        unsigned int t = (unsigned int)(*(int*)(c + 0x3a4) << 4) >> 0x10;
        if (t > 0x14 && t < 0x3c)
            *(unsigned char*)(c + 0x428) = 1;
        else
            *(unsigned char*)(c + 0x428) = 0;
    }
    return 1;
}
