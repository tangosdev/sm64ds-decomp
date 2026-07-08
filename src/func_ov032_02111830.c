// NONMATCHING: different op / idiom (div=58). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef short s16;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int Fix12i;
typedef struct { int x, y, z; } Vector3;
struct Matrix4x3 { int m[12]; };

extern int WATER_HEIGHT;
extern struct Matrix4x3 MATRIX_SCRATCH_PAPER;
extern int data_ov032_02113a50[];
extern void* data_ov032_02113a8c;

extern void Matrix4x3_FromRotationY(struct Matrix4x3 *m, s16 angY);
extern void Matrix4x3_ApplyInPlaceToRotationX(struct Matrix4x3 *m, s16 angX);
extern void MulVec3Mat4x3(const Vector3 *v, const struct Matrix4x3 *m, Vector3 *out);
extern u32 func_02022c80(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, const void *dir);
extern u32 func_02022d00(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, void *dir);
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
    if (WATER_HEIGHT - 0x64000 > *(int*)(c + 0x60))
        speed = 0;

    if (*(int*)(c + 0x424) == 0 && WATER_HEIGHT > *(int*)(c + 0x60)) {
        if (*(s16*)(c + 0x430) > 0)
            *(int*)(c + 0x424) = 1;
    }

    if (*(int*)(c + 0x424) > 0 && *(int*)(c + 0x424) < 5) {
        (*(int*)(c + 0x424))++;
        if (*(int*)(c + 0x424) == 4) {
            Vector3 v[3];

            v[0].x = *(int*)(c + 0x5c);
            v[0].y = *(int*)(c + 0x60);
            v[0].z = *(int*)(c + 0x64);
            v[1].x = 0; v[1].y = 0; v[1].z = 0;
            v[2].x = 0; v[2].y = 0; v[2].z = 0;
            v[1].z = 0xa0000;
            Matrix4x3_FromRotationY(&MATRIX_SCRATCH_PAPER, *(s16*)(c + 0x8e));
            MulVec3Mat4x3(&v[1], &MATRIX_SCRATCH_PAPER, &v[2]);
            v[0].x += v[2].x;
            v[0].z += v[2].z;
            func_02022c80(0, 0x55, v[0].x, v[0].y, v[0].z, 0);
            *(int*)(c + 0x42c) = func_02022d00(*(int*)(c + 0x42c), 0x56, v[0].x, WATER_HEIGHT, v[0].z, 0);
            v[0].y += 0x4b000;
            func_02022c80(0, 0x54, v[0].x, v[0].y, v[0].z, 0);
        }
    }

    if (func_ov032_02111350(c) == 1 || func_ov032_02111254(c) == 0) {
        speed = 0;
        *(int*)(c + 0x98) = 0;
        *(int*)(c + 0xa4) = 0;
        *(int*)(c + 0xa8) = 0;
        *(int*)(c + 0xac) = 0;
    } else {
        Vector3 in2;
        Vector3 out2;

        in2.x = 0; in2.y = 0; in2.z = 0;
        out2.x = 0; out2.y = 0; out2.z = 0;
        in2.z = 0x14000;
        Matrix4x3_FromRotationY(&MATRIX_SCRATCH_PAPER, *(s16*)(c + 0x8e));
        Matrix4x3_ApplyInPlaceToRotationX(&MATRIX_SCRATCH_PAPER, *(s16*)(c + 0x8c));
        MulVec3Mat4x3(&in2, &MATRIX_SCRATCH_PAPER, &out2);
        *(int*)(c + 0xa8) = out2.y;
    }

    _Z14ApproachLinearRsss((s16*)(c + 0x430), speed, 0x200);
    _Z14ApproachLinearRsss((s16*)(c + 0x92), *(s16*)(c + 0x430), 0x200);

    if (_ZN9Animation8FinishedEv(c + 0x39c) != 0) {
        *(unsigned char*)(c + 0x428) = 0;
        if (speed == 0) {
            s16 a = *(s16*)(c + 0x92);
            if (a < 0) a = -a;
            if (a < 0x100) {
                *(s16*)(c + 0x42a) = 0x64;
                *(s16*)(c + 0x430) = *(s16*)(c + 0x8e);
                *(int*)(c + 0xb0) = 3;
                *(int*)(c + 0x128) &= ~2;
                *(int*)(c + 0x168) &= ~2;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x34c, (void*)data_ov032_02113a50[1], 0, 0x1000, 0);
                func_ov032_02111ff4(c, &data_ov032_02113a8c);
                return 1;
            }
        }
    } else {
        u16 t = (u16)(*(int*)(c + 0x3a4) >> 12);
        if (t > 0x14 && t < 0x3c)
            *(unsigned char*)(c + 0x428) = 1;
        else
            *(unsigned char*)(c + 0x428) = 0;
    }

    return 1;
}
